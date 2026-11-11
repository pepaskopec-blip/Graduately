package org.maturita.maturita.ui

import androidx.compose.foundation.ExperimentalFoundationApi
import androidx.compose.foundation.background
import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.BoxWithConstraints
import androidx.compose.foundation.layout.ExperimentalLayoutApi
import androidx.compose.foundation.layout.FlowRow
import androidx.compose.foundation.layout.offset
import androidx.compose.foundation.layout.widthIn
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
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.foundation.verticalScroll
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.automirrored.filled.ArrowBack
import androidx.compose.material.icons.automirrored.filled.MenuBook
import androidx.compose.material.icons.filled.Apps
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
import androidx.compose.material.icons.filled.Forum
import androidx.compose.material.icons.filled.Groups
import androidx.compose.material.icons.filled.Memory
import androidx.compose.material.icons.filled.Translate
import androidx.compose.material.icons.filled.Wifi
import androidx.compose.material.icons.filled.Lock
import androidx.compose.material.icons.filled.Quiz
import androidx.compose.material.icons.filled.Science
import androidx.compose.material.icons.filled.Terminal
import androidx.compose.material.icons.filled.Search
import androidx.compose.material.icons.outlined.Lightbulb
import androidx.compose.material3.ExperimentalMaterial3Api
import androidx.compose.material3.HorizontalDivider
import androidx.compose.material3.Icon
import androidx.compose.material3.IconButton
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.OutlinedButton
import androidx.compose.material3.Scaffold
import androidx.compose.material3.TextField
import androidx.compose.material3.TextFieldDefaults
import androidx.compose.material3.Surface
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.runtime.LaunchedEffect
import androidx.compose.runtime.remember
import androidx.compose.runtime.rememberCoroutineScope
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.draw.clip
import androidx.compose.ui.draw.clipToBounds
import androidx.compose.ui.draw.drawBehind
import androidx.compose.ui.geometry.Offset
import androidx.compose.ui.graphics.Brush
import androidx.compose.ui.graphics.vector.ImageVector
import androidx.compose.ui.focus.FocusRequester
import androidx.compose.ui.focus.focusRequester
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.text.input.ImeAction
import androidx.compose.foundation.layout.statusBarsPadding
import androidx.compose.foundation.text.KeyboardOptions
import androidx.compose.ui.text.style.TextOverflow
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import kotlinx.coroutines.launch
import org.maturita.maturita.AppViewModel
import org.maturita.maturita.Route
import org.maturita.maturita.data.Accent
import org.maturita.maturita.data.ColorMode
import org.maturita.maturita.data.J
import org.maturita.maturita.data.ProgressSum
import org.maturita.maturita.data.UiLang
import org.maturita.maturita.data.normalizeAnswer
import org.maturita.maturita.data.summarize
import org.maturita.maturita.data.subjectStyle

// ---------------------------------------------------------------------------
// Roots

@Composable
fun HomeScreen(vm: AppViewModel) {
    val sum = summarize(vm.content, vm.progress)
    val open = vm.content.subjects.filter { it.bool("open") }
    val soon = vm.content.subjects.filter { !it.bool("open") }
    val parts = open.map { it to subjectSum(vm, it) }
    RootScaffold(vm, "Graduately", wordmark = true) {
        item { HomeHero(vm, sum, parts.map { (s, part) -> Triple(subjectAccent(vm, s), part.doneEx, part.totalEx) }) }
        if (vm.showChangelog) {
            item { ChangelogCard(vm) }
        }
        if (vm.update.canInstall || vm.update.status == "downloading" || vm.update.status == "staged") {
            item { UpdateBanner(vm) }
        }
        item { SectionHeader(vm.tr("subjects_title")) }
        item {
            // Covers in as many columns as fit: two on a phone, up to four in a wide window.
            BoxWithConstraints(Modifier.fillMaxWidth().padding(horizontal = 16.dp)) {
                val columns = ((maxWidth + 12.dp) / (176.dp + 12.dp)).toInt().coerceIn(2, 4)
                Column(verticalArrangement = Arrangement.spacedBy(12.dp)) {
                    parts.chunked(columns).forEach { row ->
                        Row(horizontalArrangement = Arrangement.spacedBy(12.dp)) {
                            row.forEach { (s, part) ->
                                SubjectCover(vm, s, part, Modifier.weight(1f))
                            }
                            repeat(columns - row.size) { Spacer(Modifier.weight(1f)) }
                        }
                    }
                }
            }
        }
        if (soon.isNotEmpty()) {
            item { SectionHeader(vm.tr("subjects_soon")) }
            items(soon) { s ->
                NavRow(title = vm.tr(s.str("key")), locked = true, onClick = null)
            }
        }
    }
}

