package org.maturita.maturita.ui

import androidx.compose.foundation.ExperimentalFoundationApi
import androidx.compose.foundation.background
import androidx.compose.foundation.clickable
import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.PaddingValues
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.size
import androidx.compose.foundation.layout.width
import androidx.compose.foundation.lazy.LazyColumn
import androidx.compose.foundation.lazy.items
import androidx.compose.foundation.pager.HorizontalPager
import androidx.compose.foundation.pager.rememberPagerState
import androidx.compose.foundation.rememberScrollState
import androidx.compose.foundation.shape.CircleShape
import androidx.compose.foundation.verticalScroll
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.automirrored.filled.ArrowBack
import androidx.compose.material.icons.automirrored.filled.MenuBook
import androidx.compose.material.icons.filled.Architecture
import androidx.compose.material.icons.filled.AutoAwesome
import androidx.compose.material.icons.filled.AutoStories
import androidx.compose.material.icons.filled.Bolt
import androidx.compose.material.icons.filled.Brush
import androidx.compose.material.icons.filled.Eco
import androidx.compose.material.icons.filled.Functions
import androidx.compose.material.icons.filled.Check
import androidx.compose.material.icons.filled.Close
import androidx.compose.material.icons.filled.FormatListNumbered
import androidx.compose.material.icons.automirrored.filled.KeyboardArrowLeft
import androidx.compose.material.icons.filled.Groups
import androidx.compose.material.icons.filled.Lock
import androidx.compose.material.icons.filled.Quiz
import androidx.compose.material.icons.filled.Science
import androidx.compose.material.icons.filled.Terminal
import androidx.compose.material.icons.filled.Search
import androidx.compose.material.icons.outlined.Lightbulb
import androidx.compose.material3.Card
import androidx.compose.material3.CardDefaults
import androidx.compose.material3.ExperimentalMaterial3Api
import androidx.compose.material3.FilledTonalButton
import androidx.compose.material3.HorizontalDivider
import androidx.compose.material3.Icon
import androidx.compose.material3.IconButton
import androidx.compose.material3.ListItem
import androidx.compose.material3.ListItemDefaults
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.OutlinedButton
import androidx.compose.material3.RadioButton
import androidx.compose.material3.Scaffold
import androidx.compose.material3.TextField
import androidx.compose.material3.TextFieldDefaults
import androidx.compose.material3.SegmentedButton
import androidx.compose.material3.SegmentedButtonDefaults
import androidx.compose.material3.SingleChoiceSegmentedButtonRow
import androidx.compose.material3.Surface
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.runtime.LaunchedEffect
import androidx.compose.runtime.remember
import androidx.compose.runtime.rememberCoroutineScope
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.draw.clip
import androidx.compose.ui.focus.FocusRequester
import androidx.compose.ui.focus.focusRequester
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.text.input.ImeAction
import androidx.compose.foundation.layout.statusBarsPadding
import androidx.compose.foundation.text.KeyboardOptions
import androidx.compose.ui.text.style.TextOverflow
import androidx.compose.ui.unit.dp
import kotlinx.coroutines.launch
import org.maturita.maturita.AppViewModel
import org.maturita.maturita.Route
import org.maturita.maturita.data.ColorMode
import org.maturita.maturita.data.J
import org.maturita.maturita.data.ProgressSum
import org.maturita.maturita.data.ThemeId
import org.maturita.maturita.data.UiLang
import org.maturita.maturita.data.normalizeAnswer
import org.maturita.maturita.data.summarize
import org.maturita.maturita.data.themeNames
import org.maturita.maturita.data.themePalette

// ---------------------------------------------------------------------------
// Roots

@Composable
fun HomeScreen(vm: AppViewModel) {
    val sum = summarize(vm.content, vm.progress)
    val pct = if (sum.totalEx == 0) 0f else sum.doneEx / sum.totalEx.toFloat()
    RootScaffold(vm, "Graduately") {
        if (vm.showChangelog) {
            item { ChangelogCard(vm) }
        }
        item {
            Card(
                modifier = Modifier.fillMaxWidth().padding(horizontal = 16.dp, vertical = 8.dp),
                colors = CardDefaults.cardColors(
                    containerColor = MaterialTheme.colorScheme.surfaceContainerHigh,
                    contentColor = MaterialTheme.colorScheme.onSurface,
                ),
            ) {
                Row(Modifier.padding(20.dp), verticalAlignment = Alignment.CenterVertically) {
                    Column(Modifier.weight(1f)) {
                        Text(vm.tr("welcome_title"), style = MaterialTheme.typography.headlineMedium)
                        Spacer(Modifier.height(8.dp))
                        Text(
                            vm.tr(vm.platform.welcomeKey),
                            style = MaterialTheme.typography.bodyMedium,
                            color = MaterialTheme.colorScheme.onSurfaceVariant,
                        )
                    }
                    Spacer(Modifier.width(16.dp))
                    ProgressRing(pct, Modifier.size(104.dp)) {
                        Column(horizontalAlignment = Alignment.CenterHorizontally) {
                            Text(vm.fmt("stats_pct_fmt", (pct * 100).toInt()), style = MaterialTheme.typography.titleLarge)
                            Text(
                                vm.fmt("stats_ex_fmt", sum.doneEx, sum.totalEx),
                                style = MaterialTheme.typography.labelSmall,
                                color = MaterialTheme.colorScheme.onSurfaceVariant,
                            )
                        }
                    }
                }
            }
        }
        if (vm.update.canInstall || vm.update.status == "downloading" || vm.update.status == "staged") {
            item { UpdateBanner(vm) }
        }
        val open = vm.content.subjects.filter { it.bool("open") }
        val soon = vm.content.subjects.filter { !it.bool("open") }
        item { SectionHeader(vm.tr("subjects_title")) }
        items(open) { s ->
            val part = subjectSum(vm, s)
            NavRow(
                title = vm.tr(s.str("key")),
                leading = { SubjectIcon(s.str("icon"), true) },
                progress = part.doneEx to part.totalEx,
                onClick = { vm.goPage(s.str("target")) },
            )
        }
        if (soon.isNotEmpty()) {
            item { SectionHeader(vm.tr("subjects_soon")) }
            items(soon) { s ->
                NavRow(
                    title = vm.tr(s.str("key")),
                    locked = true,
                    leading = { SubjectIcon(s.str("icon"), false) },
                    onClick = null,
                )
            }
        }
    }
}

@Composable
private fun ChangelogCard(vm: AppViewModel) {
    val langKey = if (vm.lang == UiLang.En) "en" else "cs"
    val blocks = vm.content.changelog.take(3)
    Card(
        modifier = Modifier.fillMaxWidth().padding(horizontal = 16.dp, vertical = 8.dp),
        colors = CardDefaults.cardColors(
            containerColor = MaterialTheme.colorScheme.surfaceContainerHigh,
            contentColor = MaterialTheme.colorScheme.onSurface,
        ),
    ) {
        Column(Modifier.padding(20.dp)) {
            Row(verticalAlignment = Alignment.CenterVertically) {
                Icon(Icons.Filled.AutoAwesome, null, tint = MaterialTheme.colorScheme.primary, modifier = Modifier.size(20.dp))
                Spacer(Modifier.width(8.dp))
                Text(
                    vm.tr("changelog_title"),
                    style = MaterialTheme.typography.titleMedium,
                    modifier = Modifier.weight(1f),
                )
                val date = blocks.firstOrNull()?.str("date").orEmpty()
                if (date.isNotEmpty()) {
                    Text(
                        date,
                        style = MaterialTheme.typography.labelMedium,
                        color = MaterialTheme.colorScheme.onSurfaceVariant,
                    )
                }
            }
            Spacer(Modifier.height(10.dp))
            blocks.forEachIndexed { index, block ->
                if (index > 0) Spacer(Modifier.height(8.dp))
                block.strs(langKey).ifEmpty { block.strs("cs") }.forEach { line ->
                    Text(
                        "•  $line",
                        style = MaterialTheme.typography.bodyMedium,
                        modifier = Modifier.padding(bottom = 4.dp),
                    )
                }
            }
            Spacer(Modifier.height(8.dp))
            FilledTonalButton(onClick = { vm.dismissChangelog() }) {
                Text(vm.tr("changelog_dismiss"))
            }
        }
    }
}

@Composable
private fun UpdateBanner(vm: AppViewModel) {
    Card(
        modifier = Modifier.fillMaxWidth().padding(horizontal = 16.dp, vertical = 4.dp),
        colors = CardDefaults.cardColors(containerColor = MaterialTheme.colorScheme.secondaryContainer),
    ) {
        Row(Modifier.padding(horizontal = 16.dp, vertical = 12.dp), verticalAlignment = Alignment.CenterVertically) {
            Text(
                vm.fmt(vm.update.messageKey, *(listOfNotNull(vm.update.messageArg).toTypedArray())),
                style = MaterialTheme.typography.bodyMedium,
                modifier = Modifier.weight(1f),
            )
            if (vm.update.canInstall) {
                Spacer(Modifier.width(8.dp))
                FilledTonalButton(onClick = { vm.installUpdate() }) { Text(vm.tr("update_install")) }
            }
        }
    }
}

