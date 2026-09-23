package org.maturita.maturita

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
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.Surface
import androidx.compose.material3.darkColorScheme
import androidx.compose.material3.lightColorScheme
import androidx.compose.runtime.Composable
import androidx.compose.runtime.LaunchedEffect
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.Color
import org.maturita.maturita.data.ColorMode
import org.maturita.maturita.data.Palette
import org.maturita.maturita.ui.BookListScreen
import org.maturita.maturita.ui.BookScreen
import org.maturita.maturita.ui.CzechMapScreen
import org.maturita.maturita.ui.GermanExercise
import org.maturita.maturita.ui.HomeScreen
import org.maturita.maturita.ui.HwQuizScreen
import org.maturita.maturita.ui.LessonListScreen
import org.maturita.maturita.ui.LessonRow
import org.maturita.maturita.ui.LitQuizScreen
import org.maturita.maturita.ui.MluvExercise
import org.maturita.maturita.ui.NetQuizScreen
import org.maturita.maturita.ui.NetYearsScreen
import org.maturita.maturita.ui.PlotScreen
import org.maturita.maturita.ui.RoadmapScreen
import org.maturita.maturita.ui.SearchScreen
import org.maturita.maturita.ui.SettingsScreen
import org.maturita.maturita.ui.SlidesScreen
import org.maturita.maturita.ui.StatsScreen
import org.maturita.maturita.ui.UnitMapScreen
import org.maturita.maturita.ui.VocabExercise
import org.maturita.maturita.ui.appTypography

class MainActivity : ComponentActivity() {
    private val vm: AppViewModel by viewModels()

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
        setContent { MaturitaApp(vm) }
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

/** Material 3 colour roles from the shared theme palette. */
private fun schemeFor(p: Palette, dark: Boolean) = if (dark) {
    darkColorScheme(
        primary = p.tint,
        onPrimary = p.onTint,
        primaryContainer = p.accent,
        onPrimaryContainer = p.onAccent,
        secondary = p.accent2,
        onSecondary = p.onAccent,
        secondaryContainer = p.surface1,
        onSecondaryContainer = p.text,
        tertiary = p.warning,
        onTertiary = p.crust,
        background = p.base,
        onBackground = p.text,
        surface = p.base,
        onSurface = p.text,
        surfaceVariant = p.surface0,
        onSurfaceVariant = p.subtext,
        surfaceContainerLowest = p.crust,
        surfaceContainerLow = p.mantle,
        surfaceContainer = p.mantle,
        surfaceContainerHigh = p.surface0,
        surfaceContainerHighest = p.surface1,
        outline = p.overlay,
        outlineVariant = p.surface1,
        error = p.error,
        onError = Color.White,
        errorContainer = p.error.copy(alpha = 0.22f),
        onErrorContainer = p.text,
    )
} else {
    lightColorScheme(
        primary = p.tint,
        onPrimary = p.onTint,
        primaryContainer = p.accent,
        onPrimaryContainer = p.onAccent,
        secondary = p.accent2,
        onSecondary = Color.White,
        secondaryContainer = p.surface1,
        onSecondaryContainer = p.text,
        tertiary = p.warning,
        onTertiary = Color.White,
        background = p.base,
        onBackground = p.text,
        surface = p.base,
        onSurface = p.text,
        surfaceVariant = p.surface1,
        onSurfaceVariant = p.subtext,
        surfaceContainerLowest = p.mantle,
        surfaceContainerLow = p.mantle,
        surfaceContainer = p.mantle,
        surfaceContainerHigh = p.surface1,
        surfaceContainerHighest = p.surface2,
        outline = p.overlay,
        outlineVariant = p.surface2,
        error = p.error,
        onError = Color.White,
        errorContainer = p.error.copy(alpha = 0.14f),
        onErrorContainer = p.text,
    )
}

@Composable
fun MaturitaApp(vm: AppViewModel) {
    MaterialTheme(colorScheme = schemeFor(vm.palette, vm.mode == ColorMode.Dark), typography = appTypography()) {
        Surface(Modifier.fillMaxSize(), color = MaterialTheme.colorScheme.background) {
            MaturitaAppBody(vm)
        }
    }
}

@Composable
private fun MaturitaAppBody(vm: AppViewModel) {
    LaunchedEffect(Unit) { vm.checkUpdate(false) }
    BackHandler(vm.canGoBack || vm.searchOpen) {
        if (vm.searchOpen) vm.searchOpen = false else vm.back()
    }
    Box(Modifier.fillMaxSize()) {
        when (val r = vm.route) {
            Route.Welcome, Route.Subjects -> HomeScreen(vm)
            Route.Stats -> StatsScreen(vm)
            Route.Settings -> SettingsScreen(vm)
            Route.Roadmap -> RoadmapScreen(vm)
            is Route.UnitMap -> UnitMapScreen(vm, r.unitId)
            is Route.GermanEx -> GermanExercise(vm, r.unitId, r.ex)
            is Route.Vocab -> VocabExercise(vm, r.unitId)
            Route.NetYears -> NetYearsScreen(vm)
            Route.NetMap -> LessonListScreen(
                vm, vm.tr("net_year1"), vm.tr("net_sub"),
                vm.content.netLessons.map { l ->
                    val id = l.int("id")
                    LessonRow(vm.tr(l.str("titleKey")), vm.progress.netDone(id), Route.NetLesson(id))
                },
            )
            is Route.NetLesson -> {
                val l = vm.content.netLesson(r.id)
                if (l != null) SlidesScreen(vm, vm.tr(l.str("titleKey")), vm.tr(l.str("subKey")), l.arr("slides")) {
                    vm.replace(Route.NetEx(r.id))
                }
            }
            is Route.NetEx -> NetQuizScreen(vm, r.id)
            Route.HwMap -> LessonListScreen(
                vm, vm.tr("Technické vybavení"), vm.tr("hw_sub"),
                vm.content.hw.map { l ->
                    val id = l.int("id")
                    LessonRow(vm.tr(l.str("titleKey")), vm.progress.hwDone(id), Route.HwLesson(id))
                },
            )
            is Route.HwLesson -> {
                val l = vm.content.hwLesson(r.id)
                if (l != null) SlidesScreen(vm, vm.tr(l.str("titleKey")), vm.tr(l.str("subKey")), l.arr("slides")) {
                    vm.replace(Route.HwEx(r.id))
                }
            }
            is Route.HwEx -> HwQuizScreen(vm, r.id)
            Route.CzechMap -> CzechMapScreen(vm)
            Route.Mluvnice -> LessonListScreen(
                vm, vm.tr("Mluvnice"), vm.tr("mluv_sub"),
                vm.content.mluvnice.map { m ->
                    val n = m.int("id")
                    LessonRow(m.str("name"), vm.progress.mluvDone(n), Route.MluvEx(n))
                },
            )
            is Route.MluvEx -> MluvExercise(vm, r.n)
            Route.ReadingList -> BookListScreen(vm)
            is Route.Book -> BookScreen(vm, r.id)
            is Route.BookQuiz -> LitQuizScreen(vm, r.id)
            is Route.BookPlot -> PlotScreen(vm, r.id)
        }
        if (vm.searchOpen) SearchScreen(vm)
    }
}
