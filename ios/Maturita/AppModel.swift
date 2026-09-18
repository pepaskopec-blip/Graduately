import Foundation
import SwiftUI

final class AppModel: ObservableObject {
    let content = Content.load()
    let progress = ProgressStore()
    let updater = Updater()

    @Published var themeId: ThemeId
    @Published var mode: ColorMode
    @Published var lang: UiLang
    @Published var settingsOpen = false
    @Published var searchOpen = false
    @Published var searchQuery = ""
    @Published var tick = 0
    @Published var update = UpdateState()
    @Published var stack: [Route] = [.welcome]

    init() {
        themeId = progress.themeId
        mode = progress.mode
        lang = progress.lang
    }

    var route: Route { stack.last ?? .welcome }
    var canGoBack: Bool { stack.count > 1 }
    var palette: Palette { themePalette(themeId, mode) }

    func tr(_ key: String?) -> String { content.tr(key, lang: lang) }

    func fmt(_ key: String, _ args: CVarArg...) -> String {
        content.fmt(key, lang: lang, args)
    }

    func go(_ r: Route) {
        if stack.last == r { return }
        stack.append(r)
        searchOpen = false
    }

    func goPage(_ page: String?) {
        guard let page, let r = routeFromPage(page) else { return }
        go(r)
    }

    func back() {
        if stack.count > 1 { stack.removeLast() }
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
}
