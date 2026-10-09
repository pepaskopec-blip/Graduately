import Foundation

final class ProgressStore {
    private let defaults = UserDefaults.standard
    private let ids: [String: String]

    init(content: Content) {
        ids = content.progressKeys
        migrate(content.progressLegacy)
    }

    /// Progress is stored under each lesson's id from content/, so lessons can be
    /// reordered or renamed without losing it. `address` is the in-app position.
    private func key(_ address: String) -> String { "done." + (ids[address] ?? address) }
    private func done(_ address: String) -> Bool { defaults.bool(forKey: key(address)) }
    private func mark(_ address: String) { defaults.set(true, forKey: key(address)) }

    private func migrate(_ legacy: [String: String]) {
        guard !defaults.bool(forKey: "progress_ids_v2") else { return }
        for (old, id) in legacy where defaults.bool(forKey: old) {
            defaults.set(true, forKey: "done." + id)
        }
        defaults.set(true, forKey: "progress_ids_v2")
    }

    var themeId: ThemeId {
        get { ThemeId(rawValue: defaults.integer(forKey: "theme")) ?? .catppuccin }
        set { defaults.set(newValue.rawValue, forKey: "theme") }
    }

    var mode: ColorMode {
        get { defaults.string(forKey: "mode") == "dark" ? .dark : .light }
        set { defaults.set(newValue == .light ? "light" : "dark", forKey: "mode") }
    }

    var lang: UiLang {
        get { defaults.string(forKey: "lang") == "en" ? .en : .cs }
        set { defaults.set(newValue == .en ? "en" : "cs", forKey: "lang") }
    }

    var seenCommit: String {
        get { defaults.string(forKey: "seen_commit") ?? "" }
        set { defaults.set(newValue, forKey: "seen_commit") }
    }

    func germanDone(_ unit: Int, _ ex: Int) -> Bool { done("g.\(unit).\(ex)") }
    func markGerman(_ unit: Int, _ ex: Int) { mark("g.\(unit).\(ex)") }
    func vocabDone(_ unit: Int) -> Bool { done("g.\(unit).vocab") }
    func markVocab(_ unit: Int) { mark("g.\(unit).vocab") }

    func netDone(_ id: Int) -> Bool { done("net.\(id)") }
    func markNet(_ id: Int) { mark("net.\(id)") }

    func hwDone(_ id: Int) -> Bool { done("hw.\(id)") }
    func markHw(_ id: Int) { mark("hw.\(id)") }

    func onDone(_ id: Int) -> Bool { done("on.\(id)") }
    func markOn(_ id: Int) { mark("on.\(id)") }

    func on2Done(_ id: Int) -> Bool { done("on2.\(id)") }
    func markOn2(_ id: Int) { mark("on2.\(id)") }

    func on3Done(_ id: Int) -> Bool { done("on3.\(id)") }
    func markOn3(_ id: Int) { mark("on3.\(id)") }

    func on4Done(_ id: Int) -> Bool { done("on4.\(id)") }
    func markOn4(_ id: Int) { mark("on4.\(id)") }

    func enDone(_ year: Int, _ id: Int) -> Bool { done("en.\(year).\(id)") }
    func markEn(_ year: Int, _ id: Int) { mark("en.\(year).\(id)") }
    func deDone(_ year: Int, _ id: Int) -> Bool { done("de.\(year).\(id)") }
    func markDe(_ year: Int, _ id: Int) { mark("de.\(year).\(id)") }
    func courseDone(_ course: String, _ id: Int) -> Bool { done("\(course).\(id)") }
    func markCourse(_ course: String, _ id: Int) { mark("\(course).\(id)") }

    func litDone(_ id: Int) -> Bool { done("lit.\(id)") }
    func markLit(_ id: Int) { mark("lit.\(id)") }
    func lit2Done(_ id: Int) -> Bool { done("lit2.\(id)") }
    func markLit2(_ id: Int) { mark("lit2.\(id)") }
    func lit3Done(_ id: Int) -> Bool { done("lit3.\(id)") }
    func markLit3(_ id: Int) { mark("lit3.\(id)") }
    func lit4Done(_ id: Int) -> Bool { done("lit4.\(id)") }
    func markLit4(_ id: Int) { mark("lit4.\(id)") }

