package org.maturita.maturita

import android.app.Application
import android.content.Context
import android.content.SharedPreferences
import android.os.Bundle
import android.os.Handler
import android.os.Looper
import android.speech.tts.TextToSpeech
import android.speech.tts.UtteranceProgressListener
import org.maturita.maturita.platform.Platform
import org.maturita.maturita.platform.Prefs
import org.maturita.maturita.platform.Speaker
import org.maturita.maturita.update.Updater
import java.util.Locale

class AndroidPlatform(private val app: Application) : Platform {
    override val commit: String = BuildConfig.COMMIT
    override val versionName: String = BuildConfig.VERSION_NAME
    override val welcomeKey = "welcome_body_android"
    override val prefs: Prefs = AndroidPrefs(app.getSharedPreferences("maturita", Context.MODE_PRIVATE))
    override val updater = Updater(app)

    override fun speaker(): Speaker = AndroidSpeaker(app)
}

private class AndroidPrefs(private val sp: SharedPreferences) : Prefs {
    override fun getBoolean(key: String, fallback: Boolean) = sp.getBoolean(key, fallback)
    override fun getInt(key: String, fallback: Int) = sp.getInt(key, fallback)
    override fun getString(key: String, fallback: String?): String? = sp.getString(key, fallback)
    override fun edit(): Prefs.Editor = Editor(sp.edit())

    private class Editor(private val e: SharedPreferences.Editor) : Prefs.Editor {
        override fun putBoolean(key: String, value: Boolean) = apply { e.putBoolean(key, value) }
        override fun putInt(key: String, value: Int) = apply { e.putInt(key, value) }
        override fun putString(key: String, value: String) = apply { e.putString(key, value) }
        override fun apply() = e.apply()
    }
}

private class AndroidSpeaker(context: Context) : Speaker {
    private val main = Handler(Looper.getMainLooper())
    private var ready = false
    private val tts: TextToSpeech = TextToSpeech(context) { status ->
        ready = status == TextToSpeech.SUCCESS
    }

    override fun speak(
        text: String,
        locale: Locale,
        rate: Float,
        volume: Float,
        onRange: (Int) -> Unit,
        onEnd: (Boolean) -> Unit,
    ) {
        if (!ready) {
            main.post { onEnd(false) }
            return
        }
        val id = "en-${System.nanoTime()}"
        tts.language = locale
        tts.setSpeechRate(rate)
        tts.setOnUtteranceProgressListener(object : UtteranceProgressListener() {
            override fun onStart(utteranceId: String?) {}
            override fun onRangeStart(utteranceId: String?, rangeStart: Int, rangeEnd: Int, frame: Int) {
                if (utteranceId == id) main.post { onRange(rangeStart) }
            }
            override fun onDone(utteranceId: String?) {
                if (utteranceId == id) main.post { onEnd(true) }
            }
            @Deprecated("Deprecated in Java")
            override fun onError(utteranceId: String?) {
                if (utteranceId == id) main.post { onEnd(false) }
            }
        })
        val params = Bundle().apply { putFloat(TextToSpeech.Engine.KEY_PARAM_VOLUME, volume) }
        tts.speak(text, TextToSpeech.QUEUE_FLUSH, params, id)
    }

    override fun stop() {
        tts.stop()
    }

    override fun close() {
        tts.stop()
        tts.shutdown()
    }
}
