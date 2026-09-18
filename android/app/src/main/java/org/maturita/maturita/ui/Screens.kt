package org.maturita.maturita.ui

import androidx.compose.foundation.Canvas
import androidx.compose.foundation.background
import androidx.compose.foundation.border
import androidx.compose.foundation.clickable
import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.fillMaxHeight
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.size
import androidx.compose.foundation.layout.width
import androidx.compose.foundation.rememberScrollState
import androidx.compose.foundation.shape.CircleShape
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.foundation.text.BasicTextField
import androidx.compose.foundation.verticalScroll
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.draw.alpha
import androidx.compose.ui.draw.clip
import androidx.compose.ui.geometry.Offset
import androidx.compose.ui.graphics.SolidColor
import androidx.compose.ui.text.TextStyle
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.text.style.TextAlign
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
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

@Composable
fun WelcomeScreen(vm: AppViewModel) {
    val p = vm.palette
    Column(
        Modifier
            .fillMaxSize()
            .verticalScroll(rememberScrollState())
            .padding(horizontal = 24.dp, vertical = 16.dp),
    ) {
        Text("maturita.c", color = p.text, fontWeight = FontWeight.Bold, fontSize = 15.sp)
        Text(
            "sestavení ${org.maturita.maturita.BuildConfig.VERSION_NAME} · ${org.maturita.maturita.BuildConfig.COMMIT.take(7)}",
            color = p.overlay,
            fontSize = 12.sp,
        )
        Spacer(Modifier.height(12.dp))
        Row(Modifier.fillMaxWidth(), verticalAlignment = Alignment.CenterVertically) {
            Column(Modifier.weight(1f)) {
                Text(vm.tr("welcome_title"), color = p.text, fontSize = 36.sp, fontWeight = FontWeight.Bold, lineHeight = 40.sp)
                Spacer(Modifier.height(12.dp))
                Text(
                    vm.tr("welcome_body_android"),
                    color = p.subtext,
                    fontSize = 16.sp,
                )
                Spacer(Modifier.height(22.dp))
                PrimaryButton(vm.tr("continue"), p) { vm.go(Route.Subjects) }
            }
            Spacer(Modifier.width(12.dp))
            Box(
                Modifier
                    .size(108.dp)
                    .clip(CircleShape)
                    .background(p.mantle)
                    .border(1.dp, p.text.copy(0.08f), CircleShape),
                contentAlignment = Alignment.Center,
            ) {
                Column(horizontalAlignment = Alignment.CenterHorizontally) {
                    Text(vm.tr("welcome_stat_value"), color = p.text, fontSize = 28.sp, fontWeight = FontWeight.Bold)
                    Text(vm.tr("welcome_stat_label"), color = p.subtext, fontSize = 11.sp, fontWeight = FontWeight.SemiBold, textAlign = TextAlign.Center)
                }
            }
        }
        Spacer(Modifier.height(28.dp))
        FeatureCard(vm, "welcome_feat1_title", "welcome_feat1_body", "welcome_feat1_link")
        Spacer(Modifier.height(12.dp))
        FeatureCard(vm, "welcome_feat2_title", "welcome_feat2_body", "welcome_feat2_link")
    }
}

@Composable
private fun FeatureCard(vm: AppViewModel, t: String, b: String, l: String) {
    val p = vm.palette
    CardBox(p, Modifier.fillMaxWidth()) {
        Column {
            Text(vm.tr(t), color = p.text, fontSize = 18.sp, fontWeight = FontWeight.Bold)
            Spacer(Modifier.height(6.dp))
            Text(vm.tr(b), color = p.subtext, fontSize = 15.sp)
            Spacer(Modifier.height(10.dp))
            PillButton(vm.tr(l), p) { vm.go(Route.Subjects) }
        }
    }
}

