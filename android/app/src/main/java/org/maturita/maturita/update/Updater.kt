package org.maturita.maturita.update

import android.app.Application
import android.content.Intent
import android.net.Uri
import android.os.Build
import android.os.Handler
import android.os.Looper
import android.provider.Settings
import androidx.core.content.FileProvider
import org.maturita.maturita.BuildConfig
import java.io.File
import java.net.HttpURLConnection
import java.net.URL
import java.util.concurrent.Executors

data class UpdateState(
    val status: String = "idle",
    val remote: String? = null,
    val messageKey: String = "update_current",
    val messageArg: String? = BuildConfig.COMMIT.take(7),
    val canInstall: Boolean = false,
)

/**
 * Self-update from the `builds` branch. CI rewrites that branch on every
 * push to main with a commit titled "Build from <sha>"; the APK there is
 * signed with the repository's sideload key, so Android accepts it as an
 * update over any earlier build signed the same way.
 */
class Updater(private val app: Application) {
    private val io = Executors.newSingleThreadExecutor()
    private val main = Handler(Looper.getMainLooper())
    private val repo = BuildConfig.UPDATE_REPO
    private val branch = BuildConfig.UPDATE_BRANCH

    private fun emit(cb: (UpdateState) -> Unit, state: UpdateState) {
        main.post { cb(state) }
    }

    fun check(interactive: Boolean, cb: (UpdateState) -> Unit) {
        if (BuildConfig.COMMIT == "dev") {
            emit(cb, UpdateState(status = "dev", messageKey = "update_err_devbuild", messageArg = null))
            return
        }
        if (interactive) {
            emit(cb, UpdateState(status = "checking", messageKey = "update_checking", messageArg = null))
        }
        io.execute {
            try {
                val atom = get("https://github.com/$repo/commits/$branch.atom")
                val tip = atomTip(atom) ?: error("sha")
                val remote = remoteAndroidCommit(atom, tip)
                val local = BuildConfig.COMMIT
                if (sameCommit(remote, local)) {
                    emit(cb, UpdateState("ok", remote, "update_uptodate", remote.take(7)))
                } else {
                    emit(cb, UpdateState("available", remote, "update_available", remote.take(7), true))
                }
            } catch (_: Exception) {
                emit(cb, UpdateState("error", messageKey = "update_err_network", messageArg = null))
            }
        }
    }

    fun install(cb: (UpdateState) -> Unit) {
        // Android 8+ needs a per-app opt-in before we may hand an APK to the
        // package installer. Send the user there and keep the Update button
        // so the second tap goes straight to the download.
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O &&
            !app.packageManager.canRequestPackageInstalls()
        ) {
            val settings = Intent(Settings.ACTION_MANAGE_UNKNOWN_APP_SOURCES).apply {
                data = Uri.parse("package:${app.packageName}")
                flags = Intent.FLAG_ACTIVITY_NEW_TASK
            }
            try {
                app.startActivity(settings)
            } catch (_: Exception) {
            }
            emit(cb, UpdateState("permission", messageKey = "update_android_allow", messageArg = null, canInstall = true))
            return
        }
        emit(cb, UpdateState(status = "downloading", messageKey = "update_downloading", messageArg = null))
        io.execute {
            var tip: String? = null
            try {
                val atom = get("https://github.com/$repo/commits/$branch.atom")
                val head = atomTip(atom) ?: error("sha")
                tip = head
                val remote = remoteAndroidCommit(atom, head)
                // Pin the download to the commit: the branch-named URL can be
                // served from a stale CDN cache (even a cached 404).
                val url = "https://raw.githubusercontent.com/$repo/$head/maturita-android.apk"
                val dir = File(app.cacheDir, "updates").apply { mkdirs() }
                val apk = File(dir, "maturita.apk")
                download(url, apk)
                if (!isOurApk(apk)) {
                    apk.delete()
                    emit(cb, UpdateState("error", remote, "update_err_badapk", null, canInstall = true))
                    return@execute
                }
                val uri = FileProvider.getUriForFile(app, "${app.packageName}.files", apk)
                val intent = Intent(Intent.ACTION_VIEW).apply {
                    setDataAndType(uri, "application/vnd.android.package-archive")
                    flags = Intent.FLAG_ACTIVITY_NEW_TASK or Intent.FLAG_GRANT_READ_URI_PERMISSION
                }
                main.post { app.startActivity(intent) }
                emit(cb, UpdateState("staged", remote, "update_android_installing", remote.take(7), canInstall = true))
            } catch (_: Exception) {
                emit(cb, UpdateState("error", tip, "update_err_download", null, canInstall = true))
            }
        }
    }

    /**
     * Commit whose APK sits in the builds branch. `VERSION-android` is written
     * only when the Android job produced a package; older branches lack it,
     * so fall back to the commit the whole branch was built from.
     */
    private fun remoteAndroidCommit(atom: String, tip: String): String {
        val marker = runCatching {
            get("https://raw.githubusercontent.com/$repo/$tip/VERSION-android").trim()
        }.getOrNull()
        if (marker != null && marker.length >= 7) return marker
        return atomMainSha(atom)
            ?: get("https://raw.githubusercontent.com/$repo/$tip/VERSION").trim()
    }

    private fun atomTip(atom: String): String? =
        Regex("""Commit/([0-9a-f]{40})""", RegexOption.IGNORE_CASE).find(atom)?.groupValues?.get(1)
            ?: Regex("""/commit/([0-9a-f]{7,40})""", RegexOption.IGNORE_CASE).find(atom)?.groupValues?.get(1)

    private fun atomMainSha(atom: String): String? =
        Regex("""Build from ([0-9a-f]{7,40})""", RegexOption.IGNORE_CASE).find(atom)?.groupValues?.get(1)

    private fun sameCommit(a: String, b: String): Boolean {
        val n = minOf(a.length, b.length)
        return n >= 7 && a.take(n).equals(b.take(n), ignoreCase = true)
    }

    /** Reject anything that is not a parseable APK of this very app. */
    private fun isOurApk(file: File): Boolean {
        if (file.length() < 1_000_000L) return false
        file.inputStream().use { input ->
            val header = ByteArray(2)
            if (input.read(header) != 2) return false
            if (header[0] != 'P'.code.toByte() || header[1] != 'K'.code.toByte()) return false
        }
        val info = app.packageManager.getPackageArchiveInfo(file.absolutePath, 0) ?: return false
        return info.packageName == app.packageName
    }

    private fun open(url: String, readMs: Int): HttpURLConnection {
        return (URL(url).openConnection() as HttpURLConnection).apply {
            connectTimeout = 15000
            readTimeout = readMs
            instanceFollowRedirects = true
            setRequestProperty("User-Agent", "maturita.c-android")
            setRequestProperty("Accept", "*/*")
        }
    }

    private fun get(url: String): String {
        val c = open(url, 15000)
        return try {
            if (c.responseCode !in 200..299) error("http ${c.responseCode}")
            c.inputStream.bufferedReader().use { it.readText() }
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
}
