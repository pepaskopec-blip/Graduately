package org.maturita.maturita.ui

import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.BoxWithConstraints
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
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.foundation.text.BasicTextField
import androidx.compose.foundation.verticalScroll
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.filled.Cancel
import androidx.compose.material.icons.filled.CheckCircle
import androidx.compose.material.icons.filled.Code
import androidx.compose.material.icons.filled.PlayArrow
import androidx.compose.material.icons.filled.Refresh
import androidx.compose.material.icons.filled.Terminal
import androidx.compose.material3.Button
import androidx.compose.material3.CircularProgressIndicator
import androidx.compose.material3.HorizontalDivider
import androidx.compose.material3.Icon
import androidx.compose.material3.IconButton
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.OutlinedButton
import androidx.compose.material3.OutlinedTextField
import androidx.compose.material3.Surface
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.runtime.setValue
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.graphics.SolidColor
import androidx.compose.ui.graphics.luminance
import androidx.compose.ui.text.AnnotatedString
import androidx.compose.ui.text.SpanStyle
import androidx.compose.ui.text.TextStyle
import androidx.compose.ui.text.buildAnnotatedString
import androidx.compose.ui.text.font.FontFamily
import androidx.compose.ui.text.input.OffsetMapping
import androidx.compose.ui.text.input.TransformedText
import androidx.compose.ui.text.input.VisualTransformation
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.max
import androidx.compose.ui.unit.min
import androidx.compose.ui.unit.sp
import org.maturita.maturita.AppViewModel
import org.maturita.maturita.data.J

/** Explanation on one side, the program and its output on the other. */
@Composable
fun CodeLessonScreen(vm: AppViewModel, lesson: J, onSuccess: () -> Unit) {
    val key = lesson.str("key")
    val starter = lesson.str("code")
    var code by remember(key) { mutableStateOf(starter) }
    var stdin by remember(key) { mutableStateOf("") }
    var output by remember(key) { mutableStateOf("") }
    var status by remember(key) { mutableStateOf("") }
    val running = status == "running"
    val needsInput = "ReadLine" in code

    fun run() {
        if (running) return
        status = "running"
        output = vm.tr("cs_running")
        vm.platform.runCSharp(code, if (needsInput) stdin else "") { result ->
            status = result.status
            output = when (result.status) {
                "unavailable" -> vm.tr("cs_unavailable")
                "timeout" -> (result.output + "\n\n" + vm.tr("cs_timeout")).trim()
                else -> result.output.ifEmpty { vm.tr("cs_empty") }
            }
            if (result.status == "ok") onSuccess()
        }
    }

    fun reset() {
        code = starter
        output = ""
        status = ""
    }

    DetailScaffold(vm, vm.tr(lesson.str("titleKey")), vm.tr(lesson.str("subKey")), maxContentWidth = 1640.dp) { inner ->
        BoxWithConstraints(Modifier.fillMaxSize().padding(inner)) {
            val width = maxWidth
            val height = maxHeight
            if (width >= 900.dp) {
                Row(
                    Modifier.fillMaxSize().padding(horizontal = 24.dp, vertical = 16.dp),
                    horizontalArrangement = Arrangement.spacedBy(24.dp),
                ) {
                    Column(Modifier.width(min(560.dp, width * 0.4f)).fillMaxHeight().verticalScroll(rememberScrollState())) {
                        Explanation(lesson)
                    }
                    Column(Modifier.weight(1f).fillMaxHeight(), verticalArrangement = Arrangement.spacedBy(16.dp)) {
                        EditorCard(vm, code, { code = it }, running, ::run, ::reset, compact = false, Modifier.weight(1f))
                        if (needsInput) InputField(vm, stdin) { stdin = it }
                        OutputCard(vm, output, status, Modifier.height(max(180.dp, height * 0.32f)))
                    }
                }
            } else {
                Column(
                    Modifier.fillMaxSize().verticalScroll(rememberScrollState()).padding(16.dp),
                    verticalArrangement = Arrangement.spacedBy(16.dp),
                ) {
                    Explanation(lesson)
                    EditorCard(vm, code, { code = it }, running, ::run, ::reset, compact = true, Modifier.height(320.dp))
                    if (needsInput) InputField(vm, stdin) { stdin = it }
                    OutputCard(vm, output, status, Modifier.height(220.dp))
                }
            }
        }
    }
}

