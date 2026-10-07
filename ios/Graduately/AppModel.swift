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
    @Published var practicePath = NavigationPath()
    /// Bumps whenever a full path replace is scheduled, so a queued push cannot land afterwards.
    private var pathGeneration = 0
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
        // `xcrun simctl launch <udid> com.bukovinafilip.maturita -route u1e3` opens a page for screenshots.
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
            showPracticeRoot()
        default:
            applyPracticeRoute(r, replace: practicePath.isEmpty)
        }
    }

    func showPracticeRoot() {
        tab = .practice
        schedulePracticePath(NavigationPath())
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
            showPracticeRoot()
        default:
            // Switching tabs and assigning a deep NavigationStack path in the
            // same turn crashes SwiftUI on macOS (boundPathChange / unexpectedError).
            applyPracticeRoute(r, replace: true)
        }
    }

    /// Sets the practice tab path safely, especially when jumping from Search.
    ///
    /// The sidebar-style tab view keeps one navigation column for every tab.
    /// A typed `[Route]` path does not match the other tabs, so the column
    /// traps in `boundPathChange` while an exercise is being opened. The path
    /// is therefore type-erased, and a push is applied on the next turn instead
    /// of replacing the whole stack during the click.
    private func applyPracticeRoute(_ r: Route, replace: Bool) {
        let switching = tab != .practice
        tab = .practice
        if switching || replace {
            schedulePracticePath(NavigationPath(r.stack))
        } else {
            let generation = pathGeneration
            DispatchQueue.main.async { [weak self] in
                guard let self, self.pathGeneration == generation else { return }
                self.practicePath.append(r)
            }
        }
    }

    private func schedulePracticePath(_ path: NavigationPath) {
        pathGeneration += 1
        practicePath = NavigationPath()
        DispatchQueue.main.async { [weak self] in
            self?.practicePath = path
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

    func markOn(_ id: Int) {
        progress.markOn(id)
        refresh()
    }

    func markOn2(_ id: Int) {
        progress.markOn2(id)
        refresh()
    }

    func markOn3(_ id: Int) {
        progress.markOn3(id)
        refresh()
    }

    func markOn4(_ id: Int) {
        progress.markOn4(id)
        refresh()
    }

    func markEn(_ year: Int, _ id: Int) {
        progress.markEn(year, id)
        refresh()
    }

    func markLit(_ id: Int) {
        progress.markLit(id)
        refresh()
    }

    func markLit2(_ id: Int) {
        progress.markLit2(id)
        refresh()
    }

    func markLit3(_ id: Int) {
        progress.markLit3(id)
        refresh()
    }

    func markLit4(_ id: Int) {
        progress.markLit4(id)
        refresh()
    }

    func markChem(_ id: Int) {
        progress.markChem(id)
        refresh()
    }

    func markBio(_ id: Int) {
        progress.markBio(id)
        refresh()
    }

    func markFyz(_ id: Int) {
        progress.markFyz(id)
        refresh()
    }

    func markFyz2(_ id: Int) {
        progress.markFyz2(id)
        refresh()
    }

    func markFyz3(_ id: Int) {
        progress.markFyz3(id)
        refresh()
    }

    func markFyz4(_ id: Int) {
        progress.markFyz4(id)
        refresh()
    }

    func markMat0(_ id: Int) {
        progress.markMat0(id)
        refresh()
    }

    func markMat(_ id: Int) {
        progress.markMat(id)
        refresh()
    }

    func markMat2(_ id: Int) {
        progress.markMat2(id)
        refresh()
    }

    func markMat3(_ id: Int) {
        progress.markMat3(id)
        refresh()
    }

    func markMat4(_ id: Int) {
        progress.markMat4(id)
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
