package org.maturita.maturita

import android.app.Application
import android.os.Bundle
import android.util.TypedValue
import android.view.ViewGroup
import android.widget.Button
import android.widget.LinearLayout
import android.widget.ScrollView
import android.widget.TextView
import androidx.activity.ComponentActivity
import androidx.activity.compose.BackHandler
import androidx.activity.compose.setContent
import androidx.activity.enableEdgeToEdge
import androidx.activity.viewModels
import androidx.lifecycle.AndroidViewModel
import org.maturita.maturita.data.Content

/** Keeps the shared app state alive across rotation and other config changes. */
class AppHolder(app: Application) : AndroidViewModel(app) {
    val vm = AppViewModel(
        Content.parse(runCatching { app.assets.open("content.json").bufferedReader().use { it.readText() } }.getOrNull()),
        AndroidPlatform(app),
    )
}

class MainActivity : ComponentActivity() {
    private val holder: AppHolder by viewModels()

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        val crashPrefs = getSharedPreferences("crash", MODE_PRIVATE)
        val prev = Thread.getDefaultUncaughtExceptionHandler()
        Thread.setDefaultUncaughtExceptionHandler { thread, error ->
            try {
                crashPrefs.edit().putString("last", error.stackTraceToString()).commit()
            } catch (_: Exception) {
            }
            prev?.uncaughtException(thread, error)
        }
        val lastCrash = crashPrefs.getString("last", null)
        if (lastCrash != null) {
            setContentView(crashView(lastCrash) {
                crashPrefs.edit().remove("last").apply()
                recreate()
            })
            return
        }
        enableEdgeToEdge()
        val vm = holder.vm
        setContent {
            BackHandler(vm.canGoBack || vm.searchOpen) { vm.handleBack() }
            MaturitaApp(vm)
        }
    }

    private fun crashView(dump: String, retry: () -> Unit): LinearLayout {
        val pad = (20 * resources.displayMetrics.density).toInt()
        val title = TextView(this).apply {
            text = "Graduately spadla"
            setTextSize(TypedValue.COMPLEX_UNIT_SP, 22f)
            setPadding(0, 0, 0, pad / 2)
        }
        val hint = TextView(this).apply {
            text = "Pošli tenhle text, ať jde pád opravit."
            setTextSize(TypedValue.COMPLEX_UNIT_SP, 15f)
            setPadding(0, 0, 0, pad / 2)
        }
        val body = TextView(this).apply {
            text = dump
            setTextIsSelectable(true)
            setTextSize(TypedValue.COMPLEX_UNIT_SP, 12f)
            typeface = android.graphics.Typeface.MONOSPACE
        }
        val scroll = ScrollView(this).apply {
            addView(body, ViewGroup.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.WRAP_CONTENT))
        }
        val retryBtn = Button(this).apply {
            text = "Zkusit znovu"
            setOnClickListener { retry() }
        }
        return LinearLayout(this).apply {
            orientation = LinearLayout.VERTICAL
            setPadding(pad, pad, pad, pad)
            addView(title, LinearLayout.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.WRAP_CONTENT))
            addView(hint, LinearLayout.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.WRAP_CONTENT))
            addView(scroll, LinearLayout.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT, 0, 1f))
            addView(retryBtn, LinearLayout.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.WRAP_CONTENT))
        }
    }
}

