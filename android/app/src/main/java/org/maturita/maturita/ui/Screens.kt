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
import androidx.compose.material.icons.filled.Check
import androidx.compose.material.icons.filled.Close
import androidx.compose.material.icons.filled.FormatListNumbered
import androidx.compose.material.icons.automirrored.filled.KeyboardArrowLeft
import androidx.compose.material.icons.filled.Lock
import androidx.compose.material.icons.filled.Quiz
import androidx.compose.material.icons.filled.Search
import androidx.compose.material.icons.outlined.Lightbulb
import androidx.compose.material3.Card
import androidx.compose.material3.CardDefaults
import androidx.compose.material3.ExperimentalMaterial3Api
import androidx.compose.material3.FilledTonalButton
import androidx.compose.material3.HorizontalDivider
import androidx.compose.material3.Icon
import androidx.compose.material3.IconButton
import androidx.compose.material3.LinearProgressIndicator
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
import org.maturita.maturita.BuildConfig
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
                colors = CardDefaults.cardColors(containerColor = MaterialTheme.colorScheme.primaryContainer),
            ) {
                Column(Modifier.padding(20.dp)) {
                    Text(
                        vm.tr("welcome_title"),
                        style = MaterialTheme.typography.headlineMedium,
                        color = MaterialTheme.colorScheme.onPrimaryContainer,
                    )
                    Spacer(Modifier.height(8.dp))
                    Text(
                        vm.tr("welcome_body_android"),
                        style = MaterialTheme.typography.bodyMedium,
                        color = MaterialTheme.colorScheme.onPrimaryContainer.copy(alpha = 0.8f),
                    )
                    Spacer(Modifier.height(16.dp))
                    LinearProgressIndicator(
                        progress = { pct },
                        modifier = Modifier.fillMaxWidth().height(6.dp).clip(CircleShape),
                        color = MaterialTheme.colorScheme.onPrimaryContainer,
                        trackColor = MaterialTheme.colorScheme.onPrimaryContainer.copy(alpha = 0.18f),
                    )
                    Spacer(Modifier.height(6.dp))
                    Text(
                        vm.fmt("stats_ex_fmt", sum.doneEx, sum.totalEx),
                        style = MaterialTheme.typography.labelMedium,
                        color = MaterialTheme.colorScheme.onPrimaryContainer.copy(alpha = 0.8f),
                    )
                }
            }
        }
        if (vm.update.canInstall || vm.update.status == "downloading" || vm.update.status == "staged") {
            item { UpdateBanner(vm) }
        }
        item { SectionHeader(vm.tr("subjects_title")) }
        items(vm.content.subjects) { s ->
            val open = s.bool("open")
            NavRow(
                title = vm.tr(s.str("key")),
                subtitle = if (open) null else vm.tr("stats_locked"),
                locked = !open,
                leading = { SubjectIcon(s.str("icon"), open) },
                onClick = if (open) ({ vm.goPage(s.str("target")) }) else null,
            )
        }
    }
}

