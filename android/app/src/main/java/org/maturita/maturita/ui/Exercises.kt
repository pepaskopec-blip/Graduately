package org.maturita.maturita.ui

import androidx.compose.foundation.Canvas
import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.ExperimentalLayoutApi
import androidx.compose.foundation.layout.FlowRow
import androidx.compose.foundation.layout.PaddingValues
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.width
import androidx.compose.foundation.layout.widthIn
import androidx.compose.foundation.rememberScrollState
import androidx.compose.foundation.shape.CircleShape
import androidx.compose.foundation.text.KeyboardActions
import androidx.compose.foundation.text.KeyboardOptions
import androidx.compose.foundation.verticalScroll
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.filled.ArrowDropDown
import androidx.compose.material.icons.filled.Cancel
import androidx.compose.material.icons.filled.CheckCircle
import androidx.compose.material.icons.filled.EmojiEvents
import androidx.compose.material3.AssistChip
import androidx.compose.material3.Card
import androidx.compose.material3.CardDefaults
import androidx.compose.material3.HorizontalDivider
import androidx.compose.material3.Icon
import androidx.compose.material3.LinearProgressIndicator
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.OutlinedButton
import androidx.compose.material3.OutlinedTextField
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.runtime.SideEffect
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableIntStateOf
import androidx.compose.runtime.mutableStateListOf
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.runtime.setValue
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.draw.clip
import androidx.compose.ui.geometry.Offset
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.graphics.StrokeCap
import androidx.compose.ui.graphics.drawscope.Stroke
import androidx.compose.ui.text.font.FontFamily
import androidx.compose.ui.text.input.ImeAction
import androidx.compose.ui.text.input.KeyboardCapitalization
import androidx.compose.ui.unit.dp
import org.maturita.maturita.AppViewModel
import org.maturita.maturita.data.J
import org.maturita.maturita.data.answerAccepts
import org.maturita.maturita.data.answerIncomplete
import org.maturita.maturita.data.hasUmlaut
import org.maturita.maturita.data.netTxtEq
import org.maturita.maturita.data.normalizeAnswer

/** Scrollable exercise body with the check button anchored at the bottom. */
@Composable
fun ExerciseScaffold(
    vm: AppViewModel,
    title: String,
    sub: String?,
    action: String,
    onAction: () -> Unit,
    feedback: String,
    kind: String,
    content: @Composable () -> Unit,
) {
    DetailScaffold(vm, title, sub, bottomBar = { ActionBar(action, feedback, kind, onClick = onAction) }) { inner ->
        Column(
            Modifier
                .fillMaxSize()
                .padding(inner)
                .verticalScroll(rememberScrollState())
                .padding(horizontal = 16.dp, vertical = 12.dp),
        ) {
            content()
            Spacer(Modifier.height(16.dp))
        }
    }
}

@Composable
fun GermanExercise(vm: AppViewModel, unitId: Int, ex: Int) {
    val spec = vm.content.germanUnit(unitId)?.exercise(ex) ?: return
    DispatchExercise(vm, spec, spec.str("title"), onDone = { vm.markGerman(unitId, ex) })
}

@Composable
fun VocabExercise(vm: AppViewModel, unitId: Int) {
    val spec = vm.content.germanUnit(unitId)?.obj("vocab") ?: return
    DispatchExercise(vm, spec, vm.tr("Vokabeltraining"), onDone = { vm.markVocab(unitId) })
}

@Composable
fun DispatchExercise(vm: AppViewModel, spec: J, titleFallback: String, onDone: () -> Unit) {
    val title = spec.str("title").ifEmpty { titleFallback }
    val sub = spec.strOrNull("sub")?.let { vm.tr(it) }
    when (spec.str("type")) {
        "choice" -> ChoiceEx(vm, title, sub, spec, onDone)
        "assembly" -> AssemblyEx(vm, title, sub, spec, onDone)
        "typed", "kw" -> TypedEx(vm, title, sub, spec, onDone)
        "free", "profile" -> FreeEx(vm, title, sub, spec, onDone)
        "assign" -> AssignEx(vm, title, sub, spec, onDone)
        "hangman" -> HangmanEx(vm, title, sub, spec, onDone)
        "vocab" -> VocabEx(vm, title, sub, spec, onDone)
        "dialog" -> DialogEx(vm, title, sub, spec, onDone)
        "number", "count", "seq", "verb", "verbclue", "verb_sections", "ordne", "fill" -> ComboEx(vm, title, sub, spec, onDone)
        "ex2" -> Ex2Ex(vm, title, sub, spec, onDone)
        "letters", "letters_gap" -> LettersEx(vm, title, sub, spec, onDone)
        "table" -> TableEx(vm, title, sub, spec, onDone)
        "reveal" -> RevealEx(vm, title, sub, spec, onDone)
        else -> DetailScaffold(vm, title, sub) { inner -> Text("?", Modifier.padding(inner).padding(16.dp)) }
    }
}

@Composable
private fun ChoiceEx(vm: AppViewModel, title: String, sub: String?, spec: J, onDone: () -> Unit) {
    val qs = spec.arr("questions")
    val meanings = spec.strs("meanings")
    val expls = spec.strs("expls")
    val picks = remember { mutableStateListOf(*Array(qs.size) { -1 }) }
    var fb by remember { mutableStateOf("" to "") }
    var revealed by remember { mutableStateOf(false) }
    ExerciseScaffold(vm, title, sub, vm.tr("check"), {
        val ok = qs.indices.count { picks[it] == qs[it].int("correct") }
        if (ok == qs.size) { fb = vm.tr("feedback_ok") to "ok"; onDone() }
        else fb = vm.tr("feedback_retry_short") to "err"
        revealed = true
    }, fb.first, fb.second) {
        qs.forEachIndexed { i, q ->
            Text("${i + 1}. ${q.str("prompt")}", style = MaterialTheme.typography.titleMedium, modifier = Modifier.padding(bottom = 8.dp))
            q.strs("options").forEachIndexed { o, opt ->
                val sel = picks[i] == o
                val mark = if (revealed && sel) picks[i] == q.int("correct") else null
                OptionRow(opt, sel, mark) { picks[i] = o; revealed = false }
            }
            if (revealed) {
                meanings.getOrNull(i)?.let { Meaning(vm.tr(it), true) }
                expls.getOrNull(i)?.let { Meaning(it, true) }
            }
            Spacer(Modifier.height(12.dp))
        }
    }
}

