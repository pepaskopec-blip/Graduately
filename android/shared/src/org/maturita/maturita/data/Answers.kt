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

// Same rules as src/math_check.c. Do not route numbers through normalizeAnswer.

private data class MFrac(val n: Long = 0, val d: Long = 1)

private class MTerm(
    var ok: Boolean = true,
    var a: MFrac = MFrac(),
    var b: MFrac = MFrac(),
    var rad: Long = 0,
    var p: MFrac = MFrac(),
)

private fun mGcd(a0: Long, b0: Long): Long {
    var a = if (a0 < 0) -a0 else a0
    var b = if (b0 < 0) -b0 else b0
    while (b != 0L) {
        val t = a % b
        a = b
        b = t
    }
    return if (a == 0L) 1 else a
}

private fun mMul(a: Long, b: Long): Long? = try {
    Math.multiplyExact(a, b)
} catch (_: ArithmeticException) {
    null
}

private fun mAddL(a: Long, b: Long): Long? = try {
    Math.addExact(a, b)
} catch (_: ArithmeticException) {
    null
}

private fun mSet(n0: Long, d0: Long): MFrac? {
    if (d0 == 0L) return null
    var n = n0
    var d = d0
    if (d < 0) {
        n = -n
        d = -d
    }
    val g = mGcd(n, d)
    return MFrac(n / g, d / g)
}

private fun mAddF(x: MFrac, y: MFrac): MFrac? {
    val xn = mMul(x.n, y.d) ?: return null
    val yn = mMul(y.n, x.d) ?: return null
    val d = mMul(x.d, y.d) ?: return null
    val s = mAddL(xn, yn) ?: return null
    return mSet(s, d)
}

private fun mMulF(x: MFrac, y: MFrac): MFrac? {
    val n = mMul(x.n, y.n) ?: return null
    val d = mMul(x.d, y.d) ?: return null
    return mSet(n, d)
}

private fun mDivF(x: MFrac, y: MFrac): MFrac? {
    if (y.n == 0L) return null
    return mMulF(x, MFrac(y.d, y.n))
}

private fun mSameF(x: MFrac, y: MFrac) = x.n == y.n && x.d == y.d

private fun mPow(b: Long, e: Int): Long {
    var r = 1L
    var i = 0
    while (i < e) {
        r *= b
        i++
    }
    return r
}

private fun mSqrtReduce(n0: Long): Pair<Long, Long>? {
    if (n0 < 0 || n0 > 1_000_000) return null
    var n = n0
    var out = 1L
    var f = 2L
    while (f * f <= n) {
        var c = 0
        while (n % f == 0L) {
            n /= f
            c++
        }
        if (c >= 2) out *= mPow(f, c / 2)
        if (c % 2 == 1) n *= f
        f = if (f == 2L) 3 else f + 2
        if (f > 10000) break
    }
    return out to n
}

private fun mNorm(t: MTerm) {
    if (!t.ok) return
    if (t.rad > 1) {
        val s = mSqrtReduce(t.rad) ?: run { t.ok = false; return }
        t.b = mMulF(t.b, MFrac(s.first, 1)) ?: run { t.ok = false; return }
        t.rad = s.second
    }
    if (t.rad <= 1) {
        t.a = mAddF(t.a, t.b) ?: run { t.ok = false; return }
        t.b = MFrac()
        t.rad = 0
    }
    if (t.b.n == 0L) t.rad = 0
}

private fun mFail() = MTerm(ok = false)

private fun mAdd(x: MTerm, y: MTerm): MTerm {
    if (!x.ok || !y.ok) return mFail()
    val o = MTerm()
    o.a = mAddF(x.a, y.a) ?: return mFail()
    o.p = mAddF(x.p, y.p) ?: return mFail()
    if (x.rad == 0L && y.rad == 0L) {
        o.b = MFrac()
        o.rad = 0
    } else if (x.rad == 0L) {
        o.b = y.b
        o.rad = y.rad
    } else if (y.rad == 0L || x.rad == y.rad) {
        o.b = mAddF(x.b, y.b) ?: return mFail()
        o.rad = x.rad
    } else return mFail()
    mNorm(o)
    return o
}