@Composable
private fun SubjectIcon(icon: String, open: Boolean) {
    val tint = if (open) MaterialTheme.colorScheme.onSurface else MaterialTheme.colorScheme.outline
    Box(Modifier.size(32.dp), contentAlignment = Alignment.Center) {
        when (icon) {
            "de" -> GermanFlag(28.dp)
            "wifi" -> WifiIcon(tint, 26.dp)
            "chip" -> ChipIcon(tint, 26.dp)
            "people" -> Icon(Icons.Filled.Groups, null, tint = tint, modifier = Modifier.size(26.dp))
            "cz" -> CzechFlag(28.dp)
            "uk" -> UkFlag(28.dp)
            "flask" -> Icon(Icons.Filled.Science, null, tint = tint, modifier = Modifier.size(26.dp))
            "bolt" -> Icon(Icons.Filled.Bolt, null, tint = tint, modifier = Modifier.size(26.dp))
            "function" -> Icon(Icons.Filled.Functions, null, tint = tint, modifier = Modifier.size(26.dp))
            "ruler" -> Icon(Icons.Filled.Architecture, null, tint = tint, modifier = Modifier.size(26.dp))
            "code" -> Icon(Icons.Filled.Terminal, null, tint = tint, modifier = Modifier.size(26.dp))
            "brush" -> Icon(Icons.Filled.Brush, null, tint = tint, modifier = Modifier.size(26.dp))
            else -> Icon(Icons.Filled.Lock, null, tint = tint)
        }
    }
}

@Composable
fun StatsScreen(vm: AppViewModel) {
    val sum = summarize(vm.content, vm.progress)
    val pct = if (sum.totalEx == 0) 0f else sum.doneEx / sum.totalEx.toFloat()
    RootScaffold(vm, vm.tr("stats_title")) {
        item {
            Card(modifier = Modifier.fillMaxWidth().padding(horizontal = 16.dp, vertical = 8.dp)) {
                Column(Modifier.padding(20.dp)) {
                    Row(Modifier.fillMaxWidth()) {
                        Metric(vm.tr("stats_ex_label"), vm.fmt("stats_ex_fmt", sum.doneEx, sum.totalEx), Modifier.weight(1f))
                        Metric(vm.tr("stats_pct_label"), vm.fmt("stats_pct_fmt", (pct * 100).toInt()), Modifier.weight(1f))
                        Metric(vm.tr("stats_units_label"), vm.fmt("stats_units_fmt", sum.doneUnits, sum.openUnits), Modifier.weight(1f))
                    }
                    Spacer(Modifier.height(16.dp))
                    SmoothProgress(progress = pct, modifier = Modifier.fillMaxWidth().height(6.dp).clip(CircleShape))
                }
            }
        }
        item { SectionHeader(vm.tr("stats_section")) }
        items(vm.content.subjects) { s ->
            val open = s.bool("open")
            val part = subjectSum(vm, s)
            ListItem(
                headlineContent = {
                    Text(vm.tr(s.str("key")), color = if (open) MaterialTheme.colorScheme.onSurface else MaterialTheme.colorScheme.onSurfaceVariant)
                },
                supportingContent = {
                    Column {
                        if (open && part.totalEx > 0) {
                            Spacer(Modifier.height(6.dp))
                            SmoothProgress(
                                progress = part.doneEx / part.totalEx.toFloat(),
                                modifier = Modifier.fillMaxWidth().height(4.dp).clip(CircleShape),
                            )
                        }
                    }
                },
                leadingContent = { SubjectIcon(s.str("icon"), open) },
                trailingContent = {
                    Text(
                        if (open) vm.fmt("stats_ex_fmt", part.doneEx, part.totalEx) else vm.tr("stats_locked"),
                        style = MaterialTheme.typography.labelMedium,
                        color = MaterialTheme.colorScheme.onSurfaceVariant,
                    )
                },
                colors = ListItemDefaults.colors(containerColor = Color.Transparent),
            )
        }
    }
}

@Composable
private fun Metric(label: String, value: String, modifier: Modifier) {
    Column(modifier) {
        Text(label, style = MaterialTheme.typography.labelMedium, color = MaterialTheme.colorScheme.onSurfaceVariant)
        Text(value, style = MaterialTheme.typography.titleLarge)
    }
}

/** Done and total exercises behind one row of a years screen, or null when there is no count. */
fun routeProgress(vm: AppViewModel, r: Route): Pair<Int, Int>? {
    val c = vm.content
    val p = vm.progress
    fun tally(lessons: List<J>, done: (Int) -> Boolean) = lessons.count { done(it.int("id")) } to lessons.size
    return when (r) {
        Route.NetMap -> tally(c.netLessons, p::netDone)
        Route.Net2Map -> tally(c.itLessons("net2")) { p.courseDone("net2", it) }
        Route.Net3Map -> tally(c.itLessons("net3")) { p.courseDone("net3", it) }
        Route.Net4Map -> tally(c.itLessons("net4")) { p.courseDone("net4", it) }
        Route.HwMap -> tally(c.hw, p::hwDone)
        Route.Hw2Map -> tally(c.itLessons("hw2")) { p.courseDone("hw2", it) }
        Route.Hw3Map -> tally(c.itLessons("hw3")) { p.courseDone("hw3", it) }
        Route.Hw4Map -> tally(c.itLessons("hw4")) { p.courseDone("hw4", it) }
        Route.OnMap -> tally(c.on, p::onDone)
        Route.On2Map -> tally(c.on2, p::on2Done)
        Route.On3Map -> tally(c.on3, p::on3Done)
        Route.On4Map -> tally(c.on4, p::on4Done)
        is Route.EnMap -> tally(c.enYear(r.year)) { p.enDone(r.year, it) }
        is Route.DeMap -> tally(c.deYear(r.year)) { p.deDone(r.year, it) }
        Route.Roadmap -> {
            var done = 0
            var total = 0
            c.german.filter { it.bool("unlocked") }.forEach { u ->
                val n = u.strs("names").size
                total += n
                done += (1..n).count { p.germanDone(u.int("id"), it) }
            }
            done to total
        }
        Route.LitYears -> listOf(
            tally(c.lit, p::litDone), tally(c.lit2, p::lit2Done), tally(c.lit3, p::lit3Done), tally(c.lit4, p::lit4Done),
        ).let { parts -> parts.sumOf { it.first } to parts.sumOf { it.second } }
        Route.LitMap -> tally(c.lit, p::litDone)
        Route.Lit2Map -> tally(c.lit2, p::lit2Done)
        Route.Lit3Map -> tally(c.lit3, p::lit3Done)
        Route.Lit4Map -> tally(c.lit4, p::lit4Done)
        Route.Mluvnice -> tally(c.mluvnice, p::mluvDone)
        Route.ChemMap -> tally(c.chem, p::chemDone)
        Route.BioMap -> tally(c.bio, p::bioDone)
        Route.Mat0Map -> tally(c.mat0, p::mat0Done)
        Route.MatMap -> tally(c.mat, p::matDone)
        Route.Mat2Map -> tally(c.mat2, p::mat2Done)
        Route.Mat3Map -> tally(c.mat3, p::mat3Done)
        Route.Mat4Map -> tally(c.mat4, p::mat4Done)
        Route.FyzMap -> tally(c.fyz, p::fyzDone)
        Route.Fyz2Map -> tally(c.fyz2, p::fyz2Done)
        Route.Fyz3Map -> tally(c.fyz3, p::fyz3Done)
        Route.Fyz4Map -> tally(c.fyz4, p::fyz4Done)
        Route.TgMap -> tally(c.tg, p::tgDone)
        Route.Tg2Map -> tally(c.tg2, p::tg2Done)
        Route.Tg3Map -> tally(c.tg3, p::tg3Done)
        Route.Tg4Map -> tally(c.tg4, p::tg4Done)
        Route.PgMap -> tally(c.pg, p::pgDone)
        Route.Pg2Map -> tally(c.pg2, p::pg2Done)
        Route.Pg3Map -> tally(c.pg3, p::pg3Done)
        Route.Pg4Map -> tally(c.pg4, p::pg4Done)
        Route.CsMap -> tally(c.cs, p::csDone)
        Route.Cs2Map -> tally(c.cs2, p::cs2Done)
        Route.Cs3Map -> tally(c.cs3, p::cs3Done)
        Route.Cs4Map -> tally(c.cs4, p::cs4Done)
        else -> null
    }
}