@OptIn(ExperimentalLayoutApi::class)
@Composable
private fun AssemblyEx(vm: AppViewModel, title: String, sub: String?, spec: J, onDone: () -> Unit) {
    val items = spec.arr("items")
    val meanings = spec.strs("meanings")
    val placed = remember { items.map { mutableStateListOf<Int>() } }
    val pools = remember {
        items.map { item ->
            val n = item.strs("words").size
            mutableStateListOf(*((0 until n).shuffled().toTypedArray()))
        }
    }
    var fb by remember { mutableStateOf("" to "") }
    var revealed by remember { mutableStateOf(false) }
    ExerciseScaffold(vm, title, sub, vm.tr("check"), {
        val ok = items.indices.all { i ->
            val words = items[i].strs("words")
            placed[i].size == words.size && placed[i].map { words[it] } == words
        }
        if (ok) { fb = vm.tr("feedback_ok") to "ok"; onDone() }
        else fb = vm.tr("feedback_sentences") to "err"
        revealed = true
    }, fb.first, fb.second) {
        items.forEachIndexed { i, item ->
            val words = item.strs("words")
            item.strOrNull("prompt")?.let { Hint(it) }
            WordSlot(placed[i].isEmpty()) {
                placed[i].toList().forEach { idx ->
                    val word = words.getOrNull(idx) ?: return@forEach
                    Chip(word) {
                        placed[i].remove(idx)
                        pools[i].add(idx)
                    }
                }
            }
            FlowRow(horizontalArrangement = Arrangement.spacedBy(6.dp), verticalArrangement = Arrangement.spacedBy(2.dp)) {
                pools[i].toList().forEach { idx ->
                    val word = words.getOrNull(idx) ?: return@forEach
                    Chip(word) {
                        pools[i].remove(idx)
                        placed[i].add(idx)
                    }
                }
            }
            if (revealed) meanings.getOrNull(i)?.let { Meaning(vm.tr(it), true) }
            Spacer(Modifier.height(16.dp))
        }
    }
}

@Composable
private fun TypedEx(vm: AppViewModel, title: String, sub: String?, spec: J, onDone: () -> Unit) {
    val rows = spec.arr("rows")
    val items = if (rows.isNotEmpty()) rows else spec.arr("items")
    val fields = remember {
        items.map { q ->
            val n = maxOf(1, q.strs("hints").size, q.answerList().size)
            mutableStateListOf(*Array(n) { "" })
        }
    }
    var fb by remember { mutableStateOf("" to "") }
    var revealed by remember { mutableStateOf(false) }
    val umlaut = items.any { q -> q.answerList().any { hasUmlaut(it) } || hasUmlaut(q.str("prompt")) }
    ExerciseScaffold(vm, title, sub, vm.tr("check"), {
        var ok = 0
        var total = 0
        var incomplete = false
        items.forEachIndexed { i, q ->
            val answers = q.answerList()
            fields[i].forEachIndexed { j, value ->
                total++
                val ans = answers.getOrNull(j) ?: ""
                val norm = normalizeAnswer(value)
                when {
                    answerAccepts(norm, ans) -> ok++
                    answerIncomplete(norm, ans) -> incomplete = true
                }
            }
        }
        when {
            total > 0 && ok == total -> { fb = vm.tr("feedback_ok") to "ok"; onDone() }
            incomplete -> fb = vm.tr("trans_incomplete") to "warn"
            else -> fb = vm.tr("feedback_retry") to "err"
        }
        revealed = true
    }, fb.first, fb.second) {
        spec.strOrNull("note")?.let { Hint(it) }
        spec.strOrNull("sample")?.let { Hint(it) }
        spec.strOrNull("bank")?.let { Hint("${vm.tr("wordbank")} $it") }
        if (umlaut) Hint(vm.tr("hint_umlauts"))
        items.forEachIndexed { i, q ->
            Prompt(q.str("prompt"))
            val answers = q.answerList()
            val hints = q.strs("hints")
            fields[i].forEachIndexed { j, value ->
                val ans = answers.getOrNull(j) ?: ""
                WordField(
                    value,
                    { fields[i][j] = it; revealed = false },
                    hints.getOrNull(j) ?: vm.tr("your_answer"),
                    if (revealed) answerAccepts(normalizeAnswer(value), ans) else null,
                )
            }
            if (revealed) {
                q.strOrNull("meaning")?.let { Meaning(vm.tr(it), true) }
                q.strOrNull("mean")?.let { Meaning(vm.tr(it), true) }
                q.strOrNull("german")?.let { Meaning(it, true) }
                q.strOrNull("shown")?.let { Meaning(it, true) }
            }
            Spacer(Modifier.height(10.dp))
        }
    }
}

@Composable
private fun FreeEx(vm: AppViewModel, title: String, sub: String?, spec: J, onDone: () -> Unit) {
    val qs = if (spec.arr("questions").isNotEmpty()) spec.arr("questions") else spec.arr("rows")
    val shown = remember { mutableStateListOf(*Array(qs.size) { false }) }
    var fb by remember { mutableStateOf("" to "") }
    val values = remember { mutableStateListOf(*Array(qs.size) { "" }) }
    ExerciseScaffold(vm, title, sub, vm.tr("finish"), {
        fb = vm.tr("feedback_ok") to "ok"
        qs.indices.forEach { shown[it] = true }
        onDone()
    }, fb.first, fb.second) {
        spec.strOrNull("tip")?.let { Hint(vm.tr(it)) }
        qs.forEachIndexed { i, q ->
            Prompt(q.str("question").ifEmpty { q.str("stem") })
            WordField(values[i], { values[i] = it }, vm.tr("your_answer"))
            PillButton(vm.tr("show_sample"), Modifier.padding(vertical = 4.dp)) { shown[i] = !shown[i] }
            if (shown[i]) {
                val sample = q.str("sample")
                val qcs = q.str("qCs")
                val acs = q.str("aCs")
                Meaning(if (qcs.isNotEmpty()) vm.fmt("sample_fmt", vm.tr(qcs), sample, vm.tr(acs)) else sample, true)
            }
            Spacer(Modifier.height(12.dp))
        }
    }
}