@Composable
fun SubjectsScreen(vm: AppViewModel) {
    val p = vm.palette
    Column(Modifier.fillMaxSize().padding(horizontal = 16.dp)) {
        PageTop(vm, { vm.back() }, vm.tr("subjects_title"), vm.tr("subjects_sub"))
        Column(
            Modifier
                .weight(1f)
                .fillMaxWidth()
                .verticalScroll(rememberScrollState())
                .padding(bottom = 24.dp),
            verticalArrangement = Arrangement.spacedBy(12.dp),
        ) {
            vm.content.subjects.chunked(2).forEach { row ->
                Row(Modifier.fillMaxWidth(), horizontalArrangement = Arrangement.spacedBy(12.dp)) {
                    row.forEach { s ->
                        val open = s.bool("open")
                        Box(
                            Modifier
                                .weight(1f)
                                .height(132.dp)
                                .clip(RoundedCornerShape(20.dp))
                                .background(p.mantle)
                                .border(1.dp, if (open) p.text.copy(0.08f) else p.text.copy(0.04f), RoundedCornerShape(20.dp))
                                .alpha(if (open) 1f else 0.48f)
                                .then(if (open) Modifier.clickable { vm.goPage(s.str("target")) } else Modifier)
                                .padding(16.dp),
                            contentAlignment = Alignment.Center,
                        ) {
                            Column(horizontalAlignment = Alignment.CenterHorizontally) {
                                when (s.str("icon")) {
                                    "de" -> GermanFlag(44.dp)
                                    "wifi" -> WifiIcon(p.text, 36.dp)
                                    "chip" -> ChipIcon(p.text, 36.dp)
                                    "cz" -> CzechFlag(44.dp)
                                    else -> LockIcon(p.overlay, 22.dp)
                                }
                                Spacer(Modifier.height(10.dp))
                                Text(vm.tr(s.str("key")), color = p.text, fontSize = 13.sp, fontWeight = FontWeight.SemiBold, textAlign = TextAlign.Center)
                            }
                        }
                    }
                    if (row.size == 1) Spacer(Modifier.weight(1f))
                }
            }
        }
    }
}

@Composable
fun SettingsSheet(vm: AppViewModel) {
    val p = vm.palette
    Column(
        Modifier
            .fillMaxWidth()
            .verticalScroll(rememberScrollState())
            .padding(20.dp),
    ) {
        Row(verticalAlignment = Alignment.CenterVertically) {
            Text(
                vm.tr("settings_title"),
                color = p.text,
                fontSize = 22.sp,
                fontWeight = FontWeight.Bold,
                modifier = Modifier.weight(1f),
            )
            PillButton(vm.tr("back"), p) { vm.settingsOpen = false }
        }
        Spacer(Modifier.height(16.dp))
        Text(vm.tr("mode"), color = p.subtext, fontSize = 12.sp, fontWeight = FontWeight.SemiBold)
        Spacer(Modifier.height(6.dp))
        Segmented(p) {
            SegChip(vm.tr("mode_dark"), vm.mode == ColorMode.Dark, p, Modifier.weight(1f)) { vm.applyMode(ColorMode.Dark) }
            SegChip(vm.tr("mode_light"), vm.mode == ColorMode.Light, p, Modifier.weight(1f)) { vm.applyMode(ColorMode.Light) }
        }
        Spacer(Modifier.height(16.dp))
        Text(vm.tr("theme"), color = p.subtext, fontSize = 12.sp, fontWeight = FontWeight.SemiBold)
        Spacer(Modifier.height(8.dp))
        themeNames.forEachIndexed { i, name ->
            val id = ThemeId.entries.getOrNull(i) ?: return@forEachIndexed
            val sw = themePalette(id, vm.mode)
            val sel = vm.themeId == id
            Row(
                Modifier
                    .fillMaxWidth()
                    .clip(RoundedCornerShape(14.dp))
                    .background(if (sel) p.surface1 else p.surface0)
                    .clickable { vm.setTheme(id) }
                    .padding(10.dp),
                verticalAlignment = Alignment.CenterVertically,
            ) {
                Canvas(Modifier.size(36.dp)) {
                    drawCircle(sw.base)
                    drawCircle(sw.accent, 5f, Offset(size.width * 0.28f, size.height * 0.5f))
                    drawCircle(sw.accent3, 5f, Offset(size.width * 0.5f, size.height * 0.5f))
                    drawCircle(sw.success, 5f, Offset(size.width * 0.72f, size.height * 0.5f))
                    if (sel) drawCircle(p.accent, size.minDimension / 2 - 1f, style = androidx.compose.ui.graphics.drawscope.Stroke(2.2f))
                }
                Spacer(Modifier.width(10.dp))
                Text(name, color = p.text, fontWeight = FontWeight.Medium)
            }
            Spacer(Modifier.height(6.dp))
        }
        Spacer(Modifier.height(10.dp))
        Text(vm.tr("language"), color = p.subtext, fontSize = 12.sp, fontWeight = FontWeight.SemiBold)
        Spacer(Modifier.height(6.dp))
        Segmented(p) {
            SegChip("Čeština", vm.lang == UiLang.Cs, p, Modifier.weight(1f)) { vm.applyLang(UiLang.Cs) }
            SegChip("English", vm.lang == UiLang.En, p, Modifier.weight(1f)) { vm.applyLang(UiLang.En) }
        }
        Spacer(Modifier.height(16.dp))
        Text(vm.tr("updates"), color = p.subtext, fontSize = 12.sp, fontWeight = FontWeight.SemiBold)
        Spacer(Modifier.height(6.dp))
        Text(
            vm.fmt(vm.update.messageKey, *(listOfNotNull(vm.update.messageArg).toTypedArray())),
            color = p.subtext,
            fontSize = 14.sp,
        )
        Spacer(Modifier.height(10.dp))
        Row(horizontalArrangement = Arrangement.spacedBy(8.dp)) {
            PillButton(vm.tr("update_check"), p) { vm.checkUpdate(true) }
            if (vm.update.canInstall) PrimaryButton(vm.tr("update_install"), p) { vm.installUpdate() }
        }
        Spacer(Modifier.height(24.dp))
    }
}

