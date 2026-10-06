import Foundation

func normalizeAnswer(_ input: String?) -> String {
    var pre = ""
    for ch in (input ?? "").lowercased() {
        switch ch {
        case "ä": pre += "ae"
        case "ö": pre += "oe"
        case "ü": pre += "ue"
        case "ß": pre += "ss"
        default: pre.append(ch)
        }
    }
    let decomp = pre.decomposedStringWithCanonicalMapping
    var out = ""
    var prevSpace = false
    for scalar in decomp.unicodeScalars {
        let v = scalar.value
        if (0x0300...0x036F).contains(v) || (0x1AB0...0x1AFF).contains(v) ||
            (0x1DC0...0x1DFF).contains(v) || (0x20D0...0x20FF).contains(v) ||
            (0xFE20...0xFE2F).contains(v) {
            continue
        }
        if CharacterSet.whitespacesAndNewlines.contains(scalar) {
            if !out.isEmpty && !prevSpace { out.append(" ") }
            prevSpace = true
            continue
        }
        if !CharacterSet.alphanumerics.contains(scalar) { continue }
        prevSpace = false
        out.unicodeScalars.append(scalar)
    }
    return out.trimmingCharacters(in: .whitespaces)
}

func answerAccepts(_ selNorm: String, _ ans: String?) -> Bool {
    guard let ans, !ans.isEmpty else { return false }
    return ans.split(separator: "|").contains { normalizeAnswer(String($0)) == selNorm }
}

func answerIncomplete(_ selNorm: String, _ ans: String?) -> Bool {
    guard !selNorm.isEmpty, let ans, !ans.isEmpty else { return false }
    for part in ans.split(separator: "|") {
        let a = normalizeAnswer(String(part))
        if a.hasPrefix(selNorm + " ") && a.count > selNorm.count + 1 { return true }
    }
    return false
}

func netTxtEq(_ a: String?, _ b: String?) -> Bool {
    guard let a, let b else { return a == b }
    let x = a.filter { !$0.isWhitespace }.lowercased()
    let y = b.filter { !$0.isWhitespace }.lowercased()
    return x == y
}

func hasUmlaut(_ s: String?) -> Bool {
    guard let s else { return false }
    return s.contains(where: { "äöüÄÖÜß".contains($0) })
}

// MARK: - Mathematics
// Same rules as src/math_check.c. Do not route numbers through normalizeAnswer.

private struct MFrac { var n: Int64 = 0; var d: Int64 = 1 }
private struct MTerm {
    var ok = true
    var a = MFrac()
    var b = MFrac()
    var rad: Int64 = 0
    var p = MFrac()
}

private func mGcd(_ a0: Int64, _ b0: Int64) -> Int64 {
    var a = a0 < 0 ? -a0 : a0
    var b = b0 < 0 ? -b0 : b0
    while b != 0 {
        let t = a % b
        a = b
        b = t
    }
    return a == 0 ? 1 : a
}

private func mMul(_ a: Int64, _ b: Int64) -> Int64? {
    let r = a.multipliedReportingOverflow(by: b)
    return r.overflow ? nil : r.partialValue
}

private func mSet(_ f: inout MFrac, _ n: Int64, _ d: Int64) -> Bool {
    if d == 0 { return false }
    var n = n, d = d
    if d < 0 { n = -n; d = -d }
    let g = mGcd(n, d)
    f.n = n / g
    f.d = d / g
    return true
}

private func mAddF(_ x: MFrac, _ y: MFrac) -> MFrac? {
    guard let xn = mMul(x.n, y.d), let yn = mMul(y.n, x.d),
          let d = mMul(x.d, y.d) else { return nil }
    let s = xn.addingReportingOverflow(yn)
    if s.overflow { return nil }
    var o = MFrac()
    return mSet(&o, s.partialValue, d) ? o : nil
}

private func mMulF(_ x: MFrac, _ y: MFrac) -> MFrac? {
    guard let n = mMul(x.n, y.n), let d = mMul(x.d, y.d) else { return nil }
    var o = MFrac()
    return mSet(&o, n, d) ? o : nil
}