private fun mScale(x: MTerm, f: MFrac): MTerm {
    if (!x.ok) return mFail()
    val o = MTerm()
    o.a = mMulF(x.a, f) ?: return mFail()
    o.b = mMulF(x.b, f) ?: return mFail()
    o.p = mMulF(x.p, f) ?: return mFail()
    o.rad = x.rad
    mNorm(o)
    return o
}

private fun mMulT(x: MTerm, y: MTerm): MTerm {
    if (!x.ok || !y.ok) return mFail()
    if ((x.p.n != 0L && (y.p.n != 0L || y.rad != 0L)) ||
        (y.p.n != 0L && (x.p.n != 0L || x.rad != 0L))
    ) return mFail()
    if (x.rad != 0L && y.rad != 0L && x.rad != y.rad) return mFail()
    val o = MTerm()
    o.a = mMulF(x.a, y.a) ?: return mFail()
    if (x.rad == 0L && y.rad == 0L) {
        o.b = MFrac()
        o.rad = 0
    } else if (x.rad == 0L) {
        o.b = mMulF(x.a, y.b) ?: return mFail()
        o.rad = y.rad
    } else if (y.rad == 0L) {
        o.b = mMulF(y.a, x.b) ?: return mFail()
        o.rad = x.rad
    } else {
        val ab = mMulF(x.a, y.b) ?: return mFail()
        val ba = mMulF(x.b, y.a) ?: return mFail()
        val cross = mAddF(ab, ba) ?: return mFail()
        val sq = mMul(x.rad, y.rad) ?: return mFail()
        val bb = mMulF(x.b, y.b) ?: return mFail()
        val extra = mMulF(bb, MFrac(sq, 1)) ?: return mFail()
        o.a = mAddF(o.a, extra) ?: return mFail()
        o.b = cross
        o.rad = x.rad
    }
    o.p = if (x.p.n != 0L) x.p else y.p
    if ((y.a.n != 1L || y.a.d != 1L || y.b.n != 0L || y.rad != 0L || y.p.n != 0L) && x.p.n != 0L) {
        o.p = mMulF(x.p, y.a) ?: return mFail()
    }
    if ((x.a.n != 1L || x.a.d != 1L || x.b.n != 0L || x.rad != 0L || x.p.n != 0L) &&
        y.p.n != 0L && x.p.n == 0L
    ) {
        o.p = mMulF(y.p, x.a) ?: return mFail()
    }
    mNorm(o)
    return o
}

private fun mDivT(x: MTerm, y: MTerm): MTerm {
    if (!x.ok || !y.ok || y.p.n != 0L) return mFail()
    if (y.rad == 0L) {
        val f = mDivF(MFrac(1, 1), y.a) ?: return mFail()
        return mScale(x, f)
    }
    if (x.p.n != 0L) return mFail()
    val yb = mMulF(y.b, MFrac(y.rad, 1)) ?: return mFail()
    val bb = mMulF(y.b, yb) ?: return mFail()
    val aa = mMulF(y.a, y.a) ?: return mFail()
    val den = mAddF(aa, MFrac(-bb.n, bb.d)) ?: return mFail()
    val num = MTerm()
    num.a = y.a
    num.b = MFrac(-y.b.n, y.b.d)
    num.rad = y.rad
    val inv = mDivF(MFrac(1, 1), den) ?: return mFail()
    return mMulT(x, mScale(num, inv))
}

private sealed class MTok {
    class Num(val n: Long, val d: Long) : MTok()
    object Sqrt : MTok()
    object Pi : MTok()
    object Add : MTok()
    object Sub : MTok()
    object Mul : MTok()
    object Div : MTok()
    object Lp : MTok()
    object Rp : MTok()
}

