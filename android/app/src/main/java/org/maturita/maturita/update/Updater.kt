package org.maturita.maturita.update

import android.app.Application
import android.content.Intent
import android.os.Handler
import android.os.Looper
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
                val atom = get("https://github.com/${BuildConfig.UPDATE_REPO}/commits/${BuildConfig.UPDATE_BRANCH}.atom")
                val sha = Regex("""<id>.*?/commit/([0-9a-f]{7,40})</id>""", RegexOption.IGNORE_CASE)
                    .find(atom)?.groupValues?.get(1)
                    ?: throw IllegalStateException("sha")
                val version = get(
                    "https://raw.githubusercontent.com/${BuildConfig.UPDATE_REPO}/$sha/VERSION",
                ).trim()
                val local = BuildConfig.COMMIT
                if (version.startsWith(local) || local.startsWith(version)) {
                    emit(cb, UpdateState("ok", version, "update_uptodate", version.take(7)))
                } else {
                    emit(cb, UpdateState("available", version, "update_available", version.take(7), true))
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
                val atom = get("https://github.com/${BuildConfig.UPDATE_REPO}/commits/${BuildConfig.UPDATE_BRANCH}.atom")
                val sha = Regex("""<id>.*?/commit/([0-9a-f]{7,40})</id>""", RegexOption.IGNORE_CASE)
                    .find(atom)?.groupValues?.get(1)
                    ?: throw IllegalStateException("sha")
                val url = "https://raw.githubusercontent.com/${BuildConfig.UPDATE_REPO}/$sha/maturita-android.apk"
                val dir = File(app.cacheDir, "updates").apply { mkdirs() }
                val apk = File(dir, "maturita.apk")
                download(url, apk)
                val uri = FileProvider.getUriForFile(app, "${app.packageName}.files", apk)
                val intent = Intent(Intent.ACTION_VIEW).apply {
                    setDataAndType(uri, "application/vnd.android.package-archive")
                    flags = Intent.FLAG_ACTIVITY_NEW_TASK or Intent.FLAG_GRANT_READ_URI_PERMISSION
                }
                main.post { app.startActivity(intent) }
                emit(cb, UpdateState("staged", sha, "update_staged", sha.take(7)))
            } catch (_: Exception) {
                emit(cb, UpdateState("error", messageKey = "update_err_download", messageArg = null))
            }
        }
    }

    private fun get(url: String): String {
        val c = (URL(url).openConnection() as HttpURLConnection).apply {
            connectTimeout = 15000
            readTimeout = 15000
            instanceFollowRedirects = true
        }
        return try {
            if (c.responseCode !in 200..299) error("http ${c.responseCode}")
            c.inputStream.bufferedReader().use { it.readText() }
        } finally {
            c.disconnect()
        }
    }

    private fun download(url: String, dest: File) {
        val c = (URL(url).openConnection() as HttpURLConnection).apply {
            connectTimeout = 20000
            readTimeout = 60000
            instanceFollowRedirects = true
        }
        try {
            if (c.responseCode !in 200..299) error("http ${c.responseCode}")
            c.inputStream.use { input -> dest.outputStream().use { input.copyTo(it) } }
        } finally {
            c.disconnect()
        }
    }
}
