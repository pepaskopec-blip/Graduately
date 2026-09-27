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
