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
import org.maturita.maturita.ui.HwYearsScreen
import org.maturita.maturita.ui.LessonListScreen
import org.maturita.maturita.ui.FyzQuizScreen
import org.maturita.maturita.ui.FyzYearsScreen
import org.maturita.maturita.ui.LitYearsScreen
import org.maturita.maturita.ui.LessonRow
import org.maturita.maturita.ui.LitQuizScreen
import org.maturita.maturita.ui.SciMapScreen
import org.maturita.maturita.ui.SciQuizScreen
import org.maturita.maturita.ui.MluvExercise
import org.maturita.maturita.ui.NetQuizScreen
import org.maturita.maturita.ui.NetYearsScreen
import org.maturita.maturita.ui.On2QuizScreen
import org.maturita.maturita.ui.On3QuizScreen
import org.maturita.maturita.ui.On4QuizScreen
import org.maturita.maturita.ui.OnQuizScreen
import org.maturita.maturita.ui.OnYearsScreen
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
            Route.HwYears -> HwYearsScreen(vm)
            Route.HwMap -> LessonListScreen(
                vm, vm.tr("hw_year1"), vm.tr("hw_sub"),
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
            Route.OnYears -> OnYearsScreen(vm)
            Route.OnMap -> LessonListScreen(
                vm, vm.tr("on_year1"), vm.tr("on_sub"),
                vm.content.on.map { l ->
                    val id = l.int("id")
                    LessonRow(vm.tr(l.str("titleKey")), vm.progress.onDone(id), Route.OnLesson(id))
                },
            )
            is Route.OnLesson -> {
                val l = vm.content.onLesson(r.id)
                if (l != null) SlidesScreen(vm, vm.tr(l.str("titleKey")), vm.tr(l.str("subKey")), l.arr("slides")) {
                    vm.replace(Route.OnEx(r.id))
                }
            }
            is Route.OnEx -> OnQuizScreen(vm, r.id)
            Route.On2Map -> LessonListScreen(
                vm, vm.tr("on_year2"), vm.tr("on2_sub"),
                vm.content.on2.map { l ->
                    val id = l.int("id")
                    LessonRow(vm.tr(l.str("titleKey")), vm.progress.on2Done(id), Route.On2Lesson(id))
                },
            )
            is Route.On2Lesson -> {
                val l = vm.content.on2Lesson(r.id)
                if (l != null) SlidesScreen(vm, vm.tr(l.str("titleKey")), vm.tr(l.str("subKey")), l.arr("slides")) {
                    vm.replace(Route.On2Ex(r.id))
                }
            }
            is Route.On2Ex -> On2QuizScreen(vm, r.id)
            Route.On3Map -> LessonListScreen(
                vm, vm.tr("on_year3"), vm.tr("on3_sub"),
                vm.content.on3.map { l ->
                    val id = l.int("id")
                    LessonRow(vm.tr(l.str("titleKey")), vm.progress.on3Done(id), Route.On3Lesson(id))
                },
            )
            is Route.On3Lesson -> {
                val l = vm.content.on3Lesson(r.id)
                if (l != null) SlidesScreen(vm, vm.tr(l.str("titleKey")), vm.tr(l.str("subKey")), l.arr("slides")) {
                    vm.replace(Route.On3Ex(r.id))
                }
            }
            is Route.On3Ex -> On3QuizScreen(vm, r.id)
            Route.On4Map -> LessonListScreen(
                vm, vm.tr("on_year4"), vm.tr("on4_sub"),
                vm.content.on4.map { l ->
                    val id = l.int("id")
                    LessonRow(vm.tr(l.str("titleKey")), vm.progress.on4Done(id), Route.On4Lesson(id))
                },
            )
            is Route.On4Lesson -> {
                val l = vm.content.on4Lesson(r.id)
                if (l != null) SlidesScreen(vm, vm.tr(l.str("titleKey")), vm.tr(l.str("subKey")), l.arr("slides")) {
                    vm.replace(Route.On4Ex(r.id))
                }
            }
            is Route.On4Ex -> On4QuizScreen(vm, r.id)
            Route.LitYears -> LitYearsScreen(vm)
            Route.LitMap -> LessonListScreen(
                vm, vm.tr("lit_year1"), vm.tr("lit_sub"),
                vm.content.lit.map { l ->
                    val id = l.int("id")
                    LessonRow(vm.tr(l.str("titleKey")), vm.progress.litDone(id), Route.LitLesson(id))
                },
            )
            is Route.LitLesson -> {
                val l = vm.content.litLesson(r.id)
                if (l != null) SlidesScreen(vm, vm.tr(l.str("titleKey")), vm.tr(l.str("subKey")), l.arr("slides")) {
                    vm.replace(Route.LitEx(r.id))
                }
            }
            is Route.LitEx -> LitQuizScreen(vm, 1, r.id)
            Route.Lit2Map -> LessonListScreen(
                vm, vm.tr("lit_year2"), vm.tr("lit2_sub"),
                vm.content.lit2.map { l ->
                    val id = l.int("id")
                    LessonRow(vm.tr(l.str("titleKey")), vm.progress.lit2Done(id), Route.Lit2Lesson(id))
                },
            )
            is Route.Lit2Lesson -> {
                val l = vm.content.lit2Lesson(r.id)
                if (l != null) SlidesScreen(vm, vm.tr(l.str("titleKey")), vm.tr(l.str("subKey")), l.arr("slides")) {
                    vm.replace(Route.Lit2Ex(r.id))
                }
            }
            is Route.Lit2Ex -> LitQuizScreen(vm, 2, r.id)
            Route.Lit3Map -> LessonListScreen(
                vm, vm.tr("lit_year3"), vm.tr("lit3_sub"),
                vm.content.lit3.map { l ->
                    val id = l.int("id")
                    LessonRow(vm.tr(l.str("titleKey")), vm.progress.lit3Done(id), Route.Lit3Lesson(id))
                },
            )
            is Route.Lit3Lesson -> {
                val l = vm.content.lit3Lesson(r.id)
                if (l != null) SlidesScreen(vm, vm.tr(l.str("titleKey")), vm.tr(l.str("subKey")), l.arr("slides")) {
                    vm.replace(Route.Lit3Ex(r.id))
                }
            }
            is Route.Lit3Ex -> LitQuizScreen(vm, 3, r.id)
            Route.Lit4Map -> LessonListScreen(
                vm, vm.tr("lit_year4"), vm.tr("lit4_sub"),
                vm.content.lit4.map { l ->
                    val id = l.int("id")
                    LessonRow(vm.tr(l.str("titleKey")), vm.progress.lit4Done(id), Route.Lit4Lesson(id))
                },
            )
            is Route.Lit4Lesson -> {
                val l = vm.content.lit4Lesson(r.id)
                if (l != null) SlidesScreen(vm, vm.tr(l.str("titleKey")), vm.tr(l.str("subKey")), l.arr("slides")) {
                    vm.replace(Route.Lit4Ex(r.id))
                }
            }
            is Route.Lit4Ex -> LitQuizScreen(vm, 4, r.id)
            Route.FyzYears -> FyzYearsScreen(vm)
            Route.FyzMap -> LessonListScreen(
                vm, vm.tr("fyz_year1"), vm.tr("fyz_sub"),
                vm.content.fyz.map { l ->
                    val id = l.int("id")
                    LessonRow(vm.tr(l.str("titleKey")), vm.progress.fyzDone(id), Route.FyzLesson(id))
                },
            )
            is Route.FyzLesson -> {
                val l = vm.content.fyzLesson(r.id)
                if (l != null) SlidesScreen(vm, vm.tr(l.str("titleKey")), vm.tr(l.str("subKey")), l.arr("slides")) {
                    vm.replace(Route.FyzEx(r.id))
                }
            }
            is Route.FyzEx -> FyzQuizScreen(vm, 1, r.id)
            Route.Fyz2Map -> LessonListScreen(
                vm, vm.tr("fyz_year2"), vm.tr("fyz2_sub"),
                vm.content.fyz2.map { l ->
                    val id = l.int("id")
                    LessonRow(vm.tr(l.str("titleKey")), vm.progress.fyz2Done(id), Route.Fyz2Lesson(id))
                },
            )
            is Route.Fyz2Lesson -> {
                val l = vm.content.fyz2Lesson(r.id)
                if (l != null) SlidesScreen(vm, vm.tr(l.str("titleKey")), vm.tr(l.str("subKey")), l.arr("slides")) {
                    vm.replace(Route.Fyz2Ex(r.id))
                }
            }
            is Route.Fyz2Ex -> FyzQuizScreen(vm, 2, r.id)
            Route.Fyz3Map -> LessonListScreen(
                vm, vm.tr("fyz_year3"), vm.tr("fyz3_sub"),
                vm.content.fyz3.map { l ->
                    val id = l.int("id")
                    LessonRow(vm.tr(l.str("titleKey")), vm.progress.fyz3Done(id), Route.Fyz3Lesson(id))
                },
            )
            is Route.Fyz3Lesson -> {
                val l = vm.content.fyz3Lesson(r.id)
                if (l != null) SlidesScreen(vm, vm.tr(l.str("titleKey")), vm.tr(l.str("subKey")), l.arr("slides")) {
                    vm.replace(Route.Fyz3Ex(r.id))
                }
            }
            is Route.Fyz3Ex -> FyzQuizScreen(vm, 3, r.id)
            Route.Fyz4Map -> LessonListScreen(
                vm, vm.tr("fyz_year4"), vm.tr("fyz4_sub"),
                vm.content.fyz4.map { l ->
                    val id = l.int("id")
                    LessonRow(vm.tr(l.str("titleKey")), vm.progress.fyz4Done(id), Route.Fyz4Lesson(id))
                },
            )
            is Route.Fyz4Lesson -> {
                val l = vm.content.fyz4Lesson(r.id)
                if (l != null) SlidesScreen(vm, vm.tr(l.str("titleKey")), vm.tr(l.str("subKey")), l.arr("slides")) {
                    vm.replace(Route.Fyz4Ex(r.id))
                }
            }
            is Route.Fyz4Ex -> FyzQuizScreen(vm, 4, r.id)
            Route.SciMap -> SciMapScreen(vm)
            Route.ChemMap -> LessonListScreen(
                vm, vm.tr("Chemie"), vm.tr("chem_sub"),
                vm.content.chem.map { l ->
                    val id = l.int("id")
                    LessonRow(vm.tr(l.str("titleKey")), vm.progress.chemDone(id), Route.ChemLesson(id))
                },
            )
            is Route.ChemLesson -> {
                val l = vm.content.chemLesson(r.id)
                if (l != null) SlidesScreen(vm, vm.tr(l.str("titleKey")), vm.tr(l.str("subKey")), l.arr("slides")) {
                    vm.replace(Route.ChemEx(r.id))
                }
            }
            is Route.ChemEx -> SciQuizScreen(vm, "chem", r.id)
            Route.BioMap -> LessonListScreen(
                vm, vm.tr("Biologie"), vm.tr("bio_sub"),
                vm.content.bio.map { l ->
                    val id = l.int("id")
                    LessonRow(vm.tr(l.str("titleKey")), vm.progress.bioDone(id), Route.BioLesson(id))
                },
            )
            is Route.BioLesson -> {
                val l = vm.content.bioLesson(r.id)
                if (l != null) SlidesScreen(vm, vm.tr(l.str("titleKey")), vm.tr(l.str("subKey")), l.arr("slides")) {
                    vm.replace(Route.BioEx(r.id))
                }
            }
            is Route.BioEx -> SciQuizScreen(vm, "bio", r.id)
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