@Composable
fun StatsScreen(vm: AppViewModel) {
    val p = vm.palette
    val sum = summarize(vm.content, vm.progress)
    val pct = if (sum.totalEx == 0) 0 else (100 * sum.doneEx / sum.totalEx)
    Column(Modifier.fillMaxSize().padding(horizontal = 16.dp)) {
        PageTop(vm, { vm.back() }, vm.tr("stats_title"), vm.tr("stats_sub"))
        Column(Modifier.weight(1f).verticalScroll(rememberScrollState()).padding(bottom = 24.dp)) {
            Row(Modifier.fillMaxWidth(), horizontalArrangement = Arrangement.spacedBy(10.dp)) {
                Metric(p, vm.tr("stats_ex_label"), vm.fmt("stats_ex_fmt", sum.doneEx, sum.totalEx), Modifier.weight(1f))
                Metric(p, vm.tr("stats_pct_label"), vm.fmt("stats_pct_fmt", pct), Modifier.weight(1f))
                Metric(p, vm.tr("stats_units_label"), vm.fmt("stats_units_fmt", sum.doneUnits, sum.openUnits), Modifier.weight(1f))
            }
            Spacer(Modifier.height(10.dp))
            ProgressBar(pct / 100f, p, 8.dp)
            Spacer(Modifier.height(20.dp))
            Text(vm.tr("stats_section"), color = p.subtext, fontSize = 12.sp, fontWeight = FontWeight.SemiBold)
            Spacer(Modifier.height(8.dp))
            vm.content.subjects.forEach { s ->
                val open = s.bool("open")
                val part = subjectSum(vm, s)
                CardBox(p, Modifier.fillMaxWidth().padding(bottom = 10.dp).alpha(if (open) 1f else 0.55f)) {
                    Column {
                        Text(vm.tr(s.str("key")), color = p.text, fontWeight = FontWeight.Bold)
                        Spacer(Modifier.height(4.dp))
                        Text(
                            if (open) vm.fmt("stats_ex_fmt", part.doneEx, part.totalEx) else vm.tr("stats_locked"),
                            color = p.subtext,
                            fontSize = 13.sp,
                        )
                        if (open && part.totalEx > 0) {
                            Spacer(Modifier.height(8.dp))
                            ProgressBar(part.doneEx / part.totalEx.toFloat(), p, 6.dp)
                        }
                    }
                }
            }
        }
    }
}