private fun mLex(s: String): List<MTok>? {
    val toks = ArrayList<MTok>()
    var i = 0
    fun peek(k: Int) = if (i + k < s.length) s[i + k] else null
    while (i < s.length) {
        while (i < s.length && s[i].isWhitespace()) i++
        if (i >= s.length) break
        val ch = s[i]
        if (ch.isDigit() || ((ch == '.' || ch == ',') && peek(1)?.isDigit() == true)) {
            var whole = 0L
            var saw = false
            while (i < s.length && s[i].isDigit()) {
                saw = true
                whole = whole * 10 + (s[i] - '0')
                i++
            }
            if (i < s.length && (s[i] == '.' || s[i] == ',') && i + 1 < s.length && s[i + 1].isDigit()) {
                i++
                var frac = 0L
                var div = 1L
                while (i < s.length && s[i].isDigit()) {
                    frac = frac * 10 + (s[i] - '0')
                    div *= 10
                    i++
                }
                val n = mAddL(mMul(whole, div) ?: return null, frac) ?: return null
                toks.add(MTok.Num(n, div))
            } else if (saw) {
                toks.add(MTok.Num(whole, 1))
            } else return null
            continue
        }
        when (ch) {
            '+' -> toks.add(MTok.Add)
            '-', '\u2212', '\u2013' -> toks.add(MTok.Sub)
            '*', '\u00D7', '\u00B7' -> toks.add(MTok.Mul)
            '/', ':' -> toks.add(MTok.Div)
            '(' -> toks.add(MTok.Lp)
            ')' -> toks.add(MTok.Rp)
            '\u03C0' -> toks.add(MTok.Pi)
            '\u221A' -> toks.add(MTok.Sqrt)
            else -> {
                if (ch in 'A'..'Z' || ch in 'a'..'z') {
                    val start = i
                    while (i < s.length && (s[i] in 'A'..'Z' || s[i] in 'a'..'z')) i++
                    when (s.substring(start, i).lowercase()) {
                        "pi" -> toks.add(MTok.Pi)
                        "sqrt" -> toks.add(MTok.Sqrt)
                        else -> return null
                    }
                    continue
                }
                return null
            }
        }
        i++
    }
    return toks
}

private class MParse(val t: List<MTok>) {
    var i = 0
    fun peek() = if (i < t.size) t[i] else null
    fun take(): MTok? = if (i < t.size) t[i++] else null
    fun at(k: Int) = if (k in t.indices) t[k] else null

    fun expr(): MTerm? {
        var left = term() ?: return null
        while (true) {
            val p = peek()
            if (p !is MTok.Add && p !is MTok.Sub) break
            take()
            val r = term() ?: return null
            left = if (p is MTok.Sub) mAdd(left, mScale(r, MFrac(-1, 1))) else mAdd(left, r)
            if (!left.ok) return null
        }
        return left
    }

    fun term(): MTerm? {
        var left = unary() ?: return null
        while (true) {
            val p = peek()
            if (p !is MTok.Mul && p !is MTok.Div) break
            take()
            val r = unary() ?: return null
            left = if (p is MTok.Div) mDivT(left, r) else mMulT(left, r)
            if (!left.ok) return null
        }
        return left
    }

    fun unary(): MTerm? {
        if (peek() is MTok.Sub) {
            take()
            val v = unary() ?: return null
            return mScale(v, MFrac(-1, 1))
        }
        if (peek() is MTok.Add) {
            take()
            return unary()
        }
        return factor()
    }

    fun factor(): MTerm? {
        var left = primary() ?: return null
        while (true) {
            when (peek()) {
                is MTok.Sqrt, is MTok.Pi, is MTok.Lp -> {
                    val r = primary() ?: return null
                    left = mMulT(left, r)
                    if (!left.ok) return null
                }
                is MTok.Num -> return null
                else -> return left
            }
        }
    }

