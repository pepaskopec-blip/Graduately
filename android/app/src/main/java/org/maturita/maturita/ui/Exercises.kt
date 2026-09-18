package org.maturita.maturita.ui

import androidx.compose.foundation.Canvas
import androidx.compose.foundation.background
import androidx.compose.foundation.border
import androidx.compose.foundation.clickable
import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.ExperimentalLayoutApi
import androidx.compose.foundation.layout.FlowRow
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.width
import androidx.compose.foundation.rememberScrollState
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.foundation.verticalScroll
import androidx.compose.foundation.layout.heightIn
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
import androidx.compose.ui.graphics.StrokeCap
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import androidx.compose.ui.window.Dialog
import androidx.compose.ui.window.DialogProperties
import org.maturita.maturita.AppViewModel
import org.maturita.maturita.data.J
import org.maturita.maturita.data.Palette
import org.maturita.maturita.data.answerAccepts
import org.maturita.maturita.data.answerIncomplete
import org.maturita.maturita.data.hasUmlaut
import org.maturita.maturita.data.netTxtEq
import org.maturita.maturita.data.normalizeAnswer

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
    val p = vm.palette
    Column(Modifier.fillMaxSize().padding(horizontal = 16.dp)) {
        PageTop(vm, { vm.back() }, title, sub)
        Column(Modifier.weight(1f).verticalScroll(rememberScrollState())) { content() }
        FeedbackLine(feedback, kind, p)
        Box(Modifier.padding(vertical = 12.dp)) {
            PrimaryButton(action, p, onClick = onAction)
        }
    }
}

@Composable
fun GermanExercise(vm: AppViewModel, unitId: Int, ex: Int) {
    val spec = vm.content.germanUnit(unitId)?.exercise(ex) ?: return
    DispatchExercise(vm, spec, vm.tr(spec.str("title").ifEmpty { spec.str("title") }).ifEmpty { spec.str("title") }, onDone = { vm.markGerman(unitId, ex) })
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
        else -> Text("?", color = vm.palette.text)
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
    val p = vm.palette
    ExerciseScaffold(vm, title, sub, vm.tr("check"), {
        var ok = 0
        qs.forEachIndexed { i, q -> if (picks[i] == q.int("correct")) ok++ }
        if (ok == qs.size) { fb = vm.tr("feedback_ok") to "ok"; onDone() }
        else fb = vm.tr("feedback_retry_short") to "err"
        revealed = true
    }, fb.first, fb.second) {
        qs.forEachIndexed { i, q ->
            Text("${i + 1}. ${q.str("prompt")}", color = p.text, fontWeight = FontWeight.SemiBold, modifier = Modifier.padding(bottom = 6.dp))
            q.strs("options").forEachIndexed { o, opt ->
                val sel = picks[i] == o
                val mark = if (revealed && sel) picks[i] == q.int("correct") else null
                OptionChip(opt, sel, mark, p) { picks[i] = o }
            }
            if (revealed) {
                meanings.getOrNull(i)?.let { Meaning(vm.tr(it), p, true) }
                expls.getOrNull(i)?.let { Meaning(it, p, true) }
            }
            Spacer(Modifier.height(10.dp))
        }
    }
}

@Composable
private fun OptionChip(text: String, selected: Boolean, mark: Boolean?, p: Palette, onClick: () -> Unit) {
    val border = when (mark) {
        true -> p.success
        false -> p.error
        null -> if (selected) p.accent else p.text.copy(0.1f)
    }
    Box(
        Modifier
            .fillMaxWidth()
            .padding(bottom = 6.dp)
            .clip(RoundedCornerShape(14.dp))
            .background(if (selected) p.surface1 else p.mantle)
            .border(1.dp, border, RoundedCornerShape(14.dp))
            .clickable(onClick = onClick)
            .padding(12.dp),
    ) { Text(text, color = p.text) }
}