@Composable
private fun ChangelogCard(vm: AppViewModel) {
    val langKey = if (vm.lang == UiLang.En) "en" else "cs"
    val blocks = vm.content.changelog.take(3)
    Card(
        modifier = Modifier.fillMaxWidth().padding(horizontal = 16.dp, vertical = 8.dp),
        colors = CardDefaults.cardColors(containerColor = MaterialTheme.colorScheme.tertiaryContainer),
    ) {
        Column(Modifier.padding(20.dp)) {
            Row(verticalAlignment = Alignment.CenterVertically) {
                Text(
                    vm.tr("changelog_title"),
                    style = MaterialTheme.typography.titleMedium,
                    color = MaterialTheme.colorScheme.onTertiaryContainer,
                    modifier = Modifier.weight(1f),
                )
                val date = blocks.firstOrNull()?.str("date").orEmpty()
                if (date.isNotEmpty()) {
                    Text(
                        date,
                        style = MaterialTheme.typography.labelMedium,
                        color = MaterialTheme.colorScheme.onTertiaryContainer.copy(alpha = 0.7f),
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
                        color = MaterialTheme.colorScheme.onTertiaryContainer,
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
            "cz" -> CzechFlag(28.dp)
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
                    LinearProgressIndicator(progress = { pct }, modifier = Modifier.fillMaxWidth().height(6.dp).clip(CircleShape))
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
                            LinearProgressIndicator(
                                progress = { part.doneEx / part.totalEx.toFloat() },
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

private fun subjectSum(vm: AppViewModel, s: J): ProgressSum {
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
        "hwyears", "hwmap" -> {
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
                supportingContent = { Text("${BuildConfig.VERSION_NAME} · ${BuildConfig.COMMIT.take(7)}") },
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
    add(vm.tr("hw_year1"), vm.tr("search_lesson"), "hwmap", extra = "rocnik")
    add(vm.tr("hw_year2"), vm.tr("search_lesson"), "hwyears", locked = true, extra = "rocnik")
    add(vm.tr("hw_year3"), vm.tr("search_lesson"), "hwyears", locked = true, extra = "rocnik")
    add(vm.tr("hw_year4"), vm.tr("search_lesson"), "hwyears", locked = true, extra = "rocnik")
    vm.content.hw.forEach { l ->
        add(vm.tr(l.str("titleKey")), vm.tr("search_lesson"), "hwunit${l.int("id")}", extra = "hardware")
        add(vm.tr(l.str("titleKey")), vm.tr("search_exercise"), "hwex${l.int("id")}")
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
fun NetYearsScreen(vm: AppViewModel) {
    DetailScaffold(vm, vm.tr("net_years_title"), vm.tr("net_years_sub")) { inner ->
        LazyColumn(contentPadding = PaddingValues(top = inner.calculateTopPadding(), bottom = inner.calculateBottomPadding() + 16.dp)) {
            item {
                NavRow(title = vm.tr("net_year1"), subtitle = vm.tr("net_sub"), leading = { Text("1.", style = MaterialTheme.typography.titleMedium) }) { vm.go(Route.NetMap) }
            }
            items(listOf("net_year2", "net_year3", "net_year4")) { key ->
                NavRow(title = vm.tr(key), subtitle = vm.tr("net_year_locked_sub"), locked = true, onClick = null)
            }
        }
    }
}

@Composable
fun HwYearsScreen(vm: AppViewModel) {
    DetailScaffold(vm, vm.tr("Technické vybavení"), vm.tr("hw_years_sub")) { inner ->
        LazyColumn(contentPadding = PaddingValues(top = inner.calculateTopPadding(), bottom = inner.calculateBottomPadding() + 16.dp)) {
            item {
                NavRow(title = vm.tr("hw_year1"), subtitle = vm.tr("hw_sub"), leading = { Text("1.", style = MaterialTheme.typography.titleMedium) }) { vm.go(Route.HwMap) }
            }
            items(listOf("hw_year2", "hw_year3", "hw_year4")) { key ->
                NavRow(title = vm.tr(key), subtitle = vm.tr("hw_year_locked_sub"), locked = true, onClick = null)
            }
        }
    }
}

@Composable
fun CzechMapScreen(vm: AppViewModel) {
    DetailScaffold(vm, vm.tr("Český jazyk a literatura"), vm.tr("czech_sub")) { inner ->
        LazyColumn(contentPadding = PaddingValues(top = inner.calculateTopPadding(), bottom = inner.calculateBottomPadding() + 16.dp)) {
            item { NavRow(title = vm.tr("Literatura"), locked = true, onClick = null) }
            item {
                NavRow(
                    title = vm.tr("Mluvnice"),
                    subtitle = vm.tr("mluv_sub"),
                    leading = { Icon(Icons.Filled.FormatListNumbered, null, tint = MaterialTheme.colorScheme.primary) },
                ) { vm.go(Route.Mluvnice) }
            }
            item {
                NavRow(
                    title = vm.tr("Maturitní četba"),
                    subtitle = vm.tr("reading_sub"),
                    leading = { Icon(Icons.AutoMirrored.Filled.MenuBook, null, tint = MaterialTheme.colorScheme.primary) },
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
            val slide = slides[page]
            Column(
                Modifier
                    .fillMaxSize()
                    .verticalScroll(rememberScrollState())
                    .padding(horizontal = 16.dp, vertical = 12.dp),
            ) {
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
                slide.strs("lines").forEach { line ->
                    Text(line, style = MaterialTheme.typography.bodyLarge, modifier = Modifier.padding(vertical = 6.dp))
                }
                Spacer(Modifier.height(24.dp))
            }
        }
    }
    LaunchedEffect(slides) { if (pager.currentPage > slides.lastIndex) pager.scrollToPage(0) }
}
