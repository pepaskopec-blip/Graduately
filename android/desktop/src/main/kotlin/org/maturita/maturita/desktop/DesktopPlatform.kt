package org.maturita.maturita.desktop

import org.maturita.maturita.platform.CSharpRun
import org.maturita.maturita.platform.Platform
import org.maturita.maturita.platform.Prefs
import org.maturita.maturita.platform.Speaker
import androidx.compose.ui.text.ExperimentalTextApi
import androidx.compose.ui.text.font.FontFamily
import androidx.compose.ui.text.font.FontVariation
import androidx.compose.ui.text.font.FontStyle
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.text.platform.Font
import javax.swing.SwingUtilities

/**
 * The font files are variable; without the weight axis set they render at
 * their default, Regular.
 */
@OptIn(ExperimentalTextApi::class)
fun playfairDisplay() = FontFamily(
    Font("fonts/PlayfairDisplay.ttf", FontWeight.Bold, variationSettings = FontVariation.Settings(FontVariation.weight(700))),
    Font(
        "fonts/PlayfairDisplay-Italic.ttf", FontWeight.Black, FontStyle.Italic,
        variationSettings = FontVariation.Settings(FontVariation.weight(900)),
    ),
)

class DesktopPlatform(
    override val commit: String,
    override val prefs: Prefs,
    install: Install,
) : Platform {
    override val versionName: String = if (Install.isWindows) "windows" else "linux"
    override val welcomeKey = "welcome_body_desktop"
    override val updater = DesktopUpdater(commit, install)
    override val displayFont = playfairDisplay()

    override fun speaker(): Speaker? = DesktopSpeaker.create()

    override fun runCSharp(source: String, stdin: String, cb: (CSharpRun) -> Unit) {
        Thread {
            val result = CSharpLab.run(source, stdin)
            SwingUtilities.invokeLater { cb(result) }
        }.apply { isDaemon = true }.start()
    }
}
