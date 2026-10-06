import SwiftUI

struct PracticeHome: View {
    @ObservedObject var vm: AppModel
    #if os(macOS)
    @State private var homeHeader: CGFloat = 280
    #endif

    var body: some View {
        #if os(macOS)
        macHome
        #else
        iosHome
        #endif
    }

    #if os(macOS)
    private var macHome: some View {
        let sum = summarize(vm.content, vm.progress)
        let pct = sum.totalEx == 0 ? 0 : Double(sum.doneEx) / Double(sum.totalEx)
        let subjects = vm.content.subjects
        return GeometryReader { geo in
            let width = max(geo.size.width - MacChrome.pageInset * 2, MacChrome.homeTileMin)
            let offered = max(geo.size.height - homeHeader - 20 - MacChrome.pageInset * 2, MacChrome.homeTileHeight)
            let minGrid = MacGrid.contentHeight(
                width: width,
                height: offered,
                count: subjects.count,
                minWidth: MacChrome.homeTileMin,
                minHeight: MacChrome.homeTileHeight,
                spacing: 16
            )
            let gridH = max(minGrid, geo.size.height - homeHeader - 20 - MacChrome.pageInset * 2)
            ScrollView {
                VStack(alignment: .leading, spacing: 20) {
                    VStack(alignment: .leading, spacing: 16) {
                        if vm.showChangelog {
                            ChangelogCard(vm: vm)
                        }
                        GlassCard {
                            VStack(alignment: .leading, spacing: 12) {
                                Text(vm.tr("welcome_title"))
                                    .font(.largeTitle.bold())
                                Text(vm.tr(Self.welcomeBodyKey))
                                    .font(.title3)
                                    .foregroundStyle(.secondary)
                                    .fixedSize(horizontal: false, vertical: true)
                                ProgressView(value: pct)
                                Text(vm.fmt("stats_ex_fmt", sum.doneEx, sum.totalEx))
                                    .font(.subheadline)
                                    .foregroundStyle(.secondary)
                            }
                        }
                        Text(vm.tr("subjects_title"))
                            .font(.title2.weight(.semibold))
                    }
                    .background {
                        GeometryReader { proxy in
                            Color.clear.preference(key: MacHeaderHeight.self, value: proxy.size.height)
                        }
                    }
                    MacFillGrid(
                        count: subjects.count,
                        minWidth: MacChrome.homeTileMin,
                        minHeight: MacChrome.homeTileHeight,
                        spacing: 16,
                        inset: 0
                    ) { index in
                        subjectTile(subjects[index])
                    }
                    .frame(height: gridH)
                }
                .padding(MacChrome.pageInset)
                .frame(maxWidth: .infinity, minHeight: geo.size.height, alignment: .topLeading)
            }
            .onPreferenceChange(MacHeaderHeight.self) { homeHeader = $0 }
        }
        .navigationTitle("Graduately")
    }

    @ViewBuilder
    private func subjectTile(_ s: J) -> some View {
        let open = s.bool("open")
        let dest = s.strOrNull("target").flatMap(routeFromPage)
        let part = subjectSum(vm, s)
        let progress = open && part.totalEx > 0 ? Double(part.doneEx) / Double(part.totalEx) : 0
        Group {
            if open, let dest, dest != .subjects, dest != .welcome {
                NavigationLink(value: dest) {
                    subjectTileBody(s, open: true, progress: progress, part: part)
                }
                .buttonStyle(.plain)
            } else {
                subjectTileBody(s, open: false, progress: 0, part: part)
            }
        }
    }

    private func subjectTileBody(_ s: J, open: Bool, progress: Double, part: ProgressSum) -> some View {
        VStack(alignment: .leading, spacing: 16) {
            SubjectIcon(icon: s.str("icon"), open: open, size: 44)
            Text(vm.tr(s.str("key")))
                .font(.title3.weight(.semibold))
                .foregroundStyle(open ? .primary : .secondary)
                .multilineTextAlignment(.leading)
            Spacer(minLength: 0)
            if open && part.totalEx > 0 {
                ProgressView(value: progress)
                Text(vm.fmt("stats_ex_fmt", part.doneEx, part.totalEx))
                    .font(.subheadline)
                    .foregroundStyle(.secondary)
                    .monospacedDigit()
            } else if !open {
                Text(vm.tr("stats_locked"))
                    .font(.subheadline)
                    .foregroundStyle(.secondary)
            }
        }
        .padding(22)
        .frame(maxWidth: .infinity, maxHeight: .infinity, alignment: .topLeading)
        .softSurface(cornerRadius: 22)
    }
    #endif

    #if os(iOS)
    private var iosHome: some View {
        let sum = summarize(vm.content, vm.progress)
        let pct = sum.totalEx == 0 ? 0 : Double(sum.doneEx) / Double(sum.totalEx)
        return List {
            if vm.showChangelog {
                Section {
                    ChangelogCard(vm: vm)
                        .listRowInsets(EdgeInsets(top: 8, leading: 16, bottom: 8, trailing: 16))
                        .listRowBackground(Color.clear)
                        .listRowSeparator(.hidden)
                }
            }
            Section {
                GlassCard {
                    VStack(alignment: .leading, spacing: 10) {
                        Text(vm.tr("welcome_title"))
                            .font(.title.bold())
                        Text(vm.tr(Self.welcomeBodyKey))
                            .font(.subheadline)
                            .foregroundStyle(.secondary)
                        ProgressView(value: pct)
                        Text(vm.fmt("stats_ex_fmt", sum.doneEx, sum.totalEx))
                            .font(.footnote)
                            .foregroundStyle(.secondary)
                    }
                }
                .listRowInsets(EdgeInsets(top: 8, leading: 16, bottom: 8, trailing: 16))
                .listRowBackground(Color.clear)
                .listRowSeparator(.hidden)
            }

            Section(vm.tr("subjects_title")) {
                ForEach(Array(vm.content.subjects.enumerated()), id: \.offset) { _, s in
                    let open = s.bool("open")
                    let dest = s.strOrNull("target").flatMap(routeFromPage)
                    if open, let dest, dest != .subjects, dest != .welcome {
                        NavigationLink(value: dest) {
                            subjectLabel(s)
                        }
                    } else {
                        subjectLabel(s)
                            .foregroundStyle(.secondary)
                    }
                }
            }
        }
        .navigationTitle("Graduately")
        .navigationBarTitleDisplayMode(.large)
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
    #endif

    private static var welcomeBodyKey: String {
        #if os(macOS)
        "welcome_body_macos"
        #else
        "welcome_body_ios"
        #endif
    }
}

struct ChangelogCard: View {
    @ObservedObject var vm: AppModel

    var body: some View {
        let langKey = vm.lang == .en ? "en" : "cs"
        let blocks = Array(vm.content.changelog.prefix(3))
        GlassCard {
            VStack(alignment: .leading, spacing: 10) {
                HStack {
                    Label(vm.tr("changelog_title"), systemImage: "sparkles")
                        .font(.headline)
                    Spacer()
                    if let date = blocks.first?.strOrNull("date") {
                        Text(date)
                            .font(.caption.weight(.semibold))
                            .foregroundStyle(.secondary)
                    }
                }
                ForEach(Array(blocks.enumerated()), id: \.offset) { index, block in
                    ChangelogLines(index: index, lines: changelogLines(block, langKey))
                }
                Button(vm.tr("changelog_dismiss")) { vm.dismissChangelog() }
                    #if os(macOS)
                    .buttonStyle(.borderedProminent)
                    .controlSize(.large)
                    #else
                    .buttonStyle(.glassProminent)
                    .controlSize(.small)
                    #endif
            }
        }
    }
}

private func changelogLines(_ block: J, _ langKey: String) -> [String] {
    let preferred = block.strs(langKey)
    return preferred.isEmpty ? block.strs("cs") : preferred
}

private struct ChangelogLines: View {
    let index: Int
    let lines: [String]

    var body: some View {
        VStack(alignment: .leading, spacing: 4) {
            if index > 0 { Spacer().frame(height: 4) }
            #if os(macOS)
            if lines.count >= 4 {
                LazyVGrid(
                    columns: [
                        GridItem(.flexible(minimum: 240), spacing: 22, alignment: .topLeading),
                        GridItem(.flexible(minimum: 240), spacing: 22, alignment: .topLeading),
                    ],
                    alignment: .leading,
                    spacing: 6
                ) {
                    lineList
                }
            } else {
                VStack(alignment: .leading, spacing: 4) { lineList }
            }
            #else
            VStack(alignment: .leading, spacing: 4) { lineList }
            #endif
        }
    }

    @ViewBuilder
    private var lineList: some View {
        ForEach(Array(lines.enumerated()), id: \.offset) { _, line in
            Text("•  \(line)")
                .font(.subheadline)
                .foregroundStyle(.secondary)
                .fixedSize(horizontal: false, vertical: true)
                .frame(maxWidth: .infinity, alignment: .leading)
        }
    }
}

struct SettingsScreen: View {
    @ObservedObject var vm: AppModel

    var body: some View {
        #if os(macOS)
        macSettings
        #else
        settingsForm
        #endif
    }

