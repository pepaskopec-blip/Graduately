import SwiftUI

struct WelcomeScreen: View {
    @ObservedObject var vm: AppModel

    var body: some View {
        let p = vm.palette
        ScrollView {
            VStack(alignment: .leading, spacing: 0) {
                Text("maturita.c")
                    .font(.system(size: 15, weight: .medium))
                    .foregroundStyle(p.text)
                Text("sestavení \(AppConfig.versionName) · \(String(AppConfig.commit.prefix(7)))")
                    .font(.system(size: 12))
                    .foregroundStyle(p.overlay)
                    .padding(.bottom, 12)
                HStack(alignment: .center, spacing: 12) {
                    VStack(alignment: .leading, spacing: 12) {
                        Text(vm.tr("welcome_title"))
                            .font(.system(size: 32, weight: .medium))
                            .foregroundStyle(p.text)
                        Text(vm.tr("welcome_body_ios"))
                            .font(.system(size: 16))
                            .foregroundStyle(p.subtext)
                        PrimaryButton(label: vm.tr("continue"), palette: p) { vm.go(.subjects) }
                    }
                    Spacer(minLength: 0)
                    VStack {
                        Text(vm.tr("welcome_stat_value"))
                            .font(.system(size: 28, weight: .bold))
                            .foregroundStyle(p.text)
                        Text(vm.tr("welcome_stat_label"))
                            .font(.system(size: 11, weight: .semibold))
                            .foregroundStyle(p.subtext)
                            .multilineTextAlignment(.center)
                    }
                    .frame(width: 108, height: 108)
                    .background(p.mantle, in: Circle())
                    .overlay(Circle().stroke(p.text.opacity(0.08), lineWidth: 1))
                }
                FeatureCard(vm: vm, t: "welcome_feat1_title", b: "welcome_feat1_body", l: "welcome_feat1_link")
                    .padding(.top, 28)
                FeatureCard(vm: vm, t: "welcome_feat2_title", b: "welcome_feat2_body", l: "welcome_feat2_link")
                    .padding(.top, 12)
            }
            .padding(.horizontal, 24)
            .padding(.vertical, 16)
        }
    }
}

private struct FeatureCard: View {
    @ObservedObject var vm: AppModel
    let t, b, l: String

    var body: some View {
        let p = vm.palette
        CardBox(palette: p) {
            VStack(alignment: .leading, spacing: 6) {
                Text(vm.tr(t)).font(.system(size: 18, weight: .bold)).foregroundStyle(p.text)
                Text(vm.tr(b)).font(.system(size: 15)).foregroundStyle(p.subtext)
                PillButton(label: vm.tr(l), palette: p) { vm.go(.subjects) }
                    .padding(.top, 4)
            }
        }
    }
}

struct SubjectsScreen: View {
    @ObservedObject var vm: AppModel

    var body: some View {
        let p = vm.palette
        VStack(spacing: 0) {
            PageTop(vm: vm, back: { vm.back() }, title: vm.tr("subjects_title"), subtitle: vm.tr("subjects_sub"))
                .padding(.horizontal, 16)
            ScrollView {
                VStack(spacing: 12) {
                    ForEach(Array(stride(from: 0, to: vm.content.subjects.count, by: 2)), id: \.self) { i in
                        HStack(spacing: 12) {
                            subjectTile(vm.content.subjects[i], p)
                            if i + 1 < vm.content.subjects.count {
                                subjectTile(vm.content.subjects[i + 1], p)
                            } else {
                                Color.clear.frame(maxWidth: .infinity).frame(height: 132)
                            }
                        }
                    }
                }
                .padding(.horizontal, 16)
                .padding(.bottom, 24)
            }
        }
    }

