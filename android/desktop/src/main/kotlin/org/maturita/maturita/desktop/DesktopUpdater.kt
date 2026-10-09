package org.maturita.maturita.desktop

import org.maturita.maturita.platform.AppUpdater
import org.maturita.maturita.platform.UpdateState
import java.io.File
import java.net.HttpURLConnection
import java.net.URI
import java.util.concurrent.Executors
import java.util.zip.ZipInputStream
import javax.swing.SwingUtilities
import kotlin.system.exitProcess

/**
 * Self-update from the `builds` branch, the same scheme the GTK builds used,
 * so their updater and this one hand over cleanly: the asset names stay, and
 * progress/ next to the app survives the swap.
 *
 * raw.githubusercontent.com caches branch-named URLs, so the branch tip is
 * read from the Atom feed and every file is fetched from that commit. A running
 * app cannot replace itself; a small script waits for this process to exit,
 * swaps the new version in and starts it.
 */
class DesktopUpdater(private val commit: String, private val install: Install) : AppUpdater {
    private val io = Executors.newSingleThreadExecutor { r -> Thread(r, "updater").apply { isDaemon = true } }
    private val repo = "pepaskopec-blip/Graduately"
    private val branch = "builds"
    private val platform = if (Install.isWindows) "windows" else "linux"
    private val asset = if (Install.isWindows) "graduately-windows-x64.zip" else "graduately-linux-x86_64.AppImage"

    private fun emit(cb: (UpdateState) -> Unit, state: UpdateState) = SwingUtilities.invokeLater { cb(state) }

    override fun check(interactive: Boolean, cb: (UpdateState) -> Unit) {
        if (!commit.matches(Regex("[0-9a-fA-F]{7,40}"))) {
            emit(cb, UpdateState("dev", messageKey = "update_err_devbuild"))
            return
        }
        if (interactive) emit(cb, UpdateState("checking", messageKey = "update_checking"))
        io.execute {
            try {
                val atom = get("https://github.com/$repo/commits/$branch.atom")
                val tip = atomTip(atom) ?: error("no tip")
                val remote = remoteCommit(atom, tip)
                emit(cb, when {
                    sameCommit(remote, commit) -> UpdateState("ok", remote, "update_uptodate", remote.take(7))
                    install.kind == Install.Kind.Unsupported ->
                        UpdateState("unsupported", remote, if (interactive) "update_err_unsupported" else "update_current", commit.take(7))
                    else -> UpdateState("available", remote, "update_available", remote.take(7), canInstall = true)
                })
            } catch (_: Exception) {
                emit(cb, if (interactive) UpdateState("error", messageKey = "update_err_network")
                else UpdateState(messageArg = commit.take(7)))
            }
        }
    }

    override fun install(cb: (UpdateState) -> Unit) {
        val target = install.target
        if (install.kind == Install.Kind.Unsupported || target == null) {
            emit(cb, UpdateState("error", messageKey = "update_err_unsupported"))
            return
        }
        emit(cb, UpdateState("downloading", messageKey = "update_downloading"))
        io.execute {
            val staging = File(System.getProperty("java.io.tmpdir"), "graduately-update")
            val source: File
            try {
                val tip = atomTip(get("https://github.com/$repo/commits/$branch.atom")) ?: error("no tip")
                staging.deleteRecursively()
                staging.mkdirs()
                val file = File(staging, asset)
                download("https://raw.githubusercontent.com/$repo/$tip/$asset", file)
                source = if (install.kind == Install.Kind.WindowsDir) {
                    File(staging, "new").also { unzip(file, it) }
                        .takeIf { File(it, "graduately.exe").isFile } ?: error("bad zip")
                } else {
                    file.takeIf { isElf(it) }?.also { it.setExecutable(true) } ?: error("bad AppImage")
                }
            } catch (_: Exception) {
                staging.deleteRecursively()
                emit(cb, UpdateState("error", messageKey = "update_err_download", canInstall = true))
                return@execute
            }
            try {
                startSwap(target, source, staging)
            } catch (_: Exception) {
                emit(cb, UpdateState("error", messageKey = "update_err_stage", canInstall = true))
                return@execute
            }
            emit(cb, UpdateState("staged", messageKey = "update_staged"))
            SwingUtilities.invokeLater { exitProcess(0) }
        }
    }

    private fun startSwap(target: File, source: File, staging: File) {
        val pid = ProcessHandle.current().pid().toString()
        val windows = install.kind == Install.Kind.WindowsDir
        val script = File(System.getProperty("java.io.tmpdir"), if (windows) "graduately-update.cmd" else "graduately-update.sh")
        script.writeText(if (windows) WINDOWS_SWAP else UNIX_SWAP)
        script.setExecutable(true)
        val argv = if (windows) listOf("cmd", "/c", script.path) else listOf("/bin/sh", script.path)
        val pb = ProcessBuilder(argv + listOf(pid, target.path, source.path, staging.path))
        // The new AppImage must get its own mount, not inherit this one.
        pb.environment().keys.removeAll(setOf("APPIMAGE", "APPDIR", "ARGV0", "OWD"))
        val p = pb.redirectErrorStream(true).redirectOutput(ProcessBuilder.Redirect.DISCARD).start()
        // The script detaches itself and exits; wait so we do not quit first.
        p.waitFor()
    }