private fun subjectSum(vm: AppViewModel, s: J): ProgressSum {
    val sum = ProgressSum()
    when (s.str("target")) {
        "dehome" -> {
            vm.content.german.filter { it.bool("unlocked") }.forEach { u ->
                val n = u.strs("names").size
                sum.totalEx += n
                sum.doneEx += (1..n).count { vm.progress.germanDone(u.int("id"), it) }
            }
            sum.totalEx += (1..4).sumOf { vm.content.deYear(it).size }
            sum.doneEx += (1..4).sumOf { year ->
                vm.content.deYear(year).count { vm.progress.deDone(year, it.int("id")) }
            }
        }
        "netyears" -> {
            val extra = listOf("net2", "net3", "net4")
            sum.totalEx = vm.content.netLessons.size + extra.sumOf { vm.content.itLessons(it).size }
            sum.doneEx = vm.content.netLessons.count { vm.progress.netDone(it.int("id")) } +
                extra.sumOf { course -> vm.content.itLessons(course).count { vm.progress.courseDone(course, it.int("id")) } }
        }
        "hwyears", "hwmap" -> {
            val extra = listOf("hw2", "hw3", "hw4")
            sum.totalEx = vm.content.hw.size + extra.sumOf { vm.content.itLessons(it).size }
            sum.doneEx = vm.content.hw.count { vm.progress.hwDone(it.int("id")) } +
                extra.sumOf { course -> vm.content.itLessons(course).count { vm.progress.courseDone(course, it.int("id")) } }
        }
        "enyears", "en1map", "en2map", "en3map", "en4map" -> {
            sum.totalEx = (1..4).sumOf { vm.content.enYear(it).size }
            sum.doneEx = (1..4).sumOf { year ->
                vm.content.enYear(year).count { vm.progress.enDone(year, it.int("id")) }
            }
        }
        "onyears", "onmap", "on2map", "on3map", "on4map" -> {
            sum.totalEx = vm.content.on.size + vm.content.on2.size + vm.content.on3.size + vm.content.on4.size
            sum.doneEx = vm.content.on.count { vm.progress.onDone(it.int("id")) } +
                vm.content.on2.count { vm.progress.on2Done(it.int("id")) } +
                vm.content.on3.count { vm.progress.on3Done(it.int("id")) } +
                vm.content.on4.count { vm.progress.on4Done(it.int("id")) }
        }
        "scimap" -> {
            sum.totalEx = vm.content.chem.size + vm.content.bio.size
            sum.doneEx = vm.content.chem.count { vm.progress.chemDone(it.int("id")) } +
                vm.content.bio.count { vm.progress.bioDone(it.int("id")) }
        }
        "matyears", "mat0map", "matmap", "mat2map", "mat3map", "mat4map" -> {
            sum.totalEx = vm.content.mat0.size + vm.content.mat.size + vm.content.mat2.size + vm.content.mat3.size + vm.content.mat4.size
            sum.doneEx = vm.content.mat0.count { vm.progress.mat0Done(it.int("id")) } +
                vm.content.mat.count { vm.progress.matDone(it.int("id")) } +
                vm.content.mat2.count { vm.progress.mat2Done(it.int("id")) } +
                vm.content.mat3.count { vm.progress.mat3Done(it.int("id")) } +
                vm.content.mat4.count { vm.progress.mat4Done(it.int("id")) }
        }
        "fyzyears", "fyzmap", "fyz2map", "fyz3map", "fyz4map" -> {
            sum.totalEx = vm.content.fyz.size + vm.content.fyz2.size + vm.content.fyz3.size + vm.content.fyz4.size
            sum.doneEx = vm.content.fyz.count { vm.progress.fyzDone(it.int("id")) } +
                vm.content.fyz2.count { vm.progress.fyz2Done(it.int("id")) } +
                vm.content.fyz3.count { vm.progress.fyz3Done(it.int("id")) } +
                vm.content.fyz4.count { vm.progress.fyz4Done(it.int("id")) }
        }
        "csyears", "csmap", "cs2map", "cs3map", "cs4map" -> {
            sum.totalEx = vm.content.cs.size + vm.content.cs2.size + vm.content.cs3.size + vm.content.cs4.size
            sum.doneEx = vm.content.cs.count { vm.progress.csDone(it.int("id")) } +
                vm.content.cs2.count { vm.progress.cs2Done(it.int("id")) } +
                vm.content.cs3.count { vm.progress.cs3Done(it.int("id")) } +
                vm.content.cs4.count { vm.progress.cs4Done(it.int("id")) }
        }
        "tgyears", "tgmap", "tg2map", "tg3map", "tg4map" -> {
            sum.totalEx = vm.content.tg.size + vm.content.tg2.size + vm.content.tg3.size + vm.content.tg4.size
            sum.doneEx = vm.content.tg.count { vm.progress.tgDone(it.int("id")) } +
                vm.content.tg2.count { vm.progress.tg2Done(it.int("id")) } +
                vm.content.tg3.count { vm.progress.tg3Done(it.int("id")) } +
                vm.content.tg4.count { vm.progress.tg4Done(it.int("id")) }
        }
        "pgyears", "pgmap", "pg2map", "pg3map", "pg4map" -> {
            sum.totalEx = vm.content.pg.size + vm.content.pg2.size + vm.content.pg3.size + vm.content.pg4.size
            sum.doneEx = vm.content.pg.count { vm.progress.pgDone(it.int("id")) } +
                vm.content.pg2.count { vm.progress.pg2Done(it.int("id")) } +
                vm.content.pg3.count { vm.progress.pg3Done(it.int("id")) } +
                vm.content.pg4.count { vm.progress.pg4Done(it.int("id")) }
        }
        "czechmap" -> {
            val litDone = vm.content.lit.count { vm.progress.litDone(it.int("id")) } +
                vm.content.lit2.count { vm.progress.lit2Done(it.int("id")) } +
                vm.content.lit3.count { vm.progress.lit3Done(it.int("id")) } +
                vm.content.lit4.count { vm.progress.lit4Done(it.int("id")) }
            sum.totalEx = vm.content.mluvnice.size + vm.content.lit.size + vm.content.lit2.size +
                vm.content.lit3.size + vm.content.lit4.size
            sum.doneEx = vm.content.mluvnice.count { vm.progress.mluvDone(it.int("id")) } + litDone
        }
    }
    return sum
}

@OptIn(ExperimentalMaterial3Api::class)
@Composable
fun SettingsScreen(vm: AppViewModel) {
    RootScaffold(vm, vm.tr("settings_title"), showSearch = false) {
        item { SectionHeader(vm.tr("mode")) }
        item {
            SingleChoiceSegmentedButtonRow(Modifier.fillMaxWidth().padding(horizontal = 16.dp)) {
                SegmentedButton(
                    selected = vm.mode == ColorMode.Light,
                    onClick = { vm.applyMode(ColorMode.Light) },
                    shape = SegmentedButtonDefaults.itemShape(0, 2),
                ) { Text(vm.tr("mode_light")) }
                SegmentedButton(
                    selected = vm.mode == ColorMode.Dark,
                    onClick = { vm.applyMode(ColorMode.Dark) },
                    shape = SegmentedButtonDefaults.itemShape(1, 2),
                ) { Text(vm.tr("mode_dark")) }
            }
        }
        item { SectionHeader(vm.tr("theme")) }
        items(ThemeId.entries.size) { i ->
            val id = ThemeId.entries[i]
            val sw = themePalette(id, vm.mode)
            val sel = vm.themeId == id
            ListItem(
                headlineContent = { Text(themeNames[i]) },
                leadingContent = {
                    Box(Modifier.size(28.dp).clip(CircleShape).background(sw.base), contentAlignment = Alignment.Center) {
                        Box(Modifier.size(14.dp).clip(CircleShape).background(sw.tint))
                    }
                },
                trailingContent = { RadioButton(selected = sel, onClick = null) },
                colors = ListItemDefaults.colors(containerColor = Color.Transparent),
                modifier = Modifier.clickable { vm.setTheme(id) },
            )
        }
        item { SectionHeader(vm.tr("language")) }
        item {
            SingleChoiceSegmentedButtonRow(Modifier.fillMaxWidth().padding(horizontal = 16.dp)) {
                SegmentedButton(
                    selected = vm.lang == UiLang.Cs,
                    onClick = { vm.applyLang(UiLang.Cs) },
                    shape = SegmentedButtonDefaults.itemShape(0, 2),
                ) { Text("Čeština") }
                SegmentedButton(
                    selected = vm.lang == UiLang.En,
                    onClick = { vm.applyLang(UiLang.En) },
                    shape = SegmentedButtonDefaults.itemShape(1, 2),
                ) { Text("English") }
            }
        }
        item { SectionHeader(vm.tr("updates")) }
        item {
            Column(Modifier.padding(horizontal = 16.dp)) {
                Text(
                    vm.fmt(vm.update.messageKey, *(listOfNotNull(vm.update.messageArg).toTypedArray())),
                    style = MaterialTheme.typography.bodyMedium,
                    color = MaterialTheme.colorScheme.onSurfaceVariant,
                )
                Spacer(Modifier.height(10.dp))
                Row(horizontalArrangement = Arrangement.spacedBy(8.dp)) {
                    OutlinedButton(onClick = { vm.checkUpdate(true) }) { Text(vm.tr("update_check")) }
                    if (vm.update.canInstall) {
                        PrimaryButton(vm.tr("update_install")) { vm.installUpdate() }
                    }
                }
            }
        }
        item {
            ListItem(
                headlineContent = { Text("Build") },
                supportingContent = { Text("${vm.platform.versionName} · ${vm.platform.commit.take(7)}") },
                colors = ListItemDefaults.colors(containerColor = Color.Transparent),
                modifier = Modifier.padding(top = 8.dp),
            )
        }
    }
}

// ---------------------------------------------------------------------------
// Search

