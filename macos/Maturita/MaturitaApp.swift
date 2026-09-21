import SwiftUI

@main
struct MaturitaApp: App {
    @StateObject private var vm = AppModel()

    var body: some Scene {
        WindowGroup {
            MacRoot(vm: vm)
        }
        .defaultSize(width: 1100, height: 720)
        .windowToolbarStyle(.unified)
        .windowStyle(.automatic)
        .commands {
            CommandGroup(replacing: .newItem) {}
            CommandGroup(after: .sidebar) {
                Button(vm.tr("search_home")) {
                    vm.tab = .practice
                    vm.practicePath = []
                }
                .keyboardShortcut("1", modifiers: [.command])
                Button(vm.tr("stats")) {
                    vm.tab = .progress
                }
                .keyboardShortcut("2", modifiers: [.command])
                Button(vm.tr("search")) {
                    vm.tab = .search
                }
                .keyboardShortcut("k", modifiers: [.command])
                Button(vm.tr("settings")) {
                    vm.tab = .settings
                }
                .keyboardShortcut("3", modifiers: [.command])
            }
        }

        Settings {
            SettingsScreen(vm: vm)
                .formStyle(.grouped)
                .frame(minWidth: 420, minHeight: 460)
                .tint(vm.palette.tint)
                .preferredColorScheme(vm.mode == .dark ? .dark : .light)
        }
    }
}

struct MacRoot: View {
    @ObservedObject var vm: AppModel

    var body: some View {
        tabs
            .tint(vm.palette.tint)
            .preferredColorScheme(vm.mode == .dark ? .dark : .light)
            .overlay(alignment: .top) {
                if showsUpdateAccessory {
                    UpdateBanner(vm: vm)
                        .padding(16)
                }
            }
            .onAppear { vm.checkUpdate(false) }
            .frame(minWidth: 880, minHeight: 540)
    }

    private var showsUpdateAccessory: Bool {
        vm.update.canInstall || vm.update.status == "downloading" || vm.update.status == "staged"
    }

    private var tabs: some View {
        TabView(selection: $vm.tab) {
            Tab(vm.tr("search_home"), systemImage: "square.grid.2x2.fill", value: AppTab.practice) {
                NavigationStack(path: $vm.practicePath) {
                    PracticeHome(vm: vm)
                        .navigationDestination(for: Route.self) { route in
                            RouteDestination(vm: vm, route: route)
                        }
                }
            }
            Tab(vm.tr("stats"), systemImage: "chart.bar.fill", value: AppTab.progress) {
                NavigationStack {
                    StatsScreen(vm: vm)
                }
            }
            Tab(vm.tr("settings"), systemImage: "gearshape.fill", value: AppTab.settings) {
                NavigationStack {
                    SettingsScreen(vm: vm)
                }
            }
            Tab(vm.tr("search"), systemImage: "magnifyingglass", value: AppTab.search, role: .search) {
                NavigationStack {
                    SearchScreen(vm: vm)
                }
            }
        }
        .tabViewStyle(.sidebarAdaptable)
        .toolbar {
            MacWindowToolbar(vm: vm)
        }
    }
}

private struct MacWindowToolbar: ToolbarContent {
    @ObservedObject var vm: AppModel

    var body: some ToolbarContent {
        ToolbarItemGroup(placement: .primaryAction) {
            Button {
                vm.tab = .search
            } label: {
                Label(vm.tr("search"), systemImage: "magnifyingglass")
            }
            .help(vm.tr("search"))

            Button {
                vm.tab = .progress
            } label: {
                Label(vm.tr("stats"), systemImage: "chart.bar.fill")
            }
            .help(vm.tr("stats"))
        }

        ToolbarSpacer(.fixed, placement: .primaryAction)

        ToolbarItem(placement: .primaryAction) {
            Button {
                vm.tab = .settings
            } label: {
                Label(vm.tr("settings"), systemImage: "gearshape")
            }
            .help(vm.tr("settings"))
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
                    .buttonStyle(.glassProminent)
                    .controlSize(.small)
            }
        }
        .padding(.horizontal, 16)
        .padding(.vertical, 12)
        .glassEffect(.regular, in: .rect(cornerRadius: 18, style: .continuous))
        .frame(maxWidth: 560)
    }

    private var bannerText: String {
        if let arg = vm.update.messageArg {
            return vm.fmt(vm.update.messageKey, arg)
        }
        return vm.tr(vm.update.messageKey)
    }
}