@OptIn(ExperimentalLayoutApi::class)
@Composable
private fun AssignEx(vm: AppViewModel, title: String, sub: String?, spec: J, onDone: () -> Unit) {
    val items = spec.arr("items")
    val groups = spec.strs("groups")
    val meanings = spec.strs("meanings")
    var active by remember { mutableIntStateOf(0) }
    val loc = remember { mutableStateListOf(*Array(items.size) { -1 }) }
    var fb by remember { mutableStateOf("" to "") }
    var revealed by remember { mutableStateOf(false) }
    fun label(it: J) = listOfNotNull(it.strOrNull("emoji"), it.str("label")).joinToString(" ")
    ExerciseScaffold(vm, title, sub, vm.tr("check"), {
        val ok = items.indices.all { loc[it] == items[it].int("group") }
        if (ok) { fb = vm.tr("feedback_ok") to "ok"; onDone() }
        else fb = vm.tr("feedback_retry") to "err"
        revealed = true
    }, fb.first, fb.second) {
        Hint(vm.tr("assign_hint"))
        FlowRow(horizontalArrangement = Arrangement.spacedBy(8.dp)) {
            groups.forEachIndexed { i, g -> SegChip(g, active == i) { active = i } }
        }
        Spacer(Modifier.height(12.dp))
        Text(vm.tr("wordbank"), style = MaterialTheme.typography.labelMedium, color = MaterialTheme.colorScheme.onSurfaceVariant)
        FlowRow(horizontalArrangement = Arrangement.spacedBy(6.dp)) {
            items.forEachIndexed { i, it -> if (loc[i] == -1) Chip(label(it)) { loc[i] = active } }
        }
        groups.forEachIndexed { g, name ->
            Spacer(Modifier.height(12.dp))
            Text(name, style = MaterialTheme.typography.labelMedium, color = MaterialTheme.colorScheme.primary)
            WordSlot(items.indices.none { loc[it] == g }) {
                items.forEachIndexed { i, it -> if (loc[i] == g) Chip(label(it)) { loc[i] = -1 } }
            }
        }
        if (revealed) items.indices.forEach { i -> meanings.getOrNull(i)?.let { Meaning(vm.tr(it), true) } }
    }
}

@OptIn(ExperimentalLayoutApi::class)
@Composable
private fun HangmanEx(vm: AppViewModel, title: String, sub: String?, spec: J, onDone: () -> Unit) {
    val words = spec.strs("words")
    val tips = spec.strs("tips")
    val letters = spec.strs("letters")
    var word by remember { mutableIntStateOf(0) }
    var misses by remember { mutableIntStateOf(0) }
    val guessed = remember { mutableStateListOf(*Array(letters.size) { false }) }
    var fb by remember { mutableStateOf("" to "") }
    var finished by remember { mutableStateOf(false) }
    val cur = words.getOrNull(word)?.uppercase() ?: ""
    fun idxOf(ch: Char) = letters.indexOfFirst { it.equals(ch.toString(), true) }
    fun solved() = cur.all { ch -> idxOf(ch).let { it >= 0 && guessed[it] } }
    val stroke = MaterialTheme.colorScheme.onSurface
    DetailScaffold(
        vm, title, sub,
        bottomBar = if (misses >= 6 && !finished) {
            {
                ActionBar(vm.tr("hm_retry"), fb.first, fb.second) {
                    misses = 0
                    guessed.indices.forEach { guessed[it] = false }
                    fb = "" to ""
                }
            }
        } else null,
    ) { inner ->
        Column(
            Modifier.fillMaxSize().padding(inner).verticalScroll(rememberScrollState()).padding(horizontal = 16.dp, vertical = 12.dp),
        ) {
            Text(vm.fmt("hm_progress", word + 1, words.size), style = MaterialTheme.typography.labelMedium, color = MaterialTheme.colorScheme.onSurfaceVariant)
            tips.getOrNull(word)?.let { Hint("${vm.tr("hm_hint")}: ${vm.tr(it)}") }
            Canvas(Modifier.fillMaxWidth().height(160.dp)) {
                val x = size.width * 0.3f
                drawLine(stroke, Offset(40f, size.height - 10), Offset(size.width * 0.55f, size.height - 10), 6f, StrokeCap.Round)
                drawLine(stroke, Offset(70f, size.height - 10), Offset(70f, 16f), 6f, StrokeCap.Round)
                drawLine(stroke, Offset(70f, 16f), Offset(x + 40, 16f), 6f, StrokeCap.Round)
                if (misses > 0) drawLine(stroke, Offset(x + 40, 16f), Offset(x + 40, 36f), 4f)
                if (misses > 1) drawCircle(stroke, 16f, Offset(x + 40, 52f), style = Stroke(4f))
                if (misses > 2) drawLine(stroke, Offset(x + 40, 68f), Offset(x + 40, 110f), 4f)
                if (misses > 3) drawLine(stroke, Offset(x + 40, 80f), Offset(x + 18, 100f), 4f)
                if (misses > 4) drawLine(stroke, Offset(x + 40, 80f), Offset(x + 62, 100f), 4f)
                if (misses > 5) drawLine(stroke, Offset(x + 40, 110f), Offset(x + 18, 140f), 4f)
            }
            Row(Modifier.fillMaxWidth(), horizontalArrangement = Arrangement.Center) {
                cur.forEach { ch ->
                    val show = idxOf(ch).let { it >= 0 && guessed[it] }
                    Text(
                        if (show) ch.toString() else "_",
                        style = MaterialTheme.typography.headlineMedium.copy(fontFamily = FontFamily.Monospace),
                        modifier = Modifier.padding(horizontal = 3.dp),
                    )
                }
            }
            Spacer(Modifier.height(12.dp))
            FlowRow(horizontalArrangement = Arrangement.spacedBy(6.dp), verticalArrangement = Arrangement.spacedBy(6.dp)) {
                letters.forEachIndexed { i, letter ->
                    OutlinedButton(
                        enabled = !guessed[i] && !finished,
                        contentPadding = PaddingValues(0.dp),
                        modifier = Modifier.widthIn(min = 44.dp).height(40.dp),
                        onClick = {
                            guessed[i] = true
                            if (!cur.contains(letter, true)) {
                                misses++
                                if (misses >= 6) fb = vm.fmt("hm_fail", cur) to "err"
                            } else if (solved()) {
                                if (word + 1 >= words.size) {
                                    finished = true
                                    fb = vm.tr("hm_done") to "ok"
                                    onDone()
                                } else {
                                    fb = vm.tr("hm_wrong") to "ok"
                                    word++
                                    misses = 0
                                    guessed.indices.forEach { guessed[it] = false }
                                }
                            }
                        },
                    ) { Text(letter) }
                }
            }
            Spacer(Modifier.height(12.dp))
            FeedbackLine(fb.first, fb.second)
        }
    }
}