@Composable
private fun Explanation(lesson: J) {
    Column(verticalArrangement = Arrangement.spacedBy(28.dp)) {
        lesson.arr("slides").forEach { SlideContent(it) }
    }
}

@Composable
private fun EditorCard(
    vm: AppViewModel,
    code: String,
    onCode: (String) -> Unit,
    running: Boolean,
    onRun: () -> Unit,
    onReset: () -> Unit,
    compact: Boolean,
    modifier: Modifier,
) {
    val dark = MaterialTheme.colorScheme.background.luminance() < 0.5f
    val highlight = remember(dark) { CSharpHighlight(dark) }
    Surface(modifier.fillMaxWidth(), shape = RoundedCornerShape(16.dp), color = MaterialTheme.colorScheme.surfaceContainerHigh) {
        Column {
            Row(
                Modifier.fillMaxWidth().padding(horizontal = 14.dp, vertical = 8.dp),
                verticalAlignment = Alignment.CenterVertically,
                horizontalArrangement = Arrangement.spacedBy(10.dp),
            ) {
                Icon(Icons.Filled.Code, null, tint = MaterialTheme.colorScheme.primary)
                Text("Program.cs", style = MaterialTheme.typography.titleSmall.copy(fontFamily = FontFamily.Monospace))
                Spacer(Modifier.weight(1f))
                if (compact) {
                    IconButton(onClick = onReset, enabled = !running) {
                        Icon(Icons.Filled.Refresh, contentDescription = vm.tr("cs_reset"))
                    }
                } else {
                    OutlinedButton(onClick = onReset, enabled = !running) {
                        Icon(Icons.Filled.Refresh, null, Modifier.size(18.dp))
                        Spacer(Modifier.width(6.dp))
                        Text(vm.tr("cs_reset"))
                    }
                }
                Button(onClick = onRun, enabled = !running) {
                    Icon(Icons.Filled.PlayArrow, null, Modifier.size(18.dp))
                    Spacer(Modifier.width(6.dp))
                    Text(vm.tr("cs_run"))
                }
            }
            HorizontalDivider(color = MaterialTheme.colorScheme.outlineVariant)
            BasicTextField(
                value = code,
                onValueChange = onCode,
                textStyle = codeStyle().copy(color = MaterialTheme.colorScheme.onSurface),
                cursorBrush = SolidColor(MaterialTheme.colorScheme.primary),
                visualTransformation = highlight,
                modifier = Modifier.fillMaxSize().padding(14.dp),
            )
        }
    }
}

@Composable
private fun InputField(vm: AppViewModel, stdin: String, onStdin: (String) -> Unit) {
    Column(verticalArrangement = Arrangement.spacedBy(6.dp)) {
        Text(vm.tr("cs_stdin"), style = MaterialTheme.typography.labelLarge, color = MaterialTheme.colorScheme.onSurfaceVariant)
        OutlinedTextField(
            value = stdin,
            onValueChange = onStdin,
            textStyle = codeStyle(),
            minLines = 1,
            maxLines = 4,
            modifier = Modifier.fillMaxWidth(),
        )
    }
}

@Composable
private fun OutputCard(vm: AppViewModel, output: String, status: String, modifier: Modifier) {
    val muted = output.isEmpty() || status == "running"
    Surface(modifier.fillMaxWidth(), shape = RoundedCornerShape(16.dp), color = MaterialTheme.colorScheme.surfaceContainerHigh) {
        Column {
            Row(
                Modifier.fillMaxWidth().height(48.dp).padding(horizontal = 14.dp),
                verticalAlignment = Alignment.CenterVertically,
            ) {
                Icon(Icons.Filled.Terminal, null, tint = MaterialTheme.colorScheme.onSurfaceVariant, modifier = Modifier.size(18.dp))
                Spacer(Modifier.width(8.dp))
                Text(vm.tr("cs_output"), style = MaterialTheme.typography.titleSmall)
                Spacer(Modifier.weight(1f))
                StatusBadge(vm, status)
            }
            HorizontalDivider(color = MaterialTheme.colorScheme.outlineVariant)
            Text(
                output.ifEmpty { vm.tr("cs_empty") },
                style = codeStyle(),
                color = if (muted) MaterialTheme.colorScheme.onSurfaceVariant else MaterialTheme.colorScheme.onSurface,
                modifier = Modifier.fillMaxSize().verticalScroll(rememberScrollState()).padding(14.dp),
            )
        }
    }
}