@OptIn(ExperimentalMaterial3Api::class)
@Composable
fun SearchScreen(vm: AppViewModel) {
    val q = normalizeAnswer(vm.searchQuery)
    val hits = buildSearch(vm).filter { q.isEmpty() || normalizeAnswer(it.hay).contains(q) }.take(40)
    val focus = remember { FocusRequester() }
    LaunchedEffect(Unit) { focus.requestFocus() }
    Scaffold(
        topBar = {
            Surface(color = MaterialTheme.colorScheme.surfaceContainer) {
                Row(
                    Modifier.fillMaxWidth().statusBarsPadding().padding(horizontal = 4.dp, vertical = 4.dp),
                    verticalAlignment = Alignment.CenterVertically,
                ) {
                    IconButton(onClick = { vm.searchOpen = false }) {
                        Icon(Icons.AutoMirrored.Filled.ArrowBack, contentDescription = vm.tr("back"))
                    }
                    TextField(
                        value = vm.searchQuery,
                        onValueChange = { vm.searchQuery = it },
                        placeholder = { Text(vm.tr("search_placeholder")) },
                        singleLine = true,
                        colors = TextFieldDefaults.colors(
                            focusedContainerColor = Color.Transparent,
                            unfocusedContainerColor = Color.Transparent,
                            focusedIndicatorColor = Color.Transparent,
                            unfocusedIndicatorColor = Color.Transparent,
                        ),
                        keyboardOptions = KeyboardOptions(imeAction = ImeAction.Search),
                        modifier = Modifier.weight(1f).focusRequester(focus),
                    )
                    if (vm.searchQuery.isNotEmpty()) {
                        IconButton(onClick = { vm.searchQuery = "" }) { Icon(Icons.Filled.Close, contentDescription = null) }
                    } else {
                        Icon(Icons.Filled.Search, contentDescription = null, tint = MaterialTheme.colorScheme.onSurfaceVariant, modifier = Modifier.padding(horizontal = 12.dp))
                    }
                }
            }
        },
    ) { inner ->
        if (hits.isEmpty()) {
            Text(
                if (q.isEmpty()) vm.tr("search_hint") else vm.tr("search_empty"),
                color = MaterialTheme.colorScheme.onSurfaceVariant,
                modifier = Modifier.padding(inner).padding(24.dp),
            )
        } else {
            LazyColumn(contentPadding = PaddingValues(top = inner.calculateTopPadding(), bottom = inner.calculateBottomPadding() + 24.dp)) {
                items(hits) { hit ->
                    NavRow(
                        title = hit.title,
                        subtitle = hit.sub,
                        locked = hit.locked,
                        leading = { Icon(Icons.Filled.Search, null, tint = MaterialTheme.colorScheme.outline) },
                        onClick = if (hit.locked) null else ({ vm.openPage(hit.target) }),
                    )
                }
            }
        }
    }
}

private data class Hit(val title: String, val sub: String, val target: String, val locked: Boolean, val hay: String)

