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
    case deHome
    case deMap(Int)
    case deLesson(Int, Int)
    case deEx(Int, Int)
    case unitMap(Int)
    case germanEx(Int, Int)
    case vocab(Int)
    case netYears
    case netMap
    case netLesson(Int)
    case netEx(Int)
    case hwYears
    case hwMap
    case hwLesson(Int)
    case hwEx(Int)
    case onYears
    case onMap
    case onLesson(Int)
    case onEx(Int)
    case on2Map
    case on2Lesson(Int)
    case on2Ex(Int)
    case on3Map
    case on3Lesson(Int)
    case on3Ex(Int)
    case on4Map
    case on4Lesson(Int)
    case on4Ex(Int)
    case enYears
    case enMap(Int)
    case enLesson(Int, Int)
    case enEx(Int, Int)
    case litYears
    case litMap
    case litLesson(Int)
    case litEx(Int)
    case lit2Map
    case lit2Lesson(Int)
    case lit2Ex(Int)
    case lit3Map
    case lit3Lesson(Int)
    case lit3Ex(Int)
    case lit4Map
    case lit4Lesson(Int)
    case lit4Ex(Int)
    case sciMap
    case chemMap
    case chemLesson(Int)
    case chemEx(Int)
    case bioMap
    case bioLesson(Int)
    case bioEx(Int)
    case matYears
    case mat0Map
    case mat0Lesson(Int)
    case mat0Ex(Int)
    case matMap
    case matLesson(Int)
    case matEx(Int)
    case mat2Map
    case mat2Lesson(Int)
    case mat2Ex(Int)
    case mat3Map
    case mat3Lesson(Int)
    case mat3Ex(Int)
    case mat4Map
    case mat4Lesson(Int)
    case mat4Ex(Int)
    case fyzYears
    case fyzMap
    case fyzLesson(Int)
    case fyzEx(Int)
    case fyz2Map
    case fyz2Lesson(Int)
    case fyz2Ex(Int)
    case fyz3Map
    case fyz3Lesson(Int)
    case fyz3Ex(Int)
    case fyz4Map
    case fyz4Lesson(Int)
    case fyz4Ex(Int)
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
        case .deHome: return "dehome"
        case .deMap(let year): return "de\(year)map"
        case .deLesson(let year, let id): return "de\(year)unit\(id)"
        case .deEx(let year, let id): return "de\(year)ex\(id)"
        case .unitMap(let unitId): return "unit\(unitId + 1)"
        case .germanEx(let unitId, let ex): return "u\(unitId + 1)e\(ex)"
        case .vocab(let unitId): return "u\(unitId + 1)vocab"
        case .netYears: return "netyears"
        case .netMap: return "netmap"
        case .netLesson(let id): return "netunit\(id)"
        case .netEx(let id): return "netex\(id)"
        case .hwYears: return "hwyears"
        case .hwMap: return "hwmap"
        case .hwLesson(let id): return "hwunit\(id)"
        case .hwEx(let id): return "hwex\(id)"
        case .onYears: return "onyears"
        case .onMap: return "onmap"
        case .onLesson(let id): return "onunit\(id)"
        case .onEx(let id): return "onex\(id)"
        case .on2Map: return "on2map"
        case .on2Lesson(let id): return "on2unit\(id)"
        case .on2Ex(let id): return "on2ex\(id)"
        case .on3Map: return "on3map"
        case .on3Lesson(let id): return "on3unit\(id)"
        case .on3Ex(let id): return "on3ex\(id)"
        case .on4Map: return "on4map"
        case .on4Lesson(let id): return "on4unit\(id)"
        case .on4Ex(let id): return "on4ex\(id)"
        case .enYears: return "enyears"
        case .enMap(let year): return "en\(year)map"
        case .enLesson(let year, let id): return "en\(year)unit\(id)"
        case .enEx(let year, let id): return "en\(year)ex\(id)"
        case .litYears: return "lityears"
        case .litMap: return "litmap"
        case .litLesson(let id): return "litunit\(id)"
        case .litEx(let id): return "litex\(id)"
        case .lit2Map: return "lit2map"
        case .lit2Lesson(let id): return "lit2unit\(id)"
        case .lit2Ex(let id): return "lit2ex\(id)"
        case .lit3Map: return "lit3map"
        case .lit3Lesson(let id): return "lit3unit\(id)"
        case .lit3Ex(let id): return "lit3ex\(id)"
        case .lit4Map: return "lit4map"
        case .lit4Lesson(let id): return "lit4unit\(id)"
        case .lit4Ex(let id): return "lit4ex\(id)"
        case .sciMap: return "scimap"
        case .chemMap: return "chemmap"
        case .chemLesson(let id): return "chemunit\(id)"
        case .chemEx(let id): return "chemex\(id)"
        case .bioMap: return "biomap"
        case .bioLesson(let id): return "biounit\(id)"
        case .bioEx(let id): return "bioex\(id)"
        case .matYears: return "matyears"
        case .mat0Map: return "mat0map"
        case .mat0Lesson(let id): return "mat0unit\(id)"
        case .mat0Ex(let id): return "mat0ex\(id)"
        case .matMap: return "matmap"
        case .matLesson(let id): return "matunit\(id)"
        case .matEx(let id): return "matex\(id)"
        case .mat2Map: return "mat2map"
        case .mat2Lesson(let id): return "mat2unit\(id)"
        case .mat2Ex(let id): return "mat2ex\(id)"
        case .mat3Map: return "mat3map"
        case .mat3Lesson(let id): return "mat3unit\(id)"
        case .mat3Ex(let id): return "mat3ex\(id)"
        case .mat4Map: return "mat4map"
        case .mat4Lesson(let id): return "mat4unit\(id)"
        case .mat4Ex(let id): return "mat4ex\(id)"
        case .fyzYears: return "fyzyears"
        case .fyzMap: return "fyzmap"
        case .fyzLesson(let id): return "fyzunit\(id)"
        case .fyzEx(let id): return "fyzex\(id)"
        case .fyz2Map: return "fyz2map"
        case .fyz2Lesson(let id): return "fyz2unit\(id)"
        case .fyz2Ex(let id): return "fyz2ex\(id)"
        case .fyz3Map: return "fyz3map"
        case .fyz3Lesson(let id): return "fyz3unit\(id)"
        case .fyz3Ex(let id): return "fyz3ex\(id)"
        case .fyz4Map: return "fyz4map"
        case .fyz4Lesson(let id): return "fyz4unit\(id)"
        case .fyz4Ex(let id): return "fyz4ex\(id)"
        case .czechMap: return "czechmap"
        case .mluvnice: return "mluvnice"
        case .mluvEx(let n): return "mluve\(n)"
        case .readingList: return "readinglist"
        case .book(let id):
            if id == "1984" { return "cetba1984" }
            if id == "fuks" { return "cetbaFuks" }
            return "cetba_" + id.replacingOccurrences(of: "-", with: "_")
        case .bookQuiz(let id):
            if id == "1984" { return "cetba1984quiz" }
            if id == "fuks" { return "cetbaFuksQuiz" }
            return "cetba_" + id.replacingOccurrences(of: "-", with: "_") + "_quiz"
        case .bookPlot(let id):
            if id == "1984" { return "cetba1984dej" }
            if id == "fuks" { return "cetbaFuksDej" }
            return "cetba_" + id.replacingOccurrences(of: "-", with: "_") + "_dej"
        }
    }

    /// Stack of destinations on top of the practice root.
    var stack: [Route] {
        switch self {
        case .welcome, .subjects, .stats:
            return []
        case .deHome:
            return [.deHome]
        case .deMap(let year):
            return [.deHome, .deMap(year)]
        case .deLesson(let year, let id):
            return [.deHome, .deMap(year), .deLesson(year, id)]
        case .deEx(let year, let id):
            return [.deHome, .deMap(year), .deLesson(year, id), .deEx(year, id)]
        case .roadmap:
            return [.deHome, .roadmap]
        case .unitMap(let id):
            return [.deHome, .roadmap, .unitMap(id)]
        case .germanEx(let unit, let ex):
            return [.deHome, .roadmap, .unitMap(unit), .germanEx(unit, ex)]
        case .vocab(let unit):
            return [.deHome, .roadmap, .unitMap(unit), .vocab(unit)]
        case .netYears:
            return [.netYears]
        case .netMap:
            return [.netYears, .netMap]
        case .netLesson(let id):
            return [.netYears, .netMap, .netLesson(id)]
        case .netEx(let id):
            return [.netYears, .netMap, .netLesson(id), .netEx(id)]
        case .hwYears:
            return [.hwYears]
        case .hwMap:
            return [.hwYears, .hwMap]
        case .hwLesson(let id):
            return [.hwYears, .hwMap, .hwLesson(id)]
        case .hwEx(let id):
            return [.hwYears, .hwMap, .hwLesson(id), .hwEx(id)]
        case .onYears:
            return [.onYears]
        case .onMap:
            return [.onYears, .onMap]
        case .onLesson(let id):
            return [.onYears, .onMap, .onLesson(id)]
        case .onEx(let id):
            return [.onYears, .onMap, .onLesson(id), .onEx(id)]
        case .on2Map:
            return [.onYears, .on2Map]
        case .on2Lesson(let id):
            return [.onYears, .on2Map, .on2Lesson(id)]
        case .on2Ex(let id):
            return [.onYears, .on2Map, .on2Lesson(id), .on2Ex(id)]
        case .on3Map:
            return [.onYears, .on3Map]
        case .on3Lesson(let id):
            return [.onYears, .on3Map, .on3Lesson(id)]
        case .on3Ex(let id):
            return [.onYears, .on3Map, .on3Lesson(id), .on3Ex(id)]
        case .on4Map:
            return [.onYears, .on4Map]
        case .on4Lesson(let id):
            return [.onYears, .on4Map, .on4Lesson(id)]
        case .on4Ex(let id):
            return [.onYears, .on4Map, .on4Lesson(id), .on4Ex(id)]
        case .enYears:
            return [.enYears]
        case .enMap(let year):
            return [.enYears, .enMap(year)]
        case .enLesson(let year, let id):
            return [.enYears, .enMap(year), .enLesson(year, id)]
        case .enEx(let year, let id):
            return [.enYears, .enMap(year), .enLesson(year, id), .enEx(year, id)]
        case .litYears:
            return [.czechMap, .litYears]
        case .litMap:
            return [.czechMap, .litYears, .litMap]
        case .litLesson(let id):
            return [.czechMap, .litYears, .litMap, .litLesson(id)]
        case .litEx(let id):
            return [.czechMap, .litYears, .litMap, .litLesson(id), .litEx(id)]
        case .lit2Map:
            return [.czechMap, .litYears, .lit2Map]
        case .lit2Lesson(let id):
            return [.czechMap, .litYears, .lit2Map, .lit2Lesson(id)]
        case .lit2Ex(let id):
            return [.czechMap, .litYears, .lit2Map, .lit2Lesson(id), .lit2Ex(id)]
        case .lit3Map:
            return [.czechMap, .litYears, .lit3Map]
        case .lit3Lesson(let id):
            return [.czechMap, .litYears, .lit3Map, .lit3Lesson(id)]
        case .lit3Ex(let id):
            return [.czechMap, .litYears, .lit3Map, .lit3Lesson(id), .lit3Ex(id)]
        case .lit4Map:
            return [.czechMap, .litYears, .lit4Map]
        case .lit4Lesson(let id):
            return [.czechMap, .litYears, .lit4Map, .lit4Lesson(id)]
        case .lit4Ex(let id):
            return [.czechMap, .litYears, .lit4Map, .lit4Lesson(id), .lit4Ex(id)]
        case .sciMap:
            return [.sciMap]
        case .chemMap:
            return [.sciMap, .chemMap]
        case .chemLesson(let id):
            return [.sciMap, .chemMap, .chemLesson(id)]
        case .chemEx(let id):
            return [.sciMap, .chemMap, .chemLesson(id), .chemEx(id)]
        case .bioMap:
            return [.sciMap, .bioMap]
        case .bioLesson(let id):
            return [.sciMap, .bioMap, .bioLesson(id)]
        case .bioEx(let id):
            return [.sciMap, .bioMap, .bioLesson(id), .bioEx(id)]
        case .matYears:
            return [.matYears]
        case .mat0Map:
            return [.matYears, .mat0Map]
        case .mat0Lesson(let id):
            return [.matYears, .mat0Map, .mat0Lesson(id)]
        case .mat0Ex(let id):
            return [.matYears, .mat0Map, .mat0Lesson(id), .mat0Ex(id)]
        case .matMap:
            return [.matYears, .matMap]
        case .matLesson(let id):
            return [.matYears, .matMap, .matLesson(id)]
        case .matEx(let id):
            return [.matYears, .matMap, .matLesson(id), .matEx(id)]
        case .mat2Map:
            return [.matYears, .mat2Map]
        case .mat2Lesson(let id):
            return [.matYears, .mat2Map, .mat2Lesson(id)]
        case .mat2Ex(let id):
            return [.matYears, .mat2Map, .mat2Lesson(id), .mat2Ex(id)]
        case .mat3Map:
            return [.matYears, .mat3Map]
        case .mat3Lesson(let id):
            return [.matYears, .mat3Map, .mat3Lesson(id)]
        case .mat3Ex(let id):
            return [.matYears, .mat3Map, .mat3Lesson(id), .mat3Ex(id)]
        case .mat4Map:
            return [.matYears, .mat4Map]
        case .mat4Lesson(let id):
            return [.matYears, .mat4Map, .mat4Lesson(id)]
        case .mat4Ex(let id):
            return [.matYears, .mat4Map, .mat4Lesson(id), .mat4Ex(id)]
        case .fyzYears:
            return [.fyzYears]
        case .fyzMap:
            return [.fyzYears, .fyzMap]
        case .fyzLesson(let id):
            return [.fyzYears, .fyzMap, .fyzLesson(id)]
        case .fyzEx(let id):
            return [.fyzYears, .fyzMap, .fyzLesson(id), .fyzEx(id)]
        case .fyz2Map:
            return [.fyzYears, .fyz2Map]
        case .fyz2Lesson(let id):
            return [.fyzYears, .fyz2Map, .fyz2Lesson(id)]
        case .fyz2Ex(let id):
            return [.fyzYears, .fyz2Map, .fyz2Lesson(id), .fyz2Ex(id)]
        case .fyz3Map:
            return [.fyzYears, .fyz3Map]
        case .fyz3Lesson(let id):
            return [.fyzYears, .fyz3Map, .fyz3Lesson(id)]
        case .fyz3Ex(let id):
            return [.fyzYears, .fyz3Map, .fyz3Lesson(id), .fyz3Ex(id)]
        case .fyz4Map:
            return [.fyzYears, .fyz4Map]
        case .fyz4Lesson(let id):
            return [.fyzYears, .fyz4Map, .fyz4Lesson(id)]
        case .fyz4Ex(let id):
            return [.fyzYears, .fyz4Map, .fyz4Lesson(id), .fyz4Ex(id)]
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
    case "dehome": return .deHome
    case "netyears": return .netYears
    case "netmap": return .netMap
    case "hwyears": return .hwYears
    case "hwmap": return .hwMap
    case "onyears": return .onYears
    case "onmap": return .onMap
    case "on2map": return .on2Map
    case "on3map": return .on3Map
    case "on4map": return .on4Map
    case "enyears": return .enYears
    case "lityears": return .litYears
    case "litmap": return .litMap
    case "lit2map": return .lit2Map
    case "lit3map": return .lit3Map
    case "lit4map": return .lit4Map
    case "scimap": return .sciMap
    case "chemmap": return .chemMap
    case "biomap": return .bioMap
    case "matyears": return .matYears
    case "mat0map": return .mat0Map
    case "matmap": return .matMap
    case "mat2map": return .mat2Map
    case "mat3map": return .mat3Map
    case "mat4map": return .mat4Map
    case "fyzyears": return .fyzYears
    case "fyzmap": return .fyzMap
    case "fyz2map": return .fyz2Map
    case "fyz3map": return .fyz3Map
    case "fyz4map": return .fyz4Map
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
        if page.hasPrefix("cetba_"), page.hasSuffix("_quiz") {
            let slug = String(page.dropFirst(6).dropLast(5)).replacingOccurrences(of: "_", with: "-")
            return .bookQuiz(slug)
        }
        if page.hasPrefix("cetba_"), page.hasSuffix("_dej") {
            let slug = String(page.dropFirst(6).dropLast(4)).replacingOccurrences(of: "_", with: "-")
            return .bookPlot(slug)
        }
        if page.hasPrefix("cetba_") {
            let slug = String(page.dropFirst(6)).replacingOccurrences(of: "_", with: "-")
            return .book(slug)
        }
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
        if let m = page.wholeMatch(of: /chemunit(\d+)/), let n = Int(m.1) {
            return .chemLesson(n)
        }
        if let m = page.wholeMatch(of: /chemex(\d+)/), let n = Int(m.1) {
            return .chemEx(n)
        }
        if let m = page.wholeMatch(of: /biounit(\d+)/), let n = Int(m.1) {
            return .bioLesson(n)
        }
        if let m = page.wholeMatch(of: /bioex(\d+)/), let n = Int(m.1) {
            return .bioEx(n)
        }
        if let m = page.wholeMatch(of: /mat0unit(\d+)/), let n = Int(m.1) {
            return .mat0Lesson(n)
        }
        if let m = page.wholeMatch(of: /mat0ex(\d+)/), let n = Int(m.1) {
            return .mat0Ex(n)
        }
        if let m = page.wholeMatch(of: /mat4unit(\d+)/), let n = Int(m.1) {
            return .mat4Lesson(n)
        }
        if let m = page.wholeMatch(of: /mat4ex(\d+)/), let n = Int(m.1) {
            return .mat4Ex(n)
        }
        if let m = page.wholeMatch(of: /mat3unit(\d+)/), let n = Int(m.1) {
            return .mat3Lesson(n)
        }
        if let m = page.wholeMatch(of: /mat3ex(\d+)/), let n = Int(m.1) {
            return .mat3Ex(n)
        }
        if let m = page.wholeMatch(of: /mat2unit(\d+)/), let n = Int(m.1) {
            return .mat2Lesson(n)
        }
        if let m = page.wholeMatch(of: /mat2ex(\d+)/), let n = Int(m.1) {
            return .mat2Ex(n)
        }
        if let m = page.wholeMatch(of: /matunit(\d+)/), let n = Int(m.1) {
            return .matLesson(n)
        }
        if let m = page.wholeMatch(of: /matex(\d+)/), let n = Int(m.1) {
            return .matEx(n)
        }
        if let m = page.wholeMatch(of: /fyz4unit(\d+)/), let n = Int(m.1) {
            return .fyz4Lesson(n)
        }
        if let m = page.wholeMatch(of: /fyz4ex(\d+)/), let n = Int(m.1) {
            return .fyz4Ex(n)
        }
        if let m = page.wholeMatch(of: /fyz3unit(\d+)/), let n = Int(m.1) {
            return .fyz3Lesson(n)
        }
        if let m = page.wholeMatch(of: /fyz3ex(\d+)/), let n = Int(m.1) {
            return .fyz3Ex(n)
        }
        if let m = page.wholeMatch(of: /fyz2unit(\d+)/), let n = Int(m.1) {
            return .fyz2Lesson(n)
        }
        if let m = page.wholeMatch(of: /fyz2ex(\d+)/), let n = Int(m.1) {
            return .fyz2Ex(n)
        }
        if let m = page.wholeMatch(of: /fyzunit(\d+)/), let n = Int(m.1) {
            return .fyzLesson(n)
        }
        if let m = page.wholeMatch(of: /fyzex(\d+)/), let n = Int(m.1) {
            return .fyzEx(n)
        }
        if let m = page.wholeMatch(of: /lit4unit(\d+)/), let n = Int(m.1) {
            return .lit4Lesson(n)
        }
        if let m = page.wholeMatch(of: /lit4ex(\d+)/), let n = Int(m.1) {
            return .lit4Ex(n)
        }
        if let m = page.wholeMatch(of: /lit3unit(\d+)/), let n = Int(m.1) {
            return .lit3Lesson(n)
        }
        if let m = page.wholeMatch(of: /lit3ex(\d+)/), let n = Int(m.1) {
            return .lit3Ex(n)
        }
        if let m = page.wholeMatch(of: /lit2unit(\d+)/), let n = Int(m.1) {
            return .lit2Lesson(n)
        }
        if let m = page.wholeMatch(of: /lit2ex(\d+)/), let n = Int(m.1) {
            return .lit2Ex(n)
        }
        if let m = page.wholeMatch(of: /litunit(\d+)/), let n = Int(m.1) {
            return .litLesson(n)
        }
        if let m = page.wholeMatch(of: /litex(\d+)/), let n = Int(m.1) {
            return .litEx(n)
        }
        if let m = page.wholeMatch(of: /de(\d)map/), let y = Int(m.1) {
            return .deMap(y)
        }
        if let m = page.wholeMatch(of: /de(\d)unit(\d+)/), let y = Int(m.1), let n = Int(m.2) {
            return .deLesson(y, n)
        }
        if let m = page.wholeMatch(of: /de(\d)ex(\d+)/), let y = Int(m.1), let n = Int(m.2) {
            return .deEx(y, n)
        }
        if let m = page.wholeMatch(of: /en(\d)map/), let y = Int(m.1) {
            return .enMap(y)
        }
        if let m = page.wholeMatch(of: /en(\d)unit(\d+)/), let y = Int(m.1), let n = Int(m.2) {
            return .enLesson(y, n)
        }
        if let m = page.wholeMatch(of: /en(\d)ex(\d+)/), let y = Int(m.1), let n = Int(m.2) {
            return .enEx(y, n)
        }
        if let m = page.wholeMatch(of: /on4unit(\d+)/), let n = Int(m.1) {
            return .on4Lesson(n)
        }
        if let m = page.wholeMatch(of: /on4ex(\d+)/), let n = Int(m.1) {
            return .on4Ex(n)
        }
        if let m = page.wholeMatch(of: /on3unit(\d+)/), let n = Int(m.1) {
            return .on3Lesson(n)
        }
        if let m = page.wholeMatch(of: /on3ex(\d+)/), let n = Int(m.1) {
            return .on3Ex(n)
        }
        if let m = page.wholeMatch(of: /on2unit(\d+)/), let n = Int(m.1) {
            return .on2Lesson(n)
        }
        if let m = page.wholeMatch(of: /on2ex(\d+)/), let n = Int(m.1) {
            return .on2Ex(n)
        }
        if let m = page.wholeMatch(of: /onunit(\d+)/), let n = Int(m.1) {
            return .onLesson(n)
        }
        if let m = page.wholeMatch(of: /onex(\d+)/), let n = Int(m.1) {
            return .onEx(n)
        }
        if let m = page.wholeMatch(of: /mluve(\d+)/), let n = Int(m.1) {
            return .mluvEx(n)
        }
        return nil
    }
}
