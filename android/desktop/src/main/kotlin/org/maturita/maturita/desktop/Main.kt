package org.maturita.maturita.desktop

import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.ui.ExperimentalComposeUiApi
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.painter.BitmapPainter
import androidx.compose.ui.graphics.toComposeImageBitmap
import androidx.compose.ui.input.key.Key
import androidx.compose.ui.input.key.KeyEventType
import androidx.compose.ui.input.key.isCtrlPressed
import androidx.compose.ui.input.key.isMetaPressed
import androidx.compose.ui.input.key.key
import androidx.compose.ui.input.key.type
import androidx.compose.ui.input.pointer.PointerButton
import androidx.compose.ui.input.pointer.PointerEventType
import androidx.compose.ui.input.pointer.onPointerEvent
import androidx.compose.ui.unit.dp
import androidx.compose.ui.window.Window
import androidx.compose.ui.window.application
import androidx.compose.ui.window.rememberWindowState
import org.maturita.maturita.AppViewModel
import org.maturita.maturita.MaturitaApp
import org.maturita.maturita.data.Content
import org.maturita.maturita.handleBack
import java.io.File
import java.util.Properties
import javax.imageio.ImageIO

private fun resourceText(name: String): String? =
    Thread.currentThread().contextClassLoader.getResourceAsStream(name)?.bufferedReader()?.use { it.readText() }

@OptIn(ExperimentalComposeUiApi::class)
fun main() {
    val install = Install.detect()
    val prefs = FilePrefs(File(install.progressDir, "graduately.properties"))
    importGtkProgress(install.progressDir, prefs)
    removeGtkLeftovers(install)

    val info = Properties().apply { resourceText("build-info.properties")?.let { load(it.reader()) } }
    val platform = DesktopPlatform(info.getProperty("commit") ?: "dev", prefs, install)
    val vm = AppViewModel(Content.parse(resourceText("content.json")), platform)
    val icon = Thread.currentThread().contextClassLoader.getResourceAsStream("icon.png")
        ?.use { ImageIO.read(it) }?.let { BitmapPainter(it.toComposeImageBitmap()) }

    application {
        Window(
            onCloseRequest = ::exitApplication,
            title = "Graduately",
            icon = icon,
            state = rememberWindowState(width = 1100.dp, height = 780.dp),
            onPreviewKeyEvent = { e ->
                if (e.type != KeyEventType.KeyDown) return@Window false
                val cmd = e.isCtrlPressed || e.isMetaPressed
                when {
                    e.key == Key.Escape -> vm.handleBack()
                    cmd && e.key == Key.K -> { vm.searchOpen = true; true }
                    cmd && e.key == Key.Q -> { exitApplication(); true }
                    else -> false
                }
            },
        ) {
            Box(
                Modifier.fillMaxSize().onPointerEvent(PointerEventType.Press) { e ->
                    if (e.button == PointerButton.Back) vm.handleBack()
                },
            ) {
                MaturitaApp(vm)
            }
        }
    }
}