    private func subjectTile(_ s: J, _ p: Palette) -> some View {
        let open = s.bool("open")
        return Button {
            if open { vm.goPage(s.str("target")) }
        } label: {
            VStack(spacing: 10) {
                switch s.str("icon") {
                case "de": GermanFlag(size: 44)
                case "wifi": WifiIcon(color: p.text, size: 36)
                case "chip": ChipIcon(color: p.text, size: 36)
                case "cz": CzechFlag(size: 44)
                default: LockIcon(color: p.overlay, size: 22)
                }
                Text(vm.tr(s.str("key")))
                    .font(.system(size: 13, weight: .semibold))
                    .foregroundStyle(p.text)
                    .multilineTextAlignment(.center)
            }
            .frame(maxWidth: .infinity)
            .frame(height: 132)
            .background(p.mantle, in: RoundedRectangle(cornerRadius: 20, style: .continuous))
            .overlay(
                RoundedRectangle(cornerRadius: 20, style: .continuous)
                    .stroke(p.text.opacity(open ? 0.08 : 0.04), lineWidth: 1)
            )
            .opacity(open ? 1 : 0.48)
        }
        .buttonStyle(.plain)
        .disabled(!open)
    }
}

struct SettingsSheet: View {
    @ObservedObject var vm: AppModel

    var body: some View {
        let p = vm.palette
        ScrollView {
            VStack(alignment: .leading, spacing: 8) {
                HStack {
                    Text(vm.tr("settings_title"))
                        .font(.system(size: 22, weight: .bold))
                        .foregroundStyle(p.text)
                    Spacer()
                    PillButton(label: vm.tr("back"), palette: p) { vm.settingsOpen = false }
                }
                Text(vm.tr("mode")).font(.system(size: 12, weight: .semibold)).foregroundStyle(p.subtext).padding(.top, 8)
                HStack(spacing: 4) {
                    SegChip(label: vm.tr("mode_dark"), selected: vm.mode == .dark, palette: p) { vm.applyMode(.dark) }
                    SegChip(label: vm.tr("mode_light"), selected: vm.mode == .light, palette: p) { vm.applyMode(.light) }
                }
                .padding(3)
                .background(p.surface1, in: Capsule())
                Text(vm.tr("theme")).font(.system(size: 12, weight: .semibold)).foregroundStyle(p.subtext).padding(.top, 8)
                ForEach(Array(themeNames.enumerated()), id: \.offset) { i, name in
                    if let id = ThemeId(rawValue: i) {
                        let sw = themePalette(id, vm.mode)
                        let sel = vm.themeId == id
                        Button { vm.setTheme(id) } label: {
                            HStack(spacing: 10) {
                                ZStack {
                                    Circle().fill(sw.base)
                                    Circle().fill(sw.accent).frame(width: 10, height: 10).offset(x: -8)
                                    Circle().fill(sw.accent3).frame(width: 10, height: 10)
                                    Circle().fill(sw.success).frame(width: 10, height: 10).offset(x: 8)
                                    if sel { Circle().stroke(p.accent, lineWidth: 2.2) }
                                }
                                .frame(width: 36, height: 36)
                                Text(name).font(.system(size: 16, weight: .medium)).foregroundStyle(p.text)
                                Spacer()
                            }
                            .padding(10)
                            .background(sel ? p.surface1 : p.surface0, in: RoundedRectangle(cornerRadius: 14, style: .continuous))
                        }
                        .buttonStyle(.plain)
                    }
                }
                Text(vm.tr("language")).font(.system(size: 12, weight: .semibold)).foregroundStyle(p.subtext).padding(.top, 8)
                HStack(spacing: 4) {
                    SegChip(label: "Čeština", selected: vm.lang == .cs, palette: p) { vm.applyLang(.cs) }
                    SegChip(label: "English", selected: vm.lang == .en, palette: p) { vm.applyLang(.en) }
                }
                .padding(3)
                .background(p.surface1, in: Capsule())
                Text(vm.tr("updates")).font(.system(size: 12, weight: .semibold)).foregroundStyle(p.subtext).padding(.top, 8)
                Text(updateText)
                    .font(.system(size: 14))
                    .foregroundStyle(p.subtext)
                HStack(spacing: 8) {
                    PillButton(label: vm.tr("update_check"), palette: p) { vm.checkUpdate(true) }
                    if vm.update.canInstall {
                        PrimaryButton(label: vm.tr("update_install"), palette: p) { vm.installUpdate() }
                    }
                }
                .padding(.bottom, 24)
            }
            .padding(20)
        }
        .background(p.base)
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
        let p = vm.palette
        let sum = summarize(vm.content, vm.progress)
        let pct = sum.totalEx == 0 ? 0 : (100 * sum.doneEx / sum.totalEx)
        VStack(spacing: 0) {
            PageTop(vm: vm, back: { vm.back() }, title: vm.tr("stats_title"), subtitle: vm.tr("stats_sub"))
                .padding(.horizontal, 16)
            ScrollView {
                VStack(alignment: .leading, spacing: 10) {
                    HStack(spacing: 10) {
                        metric(p, vm.tr("stats_ex_label"), vm.fmt("stats_ex_fmt", sum.doneEx, sum.totalEx))
                        metric(p, vm.tr("stats_pct_label"), vm.fmt("stats_pct_fmt", pct))
                        metric(p, vm.tr("stats_units_label"), vm.fmt("stats_units_fmt", sum.doneUnits, sum.openUnits))
                    }
                    ProgressBar(fraction: CGFloat(pct) / 100, palette: p, height: 8)
                    Text(vm.tr("stats_section"))
                        .font(.system(size: 12, weight: .semibold))
                        .foregroundStyle(p.subtext)
                        .padding(.top, 10)
                    ForEach(Array(vm.content.subjects.enumerated()), id: \.offset) { _, s in
                        let open = s.bool("open")
                        let part = subjectSum(vm, s)
                        CardBox(palette: p) {
                            VStack(alignment: .leading, spacing: 4) {
                                Text(vm.tr(s.str("key"))).font(.system(size: 16, weight: .bold)).foregroundStyle(p.text)
                                Text(open ? vm.fmt("stats_ex_fmt", part.doneEx, part.totalEx) : vm.tr("stats_locked"))
                                    .font(.system(size: 13)).foregroundStyle(p.subtext)
                                if open && part.totalEx > 0 {
                                    ProgressBar(fraction: CGFloat(part.doneEx) / CGFloat(part.totalEx), palette: p, height: 6)
                                        .padding(.top, 4)
                                }
                            }
                        }
                        .opacity(open ? 1 : 0.55)
                    }
                }
                .padding(.horizontal, 16)
                .padding(.bottom, 24)
            }
        }
    }

