package org.maturita.maturita.ui

import androidx.compose.foundation.background
import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.BoxWithConstraints
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.fillMaxHeight
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.rememberScrollState
import androidx.compose.foundation.text.BasicTextField
import androidx.compose.foundation.verticalScroll
import androidx.compose.material3.FilledTonalButton
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.OutlinedButton
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.runtime.setValue
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.SolidColor
import androidx.compose.ui.text.TextStyle
import androidx.compose.ui.text.font.FontFamily
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import org.maturita.maturita.AppViewModel
import org.maturita.maturita.data.J

/** Explanation on one side, the program and its output on the other. */
@Composable
fun CodeLessonScreen(vm: AppViewModel, lesson: J, onSuccess: () -> Unit) {
    val starter = lesson.str("code")
    var code by remember(lesson.str("key")) { mutableStateOf(starter) }
    var stdin by remember(lesson.str("key")) { mutableStateOf("") }
    var output by remember(lesson.str("key")) { mutableStateOf("") }
    var running by remember(lesson.str("key")) { mutableStateOf(false) }

    fun run() {
        if (running) return
        running = true
        output = vm.tr("cs_running")
        vm.platform.runCSharp(code, stdin) { result ->
            running = false
            output = when (result.status) {
                "unavailable" -> vm.tr("cs_unavailable")
                "timeout" -> (result.output + "\n\n" + vm.tr("cs_timeout")).trim()
                else -> result.output.ifEmpty { vm.tr("cs_empty") }
            }
            if (result.status == "ok") onSuccess()
        }
    }

    DetailScaffold(vm, vm.tr(lesson.str("titleKey")), vm.tr(lesson.str("subKey"))) { inner ->
        BoxWithConstraints(Modifier.fillMaxSize().padding(inner).padding(12.dp)) {
            if (maxWidth >= 840.dp) {
                Row(Modifier.fillMaxSize(), horizontalArrangement = Arrangement.spacedBy(12.dp)) {
                    Explanation(vm, lesson, Modifier.weight(0.9f).fillMaxHeight())
                    EditorColumn(vm, code, { code = it }, stdin, { stdin = it }, expand = true, Modifier.weight(1.15f).fillMaxHeight())
                    OutputColumn(vm, output, running, { run() }, { code = starter }, expand = true, Modifier.weight(1f).fillMaxHeight())
                }
            } else {
                Column(Modifier.fillMaxSize().verticalScroll(rememberScrollState()), verticalArrangement = Arrangement.spacedBy(12.dp)) {
                    Explanation(vm, lesson, Modifier.fillMaxWidth())
                    EditorColumn(vm, code, { code = it }, stdin, { stdin = it }, expand = false, Modifier.fillMaxWidth())
                    OutputColumn(vm, output, running, { run() }, { code = starter }, expand = false, Modifier.fillMaxWidth())
                }
            }
        }
    }
}

@Composable
private fun Explanation(vm: AppViewModel, lesson: J, modifier: Modifier) {
    Column(modifier.verticalScroll(rememberScrollState()), verticalArrangement = Arrangement.spacedBy(14.dp)) {
        lesson.arr("slides").forEach { slide ->
            Text(slide.str("kicker"), color = MaterialTheme.colorScheme.primary, style = MaterialTheme.typography.labelLarge)
            Text(slide.str("title"), style = MaterialTheme.typography.titleLarge)
            slide.strs("lines").forEach { line ->
                Text("•  $line", style = MaterialTheme.typography.bodyLarge)
            }
            slide.strOrNull("tip")?.let { tip ->
                Text(tip, color = MaterialTheme.colorScheme.onSurfaceVariant, style = MaterialTheme.typography.bodyMedium)
            }
        }
    }
}

@Composable
private fun EditorColumn(
    vm: AppViewModel,
    code: String,
    onCode: (String) -> Unit,
    stdin: String,
    onStdin: (String) -> Unit,
    expand: Boolean,
    modifier: Modifier,
) {
    Column(modifier, verticalArrangement = Arrangement.spacedBy(8.dp)) {
        Text(vm.tr("cs_code"), style = MaterialTheme.typography.titleMedium)
        BasicTextField(
            value = code,
            onValueChange = onCode,
            textStyle = codeStyle().copy(color = MaterialTheme.colorScheme.onSurface),
            cursorBrush = SolidColor(MaterialTheme.colorScheme.primary),
            modifier = Modifier
                .then(if (expand) Modifier.weight(1f) else Modifier.height(240.dp))
                .fillMaxWidth()
                .background(MaterialTheme.colorScheme.surfaceContainer)
                .padding(12.dp)
                .verticalScroll(rememberScrollState()),
        )
        Text(vm.tr("cs_stdin"), style = MaterialTheme.typography.labelLarge, color = MaterialTheme.colorScheme.onSurfaceVariant)
        BasicTextField(
            value = stdin,
            onValueChange = onStdin,
            textStyle = codeStyle().copy(color = MaterialTheme.colorScheme.onSurface),
            cursorBrush = SolidColor(MaterialTheme.colorScheme.primary),
            modifier = Modifier.fillMaxWidth().background(MaterialTheme.colorScheme.surfaceContainer).padding(12.dp),
        )
    }
}

@Composable
private fun OutputColumn(
    vm: AppViewModel,
    output: String,
    running: Boolean,
    onRun: () -> Unit,
    onReset: () -> Unit,
    expand: Boolean,
    modifier: Modifier,
) {
    Column(modifier, verticalArrangement = Arrangement.spacedBy(8.dp)) {
        Row(horizontalArrangement = Arrangement.spacedBy(8.dp)) {
            FilledTonalButton(onClick = onRun, enabled = !running) { Text(vm.tr("cs_run")) }
            OutlinedButton(onClick = onReset, enabled = !running) { Text(vm.tr("cs_reset")) }
        }
        Text(vm.tr("cs_output"), style = MaterialTheme.typography.titleMedium)
        Text(
            output.ifEmpty { vm.tr("cs_empty") },
            style = codeStyle(),
            color = MaterialTheme.colorScheme.onSurface,
            modifier = Modifier
                .then(if (expand) Modifier.weight(1f) else Modifier.height(180.dp))
                .fillMaxWidth()
                .background(MaterialTheme.colorScheme.surfaceContainer)
                .padding(12.dp)
                .verticalScroll(rememberScrollState()),
        )
    }
}

private fun codeStyle() = TextStyle(fontFamily = FontFamily.Monospace, fontSize = 14.sp, lineHeight = 20.sp)