@OptIn(ExperimentalLayoutApi::class)
@Composable
private fun AssemblyEx(vm: AppViewModel, title: String, sub: String?, spec: J, onDone: () -> Unit) {
    val items = spec.arr("items")
    val meanings = spec.strs("meanings")
    val placed = remember {
        items.map { item -> mutableStateListOf<Int>() }
    }
    val pools = remember {
        items.map { item ->
            val n = item.strs("words").size
            mutableStateListOf(*((0 until n).shuffled().toTypedArray()))
        }
    }
    var fb by remember { mutableStateOf("" to "") }
    var revealed by remember { mutableStateOf(false) }
    val p = vm.palette
    ExerciseScaffold(vm, title, sub, vm.tr("check"), {
        val all = items.indices.all { i ->
            val words = items[i].strs("words")
            placed[i].size == words.size && placed[i].mapIndexed { idx, w -> words[w] == words[idx] }.all { it } ||
                placed[i].map { words[it] } == words
        }
        // compare placed order to original word order
        val ok = items.indices.all { i ->
            val words = items[i].strs("words")
            placed[i].size == words.size && placed[i].map { words[it] } == words
        }
        if (ok) { fb = vm.tr("feedback_ok") to "ok"; onDone() }
        else fb = vm.tr("feedback_sentences") to "err"
        revealed = true
    }, fb.first, fb.second) {
        items.forEachIndexed { i, item ->
            item.strOrNull("prompt")?.let { Hint(it, p) }
            FlowRow(Modifier.fillMaxWidth().padding(bottom = 6.dp).clip(RoundedCornerShape(14.dp)).background(p.surface0).padding(8.dp), horizontalArrangement = Arrangement.spacedBy(6.dp)) {
                if (placed[i].isEmpty()) Text("…", color = p.overlay)
                placed[i].forEach { idx ->
                    val word = item.strs("words").getOrNull(idx) ?: return@forEach
                    Chip(word, p) {
                        placed[i].remove(idx)
                        pools[i].add(idx)
                    }
                }
            }
            FlowRow(horizontalArrangement = Arrangement.spacedBy(6.dp), verticalArrangement = Arrangement.spacedBy(6.dp)) {
                pools[i].toList().forEach { idx ->
                    val word = item.strs("words").getOrNull(idx) ?: return@forEach
                    Chip(word, p) {
                        pools[i].remove(idx)
                        placed[i].add(idx)
                    }
                }
            }
            if (revealed) meanings.getOrNull(i)?.let { Meaning(vm.tr(it), p, true) }
            Spacer(Modifier.height(12.dp))
        }
    }
}

