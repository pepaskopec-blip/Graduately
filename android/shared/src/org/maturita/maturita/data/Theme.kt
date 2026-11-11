package org.maturita.maturita.data

import androidx.compose.ui.graphics.Color
import kotlin.math.pow
import kotlin.math.roundToLong

enum class ColorMode { Dark, Light }
enum class UiLang { Cs, En }

/**
 * Fill, its label colour, a tint that stays readable as text on paper, and a
 * pale wash of the same hue. The brand has one, and so does every subject.
 */
data class Accent(val solid: Color, val onSolid: Color, val text: Color, val soft: Color)

/** One design in two modes: warm paper and ink, with the orange of the app icon. */
data class Palette(
    val paper: Color,
    val surface: Color,
    val sunken: Color,
    val line: Color,
    val ink: Color,
    val ink2: Color,
    val ink3: Color,
    val accent: Accent,
    val success: Color,
    val successSoft: Color,
    val error: Color,
    val errorSoft: Color,
    val warning: Color,
    val warningSoft: Color,
    val glowInner: Color,
    val glowOuter: Color,
    val onGlow: Color,
)

private class Tone(val paper: Long, val surface: Long, val ink: Long, val dark: Boolean)

private val lightTone = Tone(0xFAF5EC, 0xFFFFFF, 0x17130F, false)
private val darkTone = Tone(0x131110, 0x1E1B18, 0xF5EEE3, true)

private fun hx(v: Long) = Color(0xFF000000L or v)

private fun luminance(v: Long): Double {
    fun channel(c: Long): Double {
        val s = c / 255.0
        return if (s <= 0.03928) s / 12.92 else ((s + 0.055) / 1.055).pow(2.4)
    }
    return 0.2126 * channel((v shr 16) and 0xFF) + 0.7152 * channel((v shr 8) and 0xFF) + 0.0722 * channel(v and 0xFF)
}

private fun contrast(a: Long, b: Long): Double {
    val la = luminance(a)
    val lb = luminance(b)
    return (maxOf(la, lb) + 0.05) / (minOf(la, lb) + 0.05)
}

private fun mix(a: Long, b: Long, t: Double): Long {
    fun channel(shift: Int): Long {
        val x = ((a shr shift) and 0xFF).toDouble()
        val y = ((b shr shift) and 0xFF).toDouble()
        return (x + (y - x) * t).roundToLong()
    }
    return (channel(16) shl 16) or (channel(8) shl 8) or channel(0)
}

/**
 * Amber and olive are fine as fills but unreadable as text on paper, so the
 * text tint is the same hue pushed towards the ink until it reaches 4.5 : 1.
 */
private fun readable(hex: Long, tone: Tone): Long {
    var t = 0.0
    while (t < 1 && contrast(mix(hex, tone.ink, t), tone.paper) < 4.5) t += 0.05
    return mix(hex, tone.ink, t)
}

private fun accent(hex: Long, tone: Tone) = Accent(
    solid = hx(hex),
    onSolid = if (contrast(hex, 0xFFFFFF) >= 3) Color.White else hx(0x1F1503),
    text = hx(readable(hex, tone)),
    soft = hx(mix(hex, tone.surface, if (tone.dark) 0.78 else 0.88)),
)

fun themePalette(mode: ColorMode): Palette = if (mode == ColorMode.Light) {
    Palette(
        paper = hx(lightTone.paper), surface = hx(lightTone.surface), sunken = hx(0xF1E9DC), line = hx(0xE4DACB),
        ink = hx(lightTone.ink), ink2 = hx(0x5E564C), ink3 = hx(0x8F8577),
        accent = accent(0xD9612A, lightTone),
        success = hx(0x1E8A4C), successSoft = hx(0xDDF1E3),
        error = hx(0xC8352B), errorSoft = hx(0xFBE0DC),
        warning = hx(0x9A6700), warningSoft = hx(0xFBEBC8),
        glowInner = hx(0xFFF6E8), glowOuter = hx(0xE8935A), onGlow = hx(0x17130F),
    )
} else {
    Palette(
        paper = hx(darkTone.paper), surface = hx(darkTone.surface), sunken = hx(0x2A2622), line = hx(0x37322D),
        ink = hx(darkTone.ink), ink2 = hx(0xB5AA9B), ink3 = hx(0x80776B),
        accent = accent(0xF0854A, darkTone),
        success = hx(0x4CC27E), successSoft = hx(0x173524),
        error = hx(0xFF6B5E), errorSoft = hx(0x3D1A17),
        warning = hx(0xF2B544), warningSoft = hx(0x3A2C10),
        glowInner = hx(0x6A3A1E), glowOuter = hx(0x1E1B18), onGlow = hx(0xF5EEE3),
    )
}

/**
 * A subject keeps one colour everywhere: its cover on the home screen, the
 * list of its lessons and the buttons in its exercises.
 *
 * [key] is the `Ikona` column of content/aplikace/predmety.md; [cs] and [en]
 * are the timetable abbreviation shown as the cover's watermark.
 */
class Subject(val key: String, val cs: String, val en: String, private val hex: Long) {
    fun accent(mode: ColorMode): Accent = accent(hex, if (mode == ColorMode.Light) lightTone else darkTone)
    fun mark(lang: UiLang): String = if (lang == UiLang.En) en else cs
}

val subjects = listOf(
    Subject("de", "NJ", "DE", 0xE5A50A),
    Subject("wifi", "PS", "NET", 0x2E74DE),
    Subject("chip", "HW", "HW", 0x56616F),
    Subject("cz", "ČJ", "CZ", 0x3848B8),
    Subject("people", "ON", "CIV", 0x9A3FA8),
    Subject("uk", "AJ", "EN", 0xC4314B),
    Subject("function", "MAT", "MAT", 0x7345D8),
    Subject("bolt", "FYZ", "PHY", 0xE2681A),
    Subject("flask", "ZPV", "SCI", 0x12908A),
    Subject("ruler", "TG", "TG", 0x1787B5),
    Subject("brush", "PG", "PG", 0xD03A80),
    Subject("code", "PRG", "PRG", 0x1E8E5A),
    Subject("apps", "PV", "SW", 0x7A9A1E),
)

fun subjectStyle(key: String): Subject? = subjects.firstOrNull { it.key == key }