private func mDivF(_ x: MFrac, _ y: MFrac) -> MFrac? {
    if y.n == 0 { return nil }
    return mMulF(x, MFrac(n: y.d, d: y.n))
}

private func mSameF(_ x: MFrac, _ y: MFrac) -> Bool { x.n == y.n && x.d == y.d }

private func mSqrtReduce(_ n0: Int64) -> (out: Int64, rad: Int64)? {
    if n0 < 0 { return nil }
    if n0 > 1_000_000 { return nil }
    var n = n0
    var out: Int64 = 1
    var f: Int64 = 2
    while f * f <= n {
        var c = 0
        while n % f == 0 { n /= f; c += 1 }
        if c >= 2 { out *= mPow(f, c / 2) }
        if c % 2 == 1 { n *= f }
        if f == 2 { f = 3 } else { f += 2 }
        if f > 10000 { break }
    }
    return (out, n)
}

private func mPow(_ b: Int64, _ e: Int) -> Int64 {
    var r: Int64 = 1
    var i = 0
    while i < e { r *= b; i += 1 }
    return r
}

private func mNorm(_ t: inout MTerm) {
    if !t.ok { return }
    if t.rad > 1, let s = mSqrtReduce(t.rad) {
        guard let nb = mMulF(t.b, MFrac(n: s.out, d: 1)) else { t.ok = false; return }
        t.b = nb
        t.rad = s.rad
    }
    if t.rad <= 1 {
        guard let na = mAddF(t.a, t.b) else { t.ok = false; return }
        t.a = na
        t.b = MFrac()
        t.rad = 0
    }
    if t.b.n == 0 { t.rad = 0 }
}

private func mAdd(_ x: MTerm, _ y: MTerm) -> MTerm {
    if !x.ok || !y.ok { return MTerm(ok: false) }
    var o = MTerm()
    guard let a = mAddF(x.a, y.a), let p = mAddF(x.p, y.p) else { return MTerm(ok: false) }
    o.a = a
    o.p = p
    if x.rad == 0 && y.rad == 0 {
        o.b = MFrac(); o.rad = 0
    } else if x.rad == 0 {
        o.b = y.b; o.rad = y.rad
    } else if y.rad == 0 || x.rad == y.rad {
        guard let b = mAddF(x.b, y.b) else { return MTerm(ok: false) }
        o.b = b; o.rad = x.rad
    } else { return MTerm(ok: false) }
    mNorm(&o)
    return o
}

private func mScale(_ x: MTerm, _ f: MFrac) -> MTerm {
    if !x.ok { return MTerm(ok: false) }
    var o = MTerm()
    guard let a = mMulF(x.a, f), let b = mMulF(x.b, f), let p = mMulF(x.p, f) else {
        return MTerm(ok: false)
    }
    o.a = a; o.b = b; o.rad = x.rad; o.p = p
    mNorm(&o)
    return o
}