    func chemDone(_ id: Int) -> Bool { done("chem.\(id)") }
    func markChem(_ id: Int) { mark("chem.\(id)") }
    func bioDone(_ id: Int) -> Bool { done("bio.\(id)") }
    func markBio(_ id: Int) { mark("bio.\(id)") }
    func fyzDone(_ id: Int) -> Bool { done("fyz.\(id)") }
    func markFyz(_ id: Int) { mark("fyz.\(id)") }
    func fyz2Done(_ id: Int) -> Bool { done("fyz2.\(id)") }
    func markFyz2(_ id: Int) { mark("fyz2.\(id)") }
    func fyz3Done(_ id: Int) -> Bool { done("fyz3.\(id)") }
    func markFyz3(_ id: Int) { mark("fyz3.\(id)") }
    func fyz4Done(_ id: Int) -> Bool { done("fyz4.\(id)") }
    func markFyz4(_ id: Int) { mark("fyz4.\(id)") }
    func mat0Done(_ id: Int) -> Bool { done("mat0.\(id)") }
    func markMat0(_ id: Int) { mark("mat0.\(id)") }
    func matDone(_ id: Int) -> Bool { done("mat.\(id)") }
    func markMat(_ id: Int) { mark("mat.\(id)") }
    func mat2Done(_ id: Int) -> Bool { done("mat2.\(id)") }
    func markMat2(_ id: Int) { mark("mat2.\(id)") }
    func mat3Done(_ id: Int) -> Bool { done("mat3.\(id)") }
    func markMat3(_ id: Int) { mark("mat3.\(id)") }
    func mat4Done(_ id: Int) -> Bool { done("mat4.\(id)") }
    func markMat4(_ id: Int) { mark("mat4.\(id)") }

    func mluvDone(_ n: Int) -> Bool { done("mluv.\(n)") }
    func markMluv(_ n: Int) { mark("mluv.\(n)") }

    func bookQuiz(_ id: String) -> Bool { done("book.\(id).quiz") }
    func markBookQuiz(_ id: String) { mark("book.\(id).quiz") }
    func bookPlot(_ id: String) -> Bool { done("book.\(id).plot") }
    func markBookPlot(_ id: String) { mark("book.\(id).plot") }
}

struct ProgressSum {
    var doneEx = 0
    var totalEx = 0
    var doneUnits = 0
    var openUnits = 0
}