private fun buildSearch(vm: AppViewModel): List<Hit> {
    val out = mutableListOf<Hit>()
    fun add(title: String, sub: String, target: String, locked: Boolean = false, extra: String = "") {
        out += Hit(title, sub, target, locked, "$title $sub $target $extra")
    }
    add(vm.tr("subjects_title"), vm.tr("search_page"), "subjects", extra = "predmety")
    add(vm.tr("stats"), vm.tr("search_page"), "stats")
    add(vm.tr("settings"), vm.tr("search_page"), "settings")
    vm.content.subjects.forEach { s ->
        add(vm.tr(s.str("key")), vm.tr("search_subject"), s.strOrNull("target") ?: "subjects", !s.bool("open"))
    }
    vm.content.german.forEach { u ->
        val unlocked = u.bool("unlocked")
        add(u.str("title"), vm.tr("search_unit"), u.strOrNull("page") ?: "roadmap", !unlocked, "deutsch")
        if (unlocked) {
            u.strs("names").forEachIndexed { i, name ->
                add(name, u.str("title"), "u${u.int("id") + 1}e${i + 1}")
            }
            if (u.has("branch")) add("Vokabeltraining", u.str("title"), u.str("branch"), extra = "vokabel")
        }
    }
    vm.content.netLessons.forEach { l ->
        add(vm.tr(l.str("titleKey")), vm.tr("search_lesson"), "netunit${l.int("id")}", extra = "site")
        add(vm.tr(l.str("titleKey")), vm.tr("search_exercise"), "netex${l.int("id")}")
    }
    listOf("net2", "net3", "net4").forEach { course ->
        add(vm.tr("net_year${course.drop(3)}"), vm.tr("search_lesson"), "${course}map", extra = "rocnik site")
        vm.content.itLessons(course).forEach { l ->
            add(vm.tr(l.str("titleKey")), vm.tr("search_lesson"), "${course}unit${l.int("id")}", extra = "site")
        }
    }
    add(vm.tr("hw_year1"), vm.tr("search_lesson"), "hwmap", extra = "rocnik")
    listOf("hw2", "hw3", "hw4").forEach { course ->
        add(vm.tr("hw_year${course.drop(2)}"), vm.tr("search_lesson"), "${course}map", extra = "rocnik hardware")
        vm.content.itLessons(course).forEach { l ->
            add(vm.tr(l.str("titleKey")), vm.tr("search_lesson"), "${course}unit${l.int("id")}", extra = "hardware")
        }
    }
    vm.content.hw.forEach { l ->
        add(vm.tr(l.str("titleKey")), vm.tr("search_lesson"), "hwunit${l.int("id")}", extra = "hardware")
        add(vm.tr(l.str("titleKey")), vm.tr("search_exercise"), "hwex${l.int("id")}")
    }
    add(vm.tr("on_year1"), vm.tr("search_lesson"), "onmap", extra = "rocnik")
    add(vm.tr("on_year2"), vm.tr("search_lesson"), "on2map", extra = "rocnik")
    add(vm.tr("on_year3"), vm.tr("search_lesson"), "on3map", extra = "rocnik")
    add(vm.tr("on_year4"), vm.tr("search_lesson"), "on4map", extra = "rocnik")
    vm.content.on.forEach { l ->
        add(vm.tr(l.str("titleKey")), vm.tr("search_lesson"), "onunit${l.int("id")}", extra = "obcanka")
        add(vm.tr(l.str("titleKey")), vm.tr("search_exercise"), "onex${l.int("id")}")
    }
    vm.content.on2.forEach { l ->
        add(vm.tr(l.str("titleKey")), vm.tr("search_lesson"), "on2unit${l.int("id")}", extra = "obcanka")
        add(vm.tr(l.str("titleKey")), vm.tr("search_exercise"), "on2ex${l.int("id")}")
    }
    vm.content.on3.forEach { l ->
        add(vm.tr(l.str("titleKey")), vm.tr("search_lesson"), "on3unit${l.int("id")}", extra = "obcanka")
        add(vm.tr(l.str("titleKey")), vm.tr("search_exercise"), "on3ex${l.int("id")}")
    }
    vm.content.on4.forEach { l ->
        add(vm.tr(l.str("titleKey")), vm.tr("search_lesson"), "on4unit${l.int("id")}", extra = "obcanka")
        add(vm.tr(l.str("titleKey")), vm.tr("search_exercise"), "on4ex${l.int("id")}")
    }
    for (year in 1..4) {
        add(vm.tr("en_year$year"), vm.tr("search_lesson"), "en${year}map", extra = "anglictina english rocnik")
        vm.content.enYear(year).forEach { l ->
            add(vm.tr(l.str("titleKey")), vm.tr("search_lesson"), "en${year}unit${l.int("id")}", extra = "anglictina english")
            add(vm.tr(l.str("titleKey")), vm.tr("search_exercise"), "en${year}ex${l.int("id")}")
        }
        add(vm.tr("de_year$year"), vm.tr("search_lesson"), "de${year}map", extra = "nemcina deutsch rocnik")
        vm.content.deYear(year).forEach { l ->
            add(vm.tr(l.str("titleKey")), vm.tr("search_lesson"), "de${year}unit${l.int("id")}", extra = "nemcina deutsch")
            add(vm.tr(l.str("titleKey")), vm.tr("search_exercise"), "de${year}ex${l.int("id")}")
        }
    }
    add(vm.tr("Literatura"), vm.tr("search_lesson"), "lityears", extra = "literatura")
    add(vm.tr("lit_year1"), vm.tr("search_lesson"), "litmap", extra = "rocnik")
    add(vm.tr("lit_year2"), vm.tr("search_lesson"), "lit2map", extra = "rocnik")
    add(vm.tr("lit_year3"), vm.tr("search_lesson"), "lit3map", extra = "rocnik")
    add(vm.tr("lit_year4"), vm.tr("search_lesson"), "lit4map", extra = "rocnik")
    vm.content.lit.forEach { l ->
        add(vm.tr(l.str("titleKey")), vm.tr("search_lesson"), "litunit${l.int("id")}", extra = "literatura")
        add(vm.tr(l.str("titleKey")), vm.tr("search_exercise"), "litex${l.int("id")}")
    }
    vm.content.lit2.forEach { l ->
        add(vm.tr(l.str("titleKey")), vm.tr("search_lesson"), "lit2unit${l.int("id")}", extra = "literatura")
        add(vm.tr(l.str("titleKey")), vm.tr("search_exercise"), "lit2ex${l.int("id")}")
    }
    vm.content.lit3.forEach { l ->
        add(vm.tr(l.str("titleKey")), vm.tr("search_lesson"), "lit3unit${l.int("id")}", extra = "literatura")
        add(vm.tr(l.str("titleKey")), vm.tr("search_exercise"), "lit3ex${l.int("id")}")
    }
    vm.content.lit4.forEach { l ->
        add(vm.tr(l.str("titleKey")), vm.tr("search_lesson"), "lit4unit${l.int("id")}", extra = "literatura")
        add(vm.tr(l.str("titleKey")), vm.tr("search_exercise"), "lit4ex${l.int("id")}")
    }
    add(vm.tr("Matematika"), vm.tr("search_lesson"), "matyears", extra = "matematika")
    add(vm.tr("mat0_year"), vm.tr("search_lesson"), "mat0map", extra = "zakladka")
    add(vm.tr("mat_year1"), vm.tr("search_lesson"), "matmap", extra = "rocnik")
    add(vm.tr("mat_year2"), vm.tr("search_lesson"), "mat2map", extra = "rocnik")
    add(vm.tr("mat_year3"), vm.tr("search_lesson"), "mat3map", extra = "rocnik")
    add(vm.tr("mat_year4"), vm.tr("search_lesson"), "mat4map", extra = "rocnik")
    vm.content.mat0.forEach { l ->
        add(vm.tr(l.str("titleKey")), vm.tr("search_lesson"), "mat0unit${l.int("id")}", extra = "matematika")
        add(vm.tr(l.str("titleKey")), vm.tr("search_exercise"), "mat0ex${l.int("id")}")
    }
    vm.content.mat.forEach { l ->
        add(vm.tr(l.str("titleKey")), vm.tr("search_lesson"), "matunit${l.int("id")}", extra = "matematika")
        add(vm.tr(l.str("titleKey")), vm.tr("search_exercise"), "matex${l.int("id")}")
    }
    vm.content.mat2.forEach { l ->
        add(vm.tr(l.str("titleKey")), vm.tr("search_lesson"), "mat2unit${l.int("id")}", extra = "matematika")
        add(vm.tr(l.str("titleKey")), vm.tr("search_exercise"), "mat2ex${l.int("id")}")
    }
    vm.content.mat3.forEach { l ->
        add(vm.tr(l.str("titleKey")), vm.tr("search_lesson"), "mat3unit${l.int("id")}", extra = "matematika")
        add(vm.tr(l.str("titleKey")), vm.tr("search_exercise"), "mat3ex${l.int("id")}")
    }
    vm.content.mat4.forEach { l ->
        add(vm.tr(l.str("titleKey")), vm.tr("search_lesson"), "mat4unit${l.int("id")}", extra = "matematika")
        add(vm.tr(l.str("titleKey")), vm.tr("search_exercise"), "mat4ex${l.int("id")}")
    }
    add(vm.tr("Fyzika"), vm.tr("search_lesson"), "fyzyears", extra = "fyzika")
    add(vm.tr("fyz_year1"), vm.tr("search_lesson"), "fyzmap", extra = "rocnik")
    add(vm.tr("fyz_year2"), vm.tr("search_lesson"), "fyz2map", extra = "rocnik")
    add(vm.tr("fyz_year3"), vm.tr("search_lesson"), "fyz3map", extra = "rocnik")
    add(vm.tr("fyz_year4"), vm.tr("search_lesson"), "fyz4map", extra = "rocnik")
    vm.content.fyz.forEach { l ->
        add(vm.tr(l.str("titleKey")), vm.tr("search_lesson"), "fyzunit${l.int("id")}", extra = "fyzika")
        add(vm.tr(l.str("titleKey")), vm.tr("search_exercise"), "fyzex${l.int("id")}")
    }
    vm.content.fyz2.forEach { l ->
        add(vm.tr(l.str("titleKey")), vm.tr("search_lesson"), "fyz2unit${l.int("id")}", extra = "fyzika")
        add(vm.tr(l.str("titleKey")), vm.tr("search_exercise"), "fyz2ex${l.int("id")}")
    }
    vm.content.fyz3.forEach { l ->
        add(vm.tr(l.str("titleKey")), vm.tr("search_lesson"), "fyz3unit${l.int("id")}", extra = "fyzika")
        add(vm.tr(l.str("titleKey")), vm.tr("search_exercise"), "fyz3ex${l.int("id")}")
    }
    vm.content.fyz4.forEach { l ->
        add(vm.tr(l.str("titleKey")), vm.tr("search_lesson"), "fyz4unit${l.int("id")}", extra = "fyzika")
        add(vm.tr(l.str("titleKey")), vm.tr("search_exercise"), "fyz4ex${l.int("id")}")
    }
    add(vm.tr("Prezentační grafika"), vm.tr("search_lesson"), "pgyears", extra = "photoshop premiere illustrator")
    add(vm.tr("pg_year1"), vm.tr("search_lesson"), "pgmap", extra = "rocnik")
    add(vm.tr("pg_year2"), vm.tr("search_lesson"), "pg2map", extra = "rocnik")
    add(vm.tr("pg_year3"), vm.tr("search_lesson"), "pg3map", extra = "rocnik")
    add(vm.tr("pg_year4"), vm.tr("search_lesson"), "pg4map", extra = "rocnik")
    vm.content.pg.forEach { l ->
        add(vm.tr(l.str("titleKey")), vm.tr("search_lesson"), "pgunit${l.int("id")}", extra = "photoshop")
        add(vm.tr(l.str("titleKey")), vm.tr("search_exercise"), "pgex${l.int("id")}")
    }
    vm.content.pg2.forEach { l ->
        add(vm.tr(l.str("titleKey")), vm.tr("search_lesson"), "pg2unit${l.int("id")}", extra = "photoshop")
        add(vm.tr(l.str("titleKey")), vm.tr("search_exercise"), "pg2ex${l.int("id")}")
    }
    vm.content.pg3.forEach { l ->
        add(vm.tr(l.str("titleKey")), vm.tr("search_lesson"), "pg3unit${l.int("id")}", extra = "premiere")
        add(vm.tr(l.str("titleKey")), vm.tr("search_exercise"), "pg3ex${l.int("id")}")
    }
    vm.content.pg4.forEach { l ->
        add(vm.tr(l.str("titleKey")), vm.tr("search_lesson"), "pg4unit${l.int("id")}", extra = "illustrator")
        add(vm.tr(l.str("titleKey")), vm.tr("search_exercise"), "pg4ex${l.int("id")}")
    }
    add(vm.tr("Technická grafika"), vm.tr("search_lesson"), "tgyears", extra = "autocad grafika")
    add(vm.tr("tg_year1"), vm.tr("search_lesson"), "tgmap", extra = "rocnik")
    add(vm.tr("tg_year2"), vm.tr("search_lesson"), "tg2map", extra = "rocnik")
    add(vm.tr("tg_year3"), vm.tr("search_lesson"), "tg3map", extra = "rocnik")
    add(vm.tr("tg_year4"), vm.tr("search_lesson"), "tg4map", extra = "rocnik")
    vm.content.tg.forEach { l ->
        add(vm.tr(l.str("titleKey")), vm.tr("search_lesson"), "tgunit${l.int("id")}", extra = "autocad")
        add(vm.tr(l.str("titleKey")), vm.tr("search_exercise"), "tgex${l.int("id")}")
    }
    vm.content.tg2.forEach { l ->
        add(vm.tr(l.str("titleKey")), vm.tr("search_lesson"), "tg2unit${l.int("id")}", extra = "autocad")
        add(vm.tr(l.str("titleKey")), vm.tr("search_exercise"), "tg2ex${l.int("id")}")
    }
    vm.content.tg3.forEach { l ->
        add(vm.tr(l.str("titleKey")), vm.tr("search_lesson"), "tg3unit${l.int("id")}", extra = "autocad")
        add(vm.tr(l.str("titleKey")), vm.tr("search_exercise"), "tg3ex${l.int("id")}")
    }
    vm.content.tg4.forEach { l ->
        add(vm.tr(l.str("titleKey")), vm.tr("search_lesson"), "tg4unit${l.int("id")}", extra = "autocad")
        add(vm.tr(l.str("titleKey")), vm.tr("search_exercise"), "tg4ex${l.int("id")}")
    }
    add(vm.tr("Programování"), vm.tr("search_lesson"), "csyears", extra = "csharp c#")
    add(vm.tr("cs_year1"), vm.tr("search_lesson"), "csmap", extra = "rocnik")
    add(vm.tr("cs_year2"), vm.tr("search_lesson"), "cs2map", extra = "rocnik")
    add(vm.tr("cs_year3"), vm.tr("search_lesson"), "cs3map", extra = "rocnik")
    add(vm.tr("cs_year4"), vm.tr("search_lesson"), "cs4map", extra = "rocnik")
    vm.content.cs.forEach { l ->
        add(vm.tr(l.str("titleKey")), vm.tr("search_lesson"), "csunit${l.int("id")}", extra = "csharp")
    }
    vm.content.cs2.forEach { l ->
        add(vm.tr(l.str("titleKey")), vm.tr("search_lesson"), "cs2unit${l.int("id")}", extra = "csharp")
    }
    vm.content.cs3.forEach { l ->
        add(vm.tr(l.str("titleKey")), vm.tr("search_lesson"), "cs3unit${l.int("id")}", extra = "csharp")
    }
    vm.content.cs4.forEach { l ->
        add(vm.tr(l.str("titleKey")), vm.tr("search_lesson"), "cs4unit${l.int("id")}", extra = "csharp")
    }
    add(vm.tr("Chemie"), vm.tr("search_lesson"), "chemmap", extra = "chemie")
    add(vm.tr("Biologie"), vm.tr("search_lesson"), "biomap", extra = "biologie")
    vm.content.chem.forEach { l ->
        add(vm.tr(l.str("titleKey")), vm.tr("search_lesson"), "chemunit${l.int("id")}", extra = "chemie")
        add(vm.tr(l.str("titleKey")), vm.tr("search_exercise"), "chemex${l.int("id")}")
    }
    vm.content.bio.forEach { l ->
        add(vm.tr(l.str("titleKey")), vm.tr("search_lesson"), "biounit${l.int("id")}", extra = "biologie")
        add(vm.tr(l.str("titleKey")), vm.tr("search_exercise"), "bioex${l.int("id")}")
    }
    add(vm.tr("Mluvnice"), vm.tr("search_lesson"), "mluvnice")
    vm.content.mluvnice.forEach { m -> add(m.str("name"), vm.tr("Mluvnice"), "mluve${m.int("id")}") }
    add(vm.tr("Maturitní četba"), vm.tr("search_book"), "readinglist")
    vm.content.books.forEach { b ->
        add(b.str("title"), vm.tr("search_book"), b.str("page"))
    }
    return out
}