    #if os(iOS)
    private var settingsForm: some View {
        Form {
            Section(vm.tr("mode")) {
                Picker(vm.tr("mode"), selection: modeBinding) {
                    Text(vm.tr("mode_dark")).tag(ColorMode.dark)
                    Text(vm.tr("mode_light")).tag(ColorMode.light)
                }
                .pickerStyle(.segmented)
                #if os(macOS)
                .controlSize(.large)
                #endif
            }
            Section(vm.tr("theme")) {
                ForEach(Array(ThemeId.allCases), id: \.self) { id in
                    Button {
                        vm.setTheme(id)
                    } label: {
                        HStack(spacing: 12) {
                            Circle().fill(themePalette(id, vm.mode).tint)
                                .frame(width: {
                                    #if os(macOS)
                                    18
                                    #else
                                    14
                                    #endif
                                }(), height: {
                                    #if os(macOS)
                                    18
                                    #else
                                    14
                                    #endif
                                }())
                            Text(themeNames[id.rawValue])
                                #if os(macOS)
                                .font(.title3)
                                #endif
                                .foregroundStyle(.primary)
                            Spacer()
                            if vm.themeId == id {
                                Image(systemName: "checkmark").fontWeight(.semibold)
                            }
                        }
                        #if os(macOS)
                        .padding(.vertical, 4)
                        #endif
                    }
                }
            }
            Section(vm.tr("language")) {
                Picker(vm.tr("language"), selection: langBinding) {
                    Text("Čeština").tag(UiLang.cs)
                    Text("English").tag(UiLang.en)
                }
                .pickerStyle(.segmented)
                #if os(macOS)
                .controlSize(.large)
                #endif
            }
            Section(vm.tr("updates")) {
                Text(updateText)
                    #if os(macOS)
                    .font(.body)
                    #endif
                    .foregroundStyle(.secondary)
                Button(vm.tr("update_check")) { vm.checkUpdate(true) }
                    #if os(macOS)
                    .controlSize(.large)
                    #endif
                if vm.update.canInstall {
                    Button(vm.tr("update_install")) { vm.installUpdate() }
                        #if os(macOS)
                        .controlSize(.large)
                        #endif
                }
                LabeledContent("Build", value: "\(AppConfig.versionName) · \(String(AppConfig.commit.prefix(7)))")
            }
        }
        .navigationTitle(vm.tr("settings_title"))
        .appFormStyle()
    }
    #endif

    #if os(macOS)
    private var macSettings: some View {
        let themes = Array(ThemeId.allCases)
        return VStack(alignment: .leading, spacing: 20) {
            HStack(alignment: .top, spacing: 16) {
                settingsCard(vm.tr("mode")) {
                    Picker(vm.tr("mode"), selection: modeBinding) {
                        Text(vm.tr("mode_dark")).tag(ColorMode.dark)
                        Text(vm.tr("mode_light")).tag(ColorMode.light)
                    }
                    .pickerStyle(.segmented)
                    .labelsHidden()
                    .controlSize(.large)
                }
                settingsCard(vm.tr("language")) {
                    Picker(vm.tr("language"), selection: langBinding) {
                        Text("Čeština").tag(UiLang.cs)
                        Text("English").tag(UiLang.en)
                    }
                    .pickerStyle(.segmented)
                    .labelsHidden()
                    .controlSize(.large)
                }
                settingsCard(vm.tr("updates")) {
                    Text(updateText)
                        .font(.body)
                        .foregroundStyle(.secondary)
                        .fixedSize(horizontal: false, vertical: true)
                    Button(vm.tr("update_check")) { vm.checkUpdate(true) }
                        .controlSize(.large)
                    if vm.update.canInstall {
                        Button(vm.tr("update_install")) { vm.installUpdate() }
                            .controlSize(.large)
                    }
                    LabeledContent("Build", value: "\(AppConfig.versionName) · \(String(AppConfig.commit.prefix(7)))")
                }
            }
            .layoutPriority(1)
            Text(vm.tr("theme"))
                .font(.title2.weight(.semibold))
                .layoutPriority(1)
            LazyVGrid(columns: [GridItem(.adaptive(minimum: 280), spacing: 12)], spacing: 12) {
                ForEach(themes, id: \.self) { id in
                    Button {
                        vm.setTheme(id)
                    } label: {
                        HStack(spacing: 12) {
                            Circle().fill(themePalette(id, vm.mode).tint)
                                .frame(width: 18, height: 18)
                            Text(themeNames[id.rawValue])
                                .font(.title3)
                                .foregroundStyle(.primary)
                            Spacer(minLength: 0)
                            if vm.themeId == id {
                                Image(systemName: "checkmark").fontWeight(.semibold)
                            }
                        }
                        .padding(16)
                        .frame(maxWidth: .infinity, minHeight: 64, alignment: .leading)
                        .softSurface(cornerRadius: 16)
                    }
                    .buttonStyle(.plain)
                }
            }
        }
        .padding(MacChrome.pageInset)
        .frame(maxWidth: .infinity, maxHeight: .infinity, alignment: .topLeading)
        .navigationTitle(vm.tr("settings_title"))
    }

    private func settingsCard<Content: View>(_ title: String, @ViewBuilder content: @escaping () -> Content) -> some View {
        GlassCard {
            VStack(alignment: .leading, spacing: 12) {
                Text(title).font(.headline)
                content()
            }
            .frame(maxWidth: .infinity, alignment: .leading)
        }
    }
    #endif

    private var modeBinding: Binding<ColorMode> {
        Binding(get: { vm.mode }, set: { vm.applyMode($0) })
    }

    private var langBinding: Binding<UiLang> {
        Binding(get: { vm.lang }, set: { vm.applyLang($0) })
    }

    private var updateText: String {
        if let arg = vm.update.messageArg {
            return vm.fmt(vm.update.messageKey, arg)
        }
        return vm.tr(vm.update.messageKey)
    }
}

struct StatsScreen: View {
    @ObservedObject var vm: AppModel

    var body: some View {
        #if os(macOS)
        macStats
        #else
        iosStats
        #endif
    }

    #if os(iOS)
    private var iosStats: some View {
        let sum = summarize(vm.content, vm.progress)
        let pct = sum.totalEx == 0 ? 0.0 : Double(sum.doneEx) / Double(sum.totalEx)
        return List {
            Section {
                GlassCard {
                    VStack(alignment: .leading, spacing: 14) {
                        HStack(alignment: .top) {
                            stat(vm.tr("stats_ex_label"), vm.fmt("stats_ex_fmt", sum.doneEx, sum.totalEx))
                            stat(vm.tr("stats_pct_label"), vm.fmt("stats_pct_fmt", Int(pct * 100)))
                            stat(vm.tr("stats_units_label"), vm.fmt("stats_units_fmt", sum.doneUnits, sum.openUnits))
                        }
                        ProgressView(value: pct)
                    }
                }
                .listRowInsets(EdgeInsets(top: 8, leading: 16, bottom: 8, trailing: 16))
                .listRowBackground(Color.clear)
                .listRowSeparator(.hidden)
            }
            Section(vm.tr("stats_section")) {
                ForEach(Array(vm.content.subjects.enumerated()), id: \.offset) { _, s in
                    let open = s.bool("open")
                    let part = subjectSum(vm, s)
                    Label {
                        VStack(alignment: .leading, spacing: 6) {
                            HStack {
                                Text(vm.tr(s.str("key")))
                                    #if os(macOS)
                                    .font(.title3)
                                    #endif
                                Spacer()
                                Text(open ? vm.fmt("stats_ex_fmt", part.doneEx, part.totalEx) : vm.tr("stats_locked"))
                                    #if os(macOS)
                                    .font(.body)
                                    #else
                                    .font(.caption)
                                    #endif
                                    .foregroundStyle(.secondary)
                                    .monospacedDigit()
                            }
                            if open && part.totalEx > 0 {
                                ProgressView(value: Double(part.doneEx) / Double(part.totalEx))
                            }
                        }
                    } icon: {
                        SubjectIcon(icon: s.str("icon"), open: open, size: {
                            #if os(macOS)
                            32
                            #else
                            26
                            #endif
                        }())
                    }
                    .foregroundStyle(open ? .primary : .secondary)
                }
            }
        }
        .navigationTitle(vm.tr("stats_title"))
        .appListStyle()
    }
    #endif

    #if os(macOS)
    private var macStats: some View {
        let sum = summarize(vm.content, vm.progress)
        let pct = sum.totalEx == 0 ? 0.0 : Double(sum.doneEx) / Double(sum.totalEx)
        let ranked = rankedSubjects()
        return VStack(alignment: .leading, spacing: 22) {
            HStack(spacing: 16) {
                statMetric(vm.tr("stats_ex_label"), vm.fmt("stats_ex_fmt", sum.doneEx, sum.totalEx))
                statMetric(vm.tr("stats_pct_label"), vm.fmt("stats_pct_fmt", Int(pct * 100)))
                statMetric(vm.tr("stats_units_label"), vm.fmt("stats_units_fmt", sum.doneUnits, sum.openUnits))
            }
            Text(vm.tr("stats_section"))
                .font(.title2.weight(.semibold))
            GeometryReader { geo in
                let count = max(ranked.count, 1)
                let minRow: CGFloat = 54
                let floorH = minRow * CGFloat(ranked.count)
                let rowH = min(76, max(minRow, geo.size.height / CGFloat(count)))
                let table = VStack(spacing: 0) {
                    ForEach(Array(ranked.enumerated()), id: \.offset) { index, item in
                        statRow(item.subject, part: item.part)
                            .frame(height: floorH > geo.size.height + 1 ? minRow : rowH)
                        if index < ranked.count - 1 {
                            Divider().padding(.leading, 68)
                        }
                    }
                }
                .softSurface(cornerRadius: 18)
                if floorH > geo.size.height + 1 {
                    ScrollView { table }
                } else {
                    table.frame(maxWidth: .infinity, maxHeight: .infinity, alignment: .top)
                }
            }
        }
        .padding(MacChrome.pageInset)
        .frame(maxWidth: .infinity, maxHeight: .infinity, alignment: .topLeading)
        .navigationTitle(vm.tr("stats_title"))
    }