    private func metric(_ p: Palette, _ label: String, _ value: String) -> some View {
        CardBox(palette: p) {
            VStack(alignment: .leading) {
                Text(label).font(.system(size: 12, weight: .semibold)).foregroundStyle(p.subtext)
                Text(value).font(.system(size: 22, weight: .bold)).foregroundStyle(p.text)
            }
        }
    }
}

struct ProgressBar: View {
    let fraction: CGFloat
    let palette: Palette
    var height: CGFloat = 8

    var body: some View {
        GeometryReader { geo in
            ZStack(alignment: .leading) {
                Capsule().fill(palette.surface1)
                Capsule().fill(palette.accent).frame(width: geo.size.width * min(max(fraction, 0), 1))
            }
        }
        .frame(height: height)
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

struct SearchOverlay: View {
    @ObservedObject var vm: AppModel

    var body: some View {
        let p = vm.palette
        let q = normalizeAnswer(vm.searchQuery)
        let hits = buildSearch(vm).filter { q.isEmpty || normalizeAnswer($0.hay).contains(q) }.prefix(24)
        VStack(alignment: .leading, spacing: 12) {
            HStack {
                ZStack(alignment: .leading) {
                    if vm.searchQuery.isEmpty {
                        Text(vm.tr("search_placeholder")).foregroundStyle(p.overlay)
                    }
                    TextField("", text: $vm.searchQuery)
                        .foregroundStyle(p.text)
                        .tint(p.accent)
                }
                .padding(14)
                .background(p.mantle, in: RoundedRectangle(cornerRadius: 16, style: .continuous))
                PillButton(label: vm.tr("back"), palette: p) { vm.searchOpen = false }
            }
            if hits.isEmpty {
                Text(q.isEmpty ? vm.tr("search_hint") : vm.tr("search_empty")).foregroundStyle(p.subtext)
            } else {
                ScrollView {
                    VStack(spacing: 8) {
                        ForEach(Array(hits), id: \.target) { hit in
                            Button {
                                if !hit.locked {
                                    vm.goPage(hit.target)
                                    vm.searchOpen = false
                                }
                            } label: {
                                CardBox(palette: p) {
                                    VStack(alignment: .leading) {
                                        Text(hit.title).font(.system(size: 16, weight: .semibold)).foregroundStyle(p.text)
                                        Text(hit.sub).font(.system(size: 13)).foregroundStyle(p.subtext)
                                    }
                                }
                            }
                            .buttonStyle(.plain)
                            .opacity(hit.locked ? 0.5 : 1)
                        }
                    }
                }
            }
            Spacer(minLength: 0)
        }
        .padding(16)
        .background(p.base.opacity(0.96))
    }
}

private struct Hit: Identifiable {
    var id: String { target + title }
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
        let current = vm.content.german.firstIndex { u in
            u.bool("unlocked") && u.strs("names").indices.contains { !vm.progress.germanDone(u.int("id"), $0 + 1) }
        }
        VStack(spacing: 0) {
            PageTop(vm: vm, back: { vm.back() }, title: vm.tr("roadmap_title"), subtitle: vm.tr("roadmap_sub"))
                .padding(.horizontal, 12)
            PathMap(
                nodes: vm.content.german.enumerated().map { i, u in
                    let unlocked = u.bool("unlocked")
                    let names = u.strs("names")
                    let done = unlocked && !names.isEmpty && names.indices.allSatisfy { vm.progress.germanDone(u.int("id"), $0 + 1) }
                    return MapNode(
                        id: u.str("page"),
                        label: u.str("title"),
                        locked: !unlocked,
                        done: done,
                        current: i == current,
                        finish: i == vm.content.german.count - 1,
                        onClick: unlocked ? { vm.go(.unitMap(u.int("id"))) } : nil
                    )
                },
                palette: vm.palette
            )
            .padding(.horizontal, 12)
        }
    }
}

struct UnitMapScreen: View {
    @ObservedObject var vm: AppModel
    let unitId: Int