// ---------------------------------------------------------------------------
// Lists

data class LessonRow(val title: String, val done: Boolean, val route: Route, val subtitle: String? = null)

@Composable
fun LessonListScreen(vm: AppViewModel, title: String, sub: String, rows: List<LessonRow>) {
    DetailScaffold(vm, title, sub) { inner ->
        LazyColumn(contentPadding = PaddingValues(top = inner.calculateTopPadding(), bottom = inner.calculateBottomPadding() + 16.dp)) {
            items(rows) { row ->
                NavRow(title = row.title, subtitle = row.subtitle, done = row.done, onClick = { vm.go(row.route) })
            }
        }
    }
}

@Composable
fun RoadmapScreen(vm: AppViewModel) {
    DetailScaffold(vm, vm.tr("roadmap_title"), vm.tr("roadmap_sub")) { inner ->
        LazyColumn(contentPadding = PaddingValues(top = inner.calculateTopPadding(), bottom = inner.calculateBottomPadding() + 16.dp)) {
            items(vm.content.german) { u ->
                val unlocked = u.bool("unlocked")
                val names = u.strs("names")
                val doneCount = names.indices.count { vm.progress.germanDone(u.int("id"), it + 1) }
                val done = unlocked && names.isNotEmpty() && doneCount == names.size
                NavRow(
                    title = u.str("title"),
                    subtitle = if (unlocked) vm.fmt("stats_ex_fmt", doneCount, names.size) else null,
                    done = done,
                    locked = !unlocked,
                    onClick = if (unlocked) ({ vm.go(Route.UnitMap(u.int("id"))) }) else null,
                )
            }
        }
    }
}

@Composable
fun UnitMapScreen(vm: AppViewModel, unitId: Int) {
    val u = vm.content.germanUnit(unitId) ?: return
    val names = u.strs("names")
    DetailScaffold(vm, u.str("title"), vm.tr(u.str("sub"))) { inner ->
        LazyColumn(contentPadding = PaddingValues(top = inner.calculateTopPadding(), bottom = inner.calculateBottomPadding() + 16.dp)) {
            items(names.size) { i ->
                NavRow(title = names[i], done = vm.progress.germanDone(unitId, i + 1)) { vm.go(Route.GermanEx(unitId, i + 1)) }
            }
            if (u.has("branch")) {
                item { HorizontalDivider(Modifier.padding(vertical = 8.dp)) }
                item {
                    NavRow(
                        title = vm.tr("Vokabeltraining"),
                        subtitle = vm.tr("sub_translate"),
                        done = vm.progress.vocabDone(unitId),
                        leading = { Icon(Icons.AutoMirrored.Filled.MenuBook, null, tint = MaterialTheme.colorScheme.primary) },
                    ) { vm.go(Route.Vocab(unitId)) }
                }
            }
        }
    }
}

@Composable
fun ItYearScreen(vm: AppViewModel, course: String, titleKey: String, subKey: String, lesson: (Int) -> Route) {
    LessonListScreen(
        vm, vm.tr(titleKey), vm.tr(subKey),
        vm.content.itLessons(course).map { l ->
            val id = l.int("id")
            LessonRow(vm.tr(l.str("titleKey")), vm.progress.courseDone(course, id), lesson(id))
        },
    )
}

@Composable
fun NetYearsScreen(vm: AppViewModel) {
    DetailScaffold(vm, vm.tr("net_years_title"), vm.tr("net_years_sub")) { inner ->
        LazyColumn(contentPadding = PaddingValues(top = inner.calculateTopPadding(), bottom = inner.calculateBottomPadding() + 16.dp)) {
            item {
                NavRow(title = vm.tr("net_year1"), subtitle = vm.tr("net_sub"), leading = { LeadingBadge("1") }, progress = routeProgress(vm, Route.NetMap)) { vm.go(Route.NetMap) }
                NavRow(title = vm.tr("net_year2"), subtitle = vm.tr("net2_sub"), leading = { LeadingBadge("2") }, progress = routeProgress(vm, Route.Net2Map)) { vm.go(Route.Net2Map) }
                NavRow(title = vm.tr("net_year3"), subtitle = vm.tr("net3_sub"), leading = { LeadingBadge("3") }, progress = routeProgress(vm, Route.Net3Map)) { vm.go(Route.Net3Map) }
                NavRow(title = vm.tr("net_year4"), subtitle = vm.tr("net4_sub"), leading = { LeadingBadge("4") }, progress = routeProgress(vm, Route.Net4Map)) { vm.go(Route.Net4Map) }
            }
        }
    }
}

@Composable
fun HwYearsScreen(vm: AppViewModel) {
    DetailScaffold(vm, vm.tr("Technické vybavení"), vm.tr("hw_years_sub")) { inner ->
        LazyColumn(contentPadding = PaddingValues(top = inner.calculateTopPadding(), bottom = inner.calculateBottomPadding() + 16.dp)) {
            item {
                NavRow(title = vm.tr("hw_year1"), subtitle = vm.tr("hw_sub"), leading = { LeadingBadge("1") }, progress = routeProgress(vm, Route.HwMap)) { vm.go(Route.HwMap) }
                NavRow(title = vm.tr("hw_year2"), subtitle = vm.tr("hw2_sub"), leading = { LeadingBadge("2") }, progress = routeProgress(vm, Route.Hw2Map)) { vm.go(Route.Hw2Map) }
                NavRow(title = vm.tr("hw_year3"), subtitle = vm.tr("hw3_sub"), leading = { LeadingBadge("3") }, progress = routeProgress(vm, Route.Hw3Map)) { vm.go(Route.Hw3Map) }
                NavRow(title = vm.tr("hw_year4"), subtitle = vm.tr("hw4_sub"), leading = { LeadingBadge("4") }, progress = routeProgress(vm, Route.Hw4Map)) { vm.go(Route.Hw4Map) }
            }
        }
    }
}

@Composable
fun DeHomeScreen(vm: AppViewModel) {
    DetailScaffold(vm, vm.tr("Deutsch"), vm.tr("de_home_sub")) { inner ->
        LazyColumn(contentPadding = PaddingValues(top = inner.calculateTopPadding(), bottom = inner.calculateBottomPadding() + 16.dp)) {
            item {
                NavRow(title = vm.tr("de_book_title"), subtitle = vm.tr("de_book_sub"), leading = { LeadingBadge("U") }, progress = routeProgress(vm, Route.Roadmap)) {
                    vm.go(Route.Roadmap)
                }
            }
            items(4) { index ->
                val year = index + 1
                NavRow(
                    title = vm.tr("de_year$year"),
                    subtitle = vm.tr("de_y${year}_sub"),
                    leading = { LeadingBadge("$year") },
                    progress = routeProgress(vm, Route.DeMap(year)),
                ) { vm.go(Route.DeMap(year)) }
            }
        }
    }
}

@Composable
fun EnYearsScreen(vm: AppViewModel) {
    DetailScaffold(vm, vm.tr("English"), vm.tr("en_years_sub")) { inner ->
        LazyColumn(contentPadding = PaddingValues(top = inner.calculateTopPadding(), bottom = inner.calculateBottomPadding() + 16.dp)) {
            items(4) { index ->
                val year = index + 1
                NavRow(
                    title = vm.tr("en_year$year"),
                    subtitle = vm.tr("en_y${year}_sub"),
                    leading = { LeadingBadge("$year") },
                    progress = routeProgress(vm, Route.EnMap(year)),
                ) { vm.go(Route.EnMap(year)) }
            }
        }
    }
}