private func mMulT(_ x: MTerm, _ y: MTerm) -> MTerm {
    if !x.ok || !y.ok { return MTerm(ok: false) }
    if (x.p.n != 0 && (y.p.n != 0 || y.rad != 0)) ||
        (y.p.n != 0 && (x.p.n != 0 || x.rad != 0)) { return MTerm(ok: false) }
    if x.rad != 0 && y.rad != 0 && x.rad != y.rad { return MTerm(ok: false) }
    var o = MTerm()
    guard let aa = mMulF(x.a, y.a) else { return MTerm(ok: false) }
    o.a = aa
    if x.rad == 0 && y.rad == 0 {
        o.b = MFrac(); o.rad = 0
    } else if x.rad == 0 {
        guard let b = mMulF(x.a, y.b) else { return MTerm(ok: false) }
        o.b = b; o.rad = y.rad
    } else if y.rad == 0 {
        guard let b = mMulF(y.a, x.b) else { return MTerm(ok: false) }
        o.b = b; o.rad = x.rad
    } else {
        guard let ab = mMulF(x.a, y.b), let ba = mMulF(x.b, y.a),
              let cross = mAddF(ab, ba),
              let sq = mMul(x.rad, y.rad) else { return MTerm(ok: false) }
        guard let bb = mMulF(x.b, y.b), let extra = mMulF(bb, MFrac(n: sq, d: 1)),
              let na = mAddF(o.a, extra) else { return MTerm(ok: false) }
        o.a = na; o.b = cross; o.rad = x.rad
    }
    if x.p.n != 0 { o.p = x.p } else { o.p = y.p }
    if y.a.n != 1 || y.a.d != 1 || y.b.n != 0 || y.rad != 0 || y.p.n != 0 {
        if x.p.n != 0 {
            guard let p = mMulF(x.p, y.a) else { return MTerm(ok: false) }
            o.p = p
        }
    }
    if x.a.n != 1 || x.a.d != 1 || x.b.n != 0 || x.rad != 0 || x.p.n != 0 {
        if y.p.n != 0 && x.p.n == 0 {
            guard let p = mMulF(y.p, x.a) else { return MTerm(ok: false) }
            o.p = p
        }
    }
    mNorm(&o)
    return o
}

private func mDivT(_ x: MTerm, _ y: MTerm) -> MTerm {
    if !x.ok || !y.ok || y.p.n != 0 { return MTerm(ok: false) }
    if y.rad == 0 {
        guard let f = mDivF(MFrac(n: 1, d: 1), y.a) else { return MTerm(ok: false) }
        return mScale(x, f)
    }
    if x.p.n != 0 { return MTerm(ok: false) }
    guard let yb = mMulF(y.b, MFrac(n: y.rad, d: 1)),
          let bb = mMulF(y.b, yb),
          let aa = mMulF(y.a, y.a),
          let den = mAddF(aa, MFrac(n: -bb.n, d: bb.d)) else { return MTerm(ok: false) }
    var num = MTerm()
    num.a = y.a
    num.b = MFrac(n: -y.b.n, d: y.b.d)
    num.rad = y.rad
    guard let inv = mDivF(MFrac(n: 1, d: 1), den) else { return MTerm(ok: false) }
    return mMulT(x, mScale(num, inv))
}

private enum MTok {
    case num(Int64, Int64)
    case sqrt, pi, add, sub, mul, div, lp, rp
}

private func mLex(_ s: String) -> [MTok]? {
    var toks: [MTok] = []
    var i = s.startIndex
    func peek(_ k: Int) -> Character? {
        var j = i
        var n = 0
        while n < k && j < s.endIndex { j = s.index(after: j); n += 1 }
        return j < s.endIndex ? s[j] : nil
    }
    while i < s.endIndex {
        while i < s.endIndex && s[i].isWhitespace { i = s.index(after: i) }
        if i >= s.endIndex { break }
        let ch = s[i]
        if ch.isNumber || ((ch == "." || ch == ",") && (peek(1)?.isNumber == true)) {
            var whole: Int64 = 0
            var saw = false
            while i < s.endIndex && s[i].isNumber {
                saw = true
                whole = whole * 10 + Int64(s[i].wholeNumberValue ?? 0)
                i = s.index(after: i)
            }
            if i < s.endIndex && (s[i] == "." || s[i] == ",") && (s.index(after: i) < s.endIndex && s[s.index(after: i)].isNumber) {
                i = s.index(after: i)
                var frac: Int64 = 0
                var div: Int64 = 1
                while i < s.endIndex && s[i].isNumber {
                    frac = frac * 10 + Int64(s[i].wholeNumberValue ?? 0)
                    div *= 10
                    i = s.index(after: i)
                }
                let n = whole * div + frac
                toks.append(.num(n, div))
            } else if saw {
                toks.append(.num(whole, 1))
            } else { return nil }
            continue
        }
        if ch == "+" { toks.append(.add); i = s.index(after: i); continue }
        if ch == "-" || ch == "−" || ch == "–" { toks.append(.sub); i = s.index(after: i); continue }
        if ch == "*" || ch == "×" || ch == "·" { toks.append(.mul); i = s.index(after: i); continue }
        if ch == "/" || ch == ":" { toks.append(.div); i = s.index(after: i); continue }
        if ch == "(" { toks.append(.lp); i = s.index(after: i); continue }
        if ch == ")" { toks.append(.rp); i = s.index(after: i); continue }
        if ch == "π" { toks.append(.pi); i = s.index(after: i); continue }
        if ch == "√" { toks.append(.sqrt); i = s.index(after: i); continue }
        if ch.isLetter {
            var j = i
            while j < s.endIndex && s[j].isLetter { j = s.index(after: j) }
            let w = s[i..<j].lowercased()
            if w == "pi" { toks.append(.pi) }
            else if w == "sqrt" { toks.append(.sqrt) }
            else { return nil }
            i = j
            continue
        }
        return nil
    }
    return toks
}

