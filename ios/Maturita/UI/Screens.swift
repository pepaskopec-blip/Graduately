import SwiftUI

struct PracticeHome: View {
    @ObservedObject var vm: AppModel

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
        return ScrollView {
            VStack(alignment: .leading, spacing: 28) {
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
                VStack(alignment: .leading, spacing: 16) {
                    Text(vm.tr("subjects_title"))
                        .font(.title2.weight(.semibold))
                    LazyVGrid(columns: [GridItem(.adaptive(minimum: MacChrome.homeTileMin), spacing: 18)], spacing: 18) {
                        ForEach(Array(vm.content.subjects.enumerated()), id: \.offset) { _, s in
                            subjectTile(s)
                        }
                    }
                }
            }
            .padding(32)
            .frame(maxWidth: MacChrome.contentMaxWidth, alignment: .leading)
            .frame(maxWidth: .infinity, alignment: .leading)
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
        .frame(maxWidth: .infinity, minHeight: MacChrome.homeTileHeight, alignment: .topLeading)
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
            ForEach(Array(lines.enumerated()), id: \.offset) { _, line in
                Text("•  \(line)")
                    .font(.subheadline)
            }
        }
    }
}

struct SettingsScreen: View {
    @ObservedObject var vm: AppModel

    var body: some View {
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
        let sum = summarize(vm.content, vm.progress)
        let pct = sum.totalEx == 0 ? 0.0 : Double(sum.doneEx) / Double(sum.totalEx)
        List {
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
    case "hwmap":
        sum.totalEx = vm.content.hw.count
        sum.doneEx = vm.content.hw.filter { vm.progress.hwDone($0.int("id")) }.count
    case "czechmap":
        sum.totalEx = vm.content.mluvnice.count
        sum.doneEx = vm.content.mluvnice.filter { vm.progress.mluvDone($0.int("id")) }.count
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
        return List {
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
        #if os(iOS)
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
        #else
        .searchable(text: $vm.searchQuery, prompt: vm.tr("search_placeholder"))
        .onAppear { query = vm.searchQuery }
        .onChange(of: vm.searchQuery) { _, q in query = q }
        #endif
        .appListStyle()
    }

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
    for l in vm.content.hw {
        add(vm.tr(l.str("titleKey")), vm.tr("search_lesson"), "hwunit\(l.int("id"))", extra: "hardware")
        add(vm.tr(l.str("titleKey")), vm.tr("search_exercise"), "hwex\(l.int("id"))")
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
        ScrollView {
            VStack(alignment: .leading, spacing: 14) {
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
                ForEach(Array(slide.strs("lines").enumerated()), id: \.offset) { _, line in
                    Text(line)
                        #if os(macOS)
                        .font(.title3)
                        #endif
                        .padding(.vertical, 4)
                }
            }
            .padding()
            .padding(.bottom, 24)
            .frame(maxWidth: .infinity, alignment: .leading)
            #if os(macOS)
            .frame(maxWidth: MacChrome.exerciseMaxWidth, alignment: .leading)
            .padding(12)
            #endif
        }
    }
}

struct CzechMapScreen: View {
    @ObservedObject var vm: AppModel

    var body: some View {
        List {
            Label {
                Text(vm.tr("Literatura"))
                    #if os(macOS)
                    .font(.title3)
                    #endif
            } icon: {
                Image(systemName: "lock.fill")
            }
            .foregroundStyle(.secondary)
            NavigationLink(value: Route.mluvnice) {
                Label {
                    Text(vm.tr("Mluvnice"))
                        #if os(macOS)
                        .font(.title3)
                        #endif
                } icon: {
                    Image(systemName: "textformat")
                }
            }
            NavigationLink(value: Route.readingList) {
                Label {
                    Text(vm.tr("Maturitní četba"))
                        #if os(macOS)
                        .font(.title3)
                        #endif
                } icon: {
                    Image(systemName: "books.vertical")
                }
            }
        }
        .navigationTitle(vm.tr("Český jazyk a literatura"))
        .navigationSubtitle(vm.tr("czech_sub"))
        .appListStyle()
    }
}

struct BookListScreen: View {
    @ObservedObject var vm: AppModel

    var body: some View {
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
                    }

                    if !notes.isEmpty {
                        VStack(alignment: .leading, spacing: 14) {
                            Text(vm.tr("book_notes"))
                                #if os(macOS)
                                .font(.title.weight(.semibold))
                                #else
                                .font(.title2.weight(.semibold))
                                #endif
                            ForEach(Array(notes.enumerated()), id: \.offset) { _, note in
                                BookNoteCard(note: note)
                            }
                        }
                    }
                }
                #if os(macOS)
                .padding(28)
                .frame(maxWidth: MacChrome.exerciseMaxWidth, alignment: .leading)
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
        .frame(maxWidth: .infinity, alignment: .leading)
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
        case .hwMap:
            LessonListScreen(
                vm: vm,
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