@Composable
fun OnYearsScreen(vm: AppViewModel) {
    DetailScaffold(vm, vm.tr("Občanská nauka"), vm.tr("on_years_sub")) { inner ->
        LazyColumn(contentPadding = PaddingValues(top = inner.calculateTopPadding(), bottom = inner.calculateBottomPadding() + 16.dp)) {
            item {
                NavRow(title = vm.tr("on_year1"), subtitle = vm.tr("on_sub"), leading = { LeadingBadge("1") }, progress = routeProgress(vm, Route.OnMap)) { vm.go(Route.OnMap) }
                NavRow(title = vm.tr("on_year2"), subtitle = vm.tr("on2_sub"), leading = { LeadingBadge("2") }, progress = routeProgress(vm, Route.On2Map)) { vm.go(Route.On2Map) }
                NavRow(title = vm.tr("on_year3"), subtitle = vm.tr("on3_sub"), leading = { LeadingBadge("3") }, progress = routeProgress(vm, Route.On3Map)) { vm.go(Route.On3Map) }
                NavRow(title = vm.tr("on_year4"), subtitle = vm.tr("on4_sub"), leading = { LeadingBadge("4") }, progress = routeProgress(vm, Route.On4Map)) { vm.go(Route.On4Map) }
            }
        }
    }
}

@Composable
fun LitYearsScreen(vm: AppViewModel) {
    DetailScaffold(vm, vm.tr("Literatura"), vm.tr("lit_years_sub")) { inner ->
        LazyColumn(contentPadding = PaddingValues(top = inner.calculateTopPadding(), bottom = inner.calculateBottomPadding() + 16.dp)) {
            item {
                NavRow(title = vm.tr("lit_year1"), subtitle = vm.tr("lit_sub"), leading = { LeadingBadge("1") }, progress = routeProgress(vm, Route.LitMap)) { vm.go(Route.LitMap) }
                NavRow(title = vm.tr("lit_year2"), subtitle = vm.tr("lit2_sub"), leading = { LeadingBadge("2") }, progress = routeProgress(vm, Route.Lit2Map)) { vm.go(Route.Lit2Map) }
                NavRow(title = vm.tr("lit_year3"), subtitle = vm.tr("lit3_sub"), leading = { LeadingBadge("3") }, progress = routeProgress(vm, Route.Lit3Map)) { vm.go(Route.Lit3Map) }
                NavRow(title = vm.tr("lit_year4"), subtitle = vm.tr("lit4_sub"), leading = { LeadingBadge("4") }, progress = routeProgress(vm, Route.Lit4Map)) { vm.go(Route.Lit4Map) }
            }
        }
    }
}

@Composable
fun MatYearsScreen(vm: AppViewModel) {
    DetailScaffold(vm, vm.tr("Matematika"), vm.tr("mat_years_sub")) { inner ->
        LazyColumn(contentPadding = PaddingValues(top = inner.calculateTopPadding(), bottom = inner.calculateBottomPadding() + 16.dp)) {
            item {
                NavRow(title = vm.tr("mat0_year"), subtitle = vm.tr("mat0_sub"), leading = { LeadingBadge("ZŠ") }, progress = routeProgress(vm, Route.Mat0Map)) { vm.go(Route.Mat0Map) }
                NavRow(title = vm.tr("mat_year1"), subtitle = vm.tr("mat_sub"), leading = { LeadingBadge("1") }, progress = routeProgress(vm, Route.MatMap)) { vm.go(Route.MatMap) }
                NavRow(title = vm.tr("mat_year2"), subtitle = vm.tr("mat2_sub"), leading = { LeadingBadge("2") }, progress = routeProgress(vm, Route.Mat2Map)) { vm.go(Route.Mat2Map) }
                NavRow(title = vm.tr("mat_year3"), subtitle = vm.tr("mat3_sub"), leading = { LeadingBadge("3") }, progress = routeProgress(vm, Route.Mat3Map)) { vm.go(Route.Mat3Map) }
                NavRow(title = vm.tr("mat_year4"), subtitle = vm.tr("mat4_sub"), leading = { LeadingBadge("4") }, progress = routeProgress(vm, Route.Mat4Map)) { vm.go(Route.Mat4Map) }
            }
        }
    }
}

@Composable
fun CsYearsScreen(vm: AppViewModel) {
    DetailScaffold(vm, vm.tr("Programování"), vm.tr("cs_years_sub")) { inner ->
        LazyColumn(contentPadding = PaddingValues(top = inner.calculateTopPadding(), bottom = inner.calculateBottomPadding() + 16.dp)) {
            item {
                NavRow(title = vm.tr("cs_year1"), subtitle = vm.tr("cs_sub"), leading = { LeadingBadge("1") }, progress = routeProgress(vm, Route.CsMap)) { vm.go(Route.CsMap) }
                NavRow(title = vm.tr("cs_year2"), subtitle = vm.tr("cs2_sub"), leading = { LeadingBadge("2") }, progress = routeProgress(vm, Route.Cs2Map)) { vm.go(Route.Cs2Map) }
                NavRow(title = vm.tr("cs_year3"), subtitle = vm.tr("cs3_sub"), leading = { LeadingBadge("3") }, progress = routeProgress(vm, Route.Cs3Map)) { vm.go(Route.Cs3Map) }
                NavRow(title = vm.tr("cs_year4"), subtitle = vm.tr("cs4_sub"), leading = { LeadingBadge("4") }, progress = routeProgress(vm, Route.Cs4Map)) { vm.go(Route.Cs4Map) }
            }
        }
    }
}

@Composable
fun PgYearsScreen(vm: AppViewModel) {
    DetailScaffold(vm, vm.tr("Prezentační grafika"), vm.tr("pg_years_sub")) { inner ->
        LazyColumn(contentPadding = PaddingValues(top = inner.calculateTopPadding(), bottom = inner.calculateBottomPadding() + 16.dp)) {
            item {
                NavRow(title = vm.tr("pg_year1"), subtitle = vm.tr("pg_sub"), leading = { LeadingBadge("1") }, progress = routeProgress(vm, Route.PgMap)) { vm.go(Route.PgMap) }
                NavRow(title = vm.tr("pg_year2"), subtitle = vm.tr("pg2_sub"), leading = { LeadingBadge("2") }, progress = routeProgress(vm, Route.Pg2Map)) { vm.go(Route.Pg2Map) }
                NavRow(title = vm.tr("pg_year3"), subtitle = vm.tr("pg3_sub"), leading = { LeadingBadge("3") }, progress = routeProgress(vm, Route.Pg3Map)) { vm.go(Route.Pg3Map) }
                NavRow(title = vm.tr("pg_year4"), subtitle = vm.tr("pg4_sub"), leading = { LeadingBadge("4") }, progress = routeProgress(vm, Route.Pg4Map)) { vm.go(Route.Pg4Map) }
            }
        }
    }
}

@Composable
fun TgYearsScreen(vm: AppViewModel) {
    DetailScaffold(vm, vm.tr("Technická grafika"), vm.tr("tg_years_sub")) { inner ->
        LazyColumn(contentPadding = PaddingValues(top = inner.calculateTopPadding(), bottom = inner.calculateBottomPadding() + 16.dp)) {
            item {
                NavRow(title = vm.tr("tg_year1"), subtitle = vm.tr("tg_sub"), leading = { LeadingBadge("1") }, progress = routeProgress(vm, Route.TgMap)) { vm.go(Route.TgMap) }
                NavRow(title = vm.tr("tg_year2"), subtitle = vm.tr("tg2_sub"), leading = { LeadingBadge("2") }, progress = routeProgress(vm, Route.Tg2Map)) { vm.go(Route.Tg2Map) }
                NavRow(title = vm.tr("tg_year3"), subtitle = vm.tr("tg3_sub"), leading = { LeadingBadge("3") }, progress = routeProgress(vm, Route.Tg3Map)) { vm.go(Route.Tg3Map) }
                NavRow(title = vm.tr("tg_year4"), subtitle = vm.tr("tg4_sub"), leading = { LeadingBadge("4") }, progress = routeProgress(vm, Route.Tg4Map)) { vm.go(Route.Tg4Map) }
            }
        }
    }
}

@Composable
fun FyzYearsScreen(vm: AppViewModel) {
    DetailScaffold(vm, vm.tr("Fyzika"), vm.tr("fyz_years_sub")) { inner ->
        LazyColumn(contentPadding = PaddingValues(top = inner.calculateTopPadding(), bottom = inner.calculateBottomPadding() + 16.dp)) {
            item {
                NavRow(title = vm.tr("fyz_year1"), subtitle = vm.tr("fyz_sub"), leading = { LeadingBadge("1") }, progress = routeProgress(vm, Route.FyzMap)) { vm.go(Route.FyzMap) }
                NavRow(title = vm.tr("fyz_year2"), subtitle = vm.tr("fyz2_sub"), leading = { LeadingBadge("2") }, progress = routeProgress(vm, Route.Fyz2Map)) { vm.go(Route.Fyz2Map) }
                NavRow(title = vm.tr("fyz_year3"), subtitle = vm.tr("fyz3_sub"), leading = { LeadingBadge("3") }, progress = routeProgress(vm, Route.Fyz3Map)) { vm.go(Route.Fyz3Map) }
                NavRow(title = vm.tr("fyz_year4"), subtitle = vm.tr("fyz4_sub"), leading = { LeadingBadge("4") }, progress = routeProgress(vm, Route.Fyz4Map)) { vm.go(Route.Fyz4Map) }
            }
        }
    }
}