    private func rankedSubjects() -> [(subject: J, part: ProgressSum)] {
        let rows = vm.content.subjects.map { subject in
            (subject: subject, part: subjectSum(vm, subject))
        }
        let open = rows.filter { $0.subject.bool("open") }.sorted { lhs, rhs in
            share(lhs.part) > share(rhs.part)
        }
        let locked = rows.filter { !$0.subject.bool("open") }
        return open + locked
    }

    private func share(_ part: ProgressSum) -> Double {
        part.totalEx == 0 ? 0 : Double(part.doneEx) / Double(part.totalEx)
    }

    private func statMetric(_ label: String, _ value: String) -> some View {
        VStack(alignment: .leading, spacing: 6) {
            Text(value)
                .font(.system(size: 40, weight: .bold, design: .rounded))
                .monospacedDigit()
                .minimumScaleFactor(0.6)
                .lineLimit(1)
            Text(label)
                .font(.headline)
                .foregroundStyle(.secondary)
        }
        .padding(22)
        .frame(maxWidth: .infinity, alignment: .leading)
        .softSurface(cornerRadius: 18)
    }

    private func statRow(_ s: J, part: ProgressSum) -> some View {
        let open = s.bool("open")
        let progress = share(part)
        return HStack(spacing: 16) {
            SubjectIcon(icon: s.str("icon"), open: open, size: 28)
                .frame(width: 36)
            Text(vm.tr(s.str("key")))
                .font(.title3.weight(.semibold))
                .foregroundStyle(open ? .primary : .secondary)
                .lineLimit(1)
                .frame(width: 240, alignment: .leading)
            if open && part.totalEx > 0 {
                statTrack(progress)
                Text(vm.fmt("stats_ex_fmt", part.doneEx, part.totalEx))
                    .font(.title3.monospacedDigit())
                    .foregroundStyle(.secondary)
                    .frame(width: 88, alignment: .trailing)
            } else {
                statTrack(0)
                Text(vm.tr("stats_locked"))
                    .font(.body)
                    .foregroundStyle(.tertiary)
                    .frame(width: 88, alignment: .trailing)
            }
        }
        .padding(.horizontal, 18)
    }

    private func statTrack(_ progress: Double) -> some View {
        GeometryReader { geo in
            ZStack(alignment: .leading) {
                Capsule().fill(Color.primary.opacity(0.08))
                Capsule()
                    .fill(Color.accentColor)
                    .frame(width: max(0, geo.size.width * progress))
            }
        }
        .frame(height: 12)
        .frame(maxWidth: .infinity)
    }
    #endif

    private func stat(_ label: String, _ value: String) -> some View {
        VStack(alignment: .leading, spacing: 2) {
            Text(label)
                #if os(macOS)
                .font(.subheadline)
                #else
                .font(.caption)
                #endif
                .foregroundStyle(.secondary)
            Text(value)
                #if os(macOS)
                .font(.title2.bold())
                #else
                .font(.title3.bold())
                #endif
                .monospacedDigit()
        }
        .frame(maxWidth: .infinity, alignment: .leading)
    }
}

private func subjectSum(_ vm: AppModel, _ s: J) -> ProgressSum {
    var sum = ProgressSum()
    switch s.str("target") {
    case "roadmap":
        for u in vm.content.german where u.bool("unlocked") {
            let n = u.strs("names").count
            sum.totalEx += n
            sum.doneEx += (1...n).filter { vm.progress.germanDone(u.int("id"), $0) }.count
        }
    case "netyears":
        sum.totalEx = vm.content.netLessons.count
        sum.doneEx = vm.content.netLessons.filter { vm.progress.netDone($0.int("id")) }.count
    case "hwyears", "hwmap":
        sum.totalEx = vm.content.hw.count
        sum.doneEx = vm.content.hw.filter { vm.progress.hwDone($0.int("id")) }.count
    case "onyears", "onmap", "on2map", "on3map", "on4map":
        sum.totalEx = vm.content.on.count + vm.content.on2.count + vm.content.on3.count + vm.content.on4.count
        sum.doneEx = vm.content.on.filter { vm.progress.onDone($0.int("id")) }.count
            + vm.content.on2.filter { vm.progress.on2Done($0.int("id")) }.count
            + vm.content.on3.filter { vm.progress.on3Done($0.int("id")) }.count
            + vm.content.on4.filter { vm.progress.on4Done($0.int("id")) }.count
    case "scimap":
        sum.totalEx = vm.content.chem.count + vm.content.bio.count
        sum.doneEx = vm.content.chem.filter { vm.progress.chemDone($0.int("id")) }.count
            + vm.content.bio.filter { vm.progress.bioDone($0.int("id")) }.count
    case "czechmap":
        let litCount = vm.content.lit.count + vm.content.lit2.count + vm.content.lit3.count + vm.content.lit4.count
        let litDone = vm.content.lit.filter { vm.progress.litDone($0.int("id")) }.count
            + vm.content.lit2.filter { vm.progress.lit2Done($0.int("id")) }.count
            + vm.content.lit3.filter { vm.progress.lit3Done($0.int("id")) }.count
            + vm.content.lit4.filter { vm.progress.lit4Done($0.int("id")) }.count
        sum.totalEx = vm.content.mluvnice.count + litCount
        sum.doneEx = vm.content.mluvnice.filter { vm.progress.mluvDone($0.int("id")) }.count + litDone
    default:
        break
    }
    return sum
}

struct SearchScreen: View {
    @ObservedObject var vm: AppModel
    @State private var query = ""
    #if os(iOS)
    @FocusState private var fieldFocused: Bool
    #endif

    var body: some View {
        let q = normalizeAnswer(query)
        let hits = buildSearch(vm).filter { q.isEmpty || normalizeAnswer($0.hay).contains(q) }
        #if os(macOS)
        return macSearch(hits)
        #else
        return iosSearch(hits)
        #endif
    }

    #if os(macOS)
    private func macSearch(_ hits: [Hit]) -> some View {
        ScrollView {
            if hits.isEmpty {
                ContentUnavailableView.search
                    .frame(maxWidth: .infinity, minHeight: 360)
            } else {
                LazyVGrid(columns: [GridItem(.adaptive(minimum: 340), spacing: 12)], spacing: 12) {
                    ForEach(hits) { hit in
                        Button {
                            if !hit.locked { vm.openFromSearch(hit.target) }
                        } label: {
                            VStack(alignment: .leading, spacing: 6) {
                                Text(hit.title)
                                    .font(.title3.weight(.semibold))
                                    .foregroundStyle(hit.locked ? .secondary : .primary)
                                    .multilineTextAlignment(.leading)
                                if !hit.sub.isEmpty {
                                    Text(hit.sub)
                                        .font(.body)
                                        .foregroundStyle(.secondary)
                                        .multilineTextAlignment(.leading)
                                }
                            }
                            .padding(16)
                            .frame(maxWidth: .infinity, minHeight: 84, alignment: .leading)
                            .softSurface(cornerRadius: 16)
                        }
                        .buttonStyle(.plain)
                        .disabled(hit.locked)
                    }
                }
                .padding(MacChrome.pageInset)
            }
        }
        .navigationTitle(vm.tr("search"))
        .searchable(text: $vm.searchQuery, prompt: vm.tr("search_placeholder"))
        .onAppear { query = vm.searchQuery }
        .onChange(of: vm.searchQuery) { _, q in query = q }
    }
    #endif

    #if os(iOS)
    private func iosSearch(_ hits: [Hit]) -> some View {
        List {
            if hits.isEmpty {
                ContentUnavailableView.search
                    .listRowBackground(Color.clear)
                    .listRowSeparator(.hidden)
            } else {
                ForEach(hits) { hit in
                    Button {
                        #if os(iOS)
                        fieldFocused = false
                        #endif
                        if !hit.locked { vm.openFromSearch(hit.target) }
                    } label: {
                        ListRowLabel(title: hit.title, subtitle: hit.sub)
                    }
                    .disabled(hit.locked)
                    .foregroundStyle(hit.locked ? .secondary : .primary)
                }
            }
        }
        .navigationTitle(vm.tr("search"))
        .navigationBarTitleDisplayMode(.inline)
        .safeAreaBar(edge: .top) {
            searchField
        }
        .onAppear {
            query = vm.searchQuery
            DispatchQueue.main.async {
                fieldFocused = true
            }
        }
        .onChange(of: query) { _, q in
            vm.searchQuery = q
        }
        .onChange(of: vm.searchQuery) { _, q in
            if q != query { query = q }
        }
        .appListStyle()
    }
    #endif

