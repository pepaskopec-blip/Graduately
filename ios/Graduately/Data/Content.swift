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
            ?? "pepaskopec-blip/Graduately"
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
    let net2: [J]
    let net3: [J]
    let net4: [J]
    let hw2: [J]
    let hw3: [J]
    let hw4: [J]
    let on: [J]
    let on2: [J]
    let on3: [J]
    let on4: [J]
    let en: [J]
    let en2: [J]
    let en3: [J]
    let en4: [J]
    let de: [J]
    let de2: [J]
    let de3: [J]
    let de4: [J]
    let lit: [J]
    let lit2: [J]
    let lit3: [J]
    let lit4: [J]
    let chem: [J]
    let bio: [J]
    let fyz: [J]
    let fyz2: [J]
    let fyz3: [J]
    let fyz4: [J]
    let mat0: [J]
    let mat: [J]
    let mat2: [J]
    let mat3: [J]
    let mat4: [J]
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
        net2 = raw.arr("net2")
        net3 = raw.arr("net3")
        net4 = raw.arr("net4")
        hw2 = raw.arr("hw2")
        hw3 = raw.arr("hw3")
        hw4 = raw.arr("hw4")
        on = raw.arr("on")
        on2 = raw.arr("on2")
        on3 = raw.arr("on3")
        on4 = raw.arr("on4")
        en = raw.arr("en")
        en2 = raw.arr("en2")
        en3 = raw.arr("en3")
        en4 = raw.arr("en4")
        de = raw.arr("de")
        de2 = raw.arr("de2")
        de3 = raw.arr("de3")
        de4 = raw.arr("de4")
        lit = raw.arr("lit")
        lit2 = raw.arr("lit2")
        lit3 = raw.arr("lit3")
        lit4 = raw.arr("lit4")
        chem = raw.arr("chem")
        bio = raw.arr("bio")
        fyz = raw.arr("fyz")
        fyz2 = raw.arr("fyz2")
        fyz3 = raw.arr("fyz3")
        fyz4 = raw.arr("fyz4")
        mat0 = raw.arr("mat0")
        mat = raw.arr("mat")
        mat2 = raw.arr("mat2")
        mat3 = raw.arr("mat3")
        mat4 = raw.arr("mat4")
        mluvnice = raw.arr("mluvnice")
        books = raw.arr("books")
        changelog = raw.arr("changelog")
    }

    func germanUnit(_ id: Int) -> J? { german.first { $0.int("id") == id } }
    func netLesson(_ id: Int) -> J? { netLessons.first { $0.int("id") == id } }
    func hwLesson(_ id: Int) -> J? { hw.first { $0.int("id") == id } }
    func itLessons(_ course: String) -> [J] {
        switch course {
        case "net2": return net2
        case "net3": return net3
        case "net4": return net4
        case "hw2": return hw2
        case "hw3": return hw3
        case "hw4": return hw4
        default: return []
        }
    }
    func itLesson(_ course: String, _ id: Int) -> J? {
        itLessons(course).first { $0.int("id") == id }
    }
    func onLesson(_ id: Int) -> J? { on.first { $0.int("id") == id } }
    func on2Lesson(_ id: Int) -> J? { on2.first { $0.int("id") == id } }
    func on3Lesson(_ id: Int) -> J? { on3.first { $0.int("id") == id } }
    func on4Lesson(_ id: Int) -> J? { on4.first { $0.int("id") == id } }
    func enYear(_ year: Int) -> [J] {
        switch year {
        case 2: return en2
        case 3: return en3
        case 4: return en4
        default: return en
        }
    }
    func enLesson(_ year: Int, _ id: Int) -> J? { enYear(year).first { $0.int("id") == id } }
    func deYear(_ year: Int) -> [J] {
        switch year {
        case 2: return de2
        case 3: return de3
        case 4: return de4
        default: return de
        }
    }
    func deLesson(_ year: Int, _ id: Int) -> J? { deYear(year).first { $0.int("id") == id } }
    func litLesson(_ id: Int) -> J? { lit.first { $0.int("id") == id } }
    func lit2Lesson(_ id: Int) -> J? { lit2.first { $0.int("id") == id } }
    func lit3Lesson(_ id: Int) -> J? { lit3.first { $0.int("id") == id } }
    func lit4Lesson(_ id: Int) -> J? { lit4.first { $0.int("id") == id } }
    func chemLesson(_ id: Int) -> J? { chem.first { $0.int("id") == id } }
    func bioLesson(_ id: Int) -> J? { bio.first { $0.int("id") == id } }
    func fyzLesson(_ id: Int) -> J? { fyz.first { $0.int("id") == id } }
    func fyz2Lesson(_ id: Int) -> J? { fyz2.first { $0.int("id") == id } }
    func fyz3Lesson(_ id: Int) -> J? { fyz3.first { $0.int("id") == id } }
    func fyz4Lesson(_ id: Int) -> J? { fyz4.first { $0.int("id") == id } }
    func mat0Lesson(_ id: Int) -> J? { mat0.first { $0.int("id") == id } }
    func matLesson(_ id: Int) -> J? { mat.first { $0.int("id") == id } }
    func mat2Lesson(_ id: Int) -> J? { mat2.first { $0.int("id") == id } }
    func mat3Lesson(_ id: Int) -> J? { mat3.first { $0.int("id") == id } }
    func mat4Lesson(_ id: Int) -> J? { mat4.first { $0.int("id") == id } }
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
