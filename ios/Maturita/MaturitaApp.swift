import SwiftUI

@main
struct MaturitaApp: App {
    @StateObject private var vm = AppModel()

    var body: some Scene {
        WindowGroup {
            NativeRoot(vm: vm)
        }
    }
}

struct NativeRoot: View {
    @ObservedObject var vm: AppModel

    var body: some View {
        Group {
            if showsUpdateAccessory {
                tabs.tabViewBottomAccessory { UpdateAccessory(vm: vm) }
            } else {
                tabs
            }
        }
        .tint(vm.palette.accent)
        .preferredColorScheme(vm.mode == .dark ? .dark : .light)
        .onAppear { vm.checkUpdate(false) }
    }

    private var showsUpdateAccessory: Bool {
        vm.update.canInstall || vm.update.status == "downloading" || vm.update.status == "staged"
    }

    private var tabs: some View {
        TabView(selection: $vm.tab) {
            Tab(vm.tr("subjects_title"), systemImage: "square.grid.2x2.fill", value: .practice) {
                NavigationStack(path: $vm.practicePath) {
                    PracticeHome(vm: vm)
                        .navigationDestination(for: Route.self) { route in
                            RouteDestination(vm: vm, route: route)
                        }
                }
            }
            Tab(vm.tr("stats"), systemImage: "chart.bar.fill", value: .progress) {
                NavigationStack {
                    StatsScreen(vm: vm)
                }
            }
            Tab(vm.tr("settings"), systemImage: "gearshape.fill", value: .settings) {
                NavigationStack {
                    SettingsScreen(vm: vm)
                }
            }
            Tab(vm.tr("search_placeholder"), systemImage: "magnifyingglass", value: .search, role: .search) {
                NavigationStack {
                    SearchScreen(vm: vm)
                }
            }
        }
        .tabViewStyle(.sidebarAdaptable)
        .tabBarMinimizeBehavior(.onScrollDown)
    }
}

private struct UpdateAccessory: View {
    @ObservedObject var vm: AppModel

    var body: some View {
        HStack {
            Text(bannerText)
                .font(.subheadline)
            Spacer()
            if vm.update.canInstall {
                Button(vm.tr("update_install")) { vm.installUpdate() }
                    .buttonStyle(.borderedProminent)
            }
        }
        .padding(.horizontal, 4)
    }

    private var bannerText: String {
        if let arg = vm.update.messageArg {
            return vm.fmt(vm.update.messageKey, arg)
        }
        return vm.tr(vm.update.messageKey)
    }
}

struct RouteDestination: View {
    @ObservedObject var vm: AppModel
    let route: Route

    var body: some View {
        switch route {
        case .welcome, .subjects:
            PracticeHome(vm: vm)
        case .stats:
            StatsScreen(vm: vm)
        case .roadmap:
            RoadmapScreen(vm: vm)
        case .unitMap(let id):
            UnitMapScreen(vm: vm, unitId: id)
        case .germanEx(let unit, let ex):
            GermanExercise(vm: vm, unitId: unit, ex: ex)
        case .vocab(let id):
            VocabExercise(vm: vm, unitId: id)
        case .netYears:
            NetYearsScreen(vm: vm)
        case .netMap:
            LessonListScreen(
                title: vm.tr("net_year1"),
                subtitle: vm.tr("net_sub"),
                rows: vm.content.netLessons.map { l in
                    let id = l.int("id")
                    return LessonRow(
                        id: "n\(id)",
                        title: vm.tr(l.str("titleKey")),
                        done: vm.progress.netDone(id),
                        destination: .netLesson(id)
                    )
                }
            )
        case .netLesson(let id):
            if let l = vm.content.netLesson(id) {
                SlidesScreen(vm: vm, title: vm.tr(l.str("titleKey")), subtitle: vm.tr(l.str("subKey")), slides: l.arr("slides"), next: .netEx(id))
            }
        case .netEx(let id):
            NetQuizScreen(vm: vm, id: id)
        case .hwMap:
            LessonListScreen(
                title: vm.tr("Technické vybavení"),
                subtitle: vm.tr("hw_sub"),
                rows: vm.content.hw.map { l in
                    let id = l.int("id")
                    return LessonRow(
                        id: "h\(id)",
                        title: vm.tr(l.str("titleKey")),
                        done: vm.progress.hwDone(id),
                        destination: .hwLesson(id)
                    )
                }
            )
        case .hwLesson(let id):
            if let l = vm.content.hwLesson(id) {
                SlidesScreen(vm: vm, title: vm.tr(l.str("titleKey")), subtitle: vm.tr(l.str("subKey")), slides: l.arr("slides"), next: .hwEx(id))
            }
        case .hwEx(let id):
            HwQuizScreen(vm: vm, id: id)
        case .czechMap:
            CzechMapScreen(vm: vm)
        case .mluvnice:
            LessonListScreen(
                title: vm.tr("Mluvnice"),
                subtitle: "Cvičení z mluvnice",
                rows: vm.content.mluvnice.map { m in
                    let n = m.int("id")
                    return LessonRow(
                        id: "m\(n)",
                        title: m.str("name"),
                        done: vm.progress.mluvDone(n),
                        destination: .mluvEx(n)
                    )
                }
            )
        case .mluvEx(let n):
            MluvExercise(vm: vm, n: n)
        case .readingList:
            BookListScreen(vm: vm)
        case .book(let id):
            BookScreen(vm: vm, id: id)
        case .bookQuiz(let id):
            LitQuizScreen(vm: vm, id: id)
        case .bookPlot(let id):
            PlotScreen(vm: vm, id: id)
        }
    }
}