    #if os(iOS)
    private var searchField: some View {
        HStack(spacing: 8) {
            Image(systemName: "magnifyingglass")
                .foregroundStyle(.secondary)
            TextField(vm.tr("search_placeholder"), text: $query)
                .textInputAutocapitalization(.never)
                .autocorrectionDisabled()
                .submitLabel(.search)
                .focused($fieldFocused)
            if !query.isEmpty {
                Button {
                    query = ""
                    fieldFocused = true
                } label: {
                    Image(systemName: "xmark.circle.fill")
                        .foregroundStyle(.tertiary)
                }
                .buttonStyle(.plain)
                .accessibilityLabel("Clear")
            }
        }
        .padding(.horizontal, 12)
        .padding(.vertical, 10)
        .background(Color(.tertiarySystemFill), in: RoundedRectangle(cornerRadius: 12, style: .continuous))
        .padding(.horizontal, 16)
        .padding(.vertical, 8)
    }
    #endif
}

private struct Hit: Identifiable {
    var id: String { target + "|" + title }
    let title: String
    let sub: String
    let target: String
    let locked: Bool
    let hay: String
}

private func buildSearch(_ vm: AppModel) -> [Hit] {
    var out: [Hit] = []
    func add(_ title: String, _ sub: String, _ target: String, locked: Bool = false, extra: String = "") {
        out.append(Hit(title: title, sub: sub, target: target, locked: locked, hay: "\(title) \(sub) \(target) \(extra)"))
    }
    add(vm.tr("subjects_title"), vm.tr("search_page"), "subjects", extra: "predmety")
    add(vm.tr("stats"), vm.tr("search_page"), "stats")
    add(vm.tr("search_home"), vm.tr("search_page"), "welcome")
    for s in vm.content.subjects {
        add(vm.tr(s.str("key")), vm.tr("search_subject"), s.strOrNull("target") ?? "subjects", locked: !s.bool("open"))
    }
    for u in vm.content.german {
        let unlocked = u.bool("unlocked")
        add(u.str("title"), vm.tr("search_unit"), u.strOrNull("page") ?? "roadmap", locked: !unlocked, extra: "deutsch")
        if unlocked {
            for (i, name) in u.strs("names").enumerated() {
                add(name, u.str("title"), "u\(u.int("id") + 1)e\(i + 1)")
            }
            if u.has("branch") { add("Vokabeltraining", u.str("title"), u.str("branch"), extra: "vokabel") }
        }
    }
    for l in vm.content.netLessons {
        add(vm.tr(l.str("titleKey")), vm.tr("search_lesson"), "netunit\(l.int("id"))", extra: "site")
        add(vm.tr(l.str("titleKey")), vm.tr("search_exercise"), "netex\(l.int("id"))")
    }
    add(vm.tr("hw_year1"), vm.tr("search_lesson"), "hwmap", extra: "rocnik")
    add(vm.tr("hw_year2"), vm.tr("search_lesson"), "hwyears", locked: true, extra: "rocnik")
    add(vm.tr("hw_year3"), vm.tr("search_lesson"), "hwyears", locked: true, extra: "rocnik")
    add(vm.tr("hw_year4"), vm.tr("search_lesson"), "hwyears", locked: true, extra: "rocnik")
    for l in vm.content.hw {
        add(vm.tr(l.str("titleKey")), vm.tr("search_lesson"), "hwunit\(l.int("id"))", extra: "hardware")
        add(vm.tr(l.str("titleKey")), vm.tr("search_exercise"), "hwex\(l.int("id"))")
    }
    add(vm.tr("on_year1"), vm.tr("search_lesson"), "onmap", extra: "rocnik")
    add(vm.tr("on_year2"), vm.tr("search_lesson"), "on2map", extra: "rocnik")
    add(vm.tr("on_year3"), vm.tr("search_lesson"), "on3map", extra: "rocnik")
    add(vm.tr("on_year4"), vm.tr("search_lesson"), "on4map", extra: "rocnik")
    for l in vm.content.on {
        add(vm.tr(l.str("titleKey")), vm.tr("search_lesson"), "onunit\(l.int("id"))", extra: "obcanka")
        add(vm.tr(l.str("titleKey")), vm.tr("search_exercise"), "onex\(l.int("id"))")
    }
    for l in vm.content.on2 {
        add(vm.tr(l.str("titleKey")), vm.tr("search_lesson"), "on2unit\(l.int("id"))", extra: "obcanka")
        add(vm.tr(l.str("titleKey")), vm.tr("search_exercise"), "on2ex\(l.int("id"))")
    }
    for l in vm.content.on3 {
        add(vm.tr(l.str("titleKey")), vm.tr("search_lesson"), "on3unit\(l.int("id"))", extra: "obcanka")
        add(vm.tr(l.str("titleKey")), vm.tr("search_exercise"), "on3ex\(l.int("id"))")
    }
    for l in vm.content.on4 {
        add(vm.tr(l.str("titleKey")), vm.tr("search_lesson"), "on4unit\(l.int("id"))", extra: "obcanka")
        add(vm.tr(l.str("titleKey")), vm.tr("search_exercise"), "on4ex\(l.int("id"))")
    }
    add(vm.tr("Literatura"), vm.tr("search_lesson"), "lityears", extra: "literatura")
    add(vm.tr("lit_year1"), vm.tr("search_lesson"), "litmap", extra: "rocnik")
    add(vm.tr("lit_year2"), vm.tr("search_lesson"), "lit2map", extra: "rocnik")
    add(vm.tr("lit_year3"), vm.tr("search_lesson"), "lit3map", extra: "rocnik")
    add(vm.tr("lit_year4"), vm.tr("search_lesson"), "lit4map", extra: "rocnik")
    for l in vm.content.lit {
        add(vm.tr(l.str("titleKey")), vm.tr("search_lesson"), "litunit\(l.int("id"))", extra: "literatura")
        add(vm.tr(l.str("titleKey")), vm.tr("search_exercise"), "litex\(l.int("id"))")
    }
    for l in vm.content.lit2 {
        add(vm.tr(l.str("titleKey")), vm.tr("search_lesson"), "lit2unit\(l.int("id"))", extra: "literatura")
        add(vm.tr(l.str("titleKey")), vm.tr("search_exercise"), "lit2ex\(l.int("id"))")
    }
    for l in vm.content.lit3 {
        add(vm.tr(l.str("titleKey")), vm.tr("search_lesson"), "lit3unit\(l.int("id"))", extra: "literatura")
        add(vm.tr(l.str("titleKey")), vm.tr("search_exercise"), "lit3ex\(l.int("id"))")
    }
    for l in vm.content.lit4 {
        add(vm.tr(l.str("titleKey")), vm.tr("search_lesson"), "lit4unit\(l.int("id"))", extra: "literatura")
        add(vm.tr(l.str("titleKey")), vm.tr("search_exercise"), "lit4ex\(l.int("id"))")
    }
    add(vm.tr("Chemie"), vm.tr("search_lesson"), "chemmap", extra: "chemie")
    add(vm.tr("Biologie"), vm.tr("search_lesson"), "biomap", extra: "biologie")
    for l in vm.content.chem {
        add(vm.tr(l.str("titleKey")), vm.tr("search_lesson"), "chemunit\(l.int("id"))", extra: "chemie")
        add(vm.tr(l.str("titleKey")), vm.tr("search_exercise"), "chemex\(l.int("id"))")
    }
    for l in vm.content.bio {
        add(vm.tr(l.str("titleKey")), vm.tr("search_lesson"), "biounit\(l.int("id"))", extra: "biologie")
        add(vm.tr(l.str("titleKey")), vm.tr("search_exercise"), "bioex\(l.int("id"))")
    }
    add(vm.tr("Mluvnice"), vm.tr("search_lesson"), "mluvnice")
    add(vm.tr("Maturitní četba"), vm.tr("search_book"), "readinglist")
    for b in vm.content.books {
        add(b.str("title"), vm.tr("search_book"), b.str("page"))
    }
    return out
}

struct RoadmapScreen: View {
    @ObservedObject var vm: AppModel

    var body: some View {
        #if os(macOS)
        roadmapPath
        #else
        roadmapList
        #endif
    }

    #if os(macOS)
    private var roadmapPath: some View {
        let current = vm.content.german.firstIndex { u in
            u.bool("unlocked") && u.strs("names").indices.contains { !vm.progress.germanDone(u.int("id"), $0 + 1) }
        }
        let lit = CGFloat(max((current ?? vm.content.german.count) - 1, 0))
        return PathMap(
            nodes: vm.content.german.enumerated().map { i, u in
                let unlocked = u.bool("unlocked")
                let names = u.strs("names")
                let done = unlocked && !names.isEmpty && names.indices.allSatisfy {
                    vm.progress.germanDone(u.int("id"), $0 + 1)
                }
                return MapNode(
                    id: u.str("page"),
                    label: u.str("title"),
                    locked: !unlocked,
                    done: done,
                    current: i == current,
                    finish: i == vm.content.german.count - 1,
                    destination: unlocked ? .unitMap(u.int("id")) : nil
                )
            },
            palette: vm.palette,
            litUntil: lit,
            nodeSize: MacChrome.pathNode,
            mx: 100,
            my: 140,
            spac: 196,
            gap: 280,
            wave: 18
        )
        .padding(28)
        .navigationTitle(vm.tr("roadmap_title"))
        .navigationSubtitle(vm.tr("roadmap_sub"))
    }
    #endif

    #if os(iOS)
    private var roadmapList: some View {
        List {
            ForEach(Array(vm.content.german.enumerated()), id: \.offset) { _, u in
                let unlocked = u.bool("unlocked")
                let names = u.strs("names")
                let done = unlocked && !names.isEmpty && names.indices.allSatisfy { vm.progress.germanDone(u.int("id"), $0 + 1) }
                if unlocked {
                    NavigationLink(value: Route.unitMap(u.int("id"))) {
                        Label {
                            Text(u.str("title"))
                        } icon: {
                            Image(systemName: done ? "checkmark.circle.fill" : "circle")
                        }
                    }
                } else {
                    Label(u.str("title"), systemImage: "lock.fill")
                        .foregroundStyle(.secondary)
                }
            }
        }
        .navigationTitle(vm.tr("roadmap_title"))
        .navigationSubtitle(vm.tr("roadmap_sub"))
        .appListStyle()
    }
    #endif
}

struct UnitMapScreen: View {
    @ObservedObject var vm: AppModel
    let unitId: Int

