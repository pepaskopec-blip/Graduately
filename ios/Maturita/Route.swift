import Foundation

enum AppTab: Hashable {
    case practice
    case progress
    case search
    case settings
}

enum Route: Hashable {
    case welcome
    case subjects
    case stats
    case roadmap
    case unitMap(Int)
    case germanEx(Int, Int)
    case vocab(Int)
    case netYears
    case netMap
    case netLesson(Int)
    case netEx(Int)
    case hwMap
    case hwLesson(Int)
    case hwEx(Int)
    case czechMap
    case mluvnice
    case mluvEx(Int)
    case readingList
    case book(String)
    case bookQuiz(String)
    case bookPlot(String)

    var page: String {
        switch self {
        case .welcome: return "welcome"
        case .subjects: return "subjects"
        case .stats: return "stats"
        case .roadmap: return "roadmap"
        case .unitMap(let unitId): return "unit\(unitId + 1)"
        case .germanEx(let unitId, let ex): return "u\(unitId + 1)e\(ex)"
        case .vocab(let unitId): return "u\(unitId + 1)vocab"
        case .netYears: return "netyears"
        case .netMap: return "netmap"
        case .netLesson(let id): return "netunit\(id)"
        case .netEx(let id): return "netex\(id)"
        case .hwMap: return "hwmap"
        case .hwLesson(let id): return "hwunit\(id)"
        case .hwEx(let id): return "hwex\(id)"
        case .czechMap: return "czechmap"
        case .mluvnice: return "mluvnice"
        case .mluvEx(let n): return "mluve\(n)"
        case .readingList: return "readinglist"
        case .book(let id): return id == "1984" ? "cetba1984" : "cetbaFuks"
        case .bookQuiz(let id): return id == "1984" ? "cetba1984quiz" : "cetbaFuksQuiz"
        case .bookPlot(let id): return id == "1984" ? "cetba1984dej" : "cetbaFuksDej"
        }
    }

    /// Stack of destinations on top of the practice root.
    var stack: [Route] {
        switch self {
        case .welcome, .subjects, .stats:
            return []
        case .roadmap:
            return [.roadmap]
        case .unitMap(let id):
            return [.roadmap, .unitMap(id)]
        case .germanEx(let unit, let ex):
            return [.roadmap, .unitMap(unit), .germanEx(unit, ex)]
        case .vocab(let unit):
            return [.roadmap, .unitMap(unit), .vocab(unit)]
        case .netYears:
            return [.netYears]
        case .netMap:
            return [.netYears, .netMap]
        case .netLesson(let id):
            return [.netYears, .netMap, .netLesson(id)]
        case .netEx(let id):
            return [.netYears, .netMap, .netLesson(id), .netEx(id)]
        case .hwMap:
            return [.hwMap]
        case .hwLesson(let id):
            return [.hwMap, .hwLesson(id)]
        case .hwEx(let id):
            return [.hwMap, .hwLesson(id), .hwEx(id)]
        case .czechMap:
            return [.czechMap]
        case .mluvnice:
            return [.czechMap, .mluvnice]
        case .mluvEx(let n):
            return [.czechMap, .mluvnice, .mluvEx(n)]
        case .readingList:
            return [.czechMap, .readingList]
        case .book(let id):
            return [.czechMap, .readingList, .book(id)]
        case .bookQuiz(let id):
            return [.czechMap, .readingList, .book(id), .bookQuiz(id)]
        case .bookPlot(let id):
            return [.czechMap, .readingList, .book(id), .bookPlot(id)]
        }
    }
}

func routeFromPage(_ page: String) -> Route? {
    switch page {
    case "welcome": return .welcome
    case "subjects": return .subjects
    case "stats": return .stats
    case "roadmap": return .roadmap
    case "netyears": return .netYears
    case "netmap": return .netMap
    case "hwmap": return .hwMap
    case "czechmap": return .czechMap
    case "mluvnice": return .mluvnice
    case "readinglist": return .readingList
    case "cetba1984": return .book("1984")
    case "cetba1984quiz": return .bookQuiz("1984")
    case "cetba1984dej": return .bookPlot("1984")
    case "cetbaFuks": return .book("fuks")
    case "cetbaFuksQuiz": return .bookQuiz("fuks")
    case "cetbaFuksDej": return .bookPlot("fuks")
    default:
        if let m = page.wholeMatch(of: /unit(\d+)/), let n = Int(m.1) {
            return .unitMap(n - 1)
        }
        if let m = page.wholeMatch(of: /u(\d+)e(\d+)/), let u = Int(m.1), let e = Int(m.2) {
            return .germanEx(u - 1, e)
        }
        if let m = page.wholeMatch(of: /u(\d+)vocab/), let u = Int(m.1) {
            return .vocab(u - 1)
        }
        if let m = page.wholeMatch(of: /netunit(\d+)/), let n = Int(m.1) {
            return .netLesson(n)
        }
        if let m = page.wholeMatch(of: /netex(\d+)/), let n = Int(m.1) {
            return .netEx(n)
        }
        if let m = page.wholeMatch(of: /hwunit(\d+)/), let n = Int(m.1) {
            return .hwLesson(n)
        }
        if let m = page.wholeMatch(of: /hwex(\d+)/), let n = Int(m.1) {
            return .hwEx(n)
        }
        if let m = page.wholeMatch(of: /mluve(\d+)/), let n = Int(m.1) {
            return .mluvEx(n)
        }
        return nil
    }
}