@Composable
private fun Chip(text: String, p: Palette, onClick: () -> Unit) {
    Box(
        Modifier
            .clip(RoundedCornerShape(999.dp))
            .background(p.mantle)
            .border(1.dp, p.text.copy(0.1f), RoundedCornerShape(999.dp))
            .clickable(onClick = onClick)
            .padding(horizontal = 12.dp, vertical = 8.dp),
    ) { Text(text, color = p.text, fontWeight = FontWeight.Medium) }
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
    val p = vm.palette
    val umlaut = items.any { q ->
        q.answerList().any { hasUmlaut(it) } || hasUmlaut(q.str("prompt"))
    }
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
        spec.strOrNull("note")?.let { Hint(it, p) }
        spec.strOrNull("sample")?.let { Hint(it, p) }
        spec.strOrNull("bank")?.let { Hint("${vm.tr("wordbank")} $it", p) }
        if (umlaut) Hint(vm.tr("hint_umlauts"), p)
        items.forEachIndexed { i, q ->
            Prompt(q.str("prompt"), p)
            val answers = q.answerList()
            val hints = q.strs("hints")
            fields[i].forEachIndexed { j, value ->
                val ans = answers.getOrNull(j) ?: ""
                WordField(
                    value,
                    { fields[i][j] = it },
                    p,
                    hints.getOrNull(j) ?: vm.tr("your_answer"),
                    if (revealed) answerAccepts(normalizeAnswer(value), ans) else null,
                )
            }
            if (revealed) {
                q.strOrNull("meaning")?.let { Meaning(vm.tr(it), p, true) }
                q.strOrNull("mean")?.let { Meaning(vm.tr(it), p, true) }
                q.strOrNull("german")?.let { Meaning(it, p, true) }
                q.strOrNull("shown")?.let { Meaning(it, p, true) }
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
    val p = vm.palette
    val values = remember { mutableStateListOf(*Array(qs.size) { "" }) }
    ExerciseScaffold(vm, title, sub, vm.tr("finish"), {
        fb = vm.tr("feedback_ok") to "ok"
        qs.indices.forEach { shown[it] = true }
        onDone()
    }, fb.first, fb.second) {
        spec.strOrNull("tip")?.let { Hint(vm.tr(it), p) }
        qs.forEachIndexed { i, q ->
            Prompt(q.str("question").ifEmpty { q.str("stem") }, p)
            WordField(values[i], { values[i] = it }, p, vm.tr("your_answer"))
            PillButton(vm.tr("show_sample"), p, Modifier.padding(vertical = 6.dp)) { shown[i] = !shown[i] }
            if (shown[i]) {
                val sample = q.str("sample")
                val qcs = q.str("qCs")
                val acs = q.str("aCs")
                Meaning(
                    if (qcs.isNotEmpty()) vm.fmt("sample_fmt", vm.tr(qcs), sample, vm.tr(acs)) else sample,
                    p, true,
                )
            }
            Spacer(Modifier.height(10.dp))
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
    val p = vm.palette
    ExerciseScaffold(vm, title, sub, vm.tr("check"), {
        val ok = items.indices.all { loc[it] == items[it].int("group") }
        if (ok) { fb = vm.tr("feedback_ok") to "ok"; onDone() }
        else fb = vm.tr("feedback_retry") to "err"
        revealed = true
    }, fb.first, fb.second) {
        Hint(vm.tr("assign_hint"), p)
        Row(horizontalArrangement = Arrangement.spacedBy(8.dp)) {
            groups.forEachIndexed { i, g ->
                SegChip(g, active == i, p) { active = i }
            }
        }
        Spacer(Modifier.height(10.dp))
        Text(vm.tr("wordbank"), color = p.subtext, fontSize = 12.sp)
        FlowRow(horizontalArrangement = Arrangement.spacedBy(6.dp), verticalArrangement = Arrangement.spacedBy(6.dp)) {
            items.forEachIndexed { i, it ->
                if (loc[i] == -1) {
                    val label = listOfNotNull(it.strOrNull("emoji"), it.str("label")).joinToString(" ")
                    Chip(label, p) { loc[i] = active }
                }
            }
        }
        groups.forEachIndexed { g, name ->
            Spacer(Modifier.height(10.dp))
            Text(name, color = p.subtext, fontSize = 12.sp, fontWeight = FontWeight.SemiBold)
            FlowRow(horizontalArrangement = Arrangement.spacedBy(6.dp), verticalArrangement = Arrangement.spacedBy(6.dp)) {
                items.forEachIndexed { i, it ->
                    if (loc[i] == g) {
                        val label = listOfNotNull(it.strOrNull("emoji"), it.str("label")).joinToString(" ")
                        Chip(label, p) { loc[i] = -1 }
                    }
                }
            }
        }
        if (revealed) items.forEachIndexed { i, _ ->
            meanings.getOrNull(i)?.let { Meaning(vm.tr(it), p, true) }
        }
    }
}

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
    val p = vm.palette
    val cur = words.getOrNull(word)?.uppercase() ?: ""
    fun solved() = cur.all { ch ->
        val idx = letters.indexOfFirst { it.equals(ch.toString(), true) }
        idx >= 0 && guessed[idx]
    }
    Column(Modifier.fillMaxSize().padding(horizontal = 16.dp)) {
        PageTop(vm, { vm.back() }, title, sub)
        Text(vm.fmt("hm_progress", word + 1, words.size), color = p.subtext)
        tips.getOrNull(word)?.let { Hint("${vm.tr("hm_hint")}: ${vm.tr(it)}", p) }
        Canvas(Modifier.fillMaxWidth().height(160.dp)) {
            val x = size.width * 0.3f
            drawLine(p.text, Offset(40f, size.height - 10), Offset(size.width * 0.55f, size.height - 10), 6f, StrokeCap.Round)
            drawLine(p.text, Offset(70f, size.height - 10), Offset(70f, 16f), 6f, StrokeCap.Round)
            drawLine(p.text, Offset(70f, 16f), Offset(x + 40, 16f), 6f, StrokeCap.Round)
            if (misses > 0) drawLine(p.text, Offset(x + 40, 16f), Offset(x + 40, 36f), 4f)
            if (misses > 1) drawCircle(p.text, 16f, Offset(x + 40, 52f), style = androidx.compose.ui.graphics.drawscope.Stroke(4f))
            if (misses > 2) drawLine(p.text, Offset(x + 40, 68f), Offset(x + 40, 110f), 4f)
            if (misses > 3) drawLine(p.text, Offset(x + 40, 80f), Offset(x + 18, 100f), 4f)
            if (misses > 4) drawLine(p.text, Offset(x + 40, 80f), Offset(x + 62, 100f), 4f)
            if (misses > 5) drawLine(p.text, Offset(x + 40, 110f), Offset(x + 18, 140f), 4f)
        }
        Row(Modifier.fillMaxWidth(), horizontalArrangement = Arrangement.spacedBy(6.dp)) {
            cur.forEach { ch ->
                val idx = letters.indexOfFirst { it.equals(ch.toString(), true) }
                val show = idx >= 0 && guessed[idx]
                Text(if (show) ch.toString() else "_", color = p.text, fontSize = 26.sp, fontWeight = FontWeight.Bold)
            }
        }
        Spacer(Modifier.height(8.dp))
        letters.chunked(10).forEach { row ->
            Row(horizontalArrangement = Arrangement.spacedBy(4.dp)) {
                row.forEach { letter ->
                    val i = letters.indexOf(letter)
                    Box(
                        Modifier
                            .clip(RoundedCornerShape(8.dp))
                            .background(if (guessed[i]) p.surface1 else p.mantle)
                            .clickable(enabled = !guessed[i] && !finished) {
                                guessed[i] = true
                                if (!cur.contains(letter, true)) {
                                    misses++
                                    if (misses >= 6) {
                                        fb = vm.fmt("hm_fail", cur) to "err"
                                    }
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
                            }
                            .padding(horizontal = 10.dp, vertical = 8.dp),
                    ) { Text(letter, color = p.text, fontWeight = FontWeight.Bold) }
                }
            }
            Spacer(Modifier.height(4.dp))
        }
        FeedbackLine(fb.first, fb.second, p)
        if (misses >= 6 && !finished) {
            PillButton(vm.tr("hm_retry"), p) {
                misses = 0
                guessed.indices.forEach { guessed[it] = false }
                fb = "" to ""
            }
        }
    }
}

@Composable
private fun VocabEx(vm: AppViewModel, title: String, sub: String?, spec: J, onDone: () -> Unit) {
    val cards = remember {
        spec.arr("sections").flatMap { sec ->
            sec.arr("rows").map { Triple(sec.str("header"), it.str("prompt"), it.str("answers")) }
        }.toMutableList()
    }
    var index by remember { mutableIntStateOf(0) }
    var done by remember { mutableIntStateOf(0) }
    var value by remember { mutableStateOf("") }
    var fb by remember { mutableStateOf("" to "") }
    val total = remember { cards.size }
    val p = vm.palette
    val cur = cards.getOrNull(index)
    Column(Modifier.fillMaxSize().padding(horizontal = 16.dp)) {
        PageTop(vm, { vm.back() }, title, sub)
        Text(vm.fmt("trans_progress", done, total), color = p.subtext)
        if (cur == null) {
            Text(vm.tr("trans_done"), color = p.success, fontWeight = FontWeight.Bold)
        } else {
            Hint(cur.first, p)
            Prompt(cur.second, p)
            if (hasUmlaut(cur.third)) Hint(vm.tr("hint_umlauts"), p)
            WordField(value, { value = it }, p, vm.tr("your_answer"))
            Spacer(Modifier.height(10.dp))
            PrimaryButton(vm.tr("check"), p) {
                val norm = normalizeAnswer(value)
                when {
                    answerAccepts(norm, cur.third) -> {
                        done++
                        cards.removeAt(index)
                        if (cards.isEmpty()) { fb = vm.tr("trans_done") to "ok"; onDone() }
                        else { if (index >= cards.size) index = 0; value = ""; fb = "" to "" }
                    }
                    answerIncomplete(norm, cur.third) -> fb = vm.tr("trans_incomplete") to "warn"
                    else -> {
                        fb = vm.fmt("trans_wrong", cur.third.split('|').first()) to "err"
                        val item = cards.removeAt(index)
                        cards.add(item)
                        if (index >= cards.size) index = 0
                        value = ""
                    }
                }
            }
            FeedbackLine(fb.first, fb.second, p)
        }
    }
}

@Composable
private fun DialogEx(vm: AppViewModel, title: String, sub: String?, spec: J, onDone: () -> Unit) {
    val pool = spec.strs("pool")
    val meanings = spec.strs("meanings")
    val rows = spec.arr("dialogues").flatMap { it.arr("rows") }
    ComboRows(vm, title, sub, pool, rows.size, onDone) { picks, expected, revealed ->
        var i = 0
        spec.arr("dialogues").forEach { d ->
            Text(d.str("name"), color = vm.palette.subtext, fontWeight = FontWeight.SemiBold, modifier = Modifier.padding(vertical = 6.dp))
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
        ComboRows(vm, title, sub, spec.strs("pool"), allRows.size, onDone, spec.bool("transUpfront")) { picks, expected, revealed ->
            var i = 0
            sections.forEach { sec ->
                Text(sec.str("title"), color = vm.palette.subtext, fontWeight = FontWeight.SemiBold)
                sec.arr("rows").forEachIndexed { ri, row ->
                    val idx = i++
                    ComboLine(
                        vm, spec.strs("pool"), picks, expected, idx, row.str("answer"),
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
        ComboRows(vm, title, sub, emptyList(), rows.size * 2, onDone) { picks, expected, revealed ->
            rows.forEachIndexed { i, r ->
                ComboLine(vm, spec.strs("fwPool"), picks, expected, i * 2, r.str("fw"), prefix = "${i + 1}. ", suffix = r.str("mid"), revealed = revealed)
                ComboLine(vm, spec.strs("ansPool"), picks, expected, i * 2 + 1, r.str("ans"), prefix = "→ ", meaning = meanings.getOrNull(i), revealed = revealed)
            }
        }
        return
    }
    if (type == "fill") {
        val blanks = rows.sumOf { it.answerList().size }
        ComboRows(vm, title, sub, pool, blanks, onDone, spec.bool("preMeaning")) { picks, expected, revealed ->
            spec.strOrNull("sample")?.let { Hint(it, vm.palette) }
            var i = 0
            rows.forEachIndexed { ri, r ->
                val segs = r.strsOrEmpty("segs")
                val ans = r.answerList()
                FlowRow(
                    Modifier.fillMaxWidth().padding(bottom = 6.dp),
                    verticalArrangement = Arrangement.Center,
                    horizontalArrangement = Arrangement.spacedBy(4.dp),
                ) {
                    r.strOrNull("num")?.let { Text(it, color = vm.palette.text, fontWeight = FontWeight.SemiBold) }
                    segs.forEachIndexed { si, seg ->
                        if (!seg.isNullOrEmpty()) Text(seg, color = vm.palette.text)
                        if (si < ans.size) {
                            val idx = i++
                            ComboLine(vm, pool, picks, expected, idx, ans[si], revealed = revealed, inline = true)
                        }
                    }
                    meanings.getOrNull(ri)?.let { Text(vm.tr(it), color = vm.palette.subtext, fontSize = 12.sp) }
                }
                if (revealed || spec.bool("preMeaning") || spec.bool("showGerman")) {
                    r.strOrNull("mean")?.let { Meaning(vm.tr(it), vm.palette, true) }
                }
            }
        }
        return
    }
    ComboRows(vm, title, sub, pool, rows.size, onDone, spec.bool("preMeaning")) { picks, expected, revealed ->
        spec.strOrNull("sample")?.let { Hint(it, vm.palette) }
        rows.forEachIndexed { i, r ->
            val prefix = when (type) {
                "number" -> "${r.str("digits")} = "
                "count" -> "${List(r.int("count")) { r.str("emoji") }.joinToString(" ")} ${r.str("noun")} "
                "seq" -> r.str("before")
                "verbclue" -> "${r.str("emoji")} ${r.str("before")}"
                else -> r.str("before")
            }
            val suffix = when (type) {
                "seq" -> r.str("after")
                "verbclue", "verb" -> r.str("after")
                else -> ""
            }
            ComboLine(vm, pool, picks, expected, i, r.str("answer"), prefix, suffix, meanings.getOrNull(i) ?: r.strOrNull("mean"), revealed || spec.bool("preMeaning"))
        }
    }
}

@Composable
private fun ComboRows(
    vm: AppViewModel,
    title: String,
    sub: String?,
    @Suppress("UNUSED_PARAMETER") pool: List<String>,
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
    val p = vm.palette
    var open by remember { mutableStateOf(false) }
    val pick = picks.getOrNull(idx).orEmpty()
    val mark = if (revealed && pick.isNotEmpty()) {
        answerAccepts(normalizeAnswer(pick), answer)
    } else null
    val chip = @Composable {
        Box(
            Modifier
                .clip(RoundedCornerShape(12.dp))
                .background(p.mantle)
                .border(
                    1.dp,
                    when (mark) {
                        true -> p.success
                        false -> p.error
                        null -> p.text.copy(0.12f)
                    },
                    RoundedCornerShape(12.dp),
                )
                .clickable { open = true }
                .padding(horizontal = 12.dp, vertical = 8.dp),
        ) {
            Text(
                pick.ifEmpty { "—" },
                style = appTextStyle(15.sp, color = if (pick.isEmpty()) p.overlay else p.text),
            )
        }
    }
    if (inline) {
        chip()
    } else {
        FlowRow(
            Modifier.fillMaxWidth().padding(bottom = 8.dp),
            horizontalArrangement = Arrangement.spacedBy(6.dp),
            verticalArrangement = Arrangement.Center,
        ) {
            if (prefix.isNotBlank()) {
                Text(prefix, style = appTextStyle(16.sp, color = p.text))
            }
            chip()
            if (suffix.isNotBlank()) {
                Text(suffix, style = appTextStyle(16.sp, color = p.text))
            }
        }
    }
    if (open) {
        WordPicker(p, pool) { word ->
            if (word != null && idx in picks.indices) picks[idx] = word
            open = false
        }
    }
    if (revealed && !meaning.isNullOrBlank()) Meaning(vm.tr(meaning), p, true)
}

@Composable
private fun WordPicker(p: Palette, pool: List<String>, onPick: (String?) -> Unit) {
    Dialog(
        onDismissRequest = { onPick(null) },
        properties = DialogProperties(usePlatformDefaultWidth = true),
    ) {
        Column(
            Modifier
                .fillMaxWidth()
                .clip(RoundedCornerShape(20.dp))
                .background(p.base)
                .padding(10.dp)
                .heightIn(max = 420.dp)
                .verticalScroll(rememberScrollState()),
        ) {
            pool.forEach { w ->
                Text(
                    w,
                    style = appTextStyle(16.sp, color = p.text),
                    modifier = Modifier
                        .fillMaxWidth()
                        .clip(RoundedCornerShape(12.dp))
                        .clickable { onPick(w) }
                        .padding(horizontal = 14.dp, vertical = 12.dp),
                )
            }
        }
    }
}

@OptIn(ExperimentalLayoutApi::class)
@Composable
private fun Ex2Ex(vm: AppViewModel, title: String, sub: String?, spec: J, onDone: () -> Unit) {
    val items = spec.arr("items")
    val pool = spec.strs("pool")
    val blanks = items.sumOf { it.arr("rows").sumOf { r -> r.strs("answers").size } }
    ComboRows(vm, title, sub, pool, blanks, onDone) { picks, expected, revealed ->
        var i = 0
        items.forEach { item ->
            Meaning(item.str("czech"), vm.palette, true)
            item.arr("rows").forEach { row ->
                val segs = row.strsOrEmpty("segs")
                val ans = row.strs("answers")
                FlowRow(verticalArrangement = Arrangement.Center, horizontalArrangement = Arrangement.spacedBy(4.dp)) {
                    row.strOrNull("num")?.let { Text(it, color = vm.palette.text, modifier = Modifier.padding(end = 6.dp)) }
                    segs.forEachIndexed { si, seg ->
                        if (!seg.isNullOrEmpty()) Text(seg, color = vm.palette.text)
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
    val fields = remember {
        mutableStateListOf(*rows.flatMap { it.strs("answers") }.map { "" }.toTypedArray())
    }
    var fb by remember { mutableStateOf("" to "") }
    var revealed by remember { mutableStateOf(false) }
    val p = vm.palette
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
        if (rows.any { it.strs("answers").any { a -> hasUmlaut(a) } }) Hint(vm.tr("hint_umlauts"), p)
        var i = 0
        rows.forEach { r ->
            r.strOrNull("num")?.let { Text(it, color = p.subtext) }
            FlowRow(verticalArrangement = Arrangement.Center) {
                val segs = r.strsOrEmpty("segs")
                val ans = r.strs("answers")
                segs.forEachIndexed { si, seg ->
                    if (!seg.isNullOrEmpty()) Text(seg, color = p.text)
                    if (si < ans.size) {
                        val idx = i++
                        Box(Modifier.width(72.dp).padding(horizontal = 2.dp)) {
                            WordField(fields[idx], { fields[idx] = it }, p, "",
                                if (revealed) answerAccepts(normalizeAnswer(fields[idx]), ans[si]) else null)
                        }
                    }
                }
            }
            if (revealed) r.strOrNull("czech")?.let { Meaning(it, p, true) }
            if (revealed) r.strOrNull("mean")?.let { Meaning(it, p, true) }
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
    val p = vm.palette
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
            Text(noun, color = p.text, fontWeight = FontWeight.Bold)
            persons.forEachIndexed { c, person ->
                val i = r * persons.size + c
                Text(person, color = p.subtext, fontSize = 12.sp)
                WordField(fields[i], { fields[i] = it }, p, "")
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
    val p = vm.palette
    ExerciseScaffold(vm, title, sub, vm.tr("finish"), {
        items.indices.forEach { shown[it] = true }
        fb = vm.tr("feedback_ok") to "ok"
        onDone()
    }, fb.first, fb.second) {
        spec.strOrNull("note")?.let { Hint(it, p) }
        items.forEachIndexed { i, it ->
            Prompt(it.str("prompt"), p)
            PillButton(vm.tr("show_sample"), p, Modifier.padding(vertical = 6.dp)) { shown[i] = !shown[i] }
            if (shown[i]) Meaning(it.str("solution"), p, true)
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
        VlsmEx(vm, lesson)
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
private fun VlsmEx(vm: AppViewModel, @Suppress("UNUSED_PARAMETER") lesson: J) {
    val tasks = vm.content.netTasks
    val answers = vm.content.netAnswers
    val done = remember { mutableStateListOf(*Array(tasks.size) { false }) }
    val rows = remember {
        answers.map { block -> List(block.size) { VlsmRow() } }
    }
    var fb by remember { mutableStateOf("" to "") }
    val p = vm.palette
    fun maybeFinish() {
        if (done.isNotEmpty() && done.all { it }) vm.markNet(1)
    }
    Column(Modifier.fillMaxSize().padding(horizontal = 16.dp)) {
        PageTop(vm, { vm.back() }, vm.tr("net_ex_title"), null)
        Column(Modifier.weight(1f).verticalScroll(rememberScrollState())) {
            tasks.forEachIndexed { ti, task ->
                val block = answers.getOrNull(ti).orEmpty()
                val infeasible = block.isEmpty() || block.all { it.int("pfx") <= 0 }
                Text(task.str("prompt"), color = p.text, modifier = Modifier.padding(vertical = 8.dp))
                if (infeasible) {
                    Hint(vm.tr("net_infeasible"), p)
                } else {
                    block.forEachIndexed { si, a ->
                        val field = rows.getOrNull(ti)?.getOrNull(si) ?: return@forEachIndexed
                        Text("Podsíť ${'A' + si}", color = p.subtext, fontWeight = FontWeight.SemiBold)
                        PrefixMenu(field.pfx, p) { field.pfx = it }
                        WordField(field.net, { field.net = it }, p, "síť")
                        WordField(field.bc, { field.bc = it }, p, "broadcast")
                        WordField(field.lo, { field.lo = it }, p, "první uzel")
                        WordField(field.hi, { field.hi = it }, p, "poslední uzel")
                    }
                    PillButton(vm.tr("check"), p, Modifier.padding(vertical = 6.dp)) {
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
                PillButton(vm.tr("net_solution"), p) {
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
                if (done[ti]) Meaning(task.str("solution"), p, true)
                Spacer(Modifier.height(12.dp))
            }
        }
        FeedbackLine(fb.first, fb.second, p)
    }
}

@Composable
private fun PrefixMenu(value: String, p: Palette, onPick: (String) -> Unit) {
    var open by remember { mutableStateOf(false) }
    Box(
        Modifier
            .padding(vertical = 4.dp)
            .clip(RoundedCornerShape(12.dp))
            .background(p.mantle)
            .clickable { open = true }
            .padding(10.dp),
    ) {
        Text(value.ifEmpty { "/…" }, style = appTextStyle(15.sp, color = p.text))
    }
    if (open) {
        WordPicker(p, (8..30).map { "/$it" }) { word ->
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
    var answered by remember { mutableStateOf(false) }
    var finished by remember { mutableStateOf(false) }
    val p = vm.palette
    val kahoot = listOf(p.accent, p.error, p.warning, p.success)
    Column(Modifier.fillMaxSize().padding(16.dp)) {
        PageTop(vm, { vm.back() }, vm.tr(book.str("quizTitle")), vm.tr(book.str("quizSub")))
        if (finished) {
            Text(vm.tr("lit_finished"), color = p.text, fontWeight = FontWeight.Bold, fontSize = 22.sp)
            Text(vm.tr("lit_finished_text"), color = p.subtext)
            Text(vm.fmt("lit_score_fmt", score, qs.size), color = p.text, fontWeight = FontWeight.Bold)
            Spacer(Modifier.height(12.dp))
            PrimaryButton(vm.tr("lit_again"), p) { idx = 0; score = 0; answered = false; finished = false }
        } else {
            val q = qs[idx]
            Row(Modifier.fillMaxWidth(), horizontalArrangement = Arrangement.SpaceBetween) {
                Text(vm.fmt("lit_question_fmt", idx + 1, qs.size), color = p.subtext)
                Text(vm.fmt("lit_score_fmt", score, qs.size), color = p.subtext)
            }
            Text(q.str("prompt"), color = p.text, fontSize = 18.sp, fontWeight = FontWeight.SemiBold, modifier = Modifier.padding(vertical = 12.dp))
            q.strs("options").forEachIndexed { i, opt ->
                Box(
                    Modifier
                        .fillMaxWidth()
                        .padding(bottom = 8.dp)
                        .clip(RoundedCornerShape(16.dp))
                        .background(kahoot[i % 4])
                        .clickable(enabled = !answered) {
                            answered = true
                            if (i == q.int("correct")) score++
                        }
                        .padding(16.dp),
                ) { Text(opt, color = p.onAccent, fontWeight = FontWeight.SemiBold) }
            }
            if (answered) {
                Meaning(q.str("expl"), p, true)
                Spacer(Modifier.height(8.dp))
                PrimaryButton(if (idx + 1 >= qs.size) vm.tr("lit_show_result") else vm.tr("lit_next"), p) {
                    if (idx + 1 >= qs.size) {
                        finished = true
                        vm.markBookQuiz(id)
                    } else {
                        idx++
                        answered = false
                    }
                }
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