    var body: some View {
        if let u = vm.content.germanUnit(unitId) {
            let names = u.strs("names")
            let next = names.indices.first { !vm.progress.germanDone(unitId, $0 + 1) }
            VStack(spacing: 0) {
                PageTop(vm: vm, back: { vm.back() }, title: u.str("title"), subtitle: vm.tr(u.str("sub")))
                    .padding(.horizontal, 12)
                PathMap(
                    nodes: names.enumerated().map { i, name in
                        MapNode(
                            id: "e\(i + 1)",
                            label: name,
                            locked: false,
                            done: vm.progress.germanDone(unitId, i + 1),
                            current: i == next,
                            onClick: { vm.go(.germanEx(unitId, i + 1)) }
                        )
                    },
                    palette: vm.palette
                )
                .padding(.horizontal, 12)
                if u.has("branch") {
                    PrimaryButton(label: vm.tr("Vokabeltraining"), palette: vm.palette) { vm.go(.vocab(unitId)) }
                        .padding(.bottom, 16)
                }
            }
        }
    }
}

struct NetYearsScreen: View {
    @ObservedObject var vm: AppModel

    var body: some View {
        VStack(spacing: 0) {
            PageTop(vm: vm, back: { vm.back() }, title: vm.tr("net_years_title"), subtitle: vm.tr("net_years_sub"))
                .padding(.horizontal, 16)
            ScrollView {
                VStack(spacing: 12) {
                    yearCard(vm.tr("net_year1"), vm.tr("net_sub"), true) { vm.go(.netMap) }
                    ForEach(["net_year2", "net_year3", "net_year4"], id: \.self) { key in
                        yearCard(vm.tr(key), vm.tr("net_year_locked_sub"), false) {}
                    }
                }
                .padding(.horizontal, 16)
                .padding(.bottom, 16)
            }
        }
    }

