package org.maturita.maturita

import android.os.Bundle
import androidx.activity.ComponentActivity
import androidx.activity.compose.BackHandler
import androidx.activity.compose.setContent
import androidx.activity.enableEdgeToEdge
import androidx.activity.viewModels
import androidx.compose.foundation.background
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.navigationBarsPadding
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.statusBarsPadding
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.material3.ExperimentalMaterial3Api
import androidx.compose.material3.ModalBottomSheet
import androidx.compose.material3.Text
import androidx.compose.material3.rememberModalBottomSheetState
import androidx.compose.runtime.Composable
import androidx.compose.runtime.LaunchedEffect
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.draw.clip
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import org.maturita.maturita.ui.BookListScreen
import org.maturita.maturita.ui.BookScreen
import org.maturita.maturita.ui.CzechMapScreen
import org.maturita.maturita.ui.GermanExercise
import org.maturita.maturita.ui.HwQuizScreen
import org.maturita.maturita.ui.IconHit
import org.maturita.maturita.ui.LessonMapScreen
import org.maturita.maturita.ui.LitQuizScreen
import org.maturita.maturita.ui.MapNode
import org.maturita.maturita.ui.MluvExercise
import org.maturita.maturita.ui.NetQuizScreen
import org.maturita.maturita.ui.NetYearsScreen
import org.maturita.maturita.ui.PlotScreen
import org.maturita.maturita.ui.RoadmapScreen
import org.maturita.maturita.ui.SearchIcon
import org.maturita.maturita.ui.SearchOverlay
import org.maturita.maturita.ui.SettingsIcon
import org.maturita.maturita.ui.SettingsSheet
import org.maturita.maturita.ui.SlidesScreen
import org.maturita.maturita.ui.StatsIcon
import org.maturita.maturita.ui.StatsScreen
import org.maturita.maturita.ui.SubjectsScreen
import org.maturita.maturita.ui.UnitMapScreen
import org.maturita.maturita.ui.VocabExercise
import org.maturita.maturita.ui.WelcomeScreen

class MainActivity : ComponentActivity() {
    private val vm: AppViewModel by viewModels()

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        enableEdgeToEdge()
        setContent { MaturitaApp(vm) }
    }
}