private fun subjectAccent(vm: AppViewModel, s: J): Accent =
    subjectStyle(s.str("icon"))?.accent(vm.mode) ?: vm.palette.accent

/**
 * The glow of the app icon with the headline in its typeface, the share of
 * finished exercises, and every subject's part in it.
 */
@Composable
private fun HomeHero(vm: AppViewModel, sum: ProgressSum, parts: List<Triple<Accent, Int, Int>>) {
    val palette = LocalPalette.current
    val display = LocalDisplayFont.current
    val pct = if (sum.totalEx == 0) 0 else sum.doneEx * 100 / sum.totalEx
    Column(
        Modifier
            .fillMaxWidth()
            .padding(horizontal = 16.dp, vertical = 8.dp)
            .clip(RoundedCornerShape(28.dp))
            .drawBehind {
                drawRect(
                    Brush.radialGradient(
                        listOf(palette.glowInner, palette.glowOuter),
                        center = Offset(size.width * 0.3f, size.height * 0.3f),
                        radius = maxOf(size.width, size.height) * 0.9f,
                    ),
                )
            }
            .padding(22.dp),
    ) {
        Text(vm.tr("welcome_title"), style = displayStyle(display, 38.sp, italic = true).copy(lineHeight = 40.sp), color = palette.onGlow)
        Spacer(Modifier.height(10.dp))
        Text(
            vm.tr(vm.platform.welcomeKey),
            style = MaterialTheme.typography.bodyMedium,
            color = palette.onGlow.copy(alpha = 0.72f),
            modifier = Modifier.widthIn(max = 520.dp),
        )
        Spacer(Modifier.height(18.dp))
        Row(verticalAlignment = Alignment.Bottom) {
            Text(vm.fmt("stats_pct_fmt", pct), style = displayStyle(display, 42.sp), color = palette.onGlow)
            Spacer(Modifier.width(12.dp))
            Column(Modifier.padding(bottom = 6.dp)) {
                Text(vm.fmt("stats_ex_fmt", sum.doneEx, sum.totalEx), style = MaterialTheme.typography.titleSmall, color = palette.onGlow)
                Kicker(vm.tr("stats_ex_label"), palette.onGlow.copy(alpha = 0.7f))
            }
        }
        Spacer(Modifier.height(18.dp))
        SpectrumBar(parts, palette.onGlow.copy(alpha = 0.12f), Modifier.fillMaxWidth())
    }
}

/**
 * A subject as a book cover: its colour, its glyph, and its timetable
 * abbreviation set large behind the title.
 */