private struct MParse {
    let t: [MTok]
    var i = 0
    func peek() -> MTok? { i < t.count ? t[i] : nil }
    mutating func take() -> MTok? {
        if i >= t.count { return nil }
        defer { i += 1 }
        return t[i]
    }
    mutating func expr() -> MTerm? {
        guard var left = term() else { return nil }
        while let p = peek() {
            if case .add = p { _ = take() }
            else if case .sub = p { _ = take() }
            else { break }
            guard let r = term() else { return nil }
            if case .sub = p { left = mAdd(left, mScale(r, MFrac(n: -1, d: 1))) }
            else { left = mAdd(left, r) }
            if !left.ok { return nil }
        }
        return left
    }
    mutating func term() -> MTerm? {
        guard var left = unary() else { return nil }
        while let p = peek() {
            if case .mul = p { _ = take() }
            else if case .div = p { _ = take() }
            else { break }
            guard let r = unary() else { return nil }
            if case .div = p { left = mDivT(left, r) }
            else { left = mMulT(left, r) }
            if !left.ok { return nil }
        }
        return left
    }
    mutating func unary() -> MTerm? {
        if case .sub = peek() {
            _ = take()
            guard let v = unary() else { return nil }
            return mScale(v, MFrac(n: -1, d: 1))
        }
        if case .add = peek() {
            _ = take()
            return unary()
        }
        return factor()
    }
    mutating func factor() -> MTerm? {
        guard var left = primary() else { return nil }
        while true {
            guard let p = peek() else { break }
            switch p {
            case .sqrt, .pi, .lp:
                guard let r = primary() else { return nil }
                left = mMulT(left, r)
                if !left.ok { return nil }
            case .num:
                return nil
            default:
                return left
            }
        }
        return left
    }
    mutating func primary() -> MTerm? {
        guard let p = take() else { return nil }
        switch p {
        case .num(let n, let d):
            if case .div = peek(), case .num = t[safe: i + 1] {
                _ = take()
                if case .num(let n2, let d2) = take() {
                    if n2 <= 0 || d2 != 1 || d != 1 { return nil }
                    var o = MTerm()
                    if !mSet(&o.a, n, n2) { return nil }
                    return o
                }
                return nil
            }
            if d == 1, case .num(let n2, let d2) = peek(), n >= 0 {
                _ = take()
                if case .div = peek(), case .num(let n3, let d3) = t[safe: i + 1], d2 == 1, d3 == 1, n2 >= 0, n3 > 0 {
                    _ = take()
                    _ = take()
                    var frac = MFrac()
                    if !mSet(&frac, n2, n3) { return nil }
                    var whole = MTerm()
                    whole.a = MFrac(n: n, d: 1)
                    var part = MTerm()
                    part.a = frac
                    return mAdd(whole, part)
                }
                i -= 1
            }
            var o = MTerm()
            if !mSet(&o.a, n, d) { return nil }
            return o
        case .pi:
            var o = MTerm()
            o.p = MFrac(n: 1, d: 1)
            return o
        case .sqrt:
            if case .lp = peek() {
                _ = take()
                guard let inner = expr(), case .rp = take() else { return nil }
                return mSqrtTerm(inner)
            }
            if case .num(let n, let d) = peek(), d == 1, n >= 0 {
                _ = take()
                return mSqrtTerm(MTerm(a: MFrac(n: n, d: 1)))
            }
            return nil
        case .lp:
            guard let inner = expr(), case .rp = take() else { return nil }
            return inner
        default:
            return nil
        }
    }
}