@Composable
private fun VocabEx(vm: AppViewModel, title: String, sub: String?, spec: J, onDone: () -> Unit) {
    val cards = remember {
        spec.arr("sections").flatMap { sec ->
            sec.arr("rows").map { Triple(sec.str("header"), it.str("prompt"), it.str("answers")) }
        }.toMutableStateList()
    }
    var index by remember { mutableIntStateOf(0) }
    var done by remember { mutableIntStateOf(0) }
    var value by remember { mutableStateOf("") }
    var fb by remember { mutableStateOf("" to "") }
    val total = remember { cards.size }
    val cur = cards.getOrNull(index)
    fun check() {
        val c = cur ?: return
        val norm = normalizeAnswer(value)
        when {
            answerAccepts(norm, c.third) -> {
                done++
                cards.removeAt(index)
                if (cards.isEmpty()) { fb = vm.tr("trans_done") to "ok"; onDone() }
                else { if (index >= cards.size) index = 0; value = ""; fb = "" to "" }
            }
            answerIncomplete(norm, c.third) -> fb = vm.tr("trans_incomplete") to "warn"
            else -> {
                fb = vm.fmt("trans_wrong", c.third.split('|').first()) to "err"
                val item = cards.removeAt(index)
                cards.add(item)
                if (index >= cards.size) index = 0
                value = ""
            }
        }
    }
    DetailScaffold(
        vm, title, sub,
        bottomBar = if (cur != null) ({ ActionBar(vm.tr("check"), fb.first, fb.second) { check() } }) else null,
    ) { inner ->
        Column(Modifier.fillMaxSize().padding(inner).verticalScroll(rememberScrollState()).padding(16.dp)) {
            LinearProgressIndicator(
                progress = { if (total == 0) 0f else done / total.toFloat() },
                modifier = Modifier.fillMaxWidth().height(6.dp).clip(CircleShape),
            )
            Spacer(Modifier.height(6.dp))
            Text(vm.fmt("trans_progress", done, total), style = MaterialTheme.typography.labelMedium, color = MaterialTheme.colorScheme.onSurfaceVariant)
            Spacer(Modifier.height(12.dp))
            if (cur == null) {
                Row(verticalAlignment = Alignment.CenterVertically) {
                    Icon(Icons.Filled.EmojiEvents, null, tint = MaterialTheme.colorScheme.primary)
                    Spacer(Modifier.width(8.dp))
                    Text(vm.tr("trans_done"), style = MaterialTheme.typography.titleMedium, color = MaterialTheme.colorScheme.primary)
                }
            } else {
                Card(modifier = Modifier.fillMaxWidth(), colors = CardDefaults.cardColors(containerColor = MaterialTheme.colorScheme.secondaryContainer)) {
                    Column(Modifier.padding(20.dp)) {
                        Text(cur.first, style = MaterialTheme.typography.labelMedium, color = MaterialTheme.colorScheme.onSecondaryContainer.copy(alpha = 0.75f))
                        Spacer(Modifier.height(4.dp))
                        Text(cur.second, style = MaterialTheme.typography.headlineSmall)
                    }
                }
                Spacer(Modifier.height(12.dp))
                if (hasUmlaut(cur.third)) Hint(vm.tr("hint_umlauts"))
                OutlinedTextField(
                    value = value,
                    onValueChange = { value = it },
                    placeholder = { Text(vm.tr("your_answer")) },
                    singleLine = true,
                    keyboardOptions = KeyboardOptions(capitalization = KeyboardCapitalization.None, autoCorrectEnabled = false, imeAction = ImeAction.Done),
                    keyboardActions = KeyboardActions(onDone = { check() }),
                    modifier = Modifier.fillMaxWidth(),
                )
            }
        }
    }
}

private fun List<Triple<String, String, String>>.toMutableStateList() = mutableStateListOf(*this.toTypedArray())

@Composable
private fun DialogEx(vm: AppViewModel, title: String, sub: String?, spec: J, onDone: () -> Unit) {
    val pool = spec.strs("pool")
    val meanings = spec.strs("meanings")
    val rows = spec.arr("dialogues").flatMap { it.arr("rows") }
    ComboRows(vm, title, sub, rows.size, onDone) { picks, expected, revealed ->
        var i = 0
        spec.arr("dialogues").forEachIndexed { di, d ->
            if (di > 0) HorizontalDivider(Modifier.padding(vertical = 8.dp))
            Text(d.str("name"), style = MaterialTheme.typography.labelLarge, color = MaterialTheme.colorScheme.primary, modifier = Modifier.padding(vertical = 6.dp))
            d.arr("rows").forEach { row ->
                val idx = i++
                ComboLine(
                    vm, pool, picks, expected, idx, row.str("answer"),
                    prefix = listOf(row.str("speaker"), row.str("before")).filter { it.isNotBlank() }.joinToString(" "),
                    suffix = row.str("after"),
                    meaning = meanings.getOrNull(idx),
                    revealed = revealed,
                )
            }
        }
    }
}