@Composable
private fun ProgressBar(fraction: Float, p: org.maturita.maturita.data.Palette, barHeight: androidx.compose.ui.unit.Dp) {
    Box(
        Modifier
            .fillMaxWidth()
            .height(barHeight)
            .clip(CircleShape)
            .background(p.surface1),
    ) {
        Box(
            Modifier
                .fillMaxHeight()
                .fillMaxWidth(fraction.coerceIn(0f, 1f))
                .background(p.accent),
        )
    }
}

@Composable
private fun Metric(p: org.maturita.maturita.data.Palette, label: String, value: String, modifier: Modifier) {
    CardBox(p, modifier) {
        Column {
            Text(label, color = p.subtext, fontSize = 12.sp, fontWeight = FontWeight.SemiBold)
            Text(value, color = p.text, fontSize = 22.sp, fontWeight = FontWeight.Bold)
        }
    }
}

private fun subjectSum(vm: AppViewModel, s: J): ProgressSum {
    val key = s.str("key")
    val sum = ProgressSum()
    when (s.str("target")) {
        "roadmap" -> vm.content.german.filter { it.bool("unlocked") }.forEach { u ->
            val n = u.strs("names").size
            sum.totalEx += n
            sum.doneEx += (1..n).count { vm.progress.germanDone(u.int("id"), it) }
        }
        "netyears" -> {
            sum.totalEx = vm.content.netLessons.size
            sum.doneEx = vm.content.netLessons.count { vm.progress.netDone(it.int("id")) }
        }
        "hwmap" -> {
            sum.totalEx = vm.content.hw.size
            sum.doneEx = vm.content.hw.count { vm.progress.hwDone(it.int("id")) }
        }
        "czechmap" -> {
            sum.totalEx = vm.content.mluvnice.size
            sum.doneEx = vm.content.mluvnice.count { vm.progress.mluvDone(it.int("id")) }
        }
    }
    return sum
}