func summarize(_ content: Content, _ p: ProgressStore) -> ProgressSum {
    var sum = ProgressSum()
    for u in content.german where u.bool("unlocked") {
        let id = u.int("id")
        let names = u.strs("names")
        sum.totalEx += names.count
        sum.openUnits += 1
        let done = names.indices.filter { p.germanDone(id, $0 + 1) }.count
        sum.doneEx += done
        if !names.isEmpty && done == names.count { sum.doneUnits += 1 }
    }
    for l in content.netLessons {
        sum.totalEx += 1
        sum.openUnits += 1
        if p.netDone(l.int("id")) {
            sum.doneEx += 1
            sum.doneUnits += 1
        }
    }
    for l in content.hw {
        sum.totalEx += 1
        sum.openUnits += 1
        if p.hwDone(l.int("id")) {
            sum.doneEx += 1
            sum.doneUnits += 1
        }
    }
    for course in ["net2", "net3", "net4", "hw2", "hw3", "hw4"] {
        for l in content.itLessons(course) {
            sum.totalEx += 1
            sum.openUnits += 1
            if p.courseDone(course, l.int("id")) {
                sum.doneEx += 1
                sum.doneUnits += 1
            }
        }
    }
    for l in content.on {
        sum.totalEx += 1
        sum.openUnits += 1
        if p.onDone(l.int("id")) {
            sum.doneEx += 1
            sum.doneUnits += 1
        }
    }
    for l in content.on2 {
        sum.totalEx += 1
        sum.openUnits += 1
        if p.on2Done(l.int("id")) {
            sum.doneEx += 1
            sum.doneUnits += 1
        }
    }
    for l in content.on3 {
        sum.totalEx += 1
        sum.openUnits += 1
        if p.on3Done(l.int("id")) {
            sum.doneEx += 1
            sum.doneUnits += 1
        }
    }
    for l in content.on4 {
        sum.totalEx += 1
        sum.openUnits += 1
        if p.on4Done(l.int("id")) {
            sum.doneEx += 1
            sum.doneUnits += 1
        }
    }
    for year in 1...4 {
        for l in content.enYear(year) {
            sum.totalEx += 1
            sum.openUnits += 1
            if p.enDone(year, l.int("id")) {
                sum.doneEx += 1
                sum.doneUnits += 1
            }
        }
        for l in content.deYear(year) {
            sum.totalEx += 1
            sum.openUnits += 1
            if p.deDone(year, l.int("id")) {
                sum.doneEx += 1
                sum.doneUnits += 1
            }
        }
    }
    for l in content.lit {
        sum.totalEx += 1
        sum.openUnits += 1
        if p.litDone(l.int("id")) {
            sum.doneEx += 1
            sum.doneUnits += 1
        }
    }
    for l in content.lit2 {
        sum.totalEx += 1
        sum.openUnits += 1
        if p.lit2Done(l.int("id")) {
            sum.doneEx += 1
            sum.doneUnits += 1
        }
    }
    for l in content.lit3 {
        sum.totalEx += 1
        sum.openUnits += 1
        if p.lit3Done(l.int("id")) {
            sum.doneEx += 1
            sum.doneUnits += 1
        }
    }
    for l in content.lit4 {
        sum.totalEx += 1
        sum.openUnits += 1
        if p.lit4Done(l.int("id")) {
            sum.doneEx += 1
            sum.doneUnits += 1
        }
    }
    for l in content.chem {
        sum.totalEx += 1
        sum.openUnits += 1
        if p.chemDone(l.int("id")) {
            sum.doneEx += 1
            sum.doneUnits += 1
        }
    }
    for l in content.bio {
        sum.totalEx += 1
        sum.openUnits += 1
        if p.bioDone(l.int("id")) {
            sum.doneEx += 1
            sum.doneUnits += 1
        }
    }
    for l in content.fyz {
        sum.totalEx += 1
        sum.openUnits += 1
        if p.fyzDone(l.int("id")) {
            sum.doneEx += 1
            sum.doneUnits += 1
        }
    }
    for l in content.fyz2 {
        sum.totalEx += 1
        sum.openUnits += 1
        if p.fyz2Done(l.int("id")) {
            sum.doneEx += 1
            sum.doneUnits += 1
        }
    }
    for l in content.fyz3 {
        sum.totalEx += 1
        sum.openUnits += 1
        if p.fyz3Done(l.int("id")) {
            sum.doneEx += 1
            sum.doneUnits += 1
        }
    }
    for l in content.fyz4 {
        sum.totalEx += 1
        sum.openUnits += 1
        if p.fyz4Done(l.int("id")) {
            sum.doneEx += 1
            sum.doneUnits += 1
        }
    }
    for l in content.mat0 {
        sum.totalEx += 1
        sum.openUnits += 1
        if p.mat0Done(l.int("id")) {
            sum.doneEx += 1
            sum.doneUnits += 1
        }
    }
    for l in content.mat {
        sum.totalEx += 1
        sum.openUnits += 1
        if p.matDone(l.int("id")) {
            sum.doneEx += 1
            sum.doneUnits += 1
        }
    }
    for l in content.mat2 {
        sum.totalEx += 1
        sum.openUnits += 1
        if p.mat2Done(l.int("id")) {
            sum.doneEx += 1
            sum.doneUnits += 1
        }
    }
    for l in content.mat3 {
        sum.totalEx += 1
        sum.openUnits += 1
        if p.mat3Done(l.int("id")) {
            sum.doneEx += 1
            sum.doneUnits += 1
        }
    }
    for l in content.mat4 {
        sum.totalEx += 1
        sum.openUnits += 1
        if p.mat4Done(l.int("id")) {
            sum.doneEx += 1
            sum.doneUnits += 1
        }
    }
    sum.totalEx += content.mluvnice.count
    sum.openUnits += 1
    let md = content.mluvnice.filter { p.mluvDone($0.int("id")) }.count
    sum.doneEx += md
    if md == content.mluvnice.count && !content.mluvnice.isEmpty { sum.doneUnits += 1 }
    return sum
}
