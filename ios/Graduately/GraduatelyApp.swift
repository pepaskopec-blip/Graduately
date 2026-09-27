import SwiftUI

@main
struct MaturitaApp: App {
    @StateObject private var vm = AppModel()

    var body: some Scene {
        WindowGroup {
            NativeRoot(vm: vm)
        }
        #if os(macOS)
        .defaultSize(width: 1280, height: 840)
        .windowResizability(.contentMinSize)
        #endif
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
        .tint(vm.palette.tint)
        .preferredColorScheme(vm.mode == .dark ? .dark : .light)
        .onAppear { vm.checkUpdate(false) }
        #if os(macOS)
        .frame(minWidth: 980, minHeight: 640)
        #endif
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
            Tab(vm.tr("search"), systemImage: "magnifyingglass", value: .search) {
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
                #if os(macOS)
                .font(.body)
                #else
                .font(.subheadline)
                #endif
            Spacer()
            if vm.update.canInstall {
                Button(vm.tr("update_install")) { vm.installUpdate() }
                    .buttonStyle(.borderedProminent)
                    #if os(macOS)
                    .controlSize(.large)
                    #endif
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