private extension Array {
    subscript(safe i: Int) -> Element? { indices.contains(i) ? self[i] : nil }
}

private func mSqrtTerm(_ inner: MTerm) -> MTerm? {
    if !inner.ok || inner.p.n != 0 || inner.rad != 0 || inner.b.n != 0 { return nil }
    if inner.a.n < 0 || inner.a.d != 1 { return nil }
    guard let s = mSqrtReduce(inner.a.n) else { return nil }
    var o = MTerm()
    o.b = MFrac(n: s.out, d: 1)
    o.rad = s.rad
    mNorm(&o)
    return o.ok ? o : nil
}

private func mPrep(_ raw: String) -> String {
    var s = raw.trimmingCharacters(in: .whitespaces)
    var again = true
    while again {
        again = false
        if s.count >= 2 {
            let pairs: [(Character, Character)] = [("(", ")"), ("[", "]"), ("{", "}")]
            if let last = s.last, pairs.contains(where: { $0.0 == s.first && $0.1 == last }) {
                s = String(s.dropFirst().dropLast()).trimmingCharacters(in: .whitespaces)
                again = true
                continue
            }
        }
    }
    s = mStripUnits(s).trimmingCharacters(in: .whitespaces)
    return s
}

private func mParseTerm(_ s: String) -> MTerm? {
    let s = mPrep(s)
    if s.isEmpty { return nil }
    guard let toks = mLex(s) else { return nil }
    if toks.isEmpty { return nil }
    var p = MParse(t: toks)
    guard let v = p.expr(), p.i == toks.count, v.ok else { return nil }
    return v
}

private func mExact(_ x: MTerm, _ y: MTerm) -> Bool {
    x.ok && y.ok && mSameF(x.a, y.a) && mSameF(x.b, y.b) && x.rad == y.rad && mSameF(x.p, y.p)
}

private func mDouble(_ t: MTerm) -> Double {
    var v = Double(t.a.n) / Double(t.a.d) + Double(t.p.n) / Double(t.p.d) * Double.pi
    if t.b.n != 0 && t.rad > 0 {
        v += Double(t.b.n) / Double(t.b.d) * (Double(t.rad)).squareRoot()
    }
    return v
}

private func mClose(_ x: MTerm, _ y: MTerm) -> Bool {
    if !x.ok || !y.ok { return false }
    if mExact(x, y) { return true }
    if x.p.n != 0 || y.p.n != 0 { return false }
    let diff = abs(mDouble(x) - mDouble(y))
    if x.b.n != 0 || y.b.n != 0 { return diff <= 0.001 }
    return diff <= 0.0005
}

private func mFold(_ s: String) -> String {
    var out = ""
    for ch in s.lowercased() {
        switch ch {
        case "á", "à", "ä", "â": out.append("a")
        case "č", "ć": out.append("c")
        case "ď": out.append("d")
        case "é", "ě", "ë", "ê": out.append("e")
        case "í", "ï": out.append("i")
        case "ň": out.append("n")
        case "ó", "ö", "ô": out.append("o")
        case "ř": out.append("r")
        case "š", "ś": out.append("s")
        case "ť": out.append("t")
        case "ú", "ů", "ü": out.append("u")
        case "ý": out.append("y")
        case "ž", "ź": out.append("z")
        default:
            if ch.isLetter || ch.isNumber { out.append(ch) }
        }
    }
    return out
}