    fun primary(): MTerm? {
        when (val p = take()) {
            is MTok.Num -> {
                val n = p.n
                val d = p.d
                if (peek() is MTok.Div && at(i + 1) is MTok.Num) {
                    take()
                    val n2 = take() as? MTok.Num ?: return null
                    if (n2.n <= 0 || n2.d != 1L || d != 1L) return null
                    val o = MTerm()
                    o.a = mSet(n, n2.n) ?: return null
                    return o
                }
                if (d == 1L && n >= 0 && peek() is MTok.Num) {
                    val saved = i
                    val n2 = take() as MTok.Num
                    val n3tok = at(i + 1)
                    if (peek() is MTok.Div && n3tok is MTok.Num && n2.d == 1L && n3tok.d == 1L && n2.n >= 0 && n3tok.n > 0) {
                        take()
                        take()
                        val frac = mSet(n2.n, n3tok.n) ?: return null
                        val whole = MTerm()
                        whole.a = MFrac(n, 1)
                        val part = MTerm()
                        part.a = frac
                        return mAdd(whole, part)
                    }
                    i = saved
                }
                val o = MTerm()
                o.a = mSet(n, d) ?: return null
                return o
            }
            is MTok.Pi -> {
                val o = MTerm()
                o.p = MFrac(1, 1)
                return o
            }
            is MTok.Sqrt -> {
                if (peek() is MTok.Lp) {
                    take()
                    val inner = expr() ?: return null
                    if (take() !is MTok.Rp) return null
                    return mSqrtTerm(inner)
                }
                val num = peek()
                if (num is MTok.Num && num.d == 1L && num.n >= 0) {
                    take()
                    val inner = MTerm()
                    inner.a = MFrac(num.n, 1)
                    return mSqrtTerm(inner)
                }
                return null
            }
            is MTok.Lp -> {
                val inner = expr() ?: return null
                if (take() !is MTok.Rp) return null
                return inner
            }
            else -> return null
        }
    }
}

private fun mSqrtTerm(inner: MTerm): MTerm? {
    if (!inner.ok || inner.p.n != 0L || inner.rad != 0L || inner.b.n != 0L) return null
    if (inner.a.n < 0 || inner.a.d != 1L) return null
    val s = mSqrtReduce(inner.a.n) ?: return null
    val o = MTerm()
    o.b = MFrac(s.first, 1)
    o.rad = s.second
    mNorm(o)
    return if (o.ok) o else null
}

private fun mStripUnits(raw: String): String {
    var s = raw.trim()
    val units = listOf("procenta", "procent", "cm", "mm", "dm", "km", "m")
    var cut = true
    while (cut) {
        cut = false
        s = s.trimEnd()
        if (s.endsWith("°") || s.endsWith("%") || s.endsWith(".")) {
            s = s.dropLast(1)
            cut = true
            continue
        }
        for (u in units) {
            if (s.length <= u.length) continue
            if (s.lowercase().endsWith(u)) {
                val keep = s.dropLast(u.length)
                if (keep.lastOrNull()?.isLetter() == true) continue
                s = keep
                cut = true
                break
            }
        }
    }
    return s.trim()
}

private fun mPrep(raw: String): String {
    var s = raw.trim()
    val pairs = listOf('(' to ')', '[' to ']', '{' to '}')
    while (s.length >= 2 && pairs.any { it.first == s.first() && it.second == s.last() }) {
        s = s.substring(1, s.length - 1).trim()
    }
    return mStripUnits(s).trim()
}

private fun mParseTerm(s: String): MTerm? {
    val text = mPrep(s)
    if (text.isEmpty()) return null
    val toks = mLex(text) ?: return null
    if (toks.isEmpty()) return null
    val p = MParse(toks)
    val v = p.expr() ?: return null
    if (p.i != toks.size || !v.ok) return null
    return v
}

private fun mExact(x: MTerm, y: MTerm) =
    x.ok && y.ok && mSameF(x.a, y.a) && mSameF(x.b, y.b) && x.rad == y.rad && mSameF(x.p, y.p)

private fun mDouble(t: MTerm): Double {
    var v = t.a.n.toDouble() / t.a.d + t.p.n.toDouble() / t.p.d * Math.PI
    if (t.b.n != 0L && t.rad > 0) v += t.b.n.toDouble() / t.b.d * Math.sqrt(t.rad.toDouble())
    return v
}