    private func yearCard(_ title: String, _ sub: String, _ open: Bool, _ action: @escaping () -> Void) -> some View {
        let p = vm.palette
        return Button(action: action) {
            CardBox(palette: p) {
                HStack {
                    VStack(alignment: .leading) {
                        Text(title).font(.system(size: 18, weight: .bold)).foregroundStyle(p.text)
                        Text(sub).font(.system(size: 14)).foregroundStyle(p.subtext)
                    }
                    Spacer()
                    if !open { LockIcon(color: p.overlay) }
                }
            }
        }
        .buttonStyle(.plain)
        .disabled(!open)
        .opacity(open ? 1 : 0.48)
    }
}

struct LessonMapScreen: View {
    @ObservedObject var vm: AppModel
    let title: String
    let sub: String
    let nodes: [MapNode]

    var body: some View {
        VStack(spacing: 0) {
            PageTop(vm: vm, back: { vm.back() }, title: title, subtitle: sub)
                .padding(.horizontal, 12)
            PathMap(nodes: nodes, palette: vm.palette)
                .padding(.horizontal, 12)
        }
    }
}

struct SlidesScreen: View {
    @ObservedObject var vm: AppModel
    let title: String
    let sub: String
    let slides: [J]
    var onExercise: () -> Void
    @State private var idx = 0

    var body: some View {
        let p = vm.palette
        if let slide = slides.indices.contains(idx) ? slides[idx] : nil {
            VStack(spacing: 0) {
                PageTop(vm: vm, back: { vm.back() }, title: title, subtitle: sub)
                    .padding(.horizontal, 16)
                ScrollView {
                    VStack(alignment: .leading, spacing: 8) {
                        Text(slide.str("kicker")).font(.system(size: 12, weight: .semibold)).foregroundStyle(p.subtext)
                        Text(slide.str("title")).font(.system(size: 24, weight: .bold)).foregroundStyle(p.text)
                        if let tip = slide.strOrNull("tip") {
                            CardBox(palette: p) { Text(tip).foregroundStyle(p.text) }
                        }
                        ForEach(Array(slide.strs("lines").enumerated()), id: \.offset) { _, line in
                            CardBox(palette: p) { Text(line).font(.system(size: 16)).foregroundStyle(p.text) }
                        }
                    }
                    .padding(.horizontal, 16)
                }
                HStack {
                    if idx > 0 {
                        PillButton(label: vm.tr("net_slide_prev"), palette: p) { idx -= 1 }
                    }
                    Spacer()
                    if idx < slides.count - 1 {
                        PrimaryButton(label: vm.tr("net_slide_next"), palette: p) { idx += 1 }
                    } else {
                        PrimaryButton(label: vm.tr("net_slide_start"), palette: p, action: onExercise)
                    }
                }
                .padding(12)
            }
        }
    }
}

struct CzechMapScreen: View {
    @ObservedObject var vm: AppModel

    var body: some View {
        let p = vm.palette
        let items: [(String, String, Route?)] = [
            ("L", vm.tr("Literatura"), nil),
            ("M", vm.tr("Mluvnice"), .mluvnice),
            ("Č", vm.tr("Maturitní četba"), .readingList),
        ]
        VStack(spacing: 0) {
            PageTop(vm: vm, back: { vm.back() }, title: vm.tr("Český jazyk a literatura"), subtitle: vm.tr("czech_sub"))
                .padding(.horizontal, 16)
            ScrollView {
                VStack(spacing: 12) {
                    ForEach(items, id: \.0) { letter, name, route in
                        Button {
                            if let route { vm.go(route) }
                        } label: {
                            CardBox(palette: p) {
                                HStack(spacing: 14) {
                                    ZStack {
                                        Circle().fill(route == nil ? p.lockedBg : p.accent).frame(width: 56, height: 56)
                                        if route == nil {
                                            LockIcon(color: p.overlay)
                                        } else {
                                            Text(letter).font(.system(size: 20, weight: .heavy)).foregroundStyle(p.onAccent)
                                        }
                                    }
                                    Text(name).font(.system(size: 16, weight: .semibold)).foregroundStyle(p.text)
                                    Spacer()
                                }
                            }
                        }
                        .buttonStyle(.plain)
                        .disabled(route == nil)
                        .opacity(route == nil ? 0.48 : 1)
                    }
                }
                .padding(.horizontal, 16)
            }
        }
    }
}

struct BookListScreen: View {
    @ObservedObject var vm: AppModel

