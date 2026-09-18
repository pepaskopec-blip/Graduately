import SwiftUI

struct PracticeHome: View {
    @ObservedObject var vm: AppModel

    var body: some View {
        let sum = summarize(vm.content, vm.progress)
        let pct = sum.totalEx == 0 ? 0 : Double(sum.doneEx) / Double(sum.totalEx)
        List {
            Section {
                GlassCard {
                    VStack(alignment: .leading, spacing: 10) {
                        Text(vm.tr("welcome_title"))
                            .font(.title.bold())
                        Text(vm.tr("welcome_body_ios"))
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
        .navigationTitle("Maturita")
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
            Image(systemName: subjectSymbol(s.str("icon")))
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
            }
            Section(vm.tr("theme")) {
                ForEach(Array(ThemeId.allCases), id: \.self) { id in
                    Button {
                        vm.setTheme(id)
                    } label: {
                        HStack {
                            Circle().fill(themePalette(id, vm.mode).accent).frame(width: 14, height: 14)
                            Text(themeNames[id.rawValue])
                            Spacer()
                            if vm.themeId == id {
                                Image(systemName: "checkmark")
                            }
                        }
                    }
                }
            }
            Section(vm.tr("language")) {
                Picker(vm.tr("language"), selection: langBinding) {
                    Text("Čeština").tag(UiLang.cs)
                    Text("English").tag(UiLang.en)
                }
                .pickerStyle(.segmented)
            }
            Section(vm.tr("updates")) {
                Text(updateText).foregroundStyle(.secondary)
                Button(vm.tr("update_check")) { vm.checkUpdate(true) }
                if vm.update.canInstall {
                    Button(vm.tr("update_install")) { vm.installUpdate() }
                }
                LabeledContent("Build", value: "\(AppConfig.versionName) · \(String(AppConfig.commit.prefix(7)))")
            }
        }
        .navigationTitle(vm.tr("settings_title"))
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
                HStack {
                    stat(vm.tr("stats_ex_label"), vm.fmt("stats_ex_fmt", sum.doneEx, sum.totalEx))
                    stat(vm.tr("stats_pct_label"), vm.fmt("stats_pct_fmt", Int(pct * 100)))
                    stat(vm.tr("stats_units_label"), vm.fmt("stats_units_fmt", sum.doneUnits, sum.openUnits))
                }
                .listRowBackground(Color.clear)
                ProgressView(value: pct)
            }
            Section(vm.tr("stats_section")) {
                ForEach(Array(vm.content.subjects.enumerated()), id: \.offset) { _, s in
                    let open = s.bool("open")
                    let part = subjectSum(vm, s)
                    VStack(alignment: .leading, spacing: 6) {
                        Text(vm.tr(s.str("key")))
                        Text(open ? vm.fmt("stats_ex_fmt", part.doneEx, part.totalEx) : vm.tr("stats_locked"))
                            .font(.caption)
                            .foregroundStyle(.secondary)
                        if open && part.totalEx > 0 {
                            ProgressView(value: Double(part.doneEx) / Double(part.totalEx))
                        }
                    }
                    .opacity(open ? 1 : 0.5)
                }
            }
        }
        .navigationTitle(vm.tr("stats_title"))
    }

    private func stat(_ label: String, _ value: String) -> some View {
        VStack(alignment: .leading) {
            Text(label).font(.caption).foregroundStyle(.secondary)
            Text(value).font(.title3.bold())
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

    var body: some View {
        let q = normalizeAnswer(vm.searchQuery)
        let hits = buildSearch(vm).filter { q.isEmpty || normalizeAnswer($0.hay).contains(q) }
        List {
            if hits.isEmpty {
                ContentUnavailableView.search
            } else {
                ForEach(hits) { hit in
                    Button {
                        if !hit.locked { vm.openFromSearch(hit.target) }
                    } label: {
                        VStack(alignment: .leading, spacing: 2) {
                            Text(hit.title)
                            Text(hit.sub).font(.caption).foregroundStyle(.secondary)
                        }
                    }
                    .disabled(hit.locked)
                    .foregroundStyle(hit.locked ? .secondary : .primary)
                }
            }
        }
        .navigationTitle(vm.tr("search_placeholder"))
        .searchable(text: $vm.searchQuery, prompt: vm.tr("search_placeholder"))
    }
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
    }
}

struct UnitMapScreen: View {
    @ObservedObject var vm: AppModel
    let unitId: Int

    var body: some View {
        if let u = vm.content.germanUnit(unitId) {
            let names = u.strs("names")
            List {
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
        }
    }
}

struct NetYearsScreen: View {
    @ObservedObject var vm: AppModel