@OptIn(ExperimentalMaterial3Api::class)
@Composable
fun MaturitaApp(vm: AppViewModel) {
    val p = vm.palette
    LaunchedEffect(Unit) { vm.checkUpdate(false) }
    BackHandler(vm.canGoBack || vm.searchOpen || vm.settingsOpen) {
        when {
            vm.searchOpen -> vm.searchOpen = false
            vm.settingsOpen -> vm.settingsOpen = false
            else -> vm.back()
        }
    }
    Box(
        Modifier
            .fillMaxSize()
            .background(p.base)
            .statusBarsPadding()
            .navigationBarsPadding(),
    ) {
        Column(Modifier.fillMaxSize()) {
            Row(
                Modifier
                    .fillMaxWidth()
                    .padding(horizontal = 12.dp, vertical = 6.dp),
                verticalAlignment = Alignment.CenterVertically,
            ) {
                Text("maturita.c", color = p.text, fontWeight = FontWeight.Bold, fontSize = 15.sp, modifier = Modifier.weight(1f))
                IconHit({ vm.searchOpen = true }) { SearchIcon(p.text) }
                IconHit({ vm.go(Route.Stats) }) { StatsIcon(p.text) }
                IconHit({ vm.settingsOpen = true }) { SettingsIcon(p.text) }
            }
            if (vm.update.canInstall || vm.update.status == "downloading" || vm.update.status == "staged") {
                Row(
                    Modifier
                        .fillMaxWidth()
                        .padding(horizontal = 12.dp)
                        .clip(RoundedCornerShape(14.dp))
                        .background(p.surface1)
                        .padding(horizontal = 12.dp, vertical = 10.dp),
                    verticalAlignment = Alignment.CenterVertically,
                ) {
                    Text(
                        vm.fmt(vm.update.messageKey, *(listOfNotNull(vm.update.messageArg).toTypedArray())),
                        color = p.text,
                        fontSize = 13.sp,
                        modifier = Modifier.weight(1f),
                    )
                    if (vm.update.canInstall) {
                        org.maturita.maturita.ui.PillButton(vm.tr("update_install"), p) { vm.installUpdate() }
                    }
                }
            }
            Box(Modifier.weight(1f)) {
                when (val r = vm.route) {
                    Route.Welcome -> WelcomeScreen(vm)
                    Route.Subjects -> SubjectsScreen(vm)
                    Route.Stats -> StatsScreen(vm)
                    Route.Roadmap -> RoadmapScreen(vm)
                    is Route.UnitMap -> UnitMapScreen(vm, r.unitId)
                    is Route.GermanEx -> GermanExercise(vm, r.unitId, r.ex)
                    is Route.Vocab -> VocabExercise(vm, r.unitId)
                    Route.NetYears -> NetYearsScreen(vm)
                    Route.NetMap -> LessonMapScreen(
                        vm, vm.tr("net_year1"), vm.tr("net_sub"),
                        vm.content.netLessons.map { l ->
                            val id = l.int("id")
                            MapNode(
                                "n$id", vm.tr(l.str("titleKey")), false,
                                vm.progress.netDone(id), !vm.progress.netDone(id),
                            ) { vm.go(Route.NetLesson(id)) }
                        },
                        vm.content.netLessons.count { vm.progress.netDone(it.int("id")) }.toFloat(),
                    )
                    is Route.NetLesson -> {
                        val l = vm.content.netLesson(r.id)
                        if (l != null) SlidesScreen(vm, vm.tr(l.str("titleKey")), vm.tr(l.str("subKey")), l.arr("slides")) {
                            vm.go(Route.NetEx(r.id))
                        }
                    }
                    is Route.NetEx -> NetQuizScreen(vm, r.id)
                    Route.HwMap -> LessonMapScreen(
                        vm, vm.tr("Technické vybavení"), vm.tr("hw_sub"),
                        vm.content.hw.map { l ->
                            val id = l.int("id")
                            MapNode(
                                "h$id", vm.tr(l.str("titleKey")), false,
                                vm.progress.hwDone(id), !vm.progress.hwDone(id),
                            ) { vm.go(Route.HwLesson(id)) }
                        },
                        vm.content.hw.count { vm.progress.hwDone(it.int("id")) }.toFloat(),
                    )
                    is Route.HwLesson -> {
                        val l = vm.content.hwLesson(r.id)
                        if (l != null) SlidesScreen(vm, vm.tr(l.str("titleKey")), vm.tr(l.str("subKey")), l.arr("slides")) {
                            vm.go(Route.HwEx(r.id))
                        }
                    }
                    is Route.HwEx -> HwQuizScreen(vm, r.id)
                    Route.CzechMap -> CzechMapScreen(vm)
                    Route.Mluvnice -> LessonMapScreen(
                        vm, vm.tr("Mluvnice"), "Cvičení z mluvnice – styl maturita / přijímačky z ČJL",
                        vm.content.mluvnice.map { m ->
                            val n = m.int("id")
                            MapNode(
                                "m$n", m.str("name"), false,
                                vm.progress.mluvDone(n), !vm.progress.mluvDone(n),
                            ) { vm.go(Route.MluvEx(n)) }
                        },
                        vm.content.mluvnice.count { vm.progress.mluvDone(it.int("id")) }.toFloat(),
                    )
                    is Route.MluvEx -> MluvExercise(vm, r.n)
                    Route.ReadingList -> BookListScreen(vm)
                    is Route.Book -> BookScreen(vm, r.id)
                    is Route.BookQuiz -> LitQuizScreen(vm, r.id)
                    is Route.BookPlot -> PlotScreen(vm, r.id)
                }
            }
        }
        if (vm.searchOpen) SearchOverlay(vm)
        if (vm.settingsOpen) {
            ModalBottomSheet(
                onDismissRequest = { vm.settingsOpen = false },
                sheetState = rememberModalBottomSheetState(skipPartiallyExpanded = true),
                containerColor = p.base,
                shape = RoundedCornerShape(topStart = 20.dp, topEnd = 20.dp),
            ) { SettingsSheet(vm) }
        }
    }
}