@OptIn(ExperimentalLayoutApi::class)
@Composable
private fun ComboEx(vm: AppViewModel, title: String, sub: String?, spec: J, onDone: () -> Unit) {
    val pool = spec.strs("pool")
    val rows = spec.arr("rows")
    val meanings = spec.strs("meanings")
    val type = spec.str("type")
    if (type == "verb_sections") {
        val sections = spec.arr("sections")
        val allRows = sections.flatMap { it.arr("rows") }
        ComboRows(vm, title, sub, allRows.size, onDone, spec.bool("transUpfront")) { picks, expected, revealed ->
            var i = 0
            sections.forEach { sec ->
                Text(sec.str("title"), style = MaterialTheme.typography.labelLarge, color = MaterialTheme.colorScheme.primary, modifier = Modifier.padding(vertical = 6.dp))
                sec.arr("rows").forEachIndexed { ri, row ->
                    val idx = i++
                    ComboLine(
                        vm, pool, picks, expected, idx, row.str("answer"),
                        prefix = row.str("before"), suffix = row.str("after"),
                        meaning = sec.strs("meanings").getOrNull(ri),
                        revealed = revealed || spec.bool("transUpfront"),
                    )
                }
            }
        }
        return
    }
    if (type == "ordne") {
        ComboRows(vm, title, sub, rows.size * 2, onDone) { picks, expected, revealed ->
            rows.forEachIndexed { i, r ->
                ComboLine(vm, spec.strs("fwPool"), picks, expected, i * 2, r.str("fw"), prefix = "${i + 1}. ", suffix = r.str("mid"), revealed = revealed)
                ComboLine(vm, spec.strs("ansPool"), picks, expected, i * 2 + 1, r.str("ans"), prefix = "→ ", meaning = meanings.getOrNull(i), revealed = revealed)
                Spacer(Modifier.height(6.dp))
            }
        }
        return
    }
    if (type == "fill") {
        val blanks = rows.sumOf { it.answerList().size }
        ComboRows(vm, title, sub, blanks, onDone, spec.bool("preMeaning")) { picks, expected, revealed ->
            spec.strOrNull("sample")?.let { Hint(it) }
            var i = 0
            rows.forEachIndexed { ri, r ->
                val segs = r.strsOrEmpty("segs")
                val ans = r.answerList()
                FlowRow(
                    Modifier.fillMaxWidth().padding(bottom = 6.dp),
                    verticalArrangement = Arrangement.Center,
                    horizontalArrangement = Arrangement.spacedBy(4.dp),
                ) {
                    r.strOrNull("num")?.let { InlineText(it) }
                    segs.forEachIndexed { si, seg ->
                        if (!seg.isNullOrEmpty()) InlineText(seg)
                        if (si < ans.size) {
                            val idx = i++
                            ComboLine(vm, pool, picks, expected, idx, ans[si], revealed = revealed, inline = true)
                        }
                    }
                    meanings.getOrNull(ri)?.let { Text(vm.tr(it), style = MaterialTheme.typography.bodySmall, color = MaterialTheme.colorScheme.onSurfaceVariant) }
                }
                if (revealed || spec.bool("preMeaning") || spec.bool("showGerman")) {
                    r.strOrNull("mean")?.let { Meaning(vm.tr(it), true) }
                }
            }
        }
        return
    }
    ComboRows(vm, title, sub, rows.size, onDone, spec.bool("preMeaning")) { picks, expected, revealed ->
        spec.strOrNull("sample")?.let { Hint(it) }
        rows.forEachIndexed { i, r ->
            val prefix = when (type) {
                "number" -> "${r.str("digits")} = "
                "count" -> "${List(r.int("count")) { r.str("emoji") }.joinToString(" ")} ${r.str("noun")} "
                "seq" -> r.str("before")
                "verbclue" -> "${r.str("emoji")} ${r.str("before")}"
                else -> r.str("before")
            }
            val suffix = when (type) {
                "seq", "verbclue", "verb" -> r.str("after")
                else -> ""
            }
            ComboLine(vm, pool, picks, expected, i, r.str("answer"), prefix, suffix, meanings.getOrNull(i) ?: r.strOrNull("mean"), revealed || spec.bool("preMeaning"))
        }
    }
}

@Composable
private fun InlineText(text: String) {
    Text(text, style = MaterialTheme.typography.bodyLarge, modifier = Modifier.padding(vertical = 8.dp))
}

@Composable
private fun ComboRows(
    vm: AppViewModel,
    title: String,
    sub: String?,
    n: Int,
    onDone: () -> Unit,
    revealMean: Boolean = false,
    body: @Composable (MutableList<String>, MutableList<String>, Boolean) -> Unit,
) {
    val picks = remember { mutableStateListOf(*Array(n) { "" }) }
    val expected = remember { mutableStateListOf(*Array(n) { "" }) }
    var fb by remember { mutableStateOf("" to "") }
    var revealed by remember { mutableStateOf(revealMean) }
    ExerciseScaffold(vm, title, sub, vm.tr("check"), {
        val ok = expected.indices.all { i ->
            val exp = expected.getOrNull(i).orEmpty()
            exp.isEmpty() || answerAccepts(normalizeAnswer(picks[i]), exp)
        }
        if (ok) { fb = vm.tr("feedback_ok") to "ok"; onDone() }
        else fb = vm.tr("feedback_retry") to "err"
        revealed = true
    }, fb.first, fb.second) {
        body(picks, expected, revealed)
    }
}