private fun mClose(x: MTerm, y: MTerm): Boolean {
    if (!x.ok || !y.ok) return false
    if (mExact(x, y)) return true
    if (x.p.n != 0L || y.p.n != 0L) return false
    val diff = Math.abs(mDouble(x) - mDouble(y))
    return if (x.b.n != 0L || y.b.n != 0L) diff <= 0.001 else diff <= 0.0005
}

private fun mFold(s: String): String {
    val out = StringBuilder()
    for (ch in s.lowercase()) {
        when (ch) {
            'á', 'à', 'ä', 'â' -> out.append('a')
            'č', 'ć' -> out.append('c')
            'ď' -> out.append('d')
            'é', 'ě', 'ë', 'ê' -> out.append('e')
            'í', 'ï' -> out.append('i')
            'ň' -> out.append('n')
            'ó', 'ö', 'ô' -> out.append('o')
            'ř' -> out.append('r')
            'š', 'ś' -> out.append('s')
            'ť' -> out.append('t')
            'ú', 'ů', 'ü' -> out.append('u')
            'ý' -> out.append('y')
            'ž', 'ź' -> out.append('z')
            else -> if (ch.isLetter() || ch.isDigit()) out.append(ch)
        }
    }
    return out.toString()
}

private fun mSplit(raw: String): List<String>? {
    val s = raw.trim()
    if (s.isEmpty()) return null
    val semi = s.contains(';')
    val parts = ArrayList<String>()
    var rest = s
    while (rest.isNotEmpty()) {
        val idx = if (semi) rest.indexOf(';') else rest.indexOf(", ")
        val step = if (semi) 1 else 2
        val piece: String
        val next: String
        if (idx < 0) {
            piece = rest.trim()
            next = ""
        } else {
            piece = rest.substring(0, idx).trim()
            next = rest.substring(idx + step)
        }
        if (piece.isEmpty()) return null
        parts.add(piece)
        if (idx < 0) break
        rest = next
    }
    return parts.ifEmpty { null }
}

fun mathAnswerOk(spec: String?, user: String?): Boolean {
    if (spec == null || user == null) return false
    val bodySpec = spec.trim()
    if (bodySpec.startsWith("word:")) {
        val opts = bodySpec.removePrefix("word:").split('|').map { mFold(it) }
        return opts.contains(mFold(user))
    }
    if (bodySpec.startsWith("set:") || bodySpec.startsWith("pair:")) {
        val ordered = bodySpec.startsWith("pair:")
        val body = bodySpec.removePrefix(if (ordered) "pair:" else "set:")
        val want = mSplit(body) ?: return false
        val got = mSplit(user) ?: return false
        if (want.size != got.size || want.isEmpty()) return false
        val wt = want.map { mParseTerm(it) ?: return false }
        val gt = got.map { mParseTerm(it) ?: return false }
        if (ordered) return wt.indices.all { mClose(wt[it], gt[it]) }
        val used = BooleanArray(gt.size)
        for (w in wt) {
            var hit = gt.indices.firstOrNull { !used[it] && mExact(w, gt[it]) }
            if (hit == null) hit = gt.indices.firstOrNull { !used[it] && mClose(w, gt[it]) }
            if (hit == null) return false
            used[hit] = true
        }
        return true
    }
    if (!bodySpec.startsWith("num:")) return false
    val tw = mParseTerm(bodySpec.removePrefix("num:")) ?: return false
    val tg = mParseTerm(user) ?: return false
    return mClose(tw, tg)
}

fun mathIsDraw(spec: String?): Boolean {
    if (spec == null) return false
    return spec.startsWith("square:") || spec.startsWith("mid:") || spec.startsWith("right:") ||
        spec.startsWith("para:") || spec.startsWith("points:")
}

fun mathDrawNeed(spec: String?): Int {
    if (spec == null) return 0
    if (spec.startsWith("square:")) return 2
    if (spec.startsWith("mid:") || spec.startsWith("right:") || spec.startsWith("para:")) return 1
    if (spec.startsWith("points:")) {
        return spec.removePrefix("points:").split(';').count { !it.startsWith("fix:") }
    }
    return 0
}