@Composable
fun SearchOverlay(vm: AppViewModel) {
    val p = vm.palette
    val q = normalizeAnswer(vm.searchQuery)
    val hits = buildSearch(vm).filter { q.isEmpty() || normalizeAnswer(it.hay).contains(q) }.take(24)
    Column(
        Modifier
            .fillMaxSize()
            .background(p.base.copy(alpha = 0.96f))
            .padding(16.dp),
    ) {
        Row(verticalAlignment = Alignment.CenterVertically) {
            Box(
                Modifier
                    .weight(1f)
                    .clip(RoundedCornerShape(16.dp))
                    .background(p.mantle)
                    .padding(14.dp),
            ) {
                if (vm.searchQuery.isEmpty()) Text(vm.tr("search_placeholder"), color = p.overlay)
                BasicTextField(
                    vm.searchQuery,
                    { vm.searchQuery = it },
                    textStyle = TextStyle(color = p.text, fontSize = 16.sp),
                    cursorBrush = SolidColor(p.accent),
                    modifier = Modifier.fillMaxWidth(),
                    singleLine = true,
                )
            }
            Spacer(Modifier.width(8.dp))
            PillButton(vm.tr("back"), p) { vm.searchOpen = false }
        }
        Spacer(Modifier.height(12.dp))
        if (hits.isEmpty()) {
            Text(if (q.isEmpty()) vm.tr("search_hint") else vm.tr("search_empty"), color = p.subtext)
        } else {
            Column(Modifier.weight(1f).verticalScroll(rememberScrollState()), verticalArrangement = Arrangement.spacedBy(8.dp)) {
                hits.forEach { hit ->
                    CardBox(p, Modifier.fillMaxWidth().clickable {
                        if (!hit.locked) {
                            vm.goPage(hit.target)
                            vm.searchOpen = false
                        }
                    }.alpha(if (hit.locked) 0.5f else 1f)) {
                        Column {
                            Text(hit.title, color = p.text, fontWeight = FontWeight.SemiBold)
                            Text(hit.sub, color = p.subtext, fontSize = 13.sp)
                        }
                    }
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
    add(vm.tr("search_home"), vm.tr("search_page"), "welcome")
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
    vm.content.hw.forEach { l ->
        add(vm.tr(l.str("titleKey")), vm.tr("search_lesson"), "hwunit${l.int("id")}", extra = "hardware")
        add(vm.tr(l.str("titleKey")), vm.tr("search_exercise"), "hwex${l.int("id")}")
    }
    add(vm.tr("Mluvnice"), vm.tr("search_lesson"), "mluvnice")
    add(vm.tr("Maturitní četba"), vm.tr("search_book"), "readinglist")
    vm.content.books.forEach { b ->
        add(b.str("title"), vm.tr("search_book"), b.str("page"))
    }
    return out
}

@Composable
fun RoadmapScreen(vm: AppViewModel) {
    val p = vm.palette
    val current = vm.content.german.indexOfFirst { u ->
        u.bool("unlocked") && u.strs("names").indices.any { !vm.progress.germanDone(u.int("id"), it + 1) }
    }
    Column(Modifier.fillMaxSize().padding(horizontal = 12.dp)) {
        PageTop(vm, { vm.back() }, vm.tr("roadmap_title"), vm.tr("roadmap_sub"))
        PathMap(
            modifier = Modifier.weight(1f),
            nodes = vm.content.german.mapIndexed { i, u ->
                val unlocked = u.bool("unlocked")
                val names = u.strs("names")
                val done = unlocked && names.isNotEmpty() && names.indices.all { vm.progress.germanDone(u.int("id"), it + 1) }
                MapNode(
                    id = u.str("page"),
                    label = u.str("title"),
                    locked = !unlocked,
                    done = done,
                    current = i == current,
                    finish = i == vm.content.german.lastIndex,
                    onClick = if (unlocked) ({ vm.go(Route.UnitMap(u.int("id"))) }) else null,
                )
            },
            palette = p,
            litUntil = 2f,
        )
    }
}

@Composable
fun UnitMapScreen(vm: AppViewModel, unitId: Int) {
    val u = vm.content.germanUnit(unitId) ?: return
    val p = vm.palette
    val names = u.strs("names")
    Column(Modifier.fillMaxSize().padding(horizontal = 12.dp)) {
        PageTop(vm, { vm.back() }, u.str("title"), vm.tr(u.str("sub")))
        val next = names.indices.firstOrNull { !vm.progress.germanDone(unitId, it + 1) }
        PathMap(
            modifier = Modifier.weight(1f),
            nodes = names.mapIndexed { i, name ->
                val n = i + 1
                MapNode(
                    id = "e$n",
                    label = name,
                    locked = false,
                    done = vm.progress.germanDone(unitId, n),
                    current = i == next,
                    onClick = { vm.go(Route.GermanEx(unitId, n)) },
                )
            },
            palette = p,
            litUntil = names.indices.count { vm.progress.germanDone(unitId, it + 1) }.toFloat().coerceAtMost((names.size - 1).toFloat()),
            nodeSize = 58f,
            spac = 140f,
            mx = 70f,
            lift = if (u.has("branch")) 110f else 0f,
        )
        if (u.has("branch")) {
            Box(Modifier.fillMaxWidth().padding(bottom = 16.dp), contentAlignment = Alignment.Center) {
                PrimaryButton(u.str("vocab").let { vm.tr("Vokabeltraining") }, p) { vm.go(Route.Vocab(unitId)) }
            }
        }
    }
}

@Composable
fun NetYearsScreen(vm: AppViewModel) {
    val p = vm.palette
    Column(Modifier.fillMaxSize().padding(horizontal = 16.dp)) {
        PageTop(vm, { vm.back() }, vm.tr("net_years_title"), vm.tr("net_years_sub"))
        Column(Modifier.verticalScroll(rememberScrollState()), verticalArrangement = Arrangement.spacedBy(12.dp)) {
            YearCard(vm, vm.tr("net_year1"), vm.tr("net_sub"), true) { vm.go(Route.NetMap) }
            listOf("net_year2", "net_year3", "net_year4").forEach { key ->
                YearCard(vm, vm.tr(key), vm.tr("net_year_locked_sub"), false) {}
            }
            Spacer(Modifier.height(16.dp))
        }
    }
}

@Composable
private fun YearCard(vm: AppViewModel, title: String, sub: String, open: Boolean, onClick: () -> Unit) {
    val p = vm.palette
    CardBox(p, Modifier.fillMaxWidth().alpha(if (open) 1f else 0.48f).then(if (open) Modifier.clickable(onClick = onClick) else Modifier)) {
        Row(verticalAlignment = Alignment.CenterVertically) {
            Column(Modifier.weight(1f)) {
                Text(title, color = p.text, fontWeight = FontWeight.Bold, fontSize = 18.sp)
                Text(sub, color = p.subtext, fontSize = 14.sp)
            }
            if (!open) LockIcon(p.overlay)
        }
    }
}

@Composable
fun LessonMapScreen(
    vm: AppViewModel,
    title: String,
    sub: String,
    nodes: List<MapNode>,
    litUntil: Float,
) {
    Column(Modifier.fillMaxSize().padding(horizontal = 12.dp)) {
        PageTop(vm, { vm.back() }, title, sub)
        PathMap(nodes, vm.palette, litUntil, modifier = Modifier.weight(1f), nodeSize = 64f, spac = 150f, mx = 70f)
    }
}

@Composable
fun SlidesScreen(
    vm: AppViewModel,
    title: String,
    sub: String,
    slides: List<J>,
    onExercise: () -> Unit,
) {
    val p = vm.palette
    var idx = androidx.compose.runtime.remember { androidx.compose.runtime.mutableIntStateOf(0) }
    val slide = slides.getOrNull(idx.intValue) ?: return
    Column(Modifier.fillMaxSize().padding(horizontal = 16.dp)) {
        PageTop(vm, { vm.back() }, title, sub)
        Column(Modifier.weight(1f).verticalScroll(rememberScrollState())) {
            Text(slide.str("kicker"), color = p.subtext, fontSize = 12.sp, fontWeight = FontWeight.SemiBold)
            Text(slide.str("title"), color = p.text, fontSize = 24.sp, fontWeight = FontWeight.Bold, modifier = Modifier.padding(top = 6.dp, bottom = 12.dp))
            slide.strOrNull("tip")?.let { CardBox(p, Modifier.fillMaxWidth().padding(bottom = 12.dp)) { Text(it, color = p.text) } }
            slide.strs("lines").forEach { line ->
                CardBox(p, Modifier.fillMaxWidth().padding(bottom = 8.dp)) {
                    Text(line, color = p.text, fontSize = 16.sp)
                }
            }
        }
        Row(Modifier.fillMaxWidth().padding(vertical = 12.dp), horizontalArrangement = Arrangement.SpaceBetween) {
            if (idx.intValue > 0) PillButton(vm.tr("net_slide_prev"), p) { idx.intValue-- }
            else Spacer(Modifier.width(1.dp))
            if (idx.intValue < slides.lastIndex) PrimaryButton(vm.tr("net_slide_next"), p) { idx.intValue++ }
            else PrimaryButton(vm.tr("net_slide_start"), p, onClick = onExercise)
        }
    }
}

@Composable
fun CzechMapScreen(vm: AppViewModel) {
    val p = vm.palette
    val items = listOf(
        Triple("L", vm.tr("Literatura"), null),
        Triple("M", vm.tr("Mluvnice"), Route.Mluvnice as Route?),
        Triple("Č", vm.tr("Maturitní četba"), Route.ReadingList as Route?),
    )
    Column(Modifier.fillMaxSize().padding(horizontal = 16.dp)) {
        PageTop(vm, { vm.back() }, vm.tr("Český jazyk a literatura"), vm.tr("czech_sub"))
        Column(Modifier.verticalScroll(rememberScrollState()), verticalArrangement = Arrangement.spacedBy(12.dp)) {
            items.forEach { (letter, name, route) ->
                CardBox(p, Modifier.fillMaxWidth().alpha(if (route == null) 0.48f else 1f).then(
                    if (route != null) Modifier.clickable { vm.go(route) } else Modifier
                )) {
                    Row(verticalAlignment = Alignment.CenterVertically) {
                        Box(
                            Modifier.size(56.dp).clip(CircleShape).background(if (route == null) p.lockedBg else p.accent),
                            contentAlignment = Alignment.Center,
                        ) {
                            if (route == null) LockIcon(p.overlay) else Text(letter, color = p.onAccent, fontWeight = FontWeight.Black, fontSize = 20.sp)
                        }
                        Spacer(Modifier.width(14.dp))
                        Text(name, color = p.text, fontWeight = FontWeight.SemiBold, fontSize = 16.sp)
                    }
                }
            }
        }
    }
}

@Composable
fun BookListScreen(vm: AppViewModel) {
    val p = vm.palette
    Column(Modifier.fillMaxSize().padding(horizontal = 16.dp)) {
        PageTop(vm, { vm.back() }, vm.tr("Maturitní četba"), vm.tr("reading_sub"))
        Column(Modifier.verticalScroll(rememberScrollState()), verticalArrangement = Arrangement.spacedBy(12.dp)) {
            vm.content.books.forEach { b ->
                CardBox(p, Modifier.fillMaxWidth().clickable { vm.go(Route.Book(b.str("id"))) }) {
                    Row(verticalAlignment = Alignment.CenterVertically) {
                        BookIcon(p.text, 28.dp)
                        Spacer(Modifier.width(12.dp))
                        Column {
                            Text(b.str("title"), color = p.text, fontWeight = FontWeight.Bold)
                            Text(vm.tr(b.str("subKey")), color = p.subtext, fontSize = 13.sp)
                        }
                    }
                }
            }
        }
    }
}

@Composable
fun BookScreen(vm: AppViewModel, id: String) {
    val b = vm.content.book(id) ?: return
    val p = vm.palette
    Column(Modifier.fillMaxSize().padding(horizontal = 16.dp)) {
        PageTop(vm, { vm.back() }, b.str("title"), vm.tr(b.str("subKey")))
        Column(Modifier.verticalScroll(rememberScrollState()).padding(bottom = 20.dp), verticalArrangement = Arrangement.spacedBy(12.dp)) {
            CardBox(p, Modifier.fillMaxWidth().clickable { vm.go(Route.BookQuiz(id)) }) {
                Row(verticalAlignment = Alignment.CenterVertically) {
                    QuizIcon(p.text); Spacer(Modifier.width(10.dp))
                    Column {
                        Text(vm.tr(b.str("quizTitle")), color = p.text, fontWeight = FontWeight.Bold)
                        Text(vm.tr(b.str("quizSub")), color = p.subtext, fontSize = 13.sp)
                    }
                }
            }
            CardBox(p, Modifier.fillMaxWidth().clickable { vm.go(Route.BookPlot(id)) }) {
                Row(verticalAlignment = Alignment.CenterVertically) {
                    OrderIcon(p.text); Spacer(Modifier.width(10.dp))
                    Column {
                        Text(vm.tr(b.str("plotTitle")), color = p.text, fontWeight = FontWeight.Bold)
                        Text(vm.tr(b.str("plotSub")), color = p.subtext, fontSize = 13.sp)
                    }
                }
            }
            b.arr("notes").forEach { note ->
                CardBox(p, Modifier.fillMaxWidth()) {
                    Column {
                        Row(verticalAlignment = Alignment.CenterVertically) {
                            noteIcon(note.str("icon"), p.text)()
                            Spacer(Modifier.width(8.dp))
                            Text(note.str("title"), color = p.text, fontWeight = FontWeight.Bold)
                        }
                        Spacer(Modifier.height(8.dp))
                        note.strs("lines").forEach {
                            Text("• $it", color = p.text, fontSize = 15.sp, modifier = Modifier.padding(bottom = 4.dp))
                        }
                    }
                }
            }
        }
    }
}
