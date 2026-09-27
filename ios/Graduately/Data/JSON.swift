import Foundation

struct J {
    let o: [String: Any]

    init(_ obj: Any?) {
        o = obj as? [String: Any] ?? [:]
    }

    func has(_ key: String) -> Bool {
        guard let value = o[key], !(value is NSNull) else { return false }
        return true
    }

    func str(_ key: String, _ fallback: String = "") -> String {
        guard has(key) else { return fallback }
        if let s = o[key] as? String { return s }
        if let n = o[key] as? NSNumber { return n.stringValue }
        return fallback
    }

    func strOrNull(_ key: String) -> String? {
        guard has(key) else { return nil }
        if let s = o[key] as? String, !s.isEmpty, s != "null" { return s }
        return nil
    }

    func bool(_ key: String, _ fallback: Bool = false) -> Bool {
        if let b = o[key] as? Bool { return b }
        if let n = o[key] as? NSNumber { return n.boolValue }
        return fallback
    }

    func int(_ key: String, _ fallback: Int = 0) -> Int {
        if let n = o[key] as? Int { return n }
        if let n = o[key] as? NSNumber { return n.intValue }
        if let s = o[key] as? String, let n = Int(s) { return n }
        return fallback
    }

    func obj(_ key: String) -> J? {
        guard let value = o[key], !(value is NSNull) else { return nil }
        if value is [String: Any] { return J(value) }
        return nil
    }

    func exercise(_ n: Int) -> J? {
        guard let ex = o["exercises"] as? [String: Any] else { return nil }
        return J(ex["\(n)"])
    }

    func arr(_ key: String) -> [J] {
        guard let a = o[key] as? [Any] else { return [] }
        return a.compactMap { item in
            item is [String: Any] ? J(item) : nil
        }
    }

    func strs(_ key: String) -> [String] {
        guard let a = o[key] as? [Any] else { return [] }
        return a.compactMap { item in
            if item is NSNull { return nil }
            if let s = item as? String, !s.isEmpty { return s }
            return nil
        }
    }

    func strsOrEmpty(_ key: String) -> [String?] {
        guard let a = o[key] as? [Any] else { return [] }
        return a.map { item in
            if item is NSNull { return nil }
            return item as? String
        }
    }

    func intRows(_ key: String) -> [[String]] {
        guard let a = o[key] as? [Any] else { return [] }
        return a.map { row in
            guard let cells = row as? [Any] else { return [] }
            return cells.map { cell in
                if let s = cell as? String { return s }
                if let n = cell as? NSNumber { return n.stringValue }
                return ""
            }
        }
    }

    func rawArray(_ key: String) -> [Any] {
        o[key] as? [Any] ?? []
    }

    func answerList() -> [String] {
        if let a = o["answers"] as? [Any] {
            return a.compactMap { item in
                if item is NSNull { return nil }
                if let s = item as? String, !s.isEmpty { return s }
                return nil
            }
        }
        return [strOrNull("answers")].compactMap { $0 }
    }
}