@Composable
private fun SubjectCover(vm: AppViewModel, s: J, part: ProgressSum, modifier: Modifier = Modifier) {
    val accent = subjectAccent(vm, s)
    val ink = accent.onSolid
    val mark = subjectStyle(s.str("icon"))?.mark(vm.lang).orEmpty()
    Surface(
        onClick = { vm.goPage(s.str("target")) },
        color = accent.solid,
        contentColor = ink,
        shape = RoundedCornerShape(24.dp),
        modifier = modifier.height(168.dp),
    ) {
        Box(Modifier.clipToBounds()) {
            Text(
                mark,
                style = displayStyle(LocalDisplayFont.current, if (mark.length > 2) 56.sp else 74.sp, italic = true),
                color = ink.copy(alpha = 0.16f),
                maxLines = 1,
                softWrap = false,
                modifier = Modifier.align(Alignment.TopEnd).offset(x = 6.dp, y = if (mark.length > 2) (-6).dp else (-18).dp),
            )
            Column(Modifier.fillMaxSize().padding(16.dp)) {
                Box(Modifier.size(38.dp).clip(CircleShape).background(ink.copy(alpha = 0.16f)), contentAlignment = Alignment.Center) {
                    Icon(subjectIcon(s.str("icon")), null, tint = ink, modifier = Modifier.size(20.dp))
                }
                Spacer(Modifier.weight(1f))
                Text(vm.tr(s.str("key")), style = displayStyle(LocalDisplayFont.current, 17.sp), maxLines = 2, overflow = TextOverflow.Ellipsis)
                if (part.totalEx > 0) {
                    Row(Modifier.padding(top = 10.dp), verticalAlignment = Alignment.CenterVertically) {
                        SmoothProgress(part.doneEx / part.totalEx.toFloat(), Modifier.weight(1f).height(5.dp), ink, ink.copy(alpha = 0.24f))
                        Spacer(Modifier.width(10.dp))
                        Text("${part.doneEx} / ${part.totalEx}", style = MaterialTheme.typography.labelSmall, color = ink.copy(alpha = 0.85f))
                    }
                }
            }
        }
    }
}

@Composable
private fun ChangelogCard(vm: AppViewModel) {
    val palette = LocalPalette.current
    val accent = LocalAccent.current
    val langKey = if (vm.lang == UiLang.En) "en" else "cs"
    val blocks = vm.content.changelog.take(3)
    AppCard(Modifier.fillMaxWidth().padding(horizontal = 16.dp, vertical = 8.dp)) {
        Column(Modifier.padding(20.dp)) {
            Row(verticalAlignment = Alignment.CenterVertically) {
                Icon(Icons.Filled.AutoAwesome, null, tint = accent.text, modifier = Modifier.size(18.dp))
                Spacer(Modifier.width(8.dp))
                Kicker(vm.tr("changelog_title"), accent.text, Modifier.weight(1f))
                val date = blocks.firstOrNull()?.str("date").orEmpty()
                if (date.isNotEmpty()) {
                    Text(date, style = MaterialTheme.typography.labelMedium, color = palette.ink3)
                }
            }
            Spacer(Modifier.height(12.dp))
            blocks.forEachIndexed { index, block ->
                if (index > 0) Spacer(Modifier.height(8.dp))
                block.strs(langKey).ifEmpty { block.strs("cs") }.forEach { line ->
                    Row(Modifier.padding(bottom = 6.dp)) {
                        Box(Modifier.padding(top = 8.dp).size(5.dp).clip(CircleShape).background(accent.solid))
                        Spacer(Modifier.width(10.dp))
                        Text(line, style = MaterialTheme.typography.bodyMedium, color = palette.ink2)
                    }
                }
            }
            Spacer(Modifier.height(8.dp))
            PrimaryButton(vm.tr("changelog_dismiss")) { vm.dismissChangelog() }
        }
    }
}

@Composable
private fun UpdateBanner(vm: AppViewModel) {
    AppCard(Modifier.fillMaxWidth().padding(horizontal = 16.dp, vertical = 4.dp), radius = 16.dp) {
        Row(Modifier.padding(horizontal = 16.dp, vertical = 12.dp), verticalAlignment = Alignment.CenterVertically) {
            Text(
                vm.fmt(vm.update.messageKey, *(listOfNotNull(vm.update.messageArg).toTypedArray())),
                style = MaterialTheme.typography.bodyMedium,
                modifier = Modifier.weight(1f),
            )
            if (vm.update.canInstall) {
                Spacer(Modifier.width(8.dp))
                PrimaryButton(vm.tr("update_install")) { vm.installUpdate() }
            }
        }
    }
}

private fun subjectIcon(icon: String): ImageVector = when (icon) {
    "de" -> Icons.Filled.Translate
    "wifi" -> Icons.Filled.Wifi
    "chip" -> Icons.Filled.Memory
    "cz" -> Icons.Filled.AutoStories
    "people" -> Icons.Filled.Groups
    "uk" -> Icons.Filled.Forum
    "flask" -> Icons.Filled.Science
    "bolt" -> Icons.Filled.Bolt
    "function" -> Icons.Filled.Functions
    "ruler" -> Icons.Filled.Architecture
    "code" -> Icons.Filled.Terminal
    "brush" -> Icons.Filled.Brush
    "apps" -> Icons.Filled.Apps
    else -> Icons.Filled.Lock
}