    var body: some View {
        List {
            NavigationLink(value: Route.netMap) {
                VStack(alignment: .leading) {
                    Text(vm.tr("net_year1"))
                    Text(vm.tr("net_sub")).font(.caption).foregroundStyle(.secondary)
                }
            }
            ForEach(["net_year2", "net_year3", "net_year4"], id: \.self) { key in
                Label {
                    VStack(alignment: .leading) {
                        Text(vm.tr(key))
                        Text(vm.tr("net_year_locked_sub")).font(.caption)
                    }
                } icon: {
                    Image(systemName: "lock.fill")
                }
                .foregroundStyle(.secondary)
            }
        }
        .navigationTitle(vm.tr("net_years_title"))
        .navigationSubtitle(vm.tr("net_years_sub"))
    }
}

struct LessonRow: Identifiable {
    let id: String
    let title: String
    let done: Bool
    let destination: Route
}

struct LessonListScreen: View {
    let title: String
    let subtitle: String
    let rows: [LessonRow]

    var body: some View {
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
    }
}

struct SlidesScreen: View {
    @ObservedObject var vm: AppModel
    let title: String
    let subtitle: String
    let slides: [J]
    let next: Route
    @State private var idx = 0

    var body: some View {
        TabView(selection: $idx) {
            ForEach(slides.indices, id: \.self) { i in
                let slide = slides[i]
                ScrollView {
                    VStack(alignment: .leading, spacing: 12) {
                        Text(slide.str("kicker"))
                            .font(.caption.weight(.semibold))
                            .foregroundStyle(.secondary)
                        Text(slide.str("title")).font(.title2.bold())
                        if let tip = slide.strOrNull("tip") {
                            GlassCard { Text(tip) }
                        }
                        ForEach(Array(slide.strs("lines").enumerated()), id: \.offset) { _, line in
                            Text(line).padding(.vertical, 4)
                        }
                    }
                    .padding()
                    .frame(maxWidth: .infinity, alignment: .leading)
                }
                .tag(i)
            }
        }
        .tabViewStyle(.page(indexDisplayMode: .automatic))
        .navigationTitle(title)
        .navigationSubtitle(subtitle)
        .toolbar {
            ToolbarItem(placement: .bottomBar) {
                if idx < slides.count - 1 {
                    Button(vm.tr("net_slide_next")) { idx += 1 }
                } else {
                    NavigationLink(vm.tr("net_slide_start"), value: next)
                }
            }
        }
    }
}

struct CzechMapScreen: View {
    @ObservedObject var vm: AppModel

    var body: some View {
        List {
            Label(vm.tr("Literatura"), systemImage: "lock.fill")
                .foregroundStyle(.secondary)
            NavigationLink(value: Route.mluvnice) {
                Label(vm.tr("Mluvnice"), systemImage: "textformat")
            }
            NavigationLink(value: Route.readingList) {
                Label(vm.tr("Maturitní četba"), systemImage: "books.vertical")
            }
        }
        .navigationTitle(vm.tr("Český jazyk a literatura"))
        .navigationSubtitle(vm.tr("czech_sub"))
    }
}

struct BookListScreen: View {
    @ObservedObject var vm: AppModel

    var body: some View {
        List {
            ForEach(Array(vm.content.books.enumerated()), id: \.offset) { _, b in
                NavigationLink(value: Route.book(b.str("id"))) {
                    Label {
                        VStack(alignment: .leading) {
                            Text(b.str("title"))
                            Text(vm.tr(b.str("subKey"))).font(.caption).foregroundStyle(.secondary)
                        }
                    } icon: {
                        Image(systemName: "book.fill")
                    }
                }
            }
        }
        .navigationTitle(vm.tr("Maturitní četba"))
        .navigationSubtitle(vm.tr("reading_sub"))
    }
}

struct BookScreen: View {
    @ObservedObject var vm: AppModel
    let id: String

    var body: some View {
        if let b = vm.content.book(id) {
            List {
                NavigationLink(value: Route.bookQuiz(id)) {
                    Label {
                        VStack(alignment: .leading) {
                            Text(vm.tr(b.str("quizTitle")))
                            Text(vm.tr(b.str("quizSub"))).font(.caption).foregroundStyle(.secondary)
                        }
                    } icon: {
                        Image(systemName: "questionmark.circle")
                    }
                }
                NavigationLink(value: Route.bookPlot(id)) {
                    Label {
                        VStack(alignment: .leading) {
                            Text(vm.tr(b.str("plotTitle")))
                            Text(vm.tr(b.str("plotSub"))).font(.caption).foregroundStyle(.secondary)
                        }
                    } icon: {
                        Image(systemName: "list.number")
                    }
                }
                ForEach(Array(b.arr("notes").enumerated()), id: \.offset) { _, note in
                    Section(note.str("title")) {
                        ForEach(note.strs("lines"), id: \.self) { line in
                            Text(line)
                        }
                    }
                }
            }
            .navigationTitle(b.str("title"))
            .navigationSubtitle(vm.tr(b.str("subKey")))
        }
    }
}
