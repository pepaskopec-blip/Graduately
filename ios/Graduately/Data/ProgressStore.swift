import Foundation

final class ProgressStore {
    private let defaults = UserDefaults.standard

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

    func germanDone(_ unit: Int, _ ex: Int) -> Bool { defaults.bool(forKey: "g.\(unit).\(ex)") }
    func markGerman(_ unit: Int, _ ex: Int) { defaults.set(true, forKey: "g.\(unit).\(ex)") }
    func vocabDone(_ unit: Int) -> Bool { defaults.bool(forKey: "g.\(unit).vocab") }
    func markVocab(_ unit: Int) { defaults.set(true, forKey: "g.\(unit).vocab") }

    func netDone(_ id: Int) -> Bool { defaults.bool(forKey: "net.\(id)") }
    func markNet(_ id: Int) { defaults.set(true, forKey: "net.\(id)") }

    func hwDone(_ id: Int) -> Bool { defaults.bool(forKey: "hw.\(id)") }
    func markHw(_ id: Int) { defaults.set(true, forKey: "hw.\(id)") }

    func mluvDone(_ n: Int) -> Bool { defaults.bool(forKey: "mluv.\(n)") }
    func markMluv(_ n: Int) { defaults.set(true, forKey: "mluv.\(n)") }

    func bookQuiz(_ id: String) -> Bool { defaults.bool(forKey: "book.\(id).quiz") }
    func markBookQuiz(_ id: String) { defaults.set(true, forKey: "book.\(id).quiz") }
    func bookPlot(_ id: String) -> Bool { defaults.bool(forKey: "book.\(id).plot") }
    func markBookPlot(_ id: String) { defaults.set(true, forKey: "book.\(id).plot") }
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
    sum.totalEx += content.mluvnice.count
    sum.openUnits += 1
    let md = content.mluvnice.filter { p.mluvDone($0.int("id")) }.count
    sum.doneEx += md
    if md == content.mluvnice.count && !content.mluvnice.isEmpty { sum.doneUnits += 1 }
    return sum
}