/** One blank: a chip that opens the word bank; prefix/suffix wrap around it. */
@OptIn(ExperimentalLayoutApi::class)
@Composable
private fun ComboLine(
    vm: AppViewModel,
    pool: List<String>,
    picks: MutableList<String>,
    expected: MutableList<String>,
    idx: Int,
    answer: String,
    prefix: String = "",
    suffix: String = "",
    meaning: String? = null,
    revealed: Boolean,
    inline: Boolean = false,
) {
    SideEffect {
        if (idx in expected.indices && expected[idx] != answer) expected[idx] = answer
    }
    var open by remember { mutableStateOf(false) }
    val pick = picks.getOrNull(idx).orEmpty()
    val mark = if (revealed && pick.isNotEmpty()) answerAccepts(normalizeAnswer(pick), answer) else null
    val chip = @Composable {
        AssistChip(
            onClick = { open = true },
            label = { Text(pick.ifEmpty { "…" }, color = if (pick.isEmpty()) MaterialTheme.colorScheme.onSurfaceVariant else Color.Unspecified) },
            trailingIcon = {
                when (mark) {
                    true -> Icon(Icons.Filled.CheckCircle, null, tint = MaterialTheme.colorScheme.primary, modifier = Modifier.width(18.dp))
                    false -> Icon(Icons.Filled.Cancel, null, tint = MaterialTheme.colorScheme.error, modifier = Modifier.width(18.dp))
                    null -> Icon(Icons.Filled.ArrowDropDown, null, modifier = Modifier.width(18.dp))
                }
            },
        )
    }
    if (inline) {
        chip()
    } else {
        FlowRow(
            Modifier.fillMaxWidth().padding(bottom = 4.dp),
            horizontalArrangement = Arrangement.spacedBy(6.dp),
            verticalArrangement = Arrangement.Center,
        ) {
            if (prefix.isNotBlank()) InlineText(prefix)
            chip()
            if (suffix.isNotBlank()) InlineText(suffix)
        }
    }
    if (open) {
        WordPicker(vm.tr("wordbank"), pool) { word ->
            if (word != null && idx in picks.indices) picks[idx] = word
            open = false
        }
    }
    if (revealed && !meaning.isNullOrBlank()) Meaning(vm.tr(meaning), true)
}

@OptIn(ExperimentalLayoutApi::class)
@Composable
private fun Ex2Ex(vm: AppViewModel, title: String, sub: String?, spec: J, onDone: () -> Unit) {
    val items = spec.arr("items")
    val pool = spec.strs("pool")
    val blanks = items.sumOf { it.arr("rows").sumOf { r -> r.strs("answers").size } }
    ComboRows(vm, title, sub, blanks, onDone) { picks, expected, revealed ->
        var i = 0
        items.forEachIndexed { ii, item ->
            if (ii > 0) HorizontalDivider(Modifier.padding(vertical = 8.dp))
            Meaning(item.str("czech"), true)
            item.arr("rows").forEach { row ->
                val segs = row.strsOrEmpty("segs")
                val ans = row.strs("answers")
                FlowRow(verticalArrangement = Arrangement.Center, horizontalArrangement = Arrangement.spacedBy(4.dp)) {
                    row.strOrNull("num")?.let { InlineText(it) }
                    segs.forEachIndexed { si, seg ->
                        if (!seg.isNullOrEmpty()) InlineText(seg)
                        if (si < ans.size) {
                            val idx = i++
                            ComboLine(vm, pool, picks, expected, idx, ans[si], revealed = revealed, inline = true)
                        }
                    }
                }
            }
        }
    }
}

@OptIn(ExperimentalLayoutApi::class)
@Composable
private fun LettersEx(vm: AppViewModel, title: String, sub: String?, spec: J, onDone: () -> Unit) {
    val rows = spec.arr("rows")
    val fields = remember { mutableStateListOf(*rows.flatMap { it.strs("answers") }.map { "" }.toTypedArray()) }
    var fb by remember { mutableStateOf("" to "") }
    var revealed by remember { mutableStateOf(false) }
    ExerciseScaffold(vm, title, sub, vm.tr("check"), {
        var i = 0
        var ok = 0
        var total = 0
        rows.forEach { r ->
            r.strs("answers").forEach { ans ->
                total++
                if (answerAccepts(normalizeAnswer(fields[i]), ans)) ok++
                i++
            }
        }
        if (ok == total) { fb = vm.tr("feedback_ok") to "ok"; onDone() }
        else fb = vm.tr("feedback_retry") to "err"
        revealed = true
    }, fb.first, fb.second) {
        if (rows.any { it.strs("answers").any { a -> hasUmlaut(a) } }) Hint(vm.tr("hint_umlauts"))
        var i = 0
        rows.forEach { r ->
            r.strOrNull("num")?.let { Text(it, style = MaterialTheme.typography.labelMedium, color = MaterialTheme.colorScheme.onSurfaceVariant) }
            FlowRow(verticalArrangement = Arrangement.Center, horizontalArrangement = Arrangement.spacedBy(4.dp)) {
                val segs = r.strsOrEmpty("segs")
                val ans = r.strs("answers")
                segs.forEachIndexed { si, seg ->
                    if (!seg.isNullOrEmpty()) InlineText(seg)
                    if (si < ans.size) {
                        val idx = i++
                        Box(Modifier.width(96.dp)) {
                            WordField(fields[idx], { fields[idx] = it; revealed = false }, "",
                                if (revealed) answerAccepts(normalizeAnswer(fields[idx]), ans[si]) else null)
                        }
                    }
                }
            }
            if (revealed) r.strOrNull("czech")?.let { Meaning(it, true) }
            if (revealed) r.strOrNull("mean")?.let { Meaning(it, true) }
            Spacer(Modifier.height(8.dp))
        }
    }
}

