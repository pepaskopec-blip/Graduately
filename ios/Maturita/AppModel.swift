import Foundation
import SwiftUI

final class AppModel: ObservableObject {
    let content = Content.load()
    let progress = ProgressStore()
    let updater = Updater()

    @Published var themeId: ThemeId
    @Published var mode: ColorMode
    @Published var lang: UiLang
    @Published var tab: AppTab = .practice
    @Published var practicePath: [Route] = []
    @Published var searchQuery = ""
    @Published var tick = 0
    @Published var update = UpdateState()
    @Published var showChangelog = false

    init() {
        themeId = progress.themeId
        mode = progress.mode
        lang = progress.lang
        showChangelog = !content.changelog.isEmpty && !sameBuild(AppConfig.commit, progress.seenCommit)
        #if DEBUG
        // `xcrun simctl launch <udid> org.maturita.maturita -route u1e3` opens a page for screenshots.
        if let page = UserDefaults.standard.string(forKey: "route"), let r = routeFromPage(page) {
            openFromSearch(r.page)
        }
        if let theme = UserDefaults.standard.string(forKey: "theme"), let n = Int(theme), let id = ThemeId(rawValue: n) {
            themeId = id
        }
        if let m = UserDefaults.standard.string(forKey: "mode") {
            mode = m == "dark" ? .dark : .light
        }
        #endif
    }

    var palette: Palette { themePalette(themeId, mode) }

    func tr(_ key: String?) -> String { content.tr(key, lang: lang) }

    func fmt(_ key: String, _ args: CVarArg...) -> String {
        content.fmt(key, lang: lang, args)
    }

    func go(_ r: Route) {
        switch r {
        case .stats:
            tab = .progress
        case .welcome, .subjects:
            tab = .practice
            practicePath = []
        default:
            tab = .practice
            if practicePath.last == r { return }
            if practicePath.isEmpty {
                practicePath = r.stack
            } else {
                practicePath.append(r)
            }
        }
    }

    func goPage(_ page: String?) {
        guard let page, let r = routeFromPage(page) else { return }
        go(r)
    }

    func openFromSearch(_ page: String) {
        guard let r = routeFromPage(page) else { return }
        switch r {
        case .stats:
            tab = .progress
        case .welcome, .subjects:
            tab = .practice
            practicePath = []
        default:
            tab = .practice
            practicePath = r.stack
        }
    }

    func setTheme(_ id: ThemeId) {
        themeId = id
        progress.themeId = id
    }

    func applyMode(_ m: ColorMode) {
        mode = m
        progress.mode = m
    }

    func applyLang(_ l: UiLang) {
        lang = l
        progress.lang = l
    }

    func refresh() { tick += 1 }

    func markGerman(_ unit: Int, _ ex: Int) {
        progress.markGerman(unit, ex)
        refresh()
    }

    func markVocab(_ unit: Int) {
        progress.markVocab(unit)
        refresh()
    }

    func markNet(_ id: Int) {
        progress.markNet(id)
        refresh()
    }

    func markHw(_ id: Int) {
        progress.markHw(id)
        refresh()
    }

    func markMluv(_ n: Int) {
        progress.markMluv(n)
        refresh()
    }

    func markBookQuiz(_ id: String) {
        progress.markBookQuiz(id)
        refresh()
    }

    func markBookPlot(_ id: String) {
        progress.markBookPlot(id)
        refresh()
    }

    func checkUpdate(_ interactive: Bool) {
        updater.check(interactive: interactive) { [weak self] state in
            self?.update = state
        }
    }

    func installUpdate() {
        updater.install { [weak self] state in
            self?.update = state
        }
    }

    func dismissChangelog() {
        progress.seenCommit = AppConfig.commit
        showChangelog = false
    }
}

func sameBuild(_ a: String?, _ b: String?) -> Bool {
    guard let a, let b, !a.isEmpty, !b.isEmpty else { return false }
    if a.caseInsensitiveCompare(b) == .orderedSame { return true }
    let n = min(a.count, b.count)
    return n >= 7 && a.prefix(n).caseInsensitiveCompare(b.prefix(n)) == .orderedSame
}