    var body: some View {
        let p = vm.palette
        VStack(spacing: 0) {
            PageTop(vm: vm, back: { vm.back() }, title: vm.tr("Maturitní četba"), subtitle: vm.tr("reading_sub"))
                .padding(.horizontal, 16)
            ScrollView {
                VStack(spacing: 12) {
                    ForEach(Array(vm.content.books.enumerated()), id: \.offset) { _, b in
                        Button { vm.go(.book(b.str("id"))) } label: {
                            CardBox(palette: p) {
                                HStack(spacing: 12) {
                                    BookIcon(color: p.text, size: 28)
                                    VStack(alignment: .leading) {
                                        Text(b.str("title")).font(.system(size: 16, weight: .bold)).foregroundStyle(p.text)
                                        Text(vm.tr(b.str("subKey"))).font(.system(size: 13)).foregroundStyle(p.subtext)
                                    }
                                    Spacer()
                                }
                            }
                        }
                        .buttonStyle(.plain)
                    }
                }
                .padding(.horizontal, 16)
            }
        }
    }
}

struct BookScreen: View {
    @ObservedObject var vm: AppModel
    let id: String

    var body: some View {
        if let b = vm.content.book(id) {
            let p = vm.palette
            VStack(spacing: 0) {
                PageTop(vm: vm, back: { vm.back() }, title: b.str("title"), subtitle: vm.tr(b.str("subKey")))
                    .padding(.horizontal, 16)
                ScrollView {
                    VStack(spacing: 12) {
                        Button { vm.go(.bookQuiz(id)) } label: {
                            CardBox(palette: p) {
                                HStack(spacing: 10) {
                                    QuizIcon(color: p.text)
                                    VStack(alignment: .leading) {
                                        Text(vm.tr(b.str("quizTitle"))).font(.system(size: 16, weight: .bold)).foregroundStyle(p.text)
                                        Text(vm.tr(b.str("quizSub"))).font(.system(size: 13)).foregroundStyle(p.subtext)
                                    }
                                }
                            }
                        }
                        .buttonStyle(.plain)
                        Button { vm.go(.bookPlot(id)) } label: {
                            CardBox(palette: p) {
                                HStack(spacing: 10) {
                                    OrderIcon(color: p.text)
                                    VStack(alignment: .leading) {
                                        Text(vm.tr(b.str("plotTitle"))).font(.system(size: 16, weight: .bold)).foregroundStyle(p.text)
                                        Text(vm.tr(b.str("plotSub"))).font(.system(size: 13)).foregroundStyle(p.subtext)
                                    }
                                }
                            }
                        }
                        .buttonStyle(.plain)
                        ForEach(Array(b.arr("notes").enumerated()), id: \.offset) { _, note in
                            CardBox(palette: p) {
                                VStack(alignment: .leading, spacing: 8) {
                                    HStack(spacing: 8) {
                                        noteIcon(note.str("icon"), p.text)
                                        Text(note.str("title")).font(.system(size: 16, weight: .bold)).foregroundStyle(p.text)
                                    }
                                    ForEach(note.strs("lines"), id: \.self) { line in
                                        Text("• \(line)").font(.system(size: 15)).foregroundStyle(p.text)
                                    }
                                }
                            }
                        }
                    }
                    .padding(.horizontal, 16)
                    .padding(.bottom, 20)
                }
            }
        }
    }
}