@Composable
private fun TableEx(vm: AppViewModel, title: String, sub: String?, spec: J, onDone: () -> Unit) {
    val persons = spec.strs("persons")
    val nouns = spec.strs("nouns")
    val answers = spec.intRows("answers")
    val fields = remember { mutableStateListOf(*Array(nouns.size * persons.size) { "" }) }
    var fb by remember { mutableStateOf("" to "") }
    var revealed by remember { mutableStateOf(false) }
    ExerciseScaffold(vm, title, sub, vm.tr("check"), {
        var ok = 0
        answers.forEachIndexed { r, row ->
            row.forEachIndexed { c, ans ->
                val i = r * persons.size + c
                if (normalizeAnswer(fields[i]) == normalizeAnswer(ans)) ok++
            }
        }
        if (ok == fields.size) { fb = vm.tr("feedback_ok") to "ok"; onDone() }
        else fb = vm.tr("feedback_retry") to "err"
        revealed = true
    }, fb.first, fb.second) {
        nouns.forEachIndexed { r, noun ->
            Text(noun, style = MaterialTheme.typography.titleMedium, color = MaterialTheme.colorScheme.primary, modifier = Modifier.padding(top = 8.dp))
            persons.forEachIndexed { c, person ->
                val i = r * persons.size + c
                val ans = answers.getOrNull(r)?.getOrNull(c) ?: ""
                Text(person, style = MaterialTheme.typography.labelMedium, color = MaterialTheme.colorScheme.onSurfaceVariant, modifier = Modifier.padding(top = 6.dp))
                WordField(
                    fields[i], { fields[i] = it; revealed = false }, "",
                    if (revealed) normalizeAnswer(fields[i]) == normalizeAnswer(ans) else null,
                )
            }
            Spacer(Modifier.height(8.dp))
        }
    }
}

@Composable
private fun RevealEx(vm: AppViewModel, title: String, sub: String?, spec: J, onDone: () -> Unit) {
    val items = spec.arr("items")
    val shown = remember { mutableStateListOf(*Array(items.size) { false }) }
    var fb by remember { mutableStateOf("" to "") }
    ExerciseScaffold(vm, title, sub, vm.tr("finish"), {
        items.indices.forEach { shown[it] = true }
        fb = vm.tr("feedback_ok") to "ok"
        onDone()
    }, fb.first, fb.second) {
        spec.strOrNull("note")?.let { Hint(it) }
        items.forEachIndexed { i, it ->
            Prompt(it.str("prompt"))
            PillButton(vm.tr("show_sample"), Modifier.padding(vertical = 4.dp)) { shown[i] = !shown[i] }
            if (shown[i]) Meaning(it.str("solution"), true)
            Spacer(Modifier.height(8.dp))
        }
    }
}

@Composable
fun MluvExercise(vm: AppViewModel, n: Int) {
    val spec = vm.content.mluv(n) ?: return
    DispatchExercise(vm, spec, spec.str("title")) { vm.markMluv(n) }
}

@Composable
fun NetQuizScreen(vm: AppViewModel, id: Int) {
    val lesson = vm.content.netLesson(id) ?: return
    if (id == 1) {
        VlsmEx(vm)
        return
    }
    val spec = J(org.json.JSONObject().apply {
        put("type", "choice")
        put("title", vm.tr(lesson.str("exTitleKey")))
        put("sub", "net_quiz_intro")
        put("questions", lesson.o.optJSONArray("quiz"))
    })
    DispatchExercise(vm, spec, vm.tr(lesson.str("exTitleKey"))) { vm.markNet(id) }
}

@Composable
fun HwQuizScreen(vm: AppViewModel, id: Int) {
    val lesson = vm.content.hwLesson(id) ?: return
    val spec = J(org.json.JSONObject().apply {
        put("type", "choice")
        put("title", vm.tr(lesson.str("exTitleKey")))
        put("sub", lesson.str("quizHeadKey"))
        put("questions", lesson.o.optJSONArray("quiz"))
    })
    DispatchExercise(vm, spec, vm.tr(lesson.str("exTitleKey"))) { vm.markHw(id) }
}

private class VlsmRow {
    var pfx by mutableStateOf("")
    var net by mutableStateOf("")
    var bc by mutableStateOf("")
    var lo by mutableStateOf("")
    var hi by mutableStateOf("")
}

@Composable
private fun VlsmEx(vm: AppViewModel) {
    val tasks = vm.content.netTasks
    val answers = vm.content.netAnswers
    val done = remember { mutableStateListOf(*Array(tasks.size) { false }) }
    val rows = remember { answers.map { block -> List(block.size) { VlsmRow() } } }
    var fb by remember { mutableStateOf("" to "") }
    fun maybeFinish() {
        if (done.isNotEmpty() && done.all { it }) vm.markNet(1)
    }
    DetailScaffold(
        vm, vm.tr("net_ex_title"), null,
        bottomBar = if (fb.first.isNotEmpty()) {
            {
                Box(Modifier.fillMaxWidth().padding(16.dp)) { FeedbackLine(fb.first, fb.second) }
            }
        } else null,
    ) { inner ->
        Column(Modifier.fillMaxSize().padding(inner).verticalScroll(rememberScrollState()).padding(16.dp)) {
            tasks.forEachIndexed { ti, task ->
                val block = answers.getOrNull(ti).orEmpty()
                val infeasible = block.isEmpty() || block.all { it.int("pfx") <= 0 }
                Card(modifier = Modifier.fillMaxWidth().padding(vertical = 8.dp)) {
                    Row(Modifier.padding(16.dp), verticalAlignment = Alignment.Top) {
                        if (done[ti]) Icon(Icons.Filled.CheckCircle, null, tint = MaterialTheme.colorScheme.primary)
                        else Text("${ti + 1}", style = MaterialTheme.typography.titleMedium, color = MaterialTheme.colorScheme.primary)
                        Spacer(Modifier.width(12.dp))
                        Text(task.str("prompt"), style = MaterialTheme.typography.bodyMedium)
                    }
                }
                if (infeasible) {
                    Hint(vm.tr("net_infeasible"))
                } else {
                    block.forEachIndexed { si, _ ->
                        val field = rows.getOrNull(ti)?.getOrNull(si) ?: return@forEachIndexed
                        Text("Podsíť ${'A' + si}", style = MaterialTheme.typography.labelLarge, color = MaterialTheme.colorScheme.primary, modifier = Modifier.padding(top = 8.dp))
                        PrefixMenu(field.pfx) { field.pfx = it }
                        WordField(field.net, { field.net = it }, "síť", numeric = true)
                        WordField(field.bc, { field.bc = it }, "broadcast", numeric = true)
                        WordField(field.lo, { field.lo = it }, "první uzel", numeric = true)
                        WordField(field.hi, { field.hi = it }, "poslední uzel", numeric = true)
                    }
                }
                Row(Modifier.padding(vertical = 8.dp), horizontalArrangement = Arrangement.spacedBy(8.dp)) {
                    if (!infeasible) {
                        PrimaryButton(vm.tr("check")) {
                            val ok = block.indices.all { si ->
                                val a = block[si]
                                val field = rows[ti][si]
                                field.pfx.removePrefix("/").toIntOrNull() == a.int("pfx") &&
                                    netTxtEq(field.net, a.str("net")) &&
                                    netTxtEq(field.bc, a.str("bcast")) &&
                                    netTxtEq(field.lo, a.str("lo")) &&
                                    netTxtEq(field.hi, a.str("hi"))
                            }
                            if (ok) {
                                done[ti] = true
                                fb = vm.tr("feedback_ok") to "ok"
                                maybeFinish()
                            } else {
                                fb = vm.tr("feedback_retry") to "err"
                            }
                        }
                    }
                    PillButton(vm.tr("net_solution")) {
                        if (!infeasible) {
                            block.forEachIndexed { si, a ->
                                val field = rows[ti][si]
                                field.pfx = "/${a.int("pfx")}"
                                field.net = a.str("net")
                                field.bc = a.str("bcast")
                                field.lo = a.str("lo")
                                field.hi = a.str("hi")
                            }
                        }
                        done[ti] = true
                        fb = "" to ""
                        maybeFinish()
                    }
                }
                if (done[ti]) Meaning(task.str("solution"), true)
                HorizontalDivider(Modifier.padding(vertical = 8.dp))
            }
        }
    }
}