@Composable
private fun StatusBadge(vm: AppViewModel, status: String) {
    when (status) {
        "running" -> CircularProgressIndicator(Modifier.size(18.dp), strokeWidth = 2.dp)
        "ok" -> Badge(vm.tr("cs_done"), Icons.Filled.CheckCircle, MaterialTheme.colorScheme.primary)
        "fail", "timeout" -> Badge(vm.tr("cs_failed"), Icons.Filled.Cancel, MaterialTheme.colorScheme.error)
    }
}

@Composable
private fun Badge(text: String, icon: androidx.compose.ui.graphics.vector.ImageVector, color: Color) {
    Row(verticalAlignment = Alignment.CenterVertically) {
        Icon(icon, null, tint = color, modifier = Modifier.size(18.dp))
        Spacer(Modifier.width(6.dp))
        Text(text, style = MaterialTheme.typography.labelLarge, color = color)
    }
}

private fun codeStyle() = TextStyle(fontFamily = FontFamily.Monospace, fontSize = 15.sp, lineHeight = 22.sp)

/** Colours C# keywords, types, numbers, strings and comments without changing the text. */
private class CSharpHighlight(dark: Boolean) : VisualTransformation {
    private val palette = listOf(
        Color(if (dark) 0xFFFF7AB2 else 0xFF9B2393),
        Color(if (dark) 0xFF5DD8FF else 0xFF0B4F79),
        Color(if (dark) 0xFFD9C97C else 0xFF1C00CF),
        Color(if (dark) 0xFFFC6A5D else 0xFFC41A16),
        Color(if (dark) 0xFF7F8C98 else 0xFF5D6C79),
    )

    override fun filter(text: AnnotatedString): TransformedText =
        TransformedText(highlight(text.text), OffsetMapping.Identity)

    private fun highlight(code: String): AnnotatedString {
        val kind = IntArray(code.length) { -1 }
        RULES.forEachIndexed { index, rule ->
            rule.findAll(code).forEach { m -> for (i in m.range) kind[i] = index }
        }
        return buildAnnotatedString {
            var start = 0
            while (start < code.length) {
                var end = start
                while (end < code.length && kind[end] == kind[start]) end++
                val k = kind[start]
                if (k >= 0) {
                    pushStyle(SpanStyle(color = palette[k]))
                    append(code, start, end)
                    pop()
                } else {
                    append(code, start, end)
                }
                start = end
            }
        }
    }

    companion object {
        private val KEYWORDS = listOf(
            "abstract", "as", "async", "await", "base", "bool", "break", "case", "catch", "char", "class",
            "const", "continue", "decimal", "default", "do", "double", "else", "enum", "false", "finally",
            "float", "for", "foreach", "get", "if", "in", "int", "interface", "internal", "is", "long",
            "namespace", "new", "null", "object", "out", "override", "private", "protected", "public",
            "readonly", "record", "ref", "return", "sealed", "set", "static", "string", "struct", "switch",
            "this", "throw", "true", "try", "using", "var", "virtual", "void", "while",
        )

        /** Later rules win, so strings and comments override keywords inside them. */
        private val RULES = listOf(
            Regex("\\b(?:" + KEYWORDS.joinToString("|") + ")\\b"),
            Regex("\\b(?:Console|Math|List|Dictionary|File|Task|Exception|Func|Action|String|DateTime|Random)\\b"),
            Regex("\\b\\d+(?:\\.\\d+)?\\b"),
            Regex("\\$?@?\"(?:\\\\.|[^\"\\\\\\n])*\"|'(?:\\\\.|[^'\\\\\\n])'"),
            Regex("//[^\\n]*|/\\*[\\s\\S]*?\\*/"),
        )
    }
}
