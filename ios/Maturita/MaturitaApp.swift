import SwiftUI

@main
struct MaturitaApp: App {
    @StateObject private var vm = AppModel()

    var body: some Scene {
        WindowGroup {
            MaturitaRoot(vm: vm)
        }
    }
}

struct MaturitaRoot: View {
    @ObservedObject var vm: AppModel

    var body: some View {
        let p = vm.palette
        ZStack {
            p.base.ignoresSafeArea()
            VStack(spacing: 0) {
                HStack {
                    Text("maturita.c")
                        .font(.system(size: 15, weight: .medium))
                        .foregroundStyle(p.text)
                    Spacer()
                    IconHit(action: { vm.searchOpen = true }) { SearchIcon(color: p.text) }
                    IconHit(action: { vm.go(.stats) }) { StatsIcon(color: p.text) }
                    IconHit(action: { vm.settingsOpen = true }) { SettingsIcon(color: p.text) }
                }
                .padding(.horizontal, 12)
                .padding(.vertical, 6)
                if vm.update.canInstall || vm.update.status == "downloading" || vm.update.status == "staged" {
                    HStack {
                        Text(bannerText)
                            .font(.system(size: 13))
                            .foregroundStyle(p.text)
                        Spacer()
                        if vm.update.canInstall {
                            PillButton(label: vm.tr("update_install"), palette: p) { vm.installUpdate() }
                        }
                    }
                    .padding(.horizontal, 12)
                    .padding(.vertical, 10)
                    .background(p.surface1, in: RoundedRectangle(cornerRadius: 14, style: .continuous))
                    .padding(.horizontal, 12)
                }
                routeView
                    .id(vm.tick)
                    .frame(maxWidth: .infinity, maxHeight: .infinity)
            }
            if vm.searchOpen {
                SearchOverlay(vm: vm).ignoresSafeArea(edges: .bottom)
            }
            if vm.settingsOpen {
                SettingsSheet(vm: vm).ignoresSafeArea(edges: .bottom)
            }
        }
        .preferredColorScheme(vm.mode == .dark ? .dark : .light)
        .onAppear { vm.checkUpdate(false) }
    }

    private var bannerText: String {
        if let arg = vm.update.messageArg {
            return vm.fmt(vm.update.messageKey, arg)
        }
        return vm.tr(vm.update.messageKey)
    }

    @ViewBuilder
    private var routeView: some View {
        switch vm.route {
        case .welcome: WelcomeScreen(vm: vm)
        case .subjects: SubjectsScreen(vm: vm)
        case .stats: StatsScreen(vm: vm)
        case .roadmap: RoadmapScreen(vm: vm)
        case .unitMap(let id): UnitMapScreen(vm: vm, unitId: id)
        case .germanEx(let unit, let ex): GermanExercise(vm: vm, unitId: unit, ex: ex)
        case .vocab(let id): VocabExercise(vm: vm, unitId: id)
        case .netYears: NetYearsScreen(vm: vm)
        case .netMap:
            LessonMapScreen(
                vm: vm,
                title: vm.tr("net_year1"),
                sub: vm.tr("net_sub"),
                nodes: vm.content.netLessons.map { l in
                    let id = l.int("id")
                    return MapNode(
                        id: "n\(id)",
                        label: vm.tr(l.str("titleKey")),
                        locked: false,
                        done: vm.progress.netDone(id),
                        current: !vm.progress.netDone(id),
                        onClick: { vm.go(.netLesson(id)) }
                    )
                }
            )
        case .netLesson(let id):
            if let l = vm.content.netLesson(id) {
                SlidesScreen(vm: vm, title: vm.tr(l.str("titleKey")), sub: vm.tr(l.str("subKey")), slides: l.arr("slides")) {
                    vm.go(.netEx(id))
                }
            }
        case .netEx(let id): NetQuizScreen(vm: vm, id: id)
        case .hwMap:
            LessonMapScreen(
                vm: vm,
                title: vm.tr("Technické vybavení"),
                sub: vm.tr("hw_sub"),
                nodes: vm.content.hw.map { l in
                    let id = l.int("id")
                    return MapNode(
                        id: "h\(id)",
                        label: vm.tr(l.str("titleKey")),
                        locked: false,
                        done: vm.progress.hwDone(id),
                        current: !vm.progress.hwDone(id),
                        onClick: { vm.go(.hwLesson(id)) }
                    )
                }
            )
        case .hwLesson(let id):
            if let l = vm.content.hwLesson(id) {
                SlidesScreen(vm: vm, title: vm.tr(l.str("titleKey")), sub: vm.tr(l.str("subKey")), slides: l.arr("slides")) {
                    vm.go(.hwEx(id))
                }
            }
        case .hwEx(let id): HwQuizScreen(vm: vm, id: id)
        case .czechMap: CzechMapScreen(vm: vm)
        case .mluvnice:
            LessonMapScreen(
                vm: vm,
                title: vm.tr("Mluvnice"),
                sub: "Cvičení z mluvnice – styl maturita / přijímačky z ČJL",
                nodes: vm.content.mluvnice.map { m in
                    let n = m.int("id")
                    return MapNode(
                        id: "m\(n)",
                        label: m.str("name"),
                        locked: false,
                        done: vm.progress.mluvDone(n),
                        current: !vm.progress.mluvDone(n),
                        onClick: { vm.go(.mluvEx(n)) }
                    )
                }
            )
        case .mluvEx(let n): MluvExercise(vm: vm, n: n)
        case .readingList: BookListScreen(vm: vm)
        case .book(let id): BookScreen(vm: vm, id: id)
        case .bookQuiz(let id): LitQuizScreen(vm: vm, id: id)
        case .bookPlot(let id): PlotScreen(vm: vm, id: id)
        }
    }
}
