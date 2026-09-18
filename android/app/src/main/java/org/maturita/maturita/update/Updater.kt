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
        emit(cb, UpdateState(status = "checking", messageKey = "update_checking", messageArg = null))
        io.execute {
            try {
                val atom = get("https://github.com/$repo/commits/$branch.atom")
                val remote = atomMainSha(atom)
                    ?: run {
                        val tip = atomTip(atom) ?: error("sha")
                        get("https://raw.githubusercontent.com/$repo/$tip/VERSION").trim()
                    }
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
        emit(cb, UpdateState(status = "downloading", messageKey = "update_downloading", messageArg = null))
        io.execute {
            try {
                if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O &&
                    !app.packageManager.canRequestPackageInstalls()
                ) {
                    val settings = Intent(Settings.ACTION_MANAGE_UNKNOWN_APP_SOURCES).apply {
                        data = Uri.parse("package:${app.packageName}")
                        flags = Intent.FLAG_ACTIVITY_NEW_TASK
                    }
                    main.post { app.startActivity(settings) }
                    emit(cb, UpdateState("error", messageKey = "update_err_download", messageArg = null))
                    return@execute
                }
                val atom = get("https://github.com/$repo/commits/$branch.atom")
                val tip = atomTip(atom) ?: error("sha")
                val url = "https://raw.githubusercontent.com/$repo/$tip/maturita-android.apk"
                val dir = File(app.cacheDir, "updates").apply { mkdirs() }
                val apk = File(dir, "maturita.apk")
                download(url, apk)
                if (!isApk(apk)) {
                    apk.delete()
                    emit(cb, UpdateState("error", messageKey = "update_err_download", messageArg = null))
                    return@execute
                }
                val uri = FileProvider.getUriForFile(app, "${app.packageName}.files", apk)
                val intent = Intent(Intent.ACTION_VIEW).apply {
                    setDataAndType(uri, "application/vnd.android.package-archive")
                    flags = Intent.FLAG_ACTIVITY_NEW_TASK or Intent.FLAG_GRANT_READ_URI_PERMISSION
                }
                main.post { app.startActivity(intent) }
                emit(cb, UpdateState("staged", tip, "update_staged", tip.take(7)))
            } catch (_: Exception) {
                emit(cb, UpdateState("error", messageKey = "update_err_download", messageArg = null))
            }
        }
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

    private fun isApk(file: File): Boolean {
        if (file.length() < 1_000_000L) return false
        file.inputStream().use { input ->
            val header = ByteArray(2)
            if (input.read(header) != 2) return false
            return header[0] == 'P'.code.toByte() && header[1] == 'K'.code.toByte()
        }
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
