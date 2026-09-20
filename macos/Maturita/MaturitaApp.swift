import SwiftUI

@main
struct MaturitaApp: App {
    @StateObject private var vm = AppModel()

    var body: some Scene {
        WindowGroup {
            MacRoot(vm: vm)
        }
        .defaultSize(width: 1100, height: 720)
        .commands {
            CommandGroup(replacing: .newItem) {}
            CommandMenu(vm.tr("search_placeholder")) {
                Button(vm.tr("search_placeholder")) {
                    vm.tab = .search
                }
                .keyboardShortcut("k", modifiers: [.command])
            }
            CommandMenu(vm.tr("subjects_title")) {
                Button(vm.tr("search_home")) {
                    vm.go(.subjects)
                }
                .keyboardShortcut("1", modifiers: [.command])
                Button(vm.tr("stats")) {
                    vm.tab = .progress
                }
                .keyboardShortcut("2", modifiers: [.command])
                Button(vm.tr("settings")) {
                    vm.tab = .settings
                }
                .keyboardShortcut("3", modifiers: [.command])
            }
        }

        Settings {
            SettingsScreen(vm: vm)
                .frame(minWidth: 420, minHeight: 460)
                .tint(vm.palette.tint)
                .preferredColorScheme(vm.mode == .dark ? .dark : .light)
        }
    }
}

struct MacRoot: View {
    @ObservedObject var vm: AppModel

    var body: some View {
        NavigationSplitView {
            MacSidebar(vm: vm)
                .navigationSplitViewColumnWidth(min: 200, ideal: 240, max: 320)
        } detail: {
            switch vm.tab {
            case .practice:
                NavigationStack(path: $vm.practicePath) {
                    PracticeHome(vm: vm)
                        .navigationDestination(for: Route.self) { route in
                            RouteDestination(vm: vm, route: route)
                        }
                }
            case .progress:
                NavigationStack {
                    StatsScreen(vm: vm)
                }
            case .search:
                NavigationStack {
                    SearchScreen(vm: vm)
                }
            case .settings:
                NavigationStack {
                    SettingsScreen(vm: vm)
                }
            }
        }
        .navigationSplitViewStyle(.balanced)
        .tint(vm.palette.tint)
        .preferredColorScheme(vm.mode == .dark ? .dark : .light)
        .overlay(alignment: .top) {
            if showsUpdateAccessory {
                UpdateBanner(vm: vm)
                    .padding(12)
            }
        }
        .onAppear { vm.checkUpdate(false) }
        .frame(minWidth: 880, minHeight: 540)
    }

    private var showsUpdateAccessory: Bool {
        vm.update.canInstall || vm.update.status == "downloading" || vm.update.status == "staged"
    }
}

private struct MacSidebar: View {
    @ObservedObject var vm: AppModel

    var body: some View {
        List(selection: $vm.tab) {
            Section {
                Label(vm.tr("search_home"), systemImage: "square.grid.2x2.fill")
                    .tag(AppTab.practice)
                Label(vm.tr("stats"), systemImage: "chart.bar.fill")
                    .tag(AppTab.progress)
                Label(vm.tr("search_placeholder"), systemImage: "magnifyingglass")
                    .tag(AppTab.search)
                Label(vm.tr("settings"), systemImage: "gearshape.fill")
                    .tag(AppTab.settings)
            }

            Section(vm.tr("subjects_title")) {
                ForEach(Array(vm.content.subjects.enumerated()), id: \.offset) { _, s in
                    let open = s.bool("open")
                    let dest = s.strOrNull("target").flatMap(routeFromPage)
                    if open, let dest, dest != .subjects, dest != .welcome {
                        Button {
                            vm.openFromSearch(dest.page)
                        } label: {
                            subjectLabel(s)
                        }
                        .buttonStyle(.plain)
                    } else {
                        subjectLabel(s)
                            .foregroundStyle(.secondary)
                    }
                }
            }
        }
        .listStyle(.sidebar)
        .navigationTitle("maturita.c")
    }

    private func subjectLabel(_ s: J) -> some View {
        Label {
            VStack(alignment: .leading, spacing: 2) {
                Text(vm.tr(s.str("key")))
                if !s.bool("open") {
                    Text(vm.tr("stats_locked")).font(.caption).foregroundStyle(.secondary)
                }
            }
        } icon: {
            SubjectIcon(icon: s.str("icon"), open: s.bool("open"))
        }
    }
}

private struct UpdateBanner: View {
    @ObservedObject var vm: AppModel

    var body: some View {
        HStack(spacing: 12) {
            Image(systemName: "arrow.down.app.fill")
            Text(bannerText)
                .font(.subheadline)
            Spacer()
            if vm.update.canInstall {
                Button(vm.tr("update_install")) { vm.installUpdate() }
                    .buttonStyle(.borderedProminent)
            }
        }
        .padding(12)
        .background(.regularMaterial, in: RoundedRectangle(cornerRadius: 12, style: .continuous))
        .shadow(color: .black.opacity(0.12), radius: 8, y: 2)
        .frame(maxWidth: 640)
    }

    private var bannerText: String {
        if let arg = vm.update.messageArg {
            return vm.fmt(vm.update.messageKey, arg)
        }
        return vm.tr(vm.update.messageKey)
    }
}