    /** Commit whose package for this platform sits on the branch. */
    private fun remoteCommit(atom: String, tip: String): String {
        val marker = runCatching { get("https://raw.githubusercontent.com/$repo/$tip/VERSION-$platform").trim() }.getOrNull()
        if (marker != null && marker.length >= 7) return marker
        return atomMainSha(atom) ?: get("https://raw.githubusercontent.com/$repo/$tip/VERSION").trim()
    }

    private fun atomTip(atom: String): String? =
        Regex("""Commit/([0-9a-f]{40})""", RegexOption.IGNORE_CASE).find(atom)?.groupValues?.get(1)

    private fun atomMainSha(atom: String): String? =
        Regex("""Build from ([0-9a-f]{7,40})""", RegexOption.IGNORE_CASE).find(atom)?.groupValues?.get(1)

    private fun sameCommit(a: String, b: String): Boolean {
        val n = minOf(a.length, b.length)
        return n >= 7 && a.take(n).equals(b.take(n), ignoreCase = true)
    }

    private fun isElf(f: File): Boolean = f.length() > 1_000_000L &&
        f.inputStream().use { val h = ByteArray(4); it.read(h) == 4 && h.contentEquals(byteArrayOf(0x7f, 'E'.code.toByte(), 'L'.code.toByte(), 'F'.code.toByte())) }

    private fun unzip(zip: File, dest: File) {
        val root = dest.canonicalFile
        ZipInputStream(zip.inputStream().buffered()).use { z ->
            while (true) {
                val e = z.nextEntry ?: break
                val out = File(root, e.name).canonicalFile
                if (!out.path.startsWith(root.path + File.separator)) error("zip slip")
                if (e.isDirectory) out.mkdirs() else {
                    out.parentFile.mkdirs()
                    out.outputStream().use { z.copyTo(it) }
                }
            }
        }
    }

    private fun open(url: String, readMs: Int): HttpURLConnection =
        (URI(url).toURL().openConnection() as HttpURLConnection).apply {
            connectTimeout = 15000
            readTimeout = readMs
            instanceFollowRedirects = true
            setRequestProperty("User-Agent", "Graduately-desktop")
        }

    private fun get(url: String): String {
        val c = open(url, 15000)
        try {
            if (c.responseCode !in 200..299) error("http ${c.responseCode}")
            return c.inputStream.bufferedReader().use { it.readText() }
        } finally {
            c.disconnect()
        }
    }

    private fun download(url: String, dest: File) {
        val c = open(url, 60000)
        try {
            if (c.responseCode !in 200..299) error("http ${c.responseCode}")
            c.inputStream.use { input -> dest.outputStream().use { input.copyTo(it) } }
        } finally {
            c.disconnect()
        }
    }

    private companion object {
        // Arguments: pid target source staging.
        const val UNIX_SWAP = """#!/bin/sh
if [ -z "${'$'}GRADUATELY_UPDATE_DETACHED" ]; then
  GRADUATELY_UPDATE_DETACHED=1
  export GRADUATELY_UPDATE_DETACHED
  nohup /bin/sh "${'$'}0" "${'$'}@" >/dev/null 2>&1 &
  exit 0
fi
pid=${'$'}1; target=${'$'}2; source=${'$'}3; staging=${'$'}4
n=0
while kill -0 "${'$'}pid" 2>/dev/null && [ ${'$'}n -lt 600 ]; do
  sleep 0.2; n=${'$'}((n+1))
done
if mv -f "${'$'}source" "${'$'}target" 2>/dev/null; then
  chmod +x "${'$'}target"
fi
"${'$'}target" >/dev/null 2>&1 &
rm -rf "${'$'}staging"
rm -f "${'$'}0"
"""

        // robocopy mirrors the new build over the old one (so nothing stale is
        // left behind) but never touches progress/. ping is a sleep without a
        // console window.
        val WINDOWS_SWAP = """@echo off
if not defined GRADUATELY_UPDATE_DETACHED (
  set GRADUATELY_UPDATE_DETACHED=1
  start "" /b cmd /c "%~f0" %*
  exit /b 0
)
set "PID=%~1"
set "TARGET=%~2"
set "SOURCE=%~3"
set "STAGING=%~4"
:wait
tasklist /FI "PID eq %PID%" 2>nul | find "%PID%" >nul
if not errorlevel 1 (
  ping -n 2 127.0.0.1 >nul
  goto wait
)
robocopy "%SOURCE%" "%TARGET%" /MIR /XD progress /R:5 /W:1 /NFL /NDL /NJH /NJS /NP >nul
start "" "%TARGET%\graduately.exe"
rmdir /s /q "%STAGING%"
(goto) 2>nul & del "%~f0"
""".replace("\n", "\r\n")
    }
}