private fun mParseXY(s: String): Pair<Int, Int>? {
    val bits = s.trim().split(',')
    if (bits.size != 2) return null
    val x = bits[0].trim().toIntOrNull() ?: return null
    val y = bits[1].trim().toIntOrNull() ?: return null
    return x to y
}

fun mathDrawGiven(spec: String?): List<Pair<Int, Int>> {
    if (spec == null) return emptyList()
    if (spec.startsWith("points:")) {
        return spec.removePrefix("points:").split(';').mapNotNull { part ->
            if (!part.startsWith("fix:")) null else mParseXY(part.removePrefix("fix:"))
        }
    }
    val body: String
    val n: Int
    when {
        spec.startsWith("para:") -> { body = spec.removePrefix("para:"); n = 3 }
        spec.startsWith("square:") -> { body = spec.removePrefix("square:"); n = 2 }
        spec.startsWith("right:") -> { body = spec.removePrefix("right:"); n = 2 }
        spec.startsWith("mid:") -> { body = spec.removePrefix("mid:"); n = 2 }
        else -> return emptyList()
    }
    return body.split(';').take(n).mapNotNull { mParseXY(it) }
}

private fun mOnBoard(p: Pair<Int, Int>) = p.first in 0..10 && p.second in 0..8

fun mathDrawOk(spec: String?, pts: List<Pair<Int, Int>>): Boolean {
    if (spec == null || mathDrawNeed(spec) != pts.size) return false
    if (pts.any { !mOnBoard(it) }) return false
    if (pts.map { "${it.first},${it.second}" }.toSet().size != pts.size) return false
    val g = mathDrawGiven(spec)
    if (spec.startsWith("mid:")) {
        if (g.size != 2 || (g[0].first + g[1].first) % 2 != 0 || (g[0].second + g[1].second) % 2 != 0) return false
        return pts[0] == (g[0].first + g[1].first) / 2 to (g[0].second + g[1].second) / 2
    }
    if (spec.startsWith("para:")) {
        if (g.size != 3) return false
        return pts[0] == g[1].first + g[2].first - g[0].first to g[1].second + g[2].second - g[0].second
    }
    if (spec.startsWith("right:")) {
        if (g.size != 2) return false
        val c = pts[0]
        if (c == g[0] || c == g[1]) return false
        val ux = g[0].first - c.first
        val uy = g[0].second - c.second
        val vx = g[1].first - c.first
        val vy = g[1].second - c.second
        return ux * vx + uy * vy == 0 && (ux != 0 || uy != 0) && (vx != 0 || vy != 0)
    }
    if (spec.startsWith("square:")) {
        if (g.size != 2 || pts.size != 2) return false
        val a = g[0]
        val b = g[1]
        val dx = b.first - a.first
        val dy = b.second - a.second
        if (dx == 0 && dy == 0) return false
        val c1 = b.first - dy to b.second + dx
        val d1 = a.first - dy to a.second + dx
        val c2 = b.first + dy to b.second - dx
        val d2 = a.first + dy to a.second - dx
        fun same(p: Pair<Int, Int>, q: Pair<Int, Int>, r: Pair<Int, Int>, s: Pair<Int, Int>) =
            (p == r && q == s) || (p == s && q == r)
        if ((!mOnBoard(c1) || !mOnBoard(d1)) && (!mOnBoard(c2) || !mOnBoard(d2))) return false
        return same(pts[0], pts[1], c1, d1) || same(pts[0], pts[1], c2, d2)
    }
    if (spec.startsWith("points:")) {
        val want = spec.removePrefix("points:").split(';').mapNotNull { part ->
            if (part.startsWith("fix:")) null else mParseXY(part)
        }
        if (want.size != pts.size) return false
        val used = BooleanArray(pts.size)
        for (w in want) {
            val j = pts.indices.firstOrNull { !used[it] && pts[it] == w } ?: return false
            used[j] = true
        }
        return true
    }
    return false
}