private func mStripUnits(_ raw: String) -> String {
    var s = raw.trimmingCharacters(in: .whitespaces)
    let units = ["procenta", "procent", "cm", "mm", "dm", "km", "m"]
    var cut = true
    while cut {
        cut = false
        while s.last?.isWhitespace == true { s.removeLast() }
        if s.hasSuffix("°") || s.hasSuffix("%") || s.hasSuffix(".") {
            s.removeLast()
            cut = true
            continue
        }
        for u in units {
            guard s.count > u.count else { continue }
            if s.lowercased().hasSuffix(u) {
                let keep = s.dropLast(u.count)
                if keep.last?.isLetter == true { continue }
                s = String(keep)
                cut = true
                break
            }
        }
    }
    return s.trimmingCharacters(in: .whitespaces)
}

private func mSplit(_ s: String) -> [String]? {
    let s = s.trimmingCharacters(in: .whitespaces)
    if s.isEmpty { return nil }
    let semi = s.contains(";")
    var parts: [String] = []
    var rest = Substring(s)
    while !rest.isEmpty {
        let end: Substring.Index
        let step: Int
        if semi {
            if let i = rest.firstIndex(of: ";") { end = i; step = 1 }
            else { end = rest.endIndex; step = 0 }
        } else if let r = rest.range(of: ", ") {
            end = r.lowerBound
            step = 2
        } else {
            end = rest.endIndex
            step = 0
        }
        let piece = rest[..<end].trimmingCharacters(in: .whitespaces)
        if piece.isEmpty { return nil }
        parts.append(piece)
        if step == 0 { break }
        rest = rest[rest.index(end, offsetBy: step)...]
    }
    return parts.isEmpty ? nil : parts
}

func mathAnswerOk(_ spec: String?, _ user: String?) -> Bool {
    guard var spec, let user else { return false }
    spec = spec.trimmingCharacters(in: .whitespaces)
    if spec.hasPrefix("word:") {
        let opts = spec.dropFirst(5).split(separator: "|").map { mFold(String($0)) }
        return opts.contains(mFold(user))
    }
    if spec.hasPrefix("set:") || spec.hasPrefix("pair:") {
        let ordered = spec.hasPrefix("pair:")
        let body = String(spec.dropFirst(ordered ? 5 : 4))
        guard let want = mSplit(body), let got = mSplit(user) else { return false }
        if want.count != got.count || want.isEmpty { return false }
        var wt: [MTerm] = []
        var gt: [MTerm] = []
        for w in want {
            guard let t = mParseTerm(w) else { return false }
            wt.append(t)
        }
        for g in got {
            guard let t = mParseTerm(g) else { return false }
            gt.append(t)
        }
        if ordered {
            for i in wt.indices where !mClose(wt[i], gt[i]) { return false }
            return true
        }
        var used = Array(repeating: false, count: gt.count)
        for w in wt {
            var hit = gt.indices.first { !used[$0] && mExact(w, gt[$0]) }
            if hit == nil { hit = gt.indices.first { !used[$0] && mClose(w, gt[$0]) } }
            guard let hit else { return false }
            used[hit] = true
        }
        return true
    }
    let body = spec.hasPrefix("num:") ? String(spec.dropFirst(4)) : spec
    guard let tw = mParseTerm(body), let tg = mParseTerm(user) else { return false }
    return mClose(tw, tg)
}

func mathIsDraw(_ spec: String?) -> Bool {
    guard let spec else { return false }
    return spec.hasPrefix("square:") || spec.hasPrefix("mid:") || spec.hasPrefix("right:")
        || spec.hasPrefix("para:") || spec.hasPrefix("points:")
}

func mathDrawNeed(_ spec: String?) -> Int {
    guard let spec else { return 0 }
    if spec.hasPrefix("square:") { return 2 }
    if spec.hasPrefix("mid:") || spec.hasPrefix("right:") || spec.hasPrefix("para:") { return 1 }
    if spec.hasPrefix("points:") {
        let body = spec.dropFirst(7)
        return body.split(separator: ";").filter { !$0.hasPrefix("fix:") }.count
    }
    return 0
}

private func mParseXY(_ s: Substring) -> (Int, Int)? {
    let t = s.trimmingCharacters(in: .whitespaces)
    let bits = t.split(separator: ",")
    guard bits.count == 2, let x = Int(bits[0].trimmingCharacters(in: .whitespaces)),
          let y = Int(bits[1].trimmingCharacters(in: .whitespaces)) else { return nil }
    return (x, y)
}

