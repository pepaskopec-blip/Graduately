package org.maturita.maturita.desktop

import org.maturita.maturita.platform.Speaker
import java.io.File
import java.util.Locale
import javax.swing.SwingUtilities
import kotlin.concurrent.thread

/**
 * Speech through whatever the system already has: SAPI via PowerShell on
 * Windows, espeak-ng / espeak / spd-say on Linux, `say` on a Mac. None of them
 * report word positions, so pause and resume start the passage again.
 */
class DesktopSpeaker private constructor(private val command: (String, Locale, Float, Float) -> List<String>) : Speaker {
    @Volatile private var process: Process? = null

    override fun speak(
        text: String,
        locale: Locale,
        rate: Float,
        volume: Float,
        onRange: (Int) -> Unit,
        onEnd: (Boolean) -> Unit,
    ) {
        stop()
        val argv = command(text, locale, rate, volume)
        val stdin = argv.lastOrNull() == STDIN
        val p = runCatching {
            ProcessBuilder(if (stdin) argv.dropLast(1) else argv)
                .redirectErrorStream(true)
                .redirectOutput(ProcessBuilder.Redirect.DISCARD)
                .start()
        }.getOrElse {
            SwingUtilities.invokeLater { onEnd(false) }
            return
        }
        process = p
        thread(isDaemon = true, name = "speech") {
            if (stdin) runCatching { p.outputStream.writer(Charsets.UTF_8).use { it.write(text) } }
            val ok = p.waitFor() == 0
            if (process === p) {
                process = null
                SwingUtilities.invokeLater { onEnd(ok) }
            }
        }
    }

    override fun stop() {
        process?.let {
            process = null
            it.destroy()
        }
    }

    override fun close() = stop()

    companion object {
        /** Marks a command that reads the text from standard input. */
        private const val STDIN = "\u0000stdin"

        private fun onPath(name: String): Boolean =
            System.getenv("PATH").orEmpty().split(File.pathSeparator).any { File(it, name).canExecute() }

        fun create(): DesktopSpeaker? {
            val os = System.getProperty("os.name").orEmpty()
            return when {
                os.startsWith("Windows") -> DesktopSpeaker { _, locale, rate, volume ->
                    val sapiRate = ((rate - 1f) * 10f).toInt().coerceIn(-10, 10)
                    val script = "[Console]::InputEncoding=[Text.Encoding]::UTF8;" +
                        "Add-Type -AssemblyName System.Speech;" +
                        "\$s=New-Object System.Speech.Synthesis.SpeechSynthesizer;" +
                        "try{\$s.SelectVoiceByHints('NotSet','NotSet',0,[Globalization.CultureInfo]'${locale.toLanguageTag()}')}catch{};" +
                        "\$s.Rate=$sapiRate;\$s.Volume=${(volume * 100).toInt()};" +
                        "\$s.Speak([Console]::In.ReadToEnd())"
                    listOf("powershell", "-NoProfile", "-NonInteractive", "-Command", script, STDIN)
                }
                os.startsWith("Mac") -> DesktopSpeaker { text, _, rate, _ ->
                    listOf("say", "-r", (180 * rate).toInt().toString(), text)
                }
                onPath("espeak-ng") || onPath("espeak") -> {
                    val bin = if (onPath("espeak-ng")) "espeak-ng" else "espeak"
                    DesktopSpeaker { _, locale, rate, volume ->
                        val voice = if (locale.language == "de") "de" else "en-gb"
                        listOf(bin, "-v", voice, "-s", (165 * rate).toInt().toString(), "-a", (volume * 150).toInt().toString(), "--stdin", STDIN)
                    }
                }
                onPath("spd-say") -> DesktopSpeaker { text, locale, rate, volume ->
                    listOf(
                        "spd-say", "-w", "-l", locale.language,
                        "-r", ((rate - 1f) * 100f).toInt().coerceIn(-100, 100).toString(),
                        "-i", ((volume - 1f) * 100f).toInt().coerceIn(-100, 100).toString(),
                        text,
                    )
                }
                else -> null
            }
        }
    }
}