    var body: some View {
        if let u = vm.content.germanUnit(unitId) {
            #if os(macOS)
            unitPath(u)
            #else
            unitList(u)
            #endif
        }
    }

    #if os(macOS)
    private func unitPath(_ u: J) -> some View {
        let names = u.strs("names")
        let next = names.indices.first { !vm.progress.germanDone(unitId, $0 + 1) }
        let lit = CGFloat(next ?? names.count)
        let branch: (index: Int, node: MapNode)? = u.has("branch")
            ? (
                1,
                MapNode(
                    id: "vocab-\(unitId)",
                    label: vm.tr("Vokabeltraining"),
                    locked: false,
                    done: false,
                    current: false,
                    destination: .vocab(unitId)
                )
            )
            : nil
        return PathMap(
            nodes: names.enumerated().map { i, name in
                MapNode(
                    id: "e\(i + 1)",
                    label: name,
                    locked: false,
                    done: vm.progress.germanDone(unitId, i + 1),
                    current: i == next,
                    destination: .germanEx(unitId, i + 1)
                )
            },
            palette: vm.palette,
            litUntil: lit,
            nodeSize: MacChrome.pathNode,
            mx: 100,
            my: 136,
            spac: 190,
            gap: 268,
            wave: 16,
            branch: branch
        )
        .padding(28)
        .navigationTitle(u.str("title"))
        .navigationSubtitle(vm.tr(u.str("sub")))
    }
    #endif

    #if os(iOS)
    private func unitList(_ u: J) -> some View {
        let names = u.strs("names")
        return List {
            ForEach(names.indices, id: \.self) { i in
                NavigationLink(value: Route.germanEx(unitId, i + 1)) {
                    Label {
                        Text(names[i])
                    } icon: {
                        Image(systemName: vm.progress.germanDone(unitId, i + 1) ? "checkmark.circle.fill" : "circle")
                    }
                }
            }
            if u.has("branch") {
                NavigationLink(value: Route.vocab(unitId)) {
                    Label(vm.tr("Vokabeltraining"), systemImage: "character.book.closed")
                }
            }
        }
        .navigationTitle(u.str("title"))
        .navigationSubtitle(vm.tr(u.str("sub")))
        .appListStyle()
    }
    #endif
}

struct NetYearsScreen: View {
    @ObservedObject var vm: AppModel

    var body: some View {
        #if os(macOS)
        let keys = ["net_year1", "net_year2", "net_year3", "net_year4"]
        MacFillGrid(count: keys.count, minWidth: 260, minHeight: 180) { i in
            if i == 0 {
                NavigationLink(value: Route.netMap) {
                    MacLinkCard(title: vm.tr(keys[i]), subtitle: vm.tr("net_sub"), badge: "\(i + 1).")
                }
                .buttonStyle(.plain)
            } else {
                MacLinkCard(title: vm.tr(keys[i]), subtitle: vm.tr("net_year_locked_sub"), badge: "\(i + 1).", locked: true)
            }
        }
        .navigationTitle(vm.tr("net_years_title"))
        .navigationSubtitle(vm.tr("net_years_sub"))
        #else
        List {
            NavigationLink(value: Route.netMap) {
                ListRowLabel(title: vm.tr("net_year1"), subtitle: vm.tr("net_sub"))
            }
            ForEach(["net_year2", "net_year3", "net_year4"], id: \.self) { key in
                Label {
                    ListRowLabel(title: vm.tr(key), subtitle: vm.tr("net_year_locked_sub"))
                } icon: {
                    Image(systemName: "lock.fill")
                }
                .foregroundStyle(.secondary)
            }
        }
        .navigationTitle(vm.tr("net_years_title"))
        .navigationSubtitle(vm.tr("net_years_sub"))
        .appListStyle()
        #endif
    }
}

struct HwYearsScreen: View {
    @ObservedObject var vm: AppModel

    var body: some View {
        #if os(macOS)
        let keys = ["hw_year1", "hw_year2", "hw_year3", "hw_year4"]
        MacFillGrid(count: keys.count, minWidth: 260, minHeight: 180) { i in
            if i == 0 {
                NavigationLink(value: Route.hwMap) {
                    MacLinkCard(title: vm.tr(keys[i]), subtitle: vm.tr("hw_sub"), badge: "\(i + 1).")
                }
                .buttonStyle(.plain)
            } else {
                MacLinkCard(title: vm.tr(keys[i]), subtitle: vm.tr("hw_year_locked_sub"), badge: "\(i + 1).", locked: true)
            }
        }
        .navigationTitle(vm.tr("Technické vybavení"))
        .navigationSubtitle(vm.tr("hw_years_sub"))
        #else
        List {
            NavigationLink(value: Route.hwMap) {
                ListRowLabel(title: vm.tr("hw_year1"), subtitle: vm.tr("hw_sub"))
            }
            ForEach(["hw_year2", "hw_year3", "hw_year4"], id: \.self) { key in
                Label {
                    ListRowLabel(title: vm.tr(key), subtitle: vm.tr("hw_year_locked_sub"))
                } icon: {
                    Image(systemName: "lock.fill")
                }
                .foregroundStyle(.secondary)
            }
        }
        .navigationTitle(vm.tr("Technické vybavení"))
        .navigationSubtitle(vm.tr("hw_years_sub"))
        .appListStyle()
        #endif
    }
}

struct OnYearsScreen: View {
    @ObservedObject var vm: AppModel

    var body: some View {
        #if os(macOS)
        let years: [(String, String, Route)] = [
            ("on_year1", "on_sub", .onMap),
            ("on_year2", "on2_sub", .on2Map),
            ("on_year3", "on3_sub", .on3Map),
            ("on_year4", "on4_sub", .on4Map),
        ]
        MacFillGrid(count: years.count, minWidth: 260, minHeight: 180) { i in
            NavigationLink(value: years[i].2) {
                MacLinkCard(title: vm.tr(years[i].0), subtitle: vm.tr(years[i].1), badge: "\(i + 1).")
            }
            .buttonStyle(.plain)
        }
        .navigationTitle(vm.tr("Občanská nauka"))
        .navigationSubtitle(vm.tr("on_years_sub"))
        #else
        List {
            NavigationLink(value: Route.onMap) {
                ListRowLabel(title: vm.tr("on_year1"), subtitle: vm.tr("on_sub"))
            }
            NavigationLink(value: Route.on2Map) {
                ListRowLabel(title: vm.tr("on_year2"), subtitle: vm.tr("on2_sub"))
            }
            NavigationLink(value: Route.on3Map) {
                ListRowLabel(title: vm.tr("on_year3"), subtitle: vm.tr("on3_sub"))
            }
            NavigationLink(value: Route.on4Map) {
                ListRowLabel(title: vm.tr("on_year4"), subtitle: vm.tr("on4_sub"))
            }
        }
        .navigationTitle(vm.tr("Občanská nauka"))
        .navigationSubtitle(vm.tr("on_years_sub"))
        .appListStyle()
        #endif
    }
}

struct LessonRow: Identifiable {
    let id: String
    let title: String
    let done: Bool
    let destination: Route
}

struct LessonListScreen: View {
    @ObservedObject var vm: AppModel
    let title: String
    let subtitle: String
    let rows: [LessonRow]

    var body: some View {
        #if os(macOS)
        lessonPath
        #else
        lessonList
        #endif
    }

    #if os(macOS)
    private var lessonPath: some View {
        let next = rows.firstIndex { !$0.done }
        return PathMap(
            nodes: rows.enumerated().map { i, row in
                MapNode(
                    id: row.id,
                    label: row.title,
                    locked: false,
                    done: row.done,
                    current: i == next,
                    destination: row.destination
                )
            },
            palette: vm.palette,
            litUntil: CGFloat(next ?? rows.count),
            nodeSize: MacChrome.pathNode,
            mx: 98,
            my: 134,
            spac: 186,
            gap: 272,
            wave: 16
        )
        .padding(28)
        .navigationTitle(title)
        .navigationSubtitle(subtitle)
    }
    #endif

    #if os(iOS)
    private var lessonList: some View {
        List(rows) { row in
            NavigationLink(value: row.destination) {
                Label {
                    Text(row.title)
                } icon: {
                    Image(systemName: row.done ? "checkmark.circle.fill" : "circle")
                }
            }
        }
        .navigationTitle(title)
        .navigationSubtitle(subtitle)
        .appListStyle()
    }
    #endif
}

struct SlidesScreen: View {
    @ObservedObject var vm: AppModel
    let title: String
    let subtitle: String
    let slides: [J]
    let next: Route
    @State private var idx = 0

    var body: some View {
        Group {
            #if os(iOS)
            TabView(selection: $idx) {
                ForEach(slides.indices, id: \.self) { i in
                    slidePage(slides[i]).tag(i)
                }
            }
            .tabViewStyle(.page(indexDisplayMode: .never))
            #else
            if slides.indices.contains(idx) {
                slidePage(slides[idx])
            }
            #endif
        }
        .detailScreen(title, subtitle: subtitle)
        .glassBottomBar {
            VStack(spacing: 10) {
                HStack(spacing: 6) {
                    ForEach(slides.indices, id: \.self) { i in
                        Capsule()
                            .fill(i == idx ? Color.accentColor : Color(.tertiarySystemFill))
                            .frame(width: i == idx ? 18 : 6, height: 6)
                    }
                }
                .animation(.default, value: idx)
                HStack(spacing: 10) {
                    if idx > 0 {
                        Button {
                            withAnimation { idx -= 1 }
                        } label: {
                            Image(systemName: "chevron.left")
                                .frame(minWidth: 24)
                        }
                        #if os(macOS)
                        .buttonStyle(.bordered)
                        #else
                        .buttonStyle(.glass)
                        #endif
                        .controlSize(.large)
                    }
                    if idx < slides.count - 1 {
                        PrimaryButton(label: vm.tr("net_slide_next")) {
                            withAnimation { idx += 1 }
                        }
                    } else {
                        PrimaryButton(label: vm.tr("net_slide_start")) {
                            vm.go(next)
                        }
                    }
                }
            }
            .padding(.horizontal)
            .padding(.top, 8)
            .padding(.bottom, 4)
        }
    }