@Composable
fun SciMapScreen(vm: AppViewModel) {
    DetailScaffold(vm, vm.tr("Základy přírodních věd"), vm.tr("sci_sub")) { inner ->
        LazyColumn(contentPadding = PaddingValues(top = inner.calculateTopPadding(), bottom = inner.calculateBottomPadding() + 16.dp)) {
            item {
                NavRow(
                    title = vm.tr("Chemie"),
                    subtitle = vm.tr("chem_sub"),
                    leading = { LeadingIcon(Icons.Filled.Science) },
                    progress = routeProgress(vm, Route.ChemMap),
                ) { vm.go(Route.ChemMap) }
            }
            item {
                NavRow(
                    title = vm.tr("Biologie"),
                    subtitle = vm.tr("bio_sub"),
                    leading = { LeadingIcon(Icons.Filled.Eco) },
                    progress = routeProgress(vm, Route.BioMap),
                ) { vm.go(Route.BioMap) }
            }
        }
    }
}

@Composable
fun CzechMapScreen(vm: AppViewModel) {
    DetailScaffold(vm, vm.tr("Český jazyk a literatura"), vm.tr("czech_sub")) { inner ->
        LazyColumn(contentPadding = PaddingValues(top = inner.calculateTopPadding(), bottom = inner.calculateBottomPadding() + 16.dp)) {
            item {
                NavRow(
                    title = vm.tr("Literatura"),
                    subtitle = vm.tr("lit_years_sub"),
                    leading = { LeadingIcon(Icons.Filled.AutoStories) },
                    progress = routeProgress(vm, Route.LitYears),
                ) { vm.go(Route.LitYears) }
            }
            item {
                NavRow(
                    title = vm.tr("Mluvnice"),
                    subtitle = vm.tr("mluv_sub"),
                    leading = { LeadingIcon(Icons.Filled.FormatListNumbered) },
                    progress = routeProgress(vm, Route.Mluvnice),
                ) { vm.go(Route.Mluvnice) }
            }
            item {
                NavRow(
                    title = vm.tr("Maturitní četba"),
                    subtitle = vm.tr("reading_sub"),
                    leading = { LeadingIcon(Icons.AutoMirrored.Filled.MenuBook) },
                ) { vm.go(Route.ReadingList) }
            }
        }
    }
}

@Composable
fun BookListScreen(vm: AppViewModel) {
    DetailScaffold(vm, vm.tr("Maturitní četba"), vm.tr("reading_sub")) { inner ->
        LazyColumn(contentPadding = PaddingValues(top = inner.calculateTopPadding(), bottom = inner.calculateBottomPadding() + 16.dp)) {
            items(vm.content.books) { b ->
                val id = b.str("id")
                NavRow(
                    title = b.str("title"),
                    subtitle = vm.tr(b.str("subKey")),
                    done = vm.progress.bookQuiz(id) && vm.progress.bookPlot(id),
                    leading = { Icon(Icons.AutoMirrored.Filled.MenuBook, null, tint = MaterialTheme.colorScheme.primary) },
                ) { vm.go(Route.Book(id)) }
            }
        }
    }
}

@Composable
fun BookScreen(vm: AppViewModel, id: String) {
    val b = vm.content.book(id) ?: return
    DetailScaffold(vm, b.str("title"), vm.tr(b.str("subKey"))) { inner ->
        LazyColumn(contentPadding = PaddingValues(top = inner.calculateTopPadding(), bottom = inner.calculateBottomPadding() + 16.dp)) {
            item {
                NavRow(
                    title = vm.tr(b.str("quizTitle")),
                    subtitle = vm.tr(b.str("quizSub")),
                    done = vm.progress.bookQuiz(id),
                    leading = { Icon(Icons.Filled.Quiz, null, tint = MaterialTheme.colorScheme.primary) },
                ) { vm.go(Route.BookQuiz(id)) }
            }
            item {
                NavRow(
                    title = vm.tr(b.str("plotTitle")),
                    subtitle = vm.tr(b.str("plotSub")),
                    done = vm.progress.bookPlot(id),
                    leading = { Icon(Icons.Filled.FormatListNumbered, null, tint = MaterialTheme.colorScheme.primary) },
                ) { vm.go(Route.BookPlot(id)) }
            }
            items(b.arr("notes")) { note ->
                SectionHeader(note.str("title"))
                Card(modifier = Modifier.fillMaxWidth().padding(horizontal = 16.dp)) {
                    Column(Modifier.padding(16.dp)) {
                        note.strs("lines").forEachIndexed { i, line ->
                            if (i > 0) HorizontalDivider(Modifier.padding(vertical = 8.dp))
                            Text(line, style = MaterialTheme.typography.bodyMedium)
                        }
                    }
                }
            }
        }
    }
}

// ---------------------------------------------------------------------------
// Lesson slides

/** One explanation slide: label, title, tip and numbered points. */
@Composable
fun SlideContent(slide: J) {
    Column {
        Text(slide.str("kicker"), style = MaterialTheme.typography.labelMedium, color = MaterialTheme.colorScheme.onSurfaceVariant)
        Text(slide.str("title"), style = MaterialTheme.typography.headlineSmall, modifier = Modifier.padding(top = 4.dp, bottom = 12.dp))
        slide.strOrNull("tip")?.let { tip ->
            Card(
                modifier = Modifier.fillMaxWidth().padding(bottom = 12.dp),
                colors = CardDefaults.cardColors(containerColor = MaterialTheme.colorScheme.secondaryContainer),
            ) {
                Row(Modifier.padding(14.dp), verticalAlignment = Alignment.CenterVertically) {
                    Icon(Icons.Outlined.Lightbulb, null, tint = MaterialTheme.colorScheme.onSecondaryContainer)
                    Spacer(Modifier.width(10.dp))
                    Text(tip, style = MaterialTheme.typography.bodyMedium)
                }
            }
        }
        slide.strs("lines").forEachIndexed { index, line ->
            val numbered = Regex("""^(\d+)\.\s+(.*)$""").find(line)
            val badge = numbered?.groupValues?.get(1) ?: "${index + 1}"
            val text = numbered?.groupValues?.get(2) ?: line
            Row(
                Modifier
                    .fillMaxWidth()
                    .padding(vertical = 5.dp)
                    .background(MaterialTheme.colorScheme.surfaceVariant.copy(alpha = 0.45f), androidx.compose.foundation.shape.RoundedCornerShape(16.dp))
                    .padding(horizontal = 14.dp, vertical = 12.dp),
                verticalAlignment = Alignment.Top,
            ) {
                Box(
                    Modifier
                        .padding(top = 2.dp)
                        .size(28.dp)
                        .background(MaterialTheme.colorScheme.primary.copy(alpha = 0.14f), CircleShape),
                    contentAlignment = Alignment.Center,
                ) {
                    Text(badge, style = MaterialTheme.typography.labelLarge, color = MaterialTheme.colorScheme.primary)
                }
                Spacer(Modifier.width(12.dp))
                Text(text, style = MaterialTheme.typography.bodyLarge, modifier = Modifier.weight(1f))
            }
        }
    }
}

@OptIn(ExperimentalFoundationApi::class)
@Composable
fun SlidesScreen(
    vm: AppViewModel,
    title: String,
    sub: String,
    slides: List<J>,
    onExercise: () -> Unit,
) {
    if (slides.isEmpty()) return
    val pager = rememberPagerState { slides.size }
    val scope = rememberCoroutineScope()
    val last = pager.currentPage >= slides.lastIndex
    DetailScaffold(
        vm, title, sub,
        bottomBar = {
            Column {
                Row(
                    Modifier.fillMaxWidth().padding(top = 8.dp),
                    horizontalArrangement = Arrangement.Center,
                    verticalAlignment = Alignment.CenterVertically,
                ) {
                    slides.indices.forEach { i ->
                        val active = i == pager.currentPage
                        Box(
                            Modifier
                                .padding(horizontal = 3.dp)
                                .size(width = if (active) 18.dp else 6.dp, height = 6.dp)
                                .clip(CircleShape)
                                .background(if (active) MaterialTheme.colorScheme.primary else MaterialTheme.colorScheme.surfaceContainerHighest),
                        )
                    }
                }
                ActionBar(
                    label = if (last) vm.tr("net_slide_start") else vm.tr("net_slide_next"),
                    leading = if (pager.currentPage > 0) {
                        {
                            OutlinedButton(
                                onClick = { scope.launch { pager.animateScrollToPage(pager.currentPage - 1) } },
                                modifier = Modifier.height(50.dp),
                            ) { Icon(Icons.AutoMirrored.Filled.KeyboardArrowLeft, contentDescription = vm.tr("net_slide_prev")) }
                        }
                    } else null,
                ) {
                    if (last) onExercise() else scope.launch { pager.animateScrollToPage(pager.currentPage + 1) }
                }
            }
        },
    ) { inner ->
        HorizontalPager(state = pager, modifier = Modifier.fillMaxSize().padding(inner)) { page ->
            Column(
                Modifier
                    .fillMaxSize()
                    .verticalScroll(rememberScrollState())
                    .padding(horizontal = 16.dp, vertical = 12.dp),
            ) {
                SlideContent(slides[page])
                Spacer(Modifier.height(24.dp))
            }
        }
    }
    LaunchedEffect(slides) { if (pager.currentPage > slides.lastIndex) pager.scrollToPage(0) }
}
