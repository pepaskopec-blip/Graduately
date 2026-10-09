package org.maturita.maturita.platform

import java.util.Locale

/**
 * Everything the shared app needs from the system it runs on. Android and the
 * desktop (Linux, Windows) each provide one; the screens never touch platform
 * APIs directly.
 */
interface Platform {
    /** Source commit baked into the build, or "dev" for local builds. */
    val commit: String
    val versionName: String
    /** Text key of the welcome card blurb, which names the device. */
    val welcomeKey: String
    val prefs: Prefs
    val updater: AppUpdater

    /** A speech engine for listening exercises, or null when there is none. */
    fun speaker(): Speaker?
}

/** The subset of Android's SharedPreferences the app uses. */
interface Prefs {
    fun getBoolean(key: String, fallback: Boolean): Boolean
    fun getInt(key: String, fallback: Int): Int
    fun getString(key: String, fallback: String?): String?
    fun edit(): Editor

    interface Editor {
        fun putBoolean(key: String, value: Boolean): Editor
        fun putInt(key: String, value: Int): Editor
        fun putString(key: String, value: String): Editor
        fun apply()
    }
}

data class UpdateState(
    val status: String = "idle",
    val remote: String? = null,
    val messageKey: String = "update_current",
    val messageArg: String? = null,
    val canInstall: Boolean = false,
)

/** Callbacks always arrive on the UI thread. */
interface AppUpdater {
    fun check(interactive: Boolean, cb: (UpdateState) -> Unit)
    fun install(cb: (UpdateState) -> Unit)
}

interface Speaker {
    /**
     * Reads [text] aloud. [onRange] gets the offset of the word being spoken
     * where the engine reports it; [onEnd] gets false when speaking failed.
     * Both run on the UI thread.
     */
    fun speak(
        text: String,
        locale: Locale,
        rate: Float,
        volume: Float,
        onRange: (Int) -> Unit,
        onEnd: (Boolean) -> Unit,
    )

    fun stop()
    fun close()
}