    private func slidePage(_ slide: J) -> some View {
        #if os(macOS)
        GeometryReader { geo in
            ScrollView {
                slideStack(slide)
                    .padding(.horizontal, MacChrome.pageInset)
                    .padding(.vertical, 28)
                    .frame(maxWidth: .infinity, minHeight: geo.size.height, alignment: .center)
            }
        }
        #else
        ScrollView {
            slideStack(slide)
                .padding()
                .padding(.bottom, 24)
                .frame(maxWidth: .infinity, alignment: .leading)
        }
        #endif
    }

    private func slideStack(_ slide: J) -> some View {
        VStack(alignment: .leading, spacing: 16) {
            Text(slide.str("kicker"))
                #if os(macOS)
                .font(.subheadline.weight(.semibold))
                #else
                .font(.caption.weight(.semibold))
                #endif
                .foregroundStyle(.secondary)
            Text(slide.str("title"))
                #if os(macOS)
                .font(.title.bold())
                #else
                .font(.title2.bold())
                #endif
            if let tip = slide.strOrNull("tip") {
                GlassCard {
                    Label(tip, systemImage: "lightbulb.fill")
                }
            }
            slideLines(slide.strs("lines"))
        }
        .frame(maxWidth: .infinity, alignment: .leading)
    }

    @ViewBuilder
    private func slideLines(_ lines: [String]) -> some View {
        #if os(macOS)
        if lines.count >= 3 {
            LazyVGrid(
                columns: [
                    GridItem(.flexible(), spacing: 28, alignment: .topLeading),
                    GridItem(.flexible(), spacing: 28, alignment: .topLeading),
                ],
                alignment: .leading,
                spacing: 12
            ) {
                ForEach(Array(lines.enumerated()), id: \.offset) { _, line in
                    Text(line)
                        .font(.title3)
                        .fixedSize(horizontal: false, vertical: true)
                        .frame(maxWidth: .infinity, alignment: .leading)
                }
            }
        } else {
            VStack(alignment: .leading, spacing: 12) {
                ForEach(Array(lines.enumerated()), id: \.offset) { _, line in
                    Text(line)
                        .font(.title3)
                        .fixedSize(horizontal: false, vertical: true)
                }
            }
        }
        #else
        ForEach(Array(lines.enumerated()), id: \.offset) { _, line in
            Text(line)
                .padding(.vertical, 4)
        }
        #endif
    }
}

struct LitYearsScreen: View {
    @ObservedObject var vm: AppModel

    var body: some View {
        #if os(macOS)
        let years: [(String, String, Route)] = [
            ("lit_year1", "lit_sub", .litMap),
            ("lit_year2", "lit2_sub", .lit2Map),
            ("lit_year3", "lit3_sub", .lit3Map),
            ("lit_year4", "lit4_sub", .lit4Map),
        ]
        MacFillGrid(count: years.count, minWidth: 260, minHeight: 180) { i in
            NavigationLink(value: years[i].2) {
                MacLinkCard(title: vm.tr(years[i].0), subtitle: vm.tr(years[i].1), badge: "\(i + 1).")
            }
            .buttonStyle(.plain)
        }
        .navigationTitle(vm.tr("Literatura"))
        .navigationSubtitle(vm.tr("lit_years_sub"))
        #else
        List {
            NavigationLink(value: Route.litMap) {
                ListRowLabel(title: vm.tr("lit_year1"), subtitle: vm.tr("lit_sub"))
            }
            NavigationLink(value: Route.lit2Map) {
                ListRowLabel(title: vm.tr("lit_year2"), subtitle: vm.tr("lit2_sub"))
            }
            NavigationLink(value: Route.lit3Map) {
                ListRowLabel(title: vm.tr("lit_year3"), subtitle: vm.tr("lit3_sub"))
            }
            NavigationLink(value: Route.lit4Map) {
                ListRowLabel(title: vm.tr("lit_year4"), subtitle: vm.tr("lit4_sub"))
            }
        }
        .navigationTitle(vm.tr("Literatura"))
        .navigationSubtitle(vm.tr("lit_years_sub"))
        .appListStyle()
        #endif
    }
}

struct SciMapScreen: View {
    @ObservedObject var vm: AppModel

    var body: some View {
        #if os(macOS)
        MacFillGrid(count: 2, minWidth: 260, minHeight: 180) { i in
            if i == 0 {
                NavigationLink(value: Route.chemMap) {
                    MacLinkCard(title: vm.tr("Chemie"), subtitle: vm.tr("chem_sub"), systemImage: "flask.fill")
                }
                .buttonStyle(.plain)
            } else {
                NavigationLink(value: Route.bioMap) {
                    MacLinkCard(title: vm.tr("Biologie"), subtitle: vm.tr("bio_sub"), systemImage: "leaf.fill")
                }
                .buttonStyle(.plain)
            }
        }
        .navigationTitle(vm.tr("Základy Přírodopisných věd"))
        .navigationSubtitle(vm.tr("sci_sub"))
        #else
        List {
            NavigationLink(value: Route.chemMap) {
                Label {
                    ListRowLabel(title: vm.tr("Chemie"), subtitle: vm.tr("chem_sub"))
                } icon: {
                    Image(systemName: "flask.fill")
                }
            }
            NavigationLink(value: Route.bioMap) {
                Label {
                    ListRowLabel(title: vm.tr("Biologie"), subtitle: vm.tr("bio_sub"))
                } icon: {
                    Image(systemName: "leaf.fill")
                }
            }
        }
        .navigationTitle(vm.tr("Základy Přírodopisných věd"))
        .navigationSubtitle(vm.tr("sci_sub"))
        .appListStyle()
        #endif
    }
}

struct CzechMapScreen: View {
    @ObservedObject var vm: AppModel

    var body: some View {
        #if os(macOS)
        MacFillGrid(count: 3, minWidth: 260, minHeight: 180) { i in
            switch i {
            case 1:
                NavigationLink(value: Route.mluvnice) {
                    MacLinkCard(title: vm.tr("Mluvnice"), systemImage: "textformat")
                }
                .buttonStyle(.plain)
            case 2:
                NavigationLink(value: Route.readingList) {
                    MacLinkCard(title: vm.tr("Maturitní četba"), systemImage: "books.vertical")
                }
                .buttonStyle(.plain)
            default:
                NavigationLink(value: Route.litYears) {
                    MacLinkCard(title: vm.tr("Literatura"), systemImage: "book.closed")
                }
                .buttonStyle(.plain)
            }
        }
        .navigationTitle(vm.tr("Český jazyk a literatura"))
        .navigationSubtitle(vm.tr("czech_sub"))
        #else
        List {
            NavigationLink(value: Route.litYears) {
                Label {
                    Text(vm.tr("Literatura"))
                } icon: {
                    Image(systemName: "book.closed")
                }
            }
            NavigationLink(value: Route.mluvnice) {
                Label {
                    Text(vm.tr("Mluvnice"))
                } icon: {
                    Image(systemName: "textformat")
                }
            }
            NavigationLink(value: Route.readingList) {
                Label {
                    Text(vm.tr("Maturitní četba"))
                } icon: {
                    Image(systemName: "books.vertical")
                }
            }
        }
        .navigationTitle(vm.tr("Český jazyk a literatura"))
        .navigationSubtitle(vm.tr("czech_sub"))
        .appListStyle()
        #endif
    }
}

struct BookListScreen: View {
    @ObservedObject var vm: AppModel

    var body: some View {
        #if os(macOS)
        macBooks
        #else
        iosBooks
        #endif
    }

    #if os(macOS)
    private var macBooks: some View {
        ScrollView {
            LazyVGrid(columns: [GridItem(.adaptive(minimum: 300), spacing: 16)], spacing: 16) {
                ForEach(Array(vm.content.books.enumerated()), id: \.offset) { _, b in
                    let parts = bookTitleParts(b.str("title"))
                    let quizDone = !b.arr("quiz").isEmpty && vm.progress.bookQuiz(b.str("id"))
                    let plotDone = !b.arr("plot").isEmpty && vm.progress.bookPlot(b.str("id"))
                    NavigationLink(value: Route.book(b.str("id"))) {
                        VStack(alignment: .leading, spacing: 8) {
                            Image(systemName: "book.fill")
                                .font(.title2)
                                .foregroundStyle(.tint)
                            Text(parts.title)
                                .font(.title3.weight(.semibold))
                                .foregroundStyle(.primary)
                                .multilineTextAlignment(.leading)
                            if let author = parts.author {
                                Text(author)
                                    .font(.body)
                                    .foregroundStyle(.secondary)
                            }
                            Spacer(minLength: 0)
                            HStack(spacing: 8) {
                                let genre = b.str("genre")
                                Text(genre.isEmpty ? vm.tr(b.str("subKey")) : genre)
                                    .font(.subheadline)
                                    .foregroundStyle(.secondary)
                                Spacer(minLength: 0)
                                if quizDone || plotDone {
                                    Image(systemName: "checkmark.circle.fill")
                                        .foregroundStyle(.green)
                                        .accessibilityLabel(vm.tr("book_done"))
                                }
                            }
                        }
                        .padding(18)
                        .frame(maxWidth: .infinity, minHeight: 168, alignment: .topLeading)
                        .softSurface(cornerRadius: 18)
                    }
                    .buttonStyle(.plain)
                }
            }
            .padding(MacChrome.pageInset)
        }
        .navigationTitle(vm.tr("Maturitní četba"))
        .navigationSubtitle(vm.tr("reading_sub"))
    }
    #endif