/** Subject glyph on its own colour, for the statistics table. */
@Composable
private fun SubjectIcon(vm: AppViewModel, s: J) {
    if (s.bool("open")) {
        Badge(icon = subjectIcon(s.str("icon")), size = 38.dp, accent = subjectAccent(vm, s))
    } else {
        Badge(icon = Icons.Filled.Lock, filled = false, size = 38.dp)
    }
}

@Composable
fun StatsScreen(vm: AppViewModel) {
    val palette = LocalPalette.current
    val sum = summarize(vm.content, vm.progress)
    val pct = if (sum.totalEx == 0) 0f else sum.doneEx / sum.totalEx.toFloat()
    // Open subjects by how far along they are, locked ones last.
    val rows = vm.content.subjects.map { it to subjectSum(vm, it) }
        .sortedWith(compareBy({ !it.first.bool("open") }, { -share(it.second) }))
    RootScaffold(vm, vm.tr("stats_title")) {
        item {
            AppCard(Modifier.fillMaxWidth().padding(horizontal = 16.dp, vertical = 8.dp)) {
                Column(Modifier.padding(20.dp)) {
                    Row(Modifier.fillMaxWidth()) {
                        Metric(vm.tr("stats_pct_label"), vm.fmt("stats_pct_fmt", (pct * 100).toInt()), Modifier.weight(1f))
                        Metric(vm.tr("stats_ex_label"), vm.fmt("stats_ex_fmt", sum.doneEx, sum.totalEx), Modifier.weight(1f))
                        Metric(vm.tr("stats_units_label"), vm.fmt("stats_units_fmt", sum.doneUnits, sum.openUnits), Modifier.weight(1f))
                    }
                    Spacer(Modifier.height(18.dp))
                    SpectrumBar(
                        rows.filter { it.first.bool("open") }.map { (s, part) -> Triple(subjectAccent(vm, s), part.doneEx, part.totalEx) },
                        palette.sunken,
                        Modifier.fillMaxWidth(),
                    )
                }
            }
        }
        item { SectionHeader(vm.tr("stats_section")) }
        item {
            AppCard(Modifier.fillMaxWidth().padding(horizontal = 16.dp)) {
                Column {
                    rows.forEachIndexed { index, (s, part) ->
                        val open = s.bool("open")
                        val counted = open && part.totalEx > 0
                        if (index > 0) HorizontalDivider(Modifier.padding(start = 66.dp), color = palette.line)
                        Row(Modifier.padding(horizontal = 14.dp, vertical = 12.dp), verticalAlignment = Alignment.CenterVertically) {
                            SubjectIcon(vm, s)
                            Spacer(Modifier.width(14.dp))
                            Column(Modifier.weight(1f)) {
                                Row(verticalAlignment = Alignment.CenterVertically) {
                                    Text(
                                        vm.tr(s.str("key")),
                                        style = MaterialTheme.typography.titleMedium,
                                        color = if (open) palette.ink else palette.ink3,
                                        maxLines = 1,
                                        overflow = TextOverflow.Ellipsis,
                                        modifier = Modifier.weight(1f),
                                    )
                                    Text(
                                        if (counted) vm.fmt("stats_ex_fmt", part.doneEx, part.totalEx) else vm.tr("stats_locked"),
                                        style = MaterialTheme.typography.labelMedium,
                                        color = palette.ink2,
                                    )
                                }
                                if (counted) {
                                    Spacer(Modifier.height(8.dp))
                                    SmoothProgress(share(part), Modifier.fillMaxWidth().height(6.dp), subjectAccent(vm, s).solid)
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

private fun share(part: ProgressSum): Float = if (part.totalEx == 0) 0f else part.doneEx / part.totalEx.toFloat()

@Composable
private fun Metric(label: String, value: String, modifier: Modifier) {
    Column(modifier) {
        Text(value, style = MaterialTheme.typography.headlineSmall, maxLines = 1)
        Kicker(label)
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
        Route.SwMap -> tally(c.sw, p::swDone)
        Route.Sw2Map -> tally(c.sw2, p::sw2Done)
        Route.Sw3Map -> tally(c.sw3, p::sw3Done)
        Route.Sw4Map -> tally(c.sw4, p::sw4Done)
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
        "swyears", "swmap", "sw2map", "sw3map", "sw4map" -> {
            sum.totalEx = vm.content.sw.size + vm.content.sw2.size + vm.content.sw3.size + vm.content.sw4.size
            sum.doneEx = vm.content.sw.count { vm.progress.swDone(it.int("id")) } +
                vm.content.sw2.count { vm.progress.sw2Done(it.int("id")) } +
                vm.content.sw3.count { vm.progress.sw3Done(it.int("id")) } +
                vm.content.sw4.count { vm.progress.sw4Done(it.int("id")) }
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

@OptIn(ExperimentalLayoutApi::class)
@Composable
fun SettingsScreen(vm: AppViewModel) {
    val palette = LocalPalette.current
    @Composable
    fun card(title: String, content: @Composable () -> Unit) {
        AppCard(Modifier.fillMaxWidth().padding(horizontal = 16.dp, vertical = 6.dp)) {
            Column(Modifier.padding(horizontal = 18.dp, vertical = 16.dp)) {
                Kicker(title, modifier = Modifier.padding(bottom = 8.dp))
                content()
            }
        }
    }
    RootScaffold(vm, vm.tr("settings_title"), showSearch = false) {
        item {
            card(vm.tr("mode")) {
                FlowRow(horizontalArrangement = Arrangement.spacedBy(10.dp)) {
                    SegChip(vm.tr("mode_light"), vm.mode == ColorMode.Light) { vm.applyMode(ColorMode.Light) }
                    SegChip(vm.tr("mode_dark"), vm.mode == ColorMode.Dark) { vm.applyMode(ColorMode.Dark) }
                }
            }
        }
        item {
            card(vm.tr("language")) {
                FlowRow(horizontalArrangement = Arrangement.spacedBy(10.dp)) {
                    SegChip("Čeština", vm.lang == UiLang.Cs) { vm.applyLang(UiLang.Cs) }
                    SegChip("English", vm.lang == UiLang.En) { vm.applyLang(UiLang.En) }
                }
            }
        }
        item {
            card(vm.tr("updates")) {
                Text(
                    vm.fmt(vm.update.messageKey, *(listOfNotNull(vm.update.messageArg).toTypedArray())),
                    style = MaterialTheme.typography.bodyMedium,
                    color = palette.ink2,
                )
                Spacer(Modifier.height(10.dp))
                Row(horizontalArrangement = Arrangement.spacedBy(8.dp)) {
                    PillButton(vm.tr("update_check")) { vm.checkUpdate(true) }
                    if (vm.update.canInstall) {
                        PrimaryButton(vm.tr("update_install")) { vm.installUpdate() }
                    }
                }
            }
        }
        item {
            Column(Modifier.padding(horizontal = 20.dp, vertical = 18.dp)) {
                Text("Graduately", style = displayStyle(LocalDisplayFont.current, 24.sp, italic = true), color = palette.ink)
                Text(
                    "Build ${vm.platform.versionName} · ${vm.platform.commit.take(7)}",
                    style = MaterialTheme.typography.bodySmall,
                    color = palette.ink3,
                )
            }
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
            Surface(color = LocalPalette.current.paper) {
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
    add(vm.tr("Programové vybavení"), vm.tr("search_lesson"), "swyears", extra = "office windows excel mysql")
    add(vm.tr("sw_year1"), vm.tr("search_lesson"), "swmap", extra = "rocnik")
    add(vm.tr("sw_year2"), vm.tr("search_lesson"), "sw2map", extra = "rocnik")
    add(vm.tr("sw_year3"), vm.tr("search_lesson"), "sw3map", extra = "rocnik")
    add(vm.tr("sw_year4"), vm.tr("search_lesson"), "sw4map", extra = "rocnik")
    vm.content.sw.forEach { l ->
        add(vm.tr(l.str("titleKey")), vm.tr("search_lesson"), "swunit${l.int("id")}", extra = "windows")
        add(vm.tr(l.str("titleKey")), vm.tr("search_exercise"), "swex${l.int("id")}")
    }
    vm.content.sw2.forEach { l ->
        add(vm.tr(l.str("titleKey")), vm.tr("search_lesson"), "sw2unit${l.int("id")}", extra = "word excel")
        add(vm.tr(l.str("titleKey")), vm.tr("search_exercise"), "sw2ex${l.int("id")}")
    }
    vm.content.sw3.forEach { l ->
        add(vm.tr(l.str("titleKey")), vm.tr("search_lesson"), "sw3unit${l.int("id")}", extra = "access mysql")
        add(vm.tr(l.str("titleKey")), vm.tr("search_exercise"), "sw3ex${l.int("id")}")
    }
    vm.content.sw4.forEach { l ->
        add(vm.tr(l.str("titleKey")), vm.tr("search_lesson"), "sw4unit${l.int("id")}", extra = "wordpress")
        add(vm.tr(l.str("titleKey")), vm.tr("search_exercise"), "sw4ex${l.int("id")}")
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
            val next = rows.indexOfFirst { !it.done }
            item {
                Row(Modifier.padding(horizontal = 16.dp, vertical = 8.dp), verticalAlignment = Alignment.CenterVertically) {
                    SmoothProgress(if (rows.isEmpty()) 0f else rows.count { it.done } / rows.size.toFloat(), Modifier.weight(1f).height(6.dp))
                    Spacer(Modifier.width(10.dp))
                    Text("${rows.count { it.done }} / ${rows.size}", style = MaterialTheme.typography.labelMedium, color = LocalPalette.current.ink2)
                }
            }
            items(rows.size) { i ->
                val row = rows[i]
                NavRow(title = row.title, subtitle = row.subtitle, done = row.done, index = i + 1, current = i == next, onClick = { vm.go(row.route) })
            }
        }
    }
}

@Composable
fun RoadmapScreen(vm: AppViewModel) {
    DetailScaffold(vm, vm.tr("roadmap_title"), vm.tr("roadmap_sub")) { inner ->
        LazyColumn(contentPadding = PaddingValues(top = inner.calculateTopPadding(), bottom = inner.calculateBottomPadding() + 16.dp)) {
            items(vm.content.german.size) { i ->
                val u = vm.content.german[i]
                val unlocked = u.bool("unlocked")
                val names = u.strs("names")
                val doneCount = names.indices.count { vm.progress.germanDone(u.int("id"), it + 1) }
                val done = unlocked && names.isNotEmpty() && doneCount == names.size
                NavRow(
                    title = u.str("title"),
                    subtitle = if (unlocked) vm.fmt("stats_ex_fmt", doneCount, names.size) else null,
                    done = done,
                    locked = !unlocked,
                    index = i + 1,
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
            val next = names.indices.firstOrNull { !vm.progress.germanDone(unitId, it + 1) }
            items(names.size) { i ->
                NavRow(title = names[i], done = vm.progress.germanDone(unitId, i + 1), index = i + 1, current = i == next) { vm.go(Route.GermanEx(unitId, i + 1)) }
            }
            if (u.has("branch")) {
                item { Spacer(Modifier.height(12.dp)) }
                item {
                    NavRow(
                        title = vm.tr("Vokabeltraining"),
                        subtitle = vm.tr("sub_translate"),
                        leading = { Badge(icon = if (vm.progress.vocabDone(unitId)) Icons.Filled.Check else Icons.AutoMirrored.Filled.MenuBook) },
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
fun SwYearsScreen(vm: AppViewModel) {
    DetailScaffold(vm, vm.tr("Programové vybavení"), vm.tr("sw_years_sub")) { inner ->
        LazyColumn(contentPadding = PaddingValues(top = inner.calculateTopPadding(), bottom = inner.calculateBottomPadding() + 16.dp)) {
            item {
                NavRow(title = vm.tr("sw_year1"), subtitle = vm.tr("sw_sub"), leading = { LeadingBadge("1") }, progress = routeProgress(vm, Route.SwMap)) { vm.go(Route.SwMap) }
                NavRow(title = vm.tr("sw_year2"), subtitle = vm.tr("sw2_sub"), leading = { LeadingBadge("2") }, progress = routeProgress(vm, Route.Sw2Map)) { vm.go(Route.Sw2Map) }
                NavRow(title = vm.tr("sw_year3"), subtitle = vm.tr("sw3_sub"), leading = { LeadingBadge("3") }, progress = routeProgress(vm, Route.Sw3Map)) { vm.go(Route.Sw3Map) }
                NavRow(title = vm.tr("sw_year4"), subtitle = vm.tr("sw4_sub"), leading = { LeadingBadge("4") }, progress = routeProgress(vm, Route.Sw4Map)) { vm.go(Route.Sw4Map) }
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
                    leading = { Badge(icon = if (vm.progress.bookQuiz(id)) Icons.Filled.Check else Icons.Filled.Quiz) },
                ) { vm.go(Route.BookQuiz(id)) }
            }
            item {
                NavRow(
                    title = vm.tr(b.str("plotTitle")),
                    subtitle = vm.tr(b.str("plotSub")),
                    leading = { Badge(icon = if (vm.progress.bookPlot(id)) Icons.Filled.Check else Icons.Filled.FormatListNumbered) },
                ) { vm.go(Route.BookPlot(id)) }
            }
            items(b.arr("notes")) { note ->
                SectionHeader(note.str("title"))
                AppCard(Modifier.fillMaxWidth().padding(horizontal = 16.dp)) {
                    Column(Modifier.padding(16.dp)) {
                        note.strs("lines").forEachIndexed { i, line ->
                            if (i > 0) HorizontalDivider(Modifier.padding(vertical = 10.dp), color = LocalPalette.current.line)
                            Text(line, style = MaterialTheme.typography.bodyLarge)
                        }
                    }
                }
            }
        }
    }
}

// ---------------------------------------------------------------------------
// Lesson slides

/** One explanation slide: label, title, tip, then the points as a numbered list set apart by rules. */
@Composable
fun SlideContent(slide: J) {
    val palette = LocalPalette.current
    val accent = LocalAccent.current
    Column {
        Kicker(slide.str("kicker"), accent.text)
        Text(slide.str("title"), style = MaterialTheme.typography.headlineMedium, modifier = Modifier.padding(top = 6.dp, bottom = 14.dp))
        slide.strOrNull("tip")?.let { tip ->
            Row(
                Modifier.fillMaxWidth().padding(bottom = 8.dp).clip(RoundedCornerShape(16.dp)).background(accent.soft).padding(14.dp),
                verticalAlignment = Alignment.CenterVertically,
            ) {
                Icon(Icons.Outlined.Lightbulb, null, tint = accent.text)
                Spacer(Modifier.width(10.dp))
                Text(tip, style = MaterialTheme.typography.bodyMedium)
            }
        }
        val lines = slide.strs("lines")
        lines.forEachIndexed { index, line ->
            val numbered = Regex("""^(\d+)\.\s+(.*)$""").find(line)
            val badge = numbered?.groupValues?.get(1) ?: "${index + 1}"
            val text = numbered?.groupValues?.get(2) ?: line
            Row(Modifier.fillMaxWidth().padding(vertical = 14.dp)) {
                Text(
                    badge,
                    style = displayStyle(LocalDisplayFont.current, 20.sp),
                    color = accent.text,
                    modifier = Modifier.alignByBaseline().widthIn(min = 34.dp),
                )
                Text(text, style = MaterialTheme.typography.bodyLarge, modifier = Modifier.alignByBaseline().weight(1f))
            }
            if (index < lines.lastIndex) HorizontalDivider(color = palette.line)
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
                                .size(width = if (active) 22.dp else 6.dp, height = 6.dp)
                                .clip(CircleShape)
                                .background(if (active) LocalAccent.current.solid else LocalPalette.current.ink3.copy(alpha = 0.35f)),
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
