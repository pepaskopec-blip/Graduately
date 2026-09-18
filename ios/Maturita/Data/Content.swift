import Foundation

enum AppConfig {
    static var commit: String {
        let raw = (Bundle.main.object(forInfoDictionaryKey: "GITCommit") as? String)?
            .trimmingCharacters(in: .whitespacesAndNewlines) ?? ""
        return raw.isEmpty || raw.hasPrefix("$(") ? "dev" : raw
    }

    static var versionName: String {
        Bundle.main.infoDictionary?["CFBundleShortVersionString"] as? String ?? "ios-1"
    }

    static var updateRepo: String {
        (Bundle.main.object(forInfoDictionaryKey: "UPDATE_REPO") as? String)
            ?? "pepaskopec-blip/maturita.c"
    }

    static var updateBranch: String {
        (Bundle.main.object(forInfoDictionaryKey: "UPDATE_BRANCH") as? String) ?? "builds"
    }
}

final class Content {
    let raw: J
    let i18n: [String: (String?, String?)]
    let subjects: [J]
    let german: [J]
    let net: J
    let netLessons: [J]
    let netTasks: [J]
    let netAnswers: [[J]]
    let hw: [J]
    let mluvnice: [J]
    let books: [J]
    let changelog: [J]

    init(_ root: [String: Any]) {
        raw = J(root)
        let obj = root["i18n"] as? [String: Any] ?? [:]
        var map: [String: (String?, String?)] = [:]
        for (key, value) in obj {
            let e = J(value)
            let cs = e.strOrNull("cs")
            let en = e.strOrNull("en")
            map[key] = (cs, en)
        }
        i18n = map
        subjects = raw.arr("subjects")
        german = raw.arr("german")
        net = raw.obj("net") ?? J([:])
        netLessons = net.arr("lessons")
        netTasks = net.arr("tasks")
        if let a = net.o["answers"] as? [Any] {
            netAnswers = a.map { block in
                guard let rows = block as? [Any] else { return [] }
                return rows.compactMap { item in
                    item is [String: Any] ? J(item) : nil
                }
            }
        } else {
            netAnswers = []
        }
        hw = raw.arr("hw")
        mluvnice = raw.arr("mluvnice")
        books = raw.arr("books")
        changelog = raw.arr("changelog")
    }

    func germanUnit(_ id: Int) -> J? { german.first { $0.int("id") == id } }
    func netLesson(_ id: Int) -> J? { netLessons.first { $0.int("id") == id } }
    func hwLesson(_ id: Int) -> J? { hw.first { $0.int("id") == id } }
    func mluv(_ n: Int) -> J? { mluvnice.first { $0.int("id") == n } }
    func book(_ id: String) -> J? { books.first { $0.str("id") == id } }

    func tr(_ key: String?, lang: UiLang) -> String {
        guard let key, !key.isEmpty else { return "" }
        let hit = i18n[key]
        if lang == .en { return hit?.1 ?? key }
        return hit?.0 ?? key
    }

    func fmt(_ key: String, lang: UiLang, _ args: [CVarArg]) -> String {
        let template = tr(key, lang: lang).replacingOccurrences(of: "%s", with: "%@")
        return String(format: template, arguments: args)
    }

    static func load() -> Content {
        guard let url = Bundle.main.url(forResource: "content", withExtension: "json"),
              let data = try? Data(contentsOf: url),
              let obj = try? JSONSerialization.jsonObject(with: data) as? [String: Any]
        else {
            return Content([:])
        }
        return Content(obj)
    }
}