func mathDrawGiven(_ spec: String?) -> [(Int, Int)] {
    guard let spec else { return [] }
    if spec.hasPrefix("points:") {
        return spec.dropFirst(7).split(separator: ";").compactMap { part -> (Int, Int)? in
            guard part.hasPrefix("fix:") else { return nil }
            return mParseXY(part.dropFirst(4))
        }
    }
    let body: Substring
    let n: Int
    if spec.hasPrefix("para:") { body = spec.dropFirst(5); n = 3 }
    else if spec.hasPrefix("square:") { body = spec.dropFirst(7); n = 2 }
    else if spec.hasPrefix("right:") { body = spec.dropFirst(6); n = 2 }
    else if spec.hasPrefix("mid:") { body = spec.dropFirst(4); n = 2 }
    else { return [] }
    return body.split(separator: ";").prefix(n).compactMap { mParseXY($0) }
}

private func mOnBoard(_ p: (Int, Int)) -> Bool {
    p.0 >= 0 && p.0 <= 10 && p.1 >= 0 && p.1 <= 8
}

func mathDrawOk(_ spec: String?, _ pts: [(Int, Int)]) -> Bool {
    guard let spec, mathDrawNeed(spec) == pts.count else { return false }
    if pts.contains(where: { !mOnBoard($0) }) { return false }
    if Set(pts.map { "\($0.0),\($0.1)" }).count != pts.count { return false }
    let g = mathDrawGiven(spec)
    if spec.hasPrefix("mid:") {
        guard g.count == 2, (g[0].0 + g[1].0) % 2 == 0, (g[0].1 + g[1].1) % 2 == 0 else { return false }
        return pts[0] == ((g[0].0 + g[1].0) / 2, (g[0].1 + g[1].1) / 2)
    }
    if spec.hasPrefix("para:") {
        guard g.count == 3 else { return false }
        return pts[0] == (g[1].0 + g[2].0 - g[0].0, g[1].1 + g[2].1 - g[0].1)
    }
    if spec.hasPrefix("right:") {
        guard g.count == 2 else { return false }
        let c = pts[0]
        if c == g[0] || c == g[1] { return false }
        let ux = g[0].0 - c.0, uy = g[0].1 - c.1
        let vx = g[1].0 - c.0, vy = g[1].1 - c.1
        return ux * vx + uy * vy == 0 && (ux != 0 || uy != 0) && (vx != 0 || vy != 0)
    }
    if spec.hasPrefix("square:") {
        guard g.count == 2, pts.count == 2 else { return false }
        let a = g[0], b = g[1]
        let dx = b.0 - a.0, dy = b.1 - a.1
        if dx == 0 && dy == 0 { return false }
        let c1 = (b.0 - dy, b.1 + dx), d1 = (a.0 - dy, a.1 + dx)
        let c2 = (b.0 + dy, b.1 - dx), d2 = (a.0 + dy, a.1 - dx)
        func same(_ p: (Int, Int), _ q: (Int, Int), _ r: (Int, Int), _ s: (Int, Int)) -> Bool {
            (p == r && q == s) || (p == s && q == r)
        }
        if !mOnBoard(c1) || !mOnBoard(d1) {
            if !mOnBoard(c2) || !mOnBoard(d2) { return false }
        }
        return same(pts[0], pts[1], c1, d1) || same(pts[0], pts[1], c2, d2)
    }
    if spec.hasPrefix("points:") {
        let want = spec.dropFirst(7).split(separator: ";").compactMap { part -> (Int, Int)? in
            if part.hasPrefix("fix:") { return nil }
            return mParseXY(part)
        }
        if want.count != pts.count { return false }
        var used = Array(repeating: false, count: pts.count)
        for w in want {
            guard let j = pts.indices.first(where: { !used[$0] && pts[$0] == w }) else { return false }
            used[j] = true
        }
        return true
    }
    return false
}
