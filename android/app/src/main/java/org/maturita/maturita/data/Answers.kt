package org.maturita.maturita.data

import java.text.Normalizer

fun normalizeAnswer(input: String?): String {
    val src = (input ?: "").lowercase()
    val pre = StringBuilder()
    src.codePoints().forEach { ch ->
        when (ch) {
            0x00e4 -> pre.append("ae")
            0x00f6 -> pre.append("oe")
            0x00fc -> pre.append("ue")
            0x00df -> pre.append("ss")
            else -> pre.appendCodePoint(ch)
        }
    }
    val decomp = Normalizer.normalize(pre.toString(), Normalizer.Form.NFD)
    val out = StringBuilder()
    var prevSpace = false
    decomp.codePoints().forEach { ch ->
        val type = Character.getType(ch)
        if (type == Character.NON_SPACING_MARK.toInt() ||
            type == Character.COMBINING_SPACING_MARK.toInt() ||
            type == Character.ENCLOSING_MARK.toInt()
        ) {
            return@forEach
        }
        if (Character.isWhitespace(ch)) {
            if (out.isNotEmpty() && !prevSpace) out.append(' ')
            prevSpace = true
            return@forEach
        }
        if (!Character.isLetterOrDigit(ch)) return@forEach
        prevSpace = false
        out.appendCodePoint(ch)
    }
    return out.toString().trimEnd()
}

fun answerAccepts(selNorm: String, ans: String?): Boolean {
    if (ans.isNullOrEmpty()) return false
    return ans.split('|').any { normalizeAnswer(it) == selNorm }
}

fun answerIncomplete(selNorm: String, ans: String?): Boolean {
    if (selNorm.isEmpty() || ans.isNullOrEmpty()) return false
    for (part in ans.split('|')) {
        val a = normalizeAnswer(part)
        if (a.startsWith("$selNorm ") && a.length > selNorm.length + 1) return true
    }
    return false
}

fun netTxtEq(a: String?, b: String?): Boolean {
    if (a == null || b == null) return a == b
    val x = a.filterNot { it.isWhitespace() }.lowercase()
    val y = b.filterNot { it.isWhitespace() }.lowercase()
    return x == y
}

fun hasUmlaut(s: String?): Boolean {
    if (s == null) return false
    return s.any { it == 'ä' || it == 'ö' || it == 'ü' || it == 'ß' ||
        it == 'Ä' || it == 'Ö' || it == 'Ü' }
}