    #if os(iOS)
    private var iosBooks: some View {
        List {
            ForEach(Array(vm.content.books.enumerated()), id: \.offset) { _, b in
                let parts = bookTitleParts(b.str("title"))
                let quizDone = !b.arr("quiz").isEmpty && vm.progress.bookQuiz(b.str("id"))
                let plotDone = !b.arr("plot").isEmpty && vm.progress.bookPlot(b.str("id"))
                NavigationLink(value: Route.book(b.str("id"))) {
                    HStack(alignment: .top, spacing: 12) {
                        Image(systemName: "book.fill")
                            #if os(macOS)
                            .font(.title2)
                            #else
                            .font(.title3)
                            #endif
                            .foregroundStyle(.tint)
                            .frame(width: 28)
                        VStack(alignment: .leading, spacing: 4) {
                            Text(parts.title)
                                #if os(macOS)
                                .font(.title3.weight(.semibold))
                                #else
                                .font(.body.weight(.semibold))
                                #endif
                            if let author = parts.author {
                                Text(author)
                                    #if os(macOS)
                                    .font(.body)
                                    #else
                                    .font(.subheadline)
                                    #endif
                                    .foregroundStyle(.secondary)
                            }
                            HStack(spacing: 8) {
                                let genre = b.str("genre")
                                Text(genre.isEmpty ? vm.tr(b.str("subKey")) : genre)
                                    #if os(macOS)
                                    .font(.subheadline)
                                    #else
                                    .font(.caption)
                                    #endif
                                    .foregroundStyle(.secondary)
                                if quizDone || plotDone {
                                    Label(vm.tr("book_done"), systemImage: "checkmark.circle.fill")
                                        #if os(macOS)
                                        .font(.subheadline.weight(.semibold))
                                        #else
                                        .font(.caption.weight(.semibold))
                                        #endif
                                        .foregroundStyle(.green)
                                }
                            }
                        }
                    }
                    #if os(macOS)
                    .padding(.vertical, 4)
                    #endif
                }
            }
        }
        .navigationTitle(vm.tr("Maturitní četba"))
        .navigationSubtitle(vm.tr("reading_sub"))
        .appListStyle()
    }
    #endif
}

struct BookScreen: View {
    @ObservedObject var vm: AppModel
    let id: String

    var body: some View {
        if let b = vm.content.book(id) {
            let hasQuiz = !b.arr("quiz").isEmpty
            let hasPlot = !b.arr("plot").isEmpty
            let notes = b.arr("notes")
            let quizDone = vm.progress.bookQuiz(id)
            let plotDone = vm.progress.bookPlot(id)
            let parts = bookTitleParts(b.str("title"))
            ScrollView {
                VStack(alignment: .leading, spacing: 28) {
                    GlassCard {
                        VStack(alignment: .leading, spacing: 10) {
                            if let author = parts.author {
                                Text(author)
                                    #if os(macOS)
                                    .font(.title3)
                                    #else
                                    .font(.subheadline)
                                    #endif
                                    .foregroundStyle(.secondary)
                            }
                            if !b.str("genre").isEmpty {
                                Text(b.str("genre"))
                                    .font(.subheadline.weight(.semibold))
                                    .foregroundStyle(.tint)
                            }
                            Text(vm.tr("book_study_hint"))
                                #if os(macOS)
                                .font(.title3)
                                #else
                                .font(.body)
                                #endif
                                .foregroundStyle(.secondary)
                                .fixedSize(horizontal: false, vertical: true)
                        }
                    }

                    if hasQuiz || hasPlot {
                        VStack(alignment: .leading, spacing: 14) {
                            Text(vm.tr("book_practice"))
                                #if os(macOS)
                                .font(.title.weight(.semibold))
                                #else
                                .font(.title2.weight(.semibold))
                                #endif
                            #if os(macOS)
                            LazyVGrid(columns: [GridItem(.adaptive(minimum: 320), spacing: 16)], spacing: 16) {
                                if hasQuiz {
                                    NavigationLink(value: Route.bookQuiz(id)) {
                                        BookActionCard(
                                            title: vm.tr("book_quiz_heading"),
                                            subtitle: vm.tr("lit_quiz_sub"),
                                            systemImage: "questionmark.circle.fill",
                                            done: quizDone,
                                            doneLabel: vm.tr("book_done")
                                        )
                                    }
                                    .buttonStyle(.plain)
                                }
                                if hasPlot {
                                    NavigationLink(value: Route.bookPlot(id)) {
                                        BookActionCard(
                                            title: vm.tr("lit_plot_title"),
                                            subtitle: vm.tr("lit_plot_sub"),
                                            systemImage: "list.number",
                                            done: plotDone,
                                            doneLabel: vm.tr("book_done")
                                        )
                                    }
                                    .buttonStyle(.plain)
                                }
                            }
                            #else
                            if hasQuiz {
                                NavigationLink(value: Route.bookQuiz(id)) {
                                    BookActionCard(
                                        title: vm.tr("book_quiz_heading"),
                                        subtitle: vm.tr("lit_quiz_sub"),
                                        systemImage: "questionmark.circle.fill",
                                        done: quizDone,
                                        doneLabel: vm.tr("book_done")
                                    )
                                }
                                .buttonStyle(.plain)
                            }
                            if hasPlot {
                                NavigationLink(value: Route.bookPlot(id)) {
                                    BookActionCard(
                                        title: vm.tr("lit_plot_title"),
                                        subtitle: vm.tr("lit_plot_sub"),
                                        systemImage: "list.number",
                                        done: plotDone,
                                        doneLabel: vm.tr("book_done")
                                    )
                                }
                                .buttonStyle(.plain)
                            }
                            #endif
                        }
                    }

                    if !notes.isEmpty {
                        VStack(alignment: .leading, spacing: 14) {
                            Text(vm.tr("book_notes"))
                                #if os(macOS)
                                .font(.title.weight(.semibold))
                                #else
                                .font(.title2.weight(.semibold))
                                #endif
                            #if os(macOS)
                            LazyVGrid(columns: [GridItem(.adaptive(minimum: 360), spacing: 16)], alignment: .leading, spacing: 16) {
                                ForEach(Array(notes.enumerated()), id: \.offset) { _, note in
                                    BookNoteCard(note: note)
                                }
                            }
                            #else
                            ForEach(Array(notes.enumerated()), id: \.offset) { _, note in
                                BookNoteCard(note: note)
                            }
                            #endif
                        }
                    }
                }
                #if os(macOS)
                .padding(MacChrome.pageInset)
                .frame(maxWidth: .infinity, alignment: .leading)
                #else
                .padding()
                .padding(.bottom, 24)
                #endif
            }
            .navigationTitle(parts.title)
            .navigationSubtitle(parts.author ?? vm.tr(b.str("subKey")))
        }
    }
}

private struct BookActionCard: View {
    let title: String
    let subtitle: String
    let systemImage: String
    let done: Bool
    let doneLabel: String

    var body: some View {
        HStack(alignment: .center, spacing: 16) {
            Image(systemName: systemImage)
                #if os(macOS)
                .font(.largeTitle)
                #else
                .font(.title)
                #endif
                .foregroundStyle(.tint)
                .frame(width: 44)
            VStack(alignment: .leading, spacing: 4) {
                Text(title)
                    #if os(macOS)
                    .font(.title3.weight(.semibold))
                    #else
                    .font(.headline)
                    #endif
                    .foregroundStyle(.primary)
                Text(subtitle)
                    #if os(macOS)
                    .font(.body)
                    #else
                    .font(.subheadline)
                    #endif
                    .foregroundStyle(.secondary)
                    .fixedSize(horizontal: false, vertical: true)
            }
            Spacer(minLength: 8)
            if done {
                Image(systemName: "checkmark.circle.fill")
                    #if os(macOS)
                    .font(.title2)
                    #else
                    .font(.title3)
                    #endif
                    .foregroundStyle(.green)
                    .accessibilityLabel(doneLabel)
            } else {
                Image(systemName: "chevron.right")
                    .font(.body.weight(.semibold))
                    .foregroundStyle(.tertiary)
            }
        }
        #if os(macOS)
        .padding(20)
        #else
        .padding(16)
        #endif
        .frame(maxWidth: .infinity, maxHeight: .infinity, alignment: .leading)
        .softSurface(cornerRadius: 18)
    }
}

private struct BookNoteCard: View {
    let note: J