@Composable
private fun PrefixMenu(value: String, onPick: (String) -> Unit) {
    var open by remember { mutableStateOf(false) }
    OutlinedButton(onClick = { open = true }, modifier = Modifier.fillMaxWidth().padding(vertical = 4.dp)) {
        Text("Prefix", color = MaterialTheme.colorScheme.onSurfaceVariant)
        Spacer(Modifier.weight(1f))
        Text(value.ifEmpty { "/…" })
        Icon(Icons.Filled.ArrowDropDown, null)
    }
    if (open) {
        WordPicker("Prefix", (8..30).map { "/$it" }) { word ->
            if (word != null) onPick(word)
            open = false
        }
    }
}

@Composable
fun LitQuizScreen(vm: AppViewModel, id: String) {
    val book = vm.content.book(id) ?: return
    val qs = book.arr("quiz")
    if (qs.isEmpty()) return
    var idx by remember { mutableIntStateOf(0) }
    var score by remember { mutableIntStateOf(0) }
    var picked by remember { mutableIntStateOf(-1) }
    var finished by remember { mutableStateOf(false) }
    val answered = picked >= 0
    DetailScaffold(
        vm, vm.tr(book.str("quizTitle")), vm.tr(book.str("quizSub")),
        bottomBar = when {
            finished -> ({ ActionBar(vm.tr("lit_again")) { idx = 0; score = 0; picked = -1; finished = false } })
            answered -> ({
                ActionBar(if (idx + 1 >= qs.size) vm.tr("lit_show_result") else vm.tr("lit_next")) {
                    if (idx + 1 >= qs.size) {
                        finished = true
                        vm.markBookQuiz(id)
                    } else {
                        idx++
                        picked = -1
                    }
                }
            })
            else -> null
        },
    ) { inner ->
        Column(Modifier.fillMaxSize().padding(inner).verticalScroll(rememberScrollState()).padding(16.dp)) {
            if (finished) {
                Card(modifier = Modifier.fillMaxWidth(), colors = CardDefaults.cardColors(containerColor = MaterialTheme.colorScheme.primaryContainer)) {
                    Column(Modifier.padding(20.dp)) {
                        Row(verticalAlignment = Alignment.CenterVertically) {
                            Icon(Icons.Filled.EmojiEvents, null)
                            Spacer(Modifier.width(8.dp))
                            Text(vm.tr("lit_finished"), style = MaterialTheme.typography.headlineSmall)
                        }
                        Spacer(Modifier.height(8.dp))
                        Text(vm.tr("lit_finished_text"), style = MaterialTheme.typography.bodyMedium)
                        Spacer(Modifier.height(8.dp))
                        Text(vm.fmt("lit_score_fmt", score, qs.size), style = MaterialTheme.typography.titleMedium)
                    }
                }
            } else {
                val q = qs[idx]
                LinearProgressIndicator(progress = { idx / qs.size.toFloat() }, modifier = Modifier.fillMaxWidth().height(6.dp).clip(CircleShape))
                Spacer(Modifier.height(6.dp))
                Row(Modifier.fillMaxWidth(), horizontalArrangement = Arrangement.SpaceBetween) {
                    Text(vm.fmt("lit_question_fmt", idx + 1, qs.size), style = MaterialTheme.typography.labelMedium, color = MaterialTheme.colorScheme.onSurfaceVariant)
                    Text(vm.fmt("lit_score_fmt", score, qs.size), style = MaterialTheme.typography.labelMedium, color = MaterialTheme.colorScheme.onSurfaceVariant)
                }
                Text(q.str("prompt"), style = MaterialTheme.typography.titleLarge, modifier = Modifier.padding(vertical = 14.dp))
                q.strs("options").forEachIndexed { i, opt ->
                    val correct = i == q.int("correct")
                    val chosen = picked == i
                    OptionRow(
                        text = opt,
                        selected = chosen,
                        mark = if (answered) (if (correct) true else if (chosen) false else null) else null,
                        enabled = !answered,
                    ) {
                        picked = i
                        if (correct) score++
                    }
                }
                if (answered) Meaning(q.str("expl"), true)
            }
        }
    }
}

@Composable
fun PlotScreen(vm: AppViewModel, id: String) {
    val book = vm.content.book(id) ?: return
    val spec = J(org.json.JSONObject().apply {
        put("type", "assembly")
        put("title", vm.tr(book.str("plotTitle")))
        put("sub", book.str("plotSub"))
        put("items", book.o.optJSONArray("plot"))
        put("meanings", book.o.optJSONArray("plotMeaning"))
    })
    DispatchExercise(vm, spec, vm.tr(book.str("plotTitle"))) { vm.markBookPlot(id) }
}