    var body: some View {
        GlassCard {
            VStack(alignment: .leading, spacing: 12) {
                Label(note.str("title"), systemImage: noteSymbol(note.str("icon")))
                    #if os(macOS)
                    .font(.title2.weight(.semibold))
                    #else
                    .font(.headline)
                    #endif
                VStack(alignment: .leading, spacing: 10) {
                    ForEach(Array(note.strs("lines").enumerated()), id: \.offset) { _, line in
                        HStack(alignment: .top, spacing: 10) {
                            Text("•")
                                .foregroundStyle(.secondary)
                            Text(line)
                                #if os(macOS)
                                .font(.title3)
                                #else
                                .font(.body)
                                #endif
                                .fixedSize(horizontal: false, vertical: true)
                        }
                    }
                }
            }
        }
    }
}

private func bookTitleParts(_ title: String) -> (author: String?, title: String) {
    let separators = [" – ", " — ", " - "]
    for sep in separators {
        if let range = title.range(of: sep) {
            let author = String(title[..<range.lowerBound]).trimmingCharacters(in: .whitespaces)
            let name = String(title[range.upperBound...]).trimmingCharacters(in: .whitespaces)
            if !author.isEmpty && !name.isEmpty {
                return (author, name)
            }
        }
    }
    return (nil, title)
}

private func noteSymbol(_ icon: String) -> String {
    switch icon {
    case "book": return "book.fill"
    case "globe": return "globe.europe.africa.fill"
    case "people": return "person.2.fill"
    case "bulb": return "lightbulb.fill"
    default: return "text.alignleft"
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
                vm: vm,
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
        case .hwYears:
            HwYearsScreen(vm: vm)
        case .hwMap:
            LessonListScreen(
                vm: vm,
                title: vm.tr("hw_year1"),
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
        case .onYears:
            OnYearsScreen(vm: vm)
        case .onMap:
            LessonListScreen(
                vm: vm,
                title: vm.tr("on_year1"),
                subtitle: vm.tr("on_sub"),
                rows: vm.content.on.map { l in
                    let id = l.int("id")
                    return LessonRow(
                        id: "o\(id)",
                        title: vm.tr(l.str("titleKey")),
                        done: vm.progress.onDone(id),
                        destination: .onLesson(id)
                    )
                }
            )
        case .onLesson(let id):
            if let l = vm.content.onLesson(id) {
                SlidesScreen(vm: vm, title: vm.tr(l.str("titleKey")), subtitle: vm.tr(l.str("subKey")), slides: l.arr("slides"), next: .onEx(id))
            }
        case .onEx(let id):
            OnQuizScreen(vm: vm, id: id)
        case .on2Map:
            LessonListScreen(
                vm: vm,
                title: vm.tr("on_year2"),
                subtitle: vm.tr("on2_sub"),
                rows: vm.content.on2.map { l in
                    let id = l.int("id")
                    return LessonRow(
                        id: "o2\(id)",
                        title: vm.tr(l.str("titleKey")),
                        done: vm.progress.on2Done(id),
                        destination: .on2Lesson(id)
                    )
                }
            )
        case .on2Lesson(let id):
            if let l = vm.content.on2Lesson(id) {
                SlidesScreen(vm: vm, title: vm.tr(l.str("titleKey")), subtitle: vm.tr(l.str("subKey")), slides: l.arr("slides"), next: .on2Ex(id))
            }
        case .on2Ex(let id):
            On2QuizScreen(vm: vm, id: id)
        case .on3Map:
            LessonListScreen(
                vm: vm,
                title: vm.tr("on_year3"),
                subtitle: vm.tr("on3_sub"),
                rows: vm.content.on3.map { l in
                    let id = l.int("id")
                    return LessonRow(
                        id: "o3\(id)",
                        title: vm.tr(l.str("titleKey")),
                        done: vm.progress.on3Done(id),
                        destination: .on3Lesson(id)
                    )
                }
            )
        case .on3Lesson(let id):
            if let l = vm.content.on3Lesson(id) {
                SlidesScreen(vm: vm, title: vm.tr(l.str("titleKey")), subtitle: vm.tr(l.str("subKey")), slides: l.arr("slides"), next: .on3Ex(id))
            }
        case .on3Ex(let id):
            On3QuizScreen(vm: vm, id: id)
        case .on4Map:
            LessonListScreen(
                vm: vm,
                title: vm.tr("on_year4"),
                subtitle: vm.tr("on4_sub"),
                rows: vm.content.on4.map { l in
                    let id = l.int("id")
                    return LessonRow(
                        id: "o4\(id)",
                        title: vm.tr(l.str("titleKey")),
                        done: vm.progress.on4Done(id),
                        destination: .on4Lesson(id)
                    )
                }
            )
        case .on4Lesson(let id):
            if let l = vm.content.on4Lesson(id) {
                SlidesScreen(vm: vm, title: vm.tr(l.str("titleKey")), subtitle: vm.tr(l.str("subKey")), slides: l.arr("slides"), next: .on4Ex(id))
            }
        case .on4Ex(let id):
            On4QuizScreen(vm: vm, id: id)
        case .czechMap:
            CzechMapScreen(vm: vm)
        case .litYears:
            LitYearsScreen(vm: vm)
        case .litMap:
            LessonListScreen(
                vm: vm,
                title: vm.tr("lit_year1"),
                subtitle: vm.tr("lit_sub"),
                rows: vm.content.lit.map { l in
                    let id = l.int("id")
                    return LessonRow(
                        id: "l\(id)",
                        title: vm.tr(l.str("titleKey")),
                        done: vm.progress.litDone(id),
                        destination: .litLesson(id)
                    )
                }
            )
        case .litLesson(let id):
            if let l = vm.content.litLesson(id) {
                SlidesScreen(vm: vm, title: vm.tr(l.str("titleKey")), subtitle: vm.tr(l.str("subKey")), slides: l.arr("slides"), next: .litEx(id))
            }
        case .litEx(let id):
            LitYearQuizScreen(vm: vm, year: 1, id: id)
        case .lit2Map:
            LessonListScreen(
                vm: vm,
                title: vm.tr("lit_year2"),
                subtitle: vm.tr("lit2_sub"),
                rows: vm.content.lit2.map { l in
                    let id = l.int("id")
                    return LessonRow(
                        id: "l2\(id)",
                        title: vm.tr(l.str("titleKey")),
                        done: vm.progress.lit2Done(id),
                        destination: .lit2Lesson(id)
                    )
                }
            )
        case .lit2Lesson(let id):
            if let l = vm.content.lit2Lesson(id) {
                SlidesScreen(vm: vm, title: vm.tr(l.str("titleKey")), subtitle: vm.tr(l.str("subKey")), slides: l.arr("slides"), next: .lit2Ex(id))
            }
        case .lit2Ex(let id):
            LitYearQuizScreen(vm: vm, year: 2, id: id)
        case .lit3Map:
            LessonListScreen(
                vm: vm,
                title: vm.tr("lit_year3"),
                subtitle: vm.tr("lit3_sub"),
                rows: vm.content.lit3.map { l in
                    let id = l.int("id")
                    return LessonRow(
                        id: "l3\(id)",
                        title: vm.tr(l.str("titleKey")),
                        done: vm.progress.lit3Done(id),
                        destination: .lit3Lesson(id)
                    )
                }
            )
        case .lit3Lesson(let id):
            if let l = vm.content.lit3Lesson(id) {
                SlidesScreen(vm: vm, title: vm.tr(l.str("titleKey")), subtitle: vm.tr(l.str("subKey")), slides: l.arr("slides"), next: .lit3Ex(id))
            }
        case .lit3Ex(let id):
            LitYearQuizScreen(vm: vm, year: 3, id: id)
        case .lit4Map:
            LessonListScreen(
                vm: vm,
                title: vm.tr("lit_year4"),
                subtitle: vm.tr("lit4_sub"),
                rows: vm.content.lit4.map { l in
                    let id = l.int("id")
                    return LessonRow(
                        id: "l4\(id)",
                        title: vm.tr(l.str("titleKey")),
                        done: vm.progress.lit4Done(id),
                        destination: .lit4Lesson(id)
                    )
                }
            )
        case .lit4Lesson(let id):
            if let l = vm.content.lit4Lesson(id) {
                SlidesScreen(vm: vm, title: vm.tr(l.str("titleKey")), subtitle: vm.tr(l.str("subKey")), slides: l.arr("slides"), next: .lit4Ex(id))
            }
        case .lit4Ex(let id):
            LitYearQuizScreen(vm: vm, year: 4, id: id)
        case .sciMap:
            SciMapScreen(vm: vm)
        case .chemMap:
            LessonListScreen(
                vm: vm,
                title: vm.tr("Chemie"),
                subtitle: vm.tr("chem_sub"),
                rows: vm.content.chem.map { l in
                    let id = l.int("id")
                    return LessonRow(
                        id: "c\(id)",
                        title: vm.tr(l.str("titleKey")),
                        done: vm.progress.chemDone(id),
                        destination: .chemLesson(id)
                    )
                }
            )
        case .chemLesson(let id):
            if let l = vm.content.chemLesson(id) {
                SlidesScreen(vm: vm, title: vm.tr(l.str("titleKey")), subtitle: vm.tr(l.str("subKey")), slides: l.arr("slides"), next: .chemEx(id))
            }
        case .chemEx(let id):
            SciQuizScreen(vm: vm, area: "chem", id: id)
        case .bioMap:
            LessonListScreen(
                vm: vm,
                title: vm.tr("Biologie"),
                subtitle: vm.tr("bio_sub"),
                rows: vm.content.bio.map { l in
                    let id = l.int("id")
                    return LessonRow(
                        id: "b\(id)",
                        title: vm.tr(l.str("titleKey")),
                        done: vm.progress.bioDone(id),
                        destination: .bioLesson(id)
                    )
                }
            )
        case .bioLesson(let id):
            if let l = vm.content.bioLesson(id) {
                SlidesScreen(vm: vm, title: vm.tr(l.str("titleKey")), subtitle: vm.tr(l.str("subKey")), slides: l.arr("slides"), next: .bioEx(id))
            }
        case .bioEx(let id):
            SciQuizScreen(vm: vm, area: "bio", id: id)
        case .mluvnice:
            LessonListScreen(
                vm: vm,
                title: vm.tr("Mluvnice"),
                subtitle: vm.tr("mluv_sub"),
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
