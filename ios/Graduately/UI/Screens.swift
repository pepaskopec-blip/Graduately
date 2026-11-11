import SwiftUI
#if os(macOS)
import AppKit
#endif

struct PracticeHome: View {
    @ObservedObject var vm: AppModel
    @Environment(\.palette) private var palette

    var body: some View {
        let sum = summarize(vm.content, vm.progress)
        let open = vm.content.subjects.filter { $0.bool("open") }
        let soon = vm.content.subjects.filter { !$0.bool("open") }
        let parts = open.map { (subject: $0, sum: subjectSum(vm, $0)) }
        ScrollView {
            VStack(alignment: .leading, spacing: onMac(34, 24)) {
                #if os(iOS)
                Text("Graduately")
                    .displayItalic(32)
                    .foregroundStyle(palette.ink)
                    .padding(.top, 4)
                    .accessibilityAddTraits(.isHeader)
                #endif
                HomeHero(
                    vm: vm,
                    sum: sum,
                    parts: parts.map { part in
                        (accent: accent(part.subject), done: part.sum.doneEx, total: part.sum.totalEx)
                    }
                )
                if vm.showChangelog {
                    ChangelogCard(vm: vm)
                }
                VStack(alignment: .leading, spacing: onMac(18, 12)) {
                    SectionTitle(text: vm.tr("subjects_title"))
                    LazyVGrid(columns: columns, alignment: .leading, spacing: onMac(16, 12)) {
                        ForEach(Array(parts.enumerated()), id: \.offset) { _, part in
                            subjectTile(part.subject, part.sum)
                        }
                    }
                }
                if !soon.isEmpty {
                    VStack(alignment: .leading, spacing: 12) {
                        SectionTitle(text: vm.tr("subjects_soon"))
                        FlowLayout(spacing: 10) {
                            ForEach(Array(soon.enumerated()), id: \.offset) { _, s in
                                Label(vm.tr(s.str("key")), systemImage: "lock.fill")
                                    .font(onMac(Font.body, Font.subheadline))
                                    .foregroundStyle(palette.ink2)
                                    .padding(.horizontal, 14)
                                    .padding(.vertical, 9)
                                    .background(palette.sunken, in: Capsule())
                            }
                        }
                    }
                }
            }
            .padding(Page.inset)
            .macPage()
        }
        .paper()
        .navigationTitle("Graduately")
        #if os(iOS)
        .toolbar(.hidden, for: .navigationBar)
        #else
        .toolbar(removing: .title)
        #endif
        .softAppear()
    }

    private var columns: [GridItem] {
        #if os(macOS)
        [GridItem(.adaptive(minimum: MacChrome.homeTileMin), spacing: 16, alignment: .top)]
        #else
        [GridItem(.adaptive(minimum: 150), spacing: 12, alignment: .top)]
        #endif
    }

    private func accent(_ s: J) -> Accent {
        subjectStyle(s.str("icon"))?.accent(vm.mode) ?? palette.accent
    }

    @ViewBuilder
    private func subjectTile(_ s: J, _ part: ProgressSum) -> some View {
        let style = subjectStyle(s.str("icon"))
        let cover = SubjectCover(
            title: vm.tr(s.str("key")),
            symbol: style?.symbol ?? "book.fill",
            mark: style?.mark(vm.lang) ?? "",
            accent: accent(s),
            done: part.doneEx,
            total: part.totalEx
        )
        if let dest = s.strOrNull("target").flatMap(routeFromPage), dest != .subjects, dest != .welcome {
            NavigationLink(value: dest) { cover }
                .buttonStyle(PressScaleStyle())
        } else {
            cover
        }
    }
}

/// The glow of the app icon with the headline in its typeface, the share of
/// finished exercises, and every subject's part in it.
private struct HomeHero: View {
    @ObservedObject var vm: AppModel
    let sum: ProgressSum
    let parts: [(accent: Accent, done: Int, total: Int)]
    @Environment(\.palette) private var palette

    var body: some View {
        VStack(alignment: .leading, spacing: onMac(24, 20)) {
            #if os(macOS)
            HStack(alignment: .bottom, spacing: 32) {
                headline
                Spacer(minLength: 0)
                figure
            }
            #else
            headline
            figure
            #endif
            SpectrumBar(parts: parts, height: onMac(12, 10), track: palette.onGlow.opacity(0.12))
        }
        .foregroundStyle(palette.onGlow)
        .padding(.horizontal, onMac(36, 22))
        .padding(.vertical, onMac(30, 22))
        .frame(maxWidth: .infinity, alignment: .leading)
        .background {
            GeometryReader { geo in
                RadialGradient(
                    colors: [palette.glowInner, palette.glowOuter],
                    center: UnitPoint(x: 0.3, y: 0.3),
                    startRadius: 0,
                    endRadius: max(geo.size.width, geo.size.height) * 0.9
                )
            }
        }
        .clipShape(RoundedRectangle(cornerRadius: onMac(32, 28), style: .continuous))
    }

    private var headline: some View {
        VStack(alignment: .leading, spacing: onMac(14, 10)) {
            // Playfair's own leading is too loose for a two-line headline.
            VStack(alignment: .leading, spacing: onMac(-14, -10)) {
                ForEach(vm.tr("welcome_title").split(separator: "\n").map(String.init), id: \.self) { line in
                    Text(line)
                }
            }
            .displayItalic(onMac(54, 40))
            .accessibilityElement(children: .combine)
            Text(vm.tr(onMac("welcome_body_macos", "welcome_body_ios")))
                .font(onMac(Font.title3, Font.subheadline))
                .opacity(0.72)
                .fixedSize(horizontal: false, vertical: true)
                .frame(maxWidth: 520, alignment: .leading)
        }
    }

    private var figure: some View {
        let pct = sum.totalEx == 0 ? 0 : Int((Double(sum.doneEx) / Double(sum.totalEx) * 100).rounded(.down))
        return HStack(alignment: .firstTextBaseline, spacing: 12) {
            Text(vm.fmt("stats_pct_fmt", pct))
                .display(onMac(64, 44), relativeTo: .largeTitle)
                .monospacedDigit()
            VStack(alignment: .leading, spacing: 2) {
                Text(vm.fmt("stats_ex_fmt", sum.doneEx, sum.totalEx))
                    .font(onMac(Font.title3, Font.subheadline).weight(.semibold).monospacedDigit())
                Text(vm.tr("stats_ex_label"))
                    .kicker()
                    .opacity(0.7)
            }
        }
        .accessibilityElement(children: .combine)
    }
}

struct ChangelogCard: View {
    @ObservedObject var vm: AppModel
    @Environment(\.palette) private var palette
    @Environment(\.accent) private var accent

    var body: some View {
        let langKey = vm.lang == .en ? "en" : "cs"
        let blocks = Array(vm.content.changelog.prefix(3))
        GlassCard {
            VStack(alignment: .leading, spacing: 12) {
                HStack {
                    Label(vm.tr("changelog_title"), systemImage: "sparkles")
                        .kicker()
                        .foregroundStyle(accent.text)
                    Spacer()
                    if let date = blocks.first?.strOrNull("date") {
                        Text(date)
                            .font(.caption.weight(.semibold))
                            .foregroundStyle(palette.ink3)
                    }
                }
                ForEach(Array(blocks.enumerated()), id: \.offset) { index, block in
                    ChangelogLines(index: index, lines: changelogLines(block, langKey))
                }
                Button(vm.tr("changelog_dismiss")) { vm.dismissChangelog() }
                    .buttonStyle(SolidButtonStyle(wide: false))
                    .padding(.top, 2)
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
    @Environment(\.palette) private var palette
    @Environment(\.accent) private var accent

    var body: some View {
        VStack(alignment: .leading, spacing: 6) {
            if index > 0 { Spacer().frame(height: 2) }
            ForEach(Array(lines.enumerated()), id: \.offset) { _, line in
                HStack(alignment: .firstTextBaseline, spacing: 10) {
                    Circle().fill(accent.solid).frame(width: 5, height: 5)
                        .alignmentGuide(.firstTextBaseline) { $0[.bottom] + 3 }
                    Text(line)
                        .font(onMac(Font.body, Font.subheadline))
                        .foregroundStyle(palette.ink2)
                        .fixedSize(horizontal: false, vertical: true)
                        .frame(maxWidth: .infinity, alignment: .leading)
                }
            }
        }
    }
}

struct SettingsScreen: View {
    @ObservedObject var vm: AppModel
    @Environment(\.palette) private var palette

    var body: some View {
        ScrollView {
            VStack(alignment: .leading, spacing: onMac(20, 16)) {
                #if os(macOS)
                PageHeader(title: vm.tr("settings_title"))
                    .padding(.bottom, 8)
                #endif
                settingsCard(vm.tr("mode")) {
                    HStack(spacing: 10) {
                        SegChip(label: vm.tr("mode_light"), selected: vm.mode == .light) { vm.applyMode(.light) }
                        SegChip(label: vm.tr("mode_dark"), selected: vm.mode == .dark) { vm.applyMode(.dark) }
                    }
                }
                settingsCard(vm.tr("language")) {
                    HStack(spacing: 10) {
                        SegChip(label: "Čeština", selected: vm.lang == .cs) { vm.applyLang(.cs) }
                        SegChip(label: "English", selected: vm.lang == .en) { vm.applyLang(.en) }
                    }
                }
                settingsCard(vm.tr("updates")) {
                    Text(updateText)
                        .font(onMac(Font.body, Font.subheadline))
                        .foregroundStyle(palette.ink2)
                        .fixedSize(horizontal: false, vertical: true)
                    HStack(spacing: 10) {
                        Button(vm.tr("update_check")) { vm.checkUpdate(true) }
                            .buttonStyle(ChipStyle())
                        if vm.update.canInstall {
                            Button(vm.tr("update_install")) { vm.installUpdate() }
                                .buttonStyle(ChipStyle(selected: true))
                        }
                    }
                }
                VStack(alignment: .leading, spacing: 2) {
                    Text("Graduately")
                        .displayItalic(onMac(28, 24), relativeTo: .title2)
                        .foregroundStyle(palette.ink)
                    Text("Build \(AppConfig.versionName) · \(String(AppConfig.commit.prefix(7)))")
                        .font(.footnote.monospacedDigit())
                        .foregroundStyle(palette.ink3)
                }
                .padding(.top, 12)
                .padding(.horizontal, 4)
            }
            .padding(Page.inset)
            .frame(maxWidth: 620, alignment: .leading)
            .frame(maxWidth: .infinity, alignment: onMac(Alignment.center, Alignment.leading))
        }
        .paper()
        .pageTitle(vm.tr("settings_title"))
    }

    private func settingsCard<Content: View>(_ title: String, @ViewBuilder content: @escaping () -> Content) -> some View {
        GlassCard {
            VStack(alignment: .leading, spacing: 12) {
                Text(title)
                    .kicker()
                    .foregroundStyle(palette.ink3)
                content()
            }
            .frame(maxWidth: .infinity, alignment: .leading)
        }
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
    @Environment(\.palette) private var palette

    var body: some View {
        let sum = summarize(vm.content, vm.progress)
        let pct = sum.totalEx == 0 ? 0.0 : Double(sum.doneEx) / Double(sum.totalEx)
        let rows = rankedSubjects()
        ScrollView {
            VStack(alignment: .leading, spacing: onMac(24, 18)) {
                #if os(macOS)
                PageHeader(title: vm.tr("stats_title"), subtitle: vm.tr("stats_sub"))
                #endif
                GlassCard {
                    VStack(alignment: .leading, spacing: onMac(22, 16)) {
                        HStack(alignment: .top, spacing: 12) {
                            metric(vm.tr("stats_pct_label"), vm.fmt("stats_pct_fmt", Int(pct * 100)))
                            metric(vm.tr("stats_ex_label"), vm.fmt("stats_ex_fmt", sum.doneEx, sum.totalEx))
                            metric(vm.tr("stats_units_label"), vm.fmt("stats_units_fmt", sum.doneUnits, sum.openUnits))
                        }
                        SpectrumBar(
                            parts: rows.filter { $0.subject.bool("open") }.map { row in
                                (accent: accent(row.subject), done: row.part.doneEx, total: row.part.totalEx)
                            },
                            height: onMac(12, 10),
                            track: palette.sunken
                        )
                    }
                }
                SectionTitle(text: vm.tr("stats_section"))
                VStack(spacing: 0) {
                    ForEach(Array(rows.enumerated()), id: \.offset) { index, row in
                        statRow(row.subject, part: row.part)
                        if index < rows.count - 1 {
                            Rectangle().fill(palette.line).frame(height: 1)
                                .padding(.leading, onMac(72, 62))
                        }
                    }
                }
                .softSurface(cornerRadius: 20)
            }
            .padding(Page.inset)
            .macPage(980)
        }
        .paper()
        .pageTitle(vm.tr("stats_title"))
    }

    /// Open subjects by how far along they are, locked ones last.
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

    private func accent(_ s: J) -> Accent {
        subjectStyle(s.str("icon"))?.accent(vm.mode) ?? palette.accent
    }

    private func metric(_ label: String, _ value: String) -> some View {
        VStack(alignment: .leading, spacing: 4) {
            Text(value)
                .display(onMac(44, 22), relativeTo: .title)
                .monospacedDigit()
                .foregroundStyle(palette.ink)
                .minimumScaleFactor(0.6)
                .lineLimit(1)
            Text(label)
                .kicker()
                .foregroundStyle(palette.ink3)
        }
        .frame(maxWidth: .infinity, alignment: .leading)
        .accessibilityElement(children: .combine)
    }

    private func statRow(_ s: J, part: ProgressSum) -> some View {
        let open = s.bool("open")
        let counted = open && part.totalEx > 0
        return HStack(spacing: onMac(16, 12)) {
            SubjectIcon(icon: s.str("icon"), open: open, size: onMac(40, 36), mode: vm.mode)
            VStack(alignment: .leading, spacing: 8) {
                HStack(alignment: .firstTextBaseline) {
                    Text(vm.tr(s.str("key")))
                        .font(onMac(Font.title3, Font.body).weight(.semibold))
                        .foregroundStyle(open ? palette.ink : palette.ink3)
                        .lineLimit(1)
                    Spacer(minLength: 8)
                    Text(counted ? vm.fmt("stats_ex_fmt", part.doneEx, part.totalEx) : vm.tr("stats_locked"))
                        .font(onMac(Font.body, Font.footnote).monospacedDigit())
                        .foregroundStyle(palette.ink2)
                }
                if counted {
                    ProgressBar(value: share(part), height: 6, fill: accent(s).solid)
                }
            }
        }
        .padding(.horizontal, onMac(18, 14))
        .padding(.vertical, onMac(14, 12))
        .accessibilityElement(children: .combine)
    }
}

/// Done and total exercises behind one card of a years screen, or nil when there is no count.
func routeProgress(_ vm: AppModel, _ r: Route) -> (done: Int, total: Int)? {
    let c = vm.content
    let p = vm.progress
    func tally(_ lessons: [J], _ done: (Int) -> Bool) -> (done: Int, total: Int) {
        (lessons.filter { done($0.int("id")) }.count, lessons.count)
    }
    switch r {
    case .netMap: return tally(c.netLessons, p.netDone)
    case .net2Map: return tally(c.itLessons("net2")) { p.courseDone("net2", $0) }
    case .net3Map: return tally(c.itLessons("net3")) { p.courseDone("net3", $0) }
    case .net4Map: return tally(c.itLessons("net4")) { p.courseDone("net4", $0) }
    case .hwMap: return tally(c.hw, p.hwDone)
    case .hw2Map: return tally(c.itLessons("hw2")) { p.courseDone("hw2", $0) }
    case .hw3Map: return tally(c.itLessons("hw3")) { p.courseDone("hw3", $0) }
    case .hw4Map: return tally(c.itLessons("hw4")) { p.courseDone("hw4", $0) }
    case .onMap: return tally(c.on, p.onDone)
    case .on2Map: return tally(c.on2, p.on2Done)
    case .on3Map: return tally(c.on3, p.on3Done)
    case .on4Map: return tally(c.on4, p.on4Done)
    case .enMap(let year): return tally(c.enYear(year)) { p.enDone(year, $0) }
    case .deMap(let year): return tally(c.deYear(year)) { p.deDone(year, $0) }
    case .roadmap:
        var done = 0
        var total = 0
        for u in c.german where u.bool("unlocked") {
            let n = u.strs("names").count
            total += n
            done += (0..<n).filter { p.germanDone(u.int("id"), $0 + 1) }.count
        }
        return (done, total)
    case .litYears:
        let parts = [tally(c.lit, p.litDone), tally(c.lit2, p.lit2Done), tally(c.lit3, p.lit3Done), tally(c.lit4, p.lit4Done)]
        return (parts.reduce(0) { $0 + $1.done }, parts.reduce(0) { $0 + $1.total })
    case .litMap: return tally(c.lit, p.litDone)
    case .lit2Map: return tally(c.lit2, p.lit2Done)
    case .lit3Map: return tally(c.lit3, p.lit3Done)
    case .lit4Map: return tally(c.lit4, p.lit4Done)
    case .mluvnice: return tally(c.mluvnice, p.mluvDone)
    case .chemMap: return tally(c.chem, p.chemDone)
    case .bioMap: return tally(c.bio, p.bioDone)
    case .mat0Map: return tally(c.mat0, p.mat0Done)
    case .matMap: return tally(c.mat, p.matDone)
    case .mat2Map: return tally(c.mat2, p.mat2Done)
    case .mat3Map: return tally(c.mat3, p.mat3Done)
    case .mat4Map: return tally(c.mat4, p.mat4Done)
    case .fyzMap: return tally(c.fyz, p.fyzDone)
    case .fyz2Map: return tally(c.fyz2, p.fyz2Done)
    case .fyz3Map: return tally(c.fyz3, p.fyz3Done)
    case .fyz4Map: return tally(c.fyz4, p.fyz4Done)
    case .tgMap: return tally(c.tg, p.tgDone)
    case .tg2Map: return tally(c.tg2, p.tg2Done)
    case .tg3Map: return tally(c.tg3, p.tg3Done)
    case .tg4Map: return tally(c.tg4, p.tg4Done)
    case .pgMap: return tally(c.pg, p.pgDone)
    case .pg2Map: return tally(c.pg2, p.pg2Done)
    case .pg3Map: return tally(c.pg3, p.pg3Done)
    case .pg4Map: return tally(c.pg4, p.pg4Done)
    case .swMap: return tally(c.sw, p.swDone)
    case .sw2Map: return tally(c.sw2, p.sw2Done)
    case .sw3Map: return tally(c.sw3, p.sw3Done)
    case .sw4Map: return tally(c.sw4, p.sw4Done)
    case .csMap: return tally(c.cs, p.csDone)
    case .cs2Map: return tally(c.cs2, p.cs2Done)
    case .cs3Map: return tally(c.cs3, p.cs3Done)
    case .cs4Map: return tally(c.cs4, p.cs4Done)
    default: return nil
    }
}

private func subjectSum(_ vm: AppModel, _ s: J) -> ProgressSum {
    var sum = ProgressSum()
    switch s.str("target") {
    case "dehome":
        for u in vm.content.german where u.bool("unlocked") {
            let n = u.strs("names").count
            sum.totalEx += n
            sum.doneEx += (1...n).filter { vm.progress.germanDone(u.int("id"), $0) }.count
        }
        sum.totalEx += (1...4).reduce(0) { $0 + vm.content.deYear($1).count }
        sum.doneEx += (1...4).reduce(0) { acc, year in
            acc + vm.content.deYear(year).filter { vm.progress.deDone(year, $0.int("id")) }.count
        }
    case "netyears":
        sum.totalEx = vm.content.netLessons.count
            + vm.content.net2.count + vm.content.net3.count + vm.content.net4.count
        sum.doneEx = vm.content.netLessons.filter { vm.progress.netDone($0.int("id")) }.count
            + ["net2", "net3", "net4"].reduce(0) { acc, course in
                acc + vm.content.itLessons(course).filter { vm.progress.courseDone(course, $0.int("id")) }.count
            }
    case "hwyears", "hwmap":
        sum.totalEx = vm.content.hw.count
            + vm.content.hw2.count + vm.content.hw3.count + vm.content.hw4.count
        sum.doneEx = vm.content.hw.filter { vm.progress.hwDone($0.int("id")) }.count
            + ["hw2", "hw3", "hw4"].reduce(0) { acc, course in
                acc + vm.content.itLessons(course).filter { vm.progress.courseDone(course, $0.int("id")) }.count
            }
    case "enyears", "en1map", "en2map", "en3map", "en4map":
        sum.totalEx = (1...4).reduce(0) { $0 + vm.content.enYear($1).count }
        sum.doneEx = (1...4).reduce(0) { acc, year in
            acc + vm.content.enYear(year).filter { vm.progress.enDone(year, $0.int("id")) }.count
        }
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
    case "matyears", "mat0map", "matmap", "mat2map", "mat3map", "mat4map":
        sum.totalEx = vm.content.mat0.count + vm.content.mat.count + vm.content.mat2.count + vm.content.mat3.count + vm.content.mat4.count
        sum.doneEx = vm.content.mat0.filter { vm.progress.mat0Done($0.int("id")) }.count
            + vm.content.mat.filter { vm.progress.matDone($0.int("id")) }.count
            + vm.content.mat2.filter { vm.progress.mat2Done($0.int("id")) }.count
            + vm.content.mat3.filter { vm.progress.mat3Done($0.int("id")) }.count
            + vm.content.mat4.filter { vm.progress.mat4Done($0.int("id")) }.count
    case "csyears", "csmap", "cs2map", "cs3map", "cs4map":
        sum.totalEx = vm.content.cs.count + vm.content.cs2.count + vm.content.cs3.count + vm.content.cs4.count
        sum.doneEx = vm.content.cs.filter { vm.progress.csDone($0.int("id")) }.count
            + vm.content.cs2.filter { vm.progress.cs2Done($0.int("id")) }.count
            + vm.content.cs3.filter { vm.progress.cs3Done($0.int("id")) }.count
            + vm.content.cs4.filter { vm.progress.cs4Done($0.int("id")) }.count
    case "pgyears", "pgmap", "pg2map", "pg3map", "pg4map":
        sum.totalEx = vm.content.pg.count + vm.content.pg2.count + vm.content.pg3.count + vm.content.pg4.count
        sum.doneEx = vm.content.pg.filter { vm.progress.pgDone($0.int("id")) }.count
            + vm.content.pg2.filter { vm.progress.pg2Done($0.int("id")) }.count
            + vm.content.pg3.filter { vm.progress.pg3Done($0.int("id")) }.count
            + vm.content.pg4.filter { vm.progress.pg4Done($0.int("id")) }.count
    case "swyears", "swmap", "sw2map", "sw3map", "sw4map":
        sum.totalEx = vm.content.sw.count + vm.content.sw2.count + vm.content.sw3.count + vm.content.sw4.count
        sum.doneEx = vm.content.sw.filter { vm.progress.swDone($0.int("id")) }.count
            + vm.content.sw2.filter { vm.progress.sw2Done($0.int("id")) }.count
            + vm.content.sw3.filter { vm.progress.sw3Done($0.int("id")) }.count
            + vm.content.sw4.filter { vm.progress.sw4Done($0.int("id")) }.count
    case "tgyears", "tgmap", "tg2map", "tg3map", "tg4map":
        sum.totalEx = vm.content.tg.count + vm.content.tg2.count + vm.content.tg3.count + vm.content.tg4.count
        sum.doneEx = vm.content.tg.filter { vm.progress.tgDone($0.int("id")) }.count
            + vm.content.tg2.filter { vm.progress.tg2Done($0.int("id")) }.count
            + vm.content.tg3.filter { vm.progress.tg3Done($0.int("id")) }.count
            + vm.content.tg4.filter { vm.progress.tg4Done($0.int("id")) }.count
    case "fyzyears", "fyzmap", "fyz2map", "fyz3map", "fyz4map":
        sum.totalEx = vm.content.fyz.count + vm.content.fyz2.count + vm.content.fyz3.count + vm.content.fyz4.count
        sum.doneEx = vm.content.fyz.filter { vm.progress.fyzDone($0.int("id")) }.count
            + vm.content.fyz2.filter { vm.progress.fyz2Done($0.int("id")) }.count
            + vm.content.fyz3.filter { vm.progress.fyz3Done($0.int("id")) }.count
            + vm.content.fyz4.filter { vm.progress.fyz4Done($0.int("id")) }.count
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
    @Environment(\.palette) private var palette
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

    /// Where a result leads, in its subject's colour; pages outside any subject keep the brand's.
    private func hitRow(_ hit: Hit) -> some View {
        let subject = routeFromPage(hit.target)?.subject
        return HStack(spacing: 14) {
            Badge(symbol: hit.locked ? "lock.fill" : subject?.symbol ?? "arrow.up.right", size: onMac(40, 36), filled: !hit.locked)
                .environment(\.accent, subject?.accent(vm.mode) ?? palette.accent)
            VStack(alignment: .leading, spacing: 2) {
                Text(hit.title)
                    .font(onMac(Font.title3, Font.body).weight(.semibold))
                    .foregroundStyle(hit.locked ? palette.ink3 : palette.ink)
                    .multilineTextAlignment(.leading)
                if !hit.sub.isEmpty {
                    Text(hit.sub)
                        .font(onMac(Font.body, Font.footnote))
                        .foregroundStyle(palette.ink2)
                        .multilineTextAlignment(.leading)
                }
            }
            .frame(maxWidth: .infinity, alignment: .leading)
        }
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
                            hitRow(hit)
                                .padding(14)
                                .frame(maxWidth: .infinity, minHeight: 72, alignment: .leading)
                                .softSurface(cornerRadius: 16)
                                .contentShape(RoundedRectangle(cornerRadius: 16, style: .continuous))
                        }
                        .buttonStyle(PressScaleStyle())
                        .disabled(hit.locked)
                    }
                }
                .padding(Page.inset)
                .macPage()
            }
        }
        .paper()
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
                        fieldFocused = false
                        if !hit.locked { vm.openFromSearch(hit.target) }
                    } label: {
                        hitRow(hit)
                    }
                    .disabled(hit.locked)
                    .listRowBackground(palette.surface)
                    .listRowSeparatorTint(palette.line)
                }
            }
        }
        .paper()
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
    }

    private var searchField: some View {
        HStack(spacing: 8) {
            Image(systemName: "magnifyingglass")
                .foregroundStyle(palette.ink3)
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
                        .foregroundStyle(palette.ink3)
                }
                .buttonStyle(PressScaleStyle())
                .accessibilityLabel("Clear")
            }
        }
        .padding(.horizontal, 14)
        .padding(.vertical, 12)
        .softSurface(cornerRadius: 14)
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
    for course in ["net2", "net3", "net4"] {
        add(vm.tr("net_year\(course.dropFirst(3))"), vm.tr("search_lesson"), "\(course)map", extra: "rocnik site")
        for l in vm.content.itLessons(course) {
            add(vm.tr(l.str("titleKey")), vm.tr("search_lesson"), "\(course)unit\(l.int("id"))", extra: "site")
        }
    }
    add(vm.tr("hw_year1"), vm.tr("search_lesson"), "hwmap", extra: "rocnik")
    for l in vm.content.hw {
        add(vm.tr(l.str("titleKey")), vm.tr("search_lesson"), "hwunit\(l.int("id"))", extra: "hardware")
        add(vm.tr(l.str("titleKey")), vm.tr("search_exercise"), "hwex\(l.int("id"))")
    }
    for course in ["hw2", "hw3", "hw4"] {
        add(vm.tr("hw_year\(course.dropFirst(2))"), vm.tr("search_lesson"), "\(course)map", extra: "rocnik hardware")
        for l in vm.content.itLessons(course) {
            add(vm.tr(l.str("titleKey")), vm.tr("search_lesson"), "\(course)unit\(l.int("id"))", extra: "hardware")
        }
    }
    add(vm.tr("on_year1"), vm.tr("search_lesson"), "onmap", extra: "rocnik")
    add(vm.tr("on_year2"), vm.tr("search_lesson"), "on2map", extra: "rocnik")
    add(vm.tr("on_year3"), vm.tr("search_lesson"), "on3map", extra: "rocnik")
    add(vm.tr("on_year4"), vm.tr("search_lesson"), "on4map", extra: "rocnik")
    for year in 1...4 {
        add(vm.tr("en_year\(year)"), vm.tr("search_lesson"), "en\(year)map", extra: "anglictina english rocnik")
        for l in vm.content.enYear(year) {
            add(vm.tr(l.str("titleKey")), vm.tr("search_lesson"), "en\(year)unit\(l.int("id"))", extra: "anglictina english")
        }
        add(vm.tr("de_year\(year)"), vm.tr("search_lesson"), "de\(year)map", extra: "nemcina deutsch rocnik")
        for l in vm.content.deYear(year) {
            add(vm.tr(l.str("titleKey")), vm.tr("search_lesson"), "de\(year)unit\(l.int("id"))", extra: "nemcina deutsch")
        }
    }
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
    add(vm.tr("Matematika"), vm.tr("search_lesson"), "matyears", extra: "matematika")
    add(vm.tr("mat0_year"), vm.tr("search_lesson"), "mat0map", extra: "zakladka")
    add(vm.tr("mat_year1"), vm.tr("search_lesson"), "matmap", extra: "rocnik")
    add(vm.tr("mat_year2"), vm.tr("search_lesson"), "mat2map", extra: "rocnik")
    add(vm.tr("mat_year3"), vm.tr("search_lesson"), "mat3map", extra: "rocnik")
    add(vm.tr("mat_year4"), vm.tr("search_lesson"), "mat4map", extra: "rocnik")
    for l in vm.content.mat0 {
        add(vm.tr(l.str("titleKey")), vm.tr("search_lesson"), "mat0unit\(l.int("id"))", extra: "matematika")
        add(vm.tr(l.str("titleKey")), vm.tr("search_exercise"), "mat0ex\(l.int("id"))")
    }
    for l in vm.content.mat {
        add(vm.tr(l.str("titleKey")), vm.tr("search_lesson"), "matunit\(l.int("id"))", extra: "matematika")
        add(vm.tr(l.str("titleKey")), vm.tr("search_exercise"), "matex\(l.int("id"))")
    }
    for l in vm.content.mat2 {
        add(vm.tr(l.str("titleKey")), vm.tr("search_lesson"), "mat2unit\(l.int("id"))", extra: "matematika")
        add(vm.tr(l.str("titleKey")), vm.tr("search_exercise"), "mat2ex\(l.int("id"))")
    }
    for l in vm.content.mat3 {
        add(vm.tr(l.str("titleKey")), vm.tr("search_lesson"), "mat3unit\(l.int("id"))", extra: "matematika")
        add(vm.tr(l.str("titleKey")), vm.tr("search_exercise"), "mat3ex\(l.int("id"))")
    }
    for l in vm.content.mat4 {
        add(vm.tr(l.str("titleKey")), vm.tr("search_lesson"), "mat4unit\(l.int("id"))", extra: "matematika")
        add(vm.tr(l.str("titleKey")), vm.tr("search_exercise"), "mat4ex\(l.int("id"))")
    }
    add(vm.tr("Fyzika"), vm.tr("search_lesson"), "fyzyears", extra: "fyzika")
    add(vm.tr("fyz_year1"), vm.tr("search_lesson"), "fyzmap", extra: "rocnik")
    add(vm.tr("fyz_year2"), vm.tr("search_lesson"), "fyz2map", extra: "rocnik")
    add(vm.tr("fyz_year3"), vm.tr("search_lesson"), "fyz3map", extra: "rocnik")
    add(vm.tr("fyz_year4"), vm.tr("search_lesson"), "fyz4map", extra: "rocnik")
    for l in vm.content.fyz {
        add(vm.tr(l.str("titleKey")), vm.tr("search_lesson"), "fyzunit\(l.int("id"))", extra: "fyzika")
        add(vm.tr(l.str("titleKey")), vm.tr("search_exercise"), "fyzex\(l.int("id"))")
    }
    for l in vm.content.fyz2 {
        add(vm.tr(l.str("titleKey")), vm.tr("search_lesson"), "fyz2unit\(l.int("id"))", extra: "fyzika")
        add(vm.tr(l.str("titleKey")), vm.tr("search_exercise"), "fyz2ex\(l.int("id"))")
    }
    for l in vm.content.fyz3 {
        add(vm.tr(l.str("titleKey")), vm.tr("search_lesson"), "fyz3unit\(l.int("id"))", extra: "fyzika")
        add(vm.tr(l.str("titleKey")), vm.tr("search_exercise"), "fyz3ex\(l.int("id"))")
    }
    for l in vm.content.fyz4 {
        add(vm.tr(l.str("titleKey")), vm.tr("search_lesson"), "fyz4unit\(l.int("id"))", extra: "fyzika")
        add(vm.tr(l.str("titleKey")), vm.tr("search_exercise"), "fyz4ex\(l.int("id"))")
    }
    add(vm.tr("Programové vybavení"), vm.tr("search_lesson"), "swyears", extra: "office windows excel mysql")
    add(vm.tr("sw_year1"), vm.tr("search_lesson"), "swmap", extra: "rocnik")
    add(vm.tr("sw_year2"), vm.tr("search_lesson"), "sw2map", extra: "rocnik")
    add(vm.tr("sw_year3"), vm.tr("search_lesson"), "sw3map", extra: "rocnik")
    add(vm.tr("sw_year4"), vm.tr("search_lesson"), "sw4map", extra: "rocnik")
    for l in vm.content.sw {
        add(vm.tr(l.str("titleKey")), vm.tr("search_lesson"), "swunit\(l.int("id"))", extra: "windows")
        add(vm.tr(l.str("titleKey")), vm.tr("search_exercise"), "swex\(l.int("id"))")
    }
    for l in vm.content.sw2 {
        add(vm.tr(l.str("titleKey")), vm.tr("search_lesson"), "sw2unit\(l.int("id"))", extra: "word excel")
        add(vm.tr(l.str("titleKey")), vm.tr("search_exercise"), "sw2ex\(l.int("id"))")
    }
    for l in vm.content.sw3 {
        add(vm.tr(l.str("titleKey")), vm.tr("search_lesson"), "sw3unit\(l.int("id"))", extra: "access mysql")
        add(vm.tr(l.str("titleKey")), vm.tr("search_exercise"), "sw3ex\(l.int("id"))")
    }
    for l in vm.content.sw4 {
        add(vm.tr(l.str("titleKey")), vm.tr("search_lesson"), "sw4unit\(l.int("id"))", extra: "wordpress")
        add(vm.tr(l.str("titleKey")), vm.tr("search_exercise"), "sw4ex\(l.int("id"))")
    }
    add(vm.tr("Prezentační grafika"), vm.tr("search_lesson"), "pgyears", extra: "photoshop premiere illustrator")
    add(vm.tr("pg_year1"), vm.tr("search_lesson"), "pgmap", extra: "rocnik")
    add(vm.tr("pg_year2"), vm.tr("search_lesson"), "pg2map", extra: "rocnik")
    add(vm.tr("pg_year3"), vm.tr("search_lesson"), "pg3map", extra: "rocnik")
    add(vm.tr("pg_year4"), vm.tr("search_lesson"), "pg4map", extra: "rocnik")
    for l in vm.content.pg {
        add(vm.tr(l.str("titleKey")), vm.tr("search_lesson"), "pgunit\(l.int("id"))", extra: "photoshop")
        add(vm.tr(l.str("titleKey")), vm.tr("search_exercise"), "pgex\(l.int("id"))")
    }
    for l in vm.content.pg2 {
        add(vm.tr(l.str("titleKey")), vm.tr("search_lesson"), "pg2unit\(l.int("id"))", extra: "photoshop")
        add(vm.tr(l.str("titleKey")), vm.tr("search_exercise"), "pg2ex\(l.int("id"))")
    }
    for l in vm.content.pg3 {
        add(vm.tr(l.str("titleKey")), vm.tr("search_lesson"), "pg3unit\(l.int("id"))", extra: "premiere")
        add(vm.tr(l.str("titleKey")), vm.tr("search_exercise"), "pg3ex\(l.int("id"))")
    }
    for l in vm.content.pg4 {
        add(vm.tr(l.str("titleKey")), vm.tr("search_lesson"), "pg4unit\(l.int("id"))", extra: "illustrator")
        add(vm.tr(l.str("titleKey")), vm.tr("search_exercise"), "pg4ex\(l.int("id"))")
    }
    add(vm.tr("Technická grafika"), vm.tr("search_lesson"), "tgyears", extra: "autocad grafika")
    add(vm.tr("tg_year1"), vm.tr("search_lesson"), "tgmap", extra: "rocnik")
    add(vm.tr("tg_year2"), vm.tr("search_lesson"), "tg2map", extra: "rocnik")
    add(vm.tr("tg_year3"), vm.tr("search_lesson"), "tg3map", extra: "rocnik")
    add(vm.tr("tg_year4"), vm.tr("search_lesson"), "tg4map", extra: "rocnik")
    for l in vm.content.tg {
        add(vm.tr(l.str("titleKey")), vm.tr("search_lesson"), "tgunit\(l.int("id"))", extra: "autocad")
        add(vm.tr(l.str("titleKey")), vm.tr("search_exercise"), "tgex\(l.int("id"))")
    }
    for l in vm.content.tg2 {
        add(vm.tr(l.str("titleKey")), vm.tr("search_lesson"), "tg2unit\(l.int("id"))", extra: "autocad")
        add(vm.tr(l.str("titleKey")), vm.tr("search_exercise"), "tg2ex\(l.int("id"))")
    }
    for l in vm.content.tg3 {
        add(vm.tr(l.str("titleKey")), vm.tr("search_lesson"), "tg3unit\(l.int("id"))", extra: "autocad")
        add(vm.tr(l.str("titleKey")), vm.tr("search_exercise"), "tg3ex\(l.int("id"))")
    }
    for l in vm.content.tg4 {
        add(vm.tr(l.str("titleKey")), vm.tr("search_lesson"), "tg4unit\(l.int("id"))", extra: "autocad")
        add(vm.tr(l.str("titleKey")), vm.tr("search_exercise"), "tg4ex\(l.int("id"))")
    }
    add(vm.tr("Programování"), vm.tr("search_lesson"), "csyears", extra: "csharp c#")
    add(vm.tr("cs_year1"), vm.tr("search_lesson"), "csmap", extra: "rocnik")
    add(vm.tr("cs_year2"), vm.tr("search_lesson"), "cs2map", extra: "rocnik")
    add(vm.tr("cs_year3"), vm.tr("search_lesson"), "cs3map", extra: "rocnik")
    add(vm.tr("cs_year4"), vm.tr("search_lesson"), "cs4map", extra: "rocnik")
    for l in vm.content.cs {
        add(vm.tr(l.str("titleKey")), vm.tr("search_lesson"), "csunit\(l.int("id"))", extra: "csharp")
    }
    for l in vm.content.cs2 {
        add(vm.tr(l.str("titleKey")), vm.tr("search_lesson"), "cs2unit\(l.int("id"))", extra: "csharp")
    }
    for l in vm.content.cs3 {
        add(vm.tr(l.str("titleKey")), vm.tr("search_lesson"), "cs3unit\(l.int("id"))", extra: "csharp")
    }
    for l in vm.content.cs4 {
        add(vm.tr(l.str("titleKey")), vm.tr("search_lesson"), "cs4unit\(l.int("id"))", extra: "csharp")
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
        let units = vm.content.german
        let finished = units.map { u -> Bool in
            let names = u.strs("names")
            return u.bool("unlocked") && !names.isEmpty && names.indices.allSatisfy { vm.progress.germanDone(u.int("id"), $0 + 1) }
        }
        let current = units.indices.first { units[$0].bool("unlocked") && !finished[$0] }
        #if os(macOS)
        PathMap(
            nodes: units.enumerated().map { i, u in
                let unlocked = u.bool("unlocked")
                return MapNode(
                    id: "unit-\(i)",
                    label: u.str("title"),
                    locked: !unlocked,
                    done: finished[i],
                    current: i == current,
                    finish: i == units.count - 1,
                    destination: unlocked ? .unitMap(u.int("id")) : nil
                )
            },
            palette: vm.palette,
            litUntil: CGFloat(max((current ?? units.count) - 1, 0)),
            nodeSize: MacChrome.pathNode,
            mx: 100,
            my: 140,
            spac: 196,
            gap: 280,
            wave: 18,
            heading: (vm.tr("roadmap_title"), vm.tr("roadmap_sub"), finished.filter { $0 }.count, units.count)
        )
        .pageTitle(vm.tr("roadmap_title"))
        #else
        List {
            ForEach(Array(units.enumerated()), id: \.offset) { i, u in
                let row = StepRow(index: i + 1, title: u.str("title"), done: finished[i], current: i == current, locked: !u.bool("unlocked"))
                if u.bool("unlocked") {
                    NavigationLink(value: Route.unitMap(u.int("id"))) { row }
                } else {
                    row
                }
            }
            .stepRows()
        }
        .navigationTitle(vm.tr("roadmap_title"))
        .navigationSubtitle(vm.tr("roadmap_sub"))
        #endif
    }
}

struct UnitMapScreen: View {
    @ObservedObject var vm: AppModel
    let unitId: Int

    var body: some View {
        if let u = vm.content.germanUnit(unitId) {
            let names = u.strs("names")
            let next = names.indices.first { !vm.progress.germanDone(unitId, $0 + 1) }
            #if os(macOS)
            let branch: (index: Int, node: MapNode)? = u.has("branch")
                ? (
                    1,
                    MapNode(
                        id: "vocab-\(unitId)",
                        label: vm.tr("Vokabeltraining"),
                        locked: false,
                        done: vm.progress.vocabDone(unitId),
                        current: false,
                        destination: .vocab(unitId),
                        symbol: "character.book.closed.fill"
                    )
                )
                : nil
            PathMap(
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
                litUntil: CGFloat(next ?? names.count),
                nodeSize: MacChrome.pathNode,
                mx: 100,
                my: 136,
                spac: 190,
                gap: 268,
                wave: 16,
                branch: branch,
                heading: (
                    u.str("title"),
                    vm.tr(u.str("sub")),
                    names.indices.filter { vm.progress.germanDone(unitId, $0 + 1) }.count,
                    names.count
                )
            )
            .pageTitle(u.str("title"))
            #else
            List {
                Section {
                    ForEach(names.indices, id: \.self) { i in
                        NavigationLink(value: Route.germanEx(unitId, i + 1)) {
                            StepRow(index: i + 1, title: names[i], done: vm.progress.germanDone(unitId, i + 1), current: i == next)
                        }
                    }
                    .stepRows()
                }
                if u.has("branch") {
                    Section {
                        NavigationLink(value: Route.vocab(unitId)) {
                            StepRow(index: nil, title: vm.tr("Vokabeltraining"), done: vm.progress.vocabDone(unitId), symbol: "character.book.closed.fill")
                        }
                        .stepRows()
                    }
                }
            }
            .navigationTitle(u.str("title"))
            .navigationSubtitle(vm.tr(u.str("sub")))
            #endif
        }
    }
}

/// One card of a subject's index: a year, or a branch such as grammar.
struct IndexEntry {
    let title: String
    let subtitle: String
    let route: Route
    var badge: String? = nil
    var symbol: String? = nil
}

/// A subject's index. The Mac sets the title in the page; the iPhone keeps it in the bar.
struct IndexScreen: View {
    @ObservedObject var vm: AppModel
    let title: String
    let subtitle: String
    let entries: [IndexEntry]

    var body: some View {
        IndexGrid(count: entries.count, title: title, subtitle: subtitle) { i in
            let entry = entries[i]
            NavigationLink(value: entry.route) {
                IndexCard(
                    title: entry.title,
                    subtitle: entry.subtitle,
                    badge: entry.badge,
                    symbol: entry.symbol,
                    progress: routeProgress(vm, entry.route)
                )
            }
            .buttonStyle(PressScaleStyle())
        }
        .pageTitle(title, subtitle: subtitle)
    }

    /// Four years that follow one naming pattern, numbered 1 to 4.
    static func years(_ vm: AppModel, _ rows: [(title: String, subtitle: String, route: Route)]) -> [IndexEntry] {
        rows.enumerated().map { i, row in
            IndexEntry(title: vm.tr(row.title), subtitle: vm.tr(row.subtitle), route: row.route, badge: "\(i + 1)")
        }
    }
}

struct NetYearsScreen: View {
    @ObservedObject var vm: AppModel

    var body: some View {
        IndexScreen(vm: vm, title: vm.tr("net_years_title"), subtitle: vm.tr("net_years_sub"), entries: IndexScreen.years(vm, [
            ("net_year1", "net_sub", .netMap),
            ("net_year2", "net2_sub", .net2Map),
            ("net_year3", "net3_sub", .net3Map),
            ("net_year4", "net4_sub", .net4Map),
        ]))
    }
}

struct HwYearsScreen: View {
    @ObservedObject var vm: AppModel

    var body: some View {
        IndexScreen(vm: vm, title: vm.tr("Technické vybavení"), subtitle: vm.tr("hw_years_sub"), entries: IndexScreen.years(vm, [
            ("hw_year1", "hw_sub", .hwMap),
            ("hw_year2", "hw2_sub", .hw2Map),
            ("hw_year3", "hw3_sub", .hw3Map),
            ("hw_year4", "hw4_sub", .hw4Map),
        ]))
    }
}

struct DeHomeScreen: View {
    @ObservedObject var vm: AppModel

    var body: some View {
        IndexScreen(
            vm: vm,
            title: vm.tr("Deutsch"),
            subtitle: vm.tr("de_home_sub"),
            entries: [
                IndexEntry(title: vm.tr("de_book_title"), subtitle: vm.tr("de_book_sub"), route: .roadmap, symbol: "text.book.closed.fill"),
            ] + IndexScreen.years(vm, (1...4).map { ("de_year\($0)", "de_y\($0)_sub", Route.deMap($0)) })
        )
    }
}

struct EnYearsScreen: View {
    @ObservedObject var vm: AppModel

    var body: some View {
        IndexScreen(
            vm: vm,
            title: vm.tr("English"),
            subtitle: vm.tr("en_years_sub"),
            entries: IndexScreen.years(vm, (1...4).map { ("en_year\($0)", "en_y\($0)_sub", Route.enMap($0)) })
        )
    }
}

struct OnYearsScreen: View {
    @ObservedObject var vm: AppModel

    var body: some View {
        IndexScreen(vm: vm, title: vm.tr("Občanská nauka"), subtitle: vm.tr("on_years_sub"), entries: IndexScreen.years(vm, [
            ("on_year1", "on_sub", .onMap),
            ("on_year2", "on2_sub", .on2Map),
            ("on_year3", "on3_sub", .on3Map),
            ("on_year4", "on4_sub", .on4Map),
        ]))
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
        let next = rows.firstIndex { !$0.done }
        #if os(macOS)
        PathMap(
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
            wave: 16,
            heading: (title, subtitle, rows.filter(\.done).count, rows.count)
        )
        .pageTitle(title)
        #else
        List {
            Section {
                ForEach(Array(rows.enumerated()), id: \.element.id) { i, row in
                    NavigationLink(value: row.destination) {
                        StepRow(index: i + 1, title: row.title, done: row.done, current: i == next)
                    }
                }
                .stepRows()
            } header: {
                ProgressLine(done: rows.filter(\.done).count, total: rows.count)
                    .textCase(nil)
                    .padding(.bottom, 6)
            }
        }
        .navigationTitle(title)
        .navigationSubtitle(subtitle)
        #endif
    }
}

struct SlidesScreen: View {
    @ObservedObject var vm: AppModel
    let title: String
    let subtitle: String
    let slides: [J]
    let next: Route
    @State private var idx = 0
    @Environment(\.palette) private var palette
    @Environment(\.accent) private var accent

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
                    .id(idx)
                    .transition(.opacity)
            }
            #endif
        }
        #if os(macOS)
        .animation(Motion.settle, value: idx)
        #endif
        .detailScreen(title, subtitle: subtitle)
        .glassBottomBar {
            VStack(spacing: 12) {
                HStack(spacing: 6) {
                    ForEach(slides.indices, id: \.self) { i in
                        Capsule()
                            .fill(i == idx ? accent.solid : palette.ink3.opacity(0.35))
                            .frame(width: i == idx ? 22 : 6, height: 6)
                    }
                }
                .animation(Motion.settle, value: idx)
                .accessibilityHidden(true)
                HStack(spacing: 10) {
                    if idx > 0 {
                        Button {
                            withAnimation { idx -= 1 }
                        } label: {
                            Image(systemName: "chevron.left")
                                .frame(minWidth: 24)
                        }
                        .buttonStyle(SolidButtonStyle(wide: false, quiet: true))
                        .accessibilityLabel(vm.tr("back"))
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
            #if os(macOS)
            .macPage(MacChrome.actionWidth)
            #endif
        }
    }

    private func slidePage(_ slide: J) -> some View {
        ScrollView {
            SlideBody(slide: slide)
                .padding(.horizontal, Page.inset)
                .padding(.top, onMac(30, 20))
                .padding(.bottom, 28)
                .frame(maxWidth: 820)
                .frame(maxWidth: .infinity, alignment: .top)
        }
    }
}

/// One explanation slide: label, title, tip and numbered points.
struct SlideBody: View {
    let slide: J
    @Environment(\.palette) private var palette
    @Environment(\.accent) private var accent

    var body: some View {
        VStack(alignment: .leading, spacing: onMac(20, 16)) {
            VStack(alignment: .leading, spacing: 8) {
                Text(slide.str("kicker"))
                    .kicker()
                    .foregroundStyle(accent.text)
                Text(slide.str("title"))
                    .display(onMac(36, 28), relativeTo: .title)
                    .foregroundStyle(palette.ink)
                    .fixedSize(horizontal: false, vertical: true)
            }
            if let tip = slide.strOrNull("tip") {
                HStack(alignment: .top, spacing: 12) {
                    Image(systemName: "lightbulb.fill")
                        .foregroundStyle(accent.text)
                        .padding(.top, 2)
                    Text(tip)
                        .font(onMac(Font.title3, Font.subheadline))
                        .foregroundStyle(palette.ink)
                        .fixedSize(horizontal: false, vertical: true)
                        .frame(maxWidth: .infinity, alignment: .leading)
                }
                .padding(16)
                .frame(maxWidth: .infinity, alignment: .leading)
                .background(accent.soft, in: RoundedRectangle(cornerRadius: 16, style: .continuous))
            }
            slideLines(slide.strs("lines"))
        }
        .frame(maxWidth: .infinity, alignment: .leading)
    }

    /// Points read as a numbered list on the page, set apart by rules rather than boxes.
    private func slideLines(_ lines: [String]) -> some View {
        let points = studyPoints(lines)
        return VStack(alignment: .leading, spacing: 0) {
            ForEach(Array(points.enumerated()), id: \.offset) { index, point in
                HStack(alignment: .firstTextBaseline, spacing: 16) {
                    Text(point.0)
                        .display(onMac(24, 20), relativeTo: .title3)
                        .foregroundStyle(accent.text)
                        .frame(minWidth: onMac(34, 28), alignment: .leading)
                    Text(point.1)
                        .font(onMac(Font.title3, Font.body))
                        .foregroundStyle(palette.ink)
                        .lineSpacing(3)
                        .fixedSize(horizontal: false, vertical: true)
                        .frame(maxWidth: .infinity, alignment: .leading)
                }
                .padding(.vertical, onMac(16, 14))
                if index < points.count - 1 {
                    Rectangle().fill(palette.line).frame(height: 1)
                }
            }
        }
    }

    /// Lines that end in the middle of a sentence stay one point. A leading
    /// "1. " is the point number, not part of the sentence.
    private func studyPoints(_ lines: [String]) -> [(String, String)] {
        var chunks: [String] = []
        var buffer = ""
        func flush() {
            let text = buffer.trimmingCharacters(in: .whitespacesAndNewlines)
            if !text.isEmpty { chunks.append(text) }
            buffer = ""
        }
        for line in lines {
            let trimmed = line.trimmingCharacters(in: .whitespacesAndNewlines)
            if trimmed.isEmpty { continue }
            if buffer.isEmpty {
                buffer = trimmed
            } else if continuesStudyPoint(buffer) {
                buffer += " " + trimmed
            } else {
                flush()
                buffer = trimmed
            }
        }
        flush()
        return chunks.enumerated().map { index, chunk in
            let pattern = #"^(\d+)\.\s+(.*)$"#
            if let regex = try? NSRegularExpression(pattern: pattern),
               let match = regex.firstMatch(in: chunk, range: NSRange(chunk.startIndex..., in: chunk)),
               let number = Range(match.range(at: 1), in: chunk),
               let rest = Range(match.range(at: 2), in: chunk),
               !continuesStudyPoint(String(chunk[rest])) {
                return (String(chunk[number]), String(chunk[rest]))
            }
            return (String(index + 1), chunk)
        }
    }

    private func continuesStudyPoint(_ text: String) -> Bool {
        guard let last = text.last else { return false }
        return last == "," || last == ":" || last == ";" || last == "–" || last == "-" || last == "("
    }
}

struct LitYearsScreen: View {
    @ObservedObject var vm: AppModel

    var body: some View {
        IndexScreen(vm: vm, title: vm.tr("Literatura"), subtitle: vm.tr("lit_years_sub"), entries: IndexScreen.years(vm, [
            ("lit_year1", "lit_sub", .litMap),
            ("lit_year2", "lit2_sub", .lit2Map),
            ("lit_year3", "lit3_sub", .lit3Map),
            ("lit_year4", "lit4_sub", .lit4Map),
        ]))
    }
}

struct MatYearsScreen: View {
    @ObservedObject var vm: AppModel

    var body: some View {
        IndexScreen(
            vm: vm,
            title: vm.tr("Matematika"),
            subtitle: vm.tr("mat_years_sub"),
            entries: [
                IndexEntry(title: vm.tr("mat0_year"), subtitle: vm.tr("mat0_sub"), route: .mat0Map, badge: "ZŠ"),
            ] + IndexScreen.years(vm, [
                ("mat_year1", "mat_sub", .matMap),
                ("mat_year2", "mat2_sub", .mat2Map),
                ("mat_year3", "mat3_sub", .mat3Map),
                ("mat_year4", "mat4_sub", .mat4Map),
            ])
        )
    }
}

struct CsYearsScreen: View {
    @ObservedObject var vm: AppModel

    var body: some View {
        IndexScreen(vm: vm, title: vm.tr("Programování"), subtitle: vm.tr("cs_years_sub"), entries: IndexScreen.years(vm, [
            ("cs_year1", "cs_sub", .csMap),
            ("cs_year2", "cs2_sub", .cs2Map),
            ("cs_year3", "cs3_sub", .cs3Map),
            ("cs_year4", "cs4_sub", .cs4Map),
        ]))
    }
}

struct SwYearsScreen: View {
    @ObservedObject var vm: AppModel

    var body: some View {
        IndexScreen(vm: vm, title: vm.tr("Programové vybavení"), subtitle: vm.tr("sw_years_sub"), entries: IndexScreen.years(vm, [
            ("sw_year1", "sw_sub", .swMap),
            ("sw_year2", "sw2_sub", .sw2Map),
            ("sw_year3", "sw3_sub", .sw3Map),
            ("sw_year4", "sw4_sub", .sw4Map),
        ]))
    }
}

struct PgYearsScreen: View {
    @ObservedObject var vm: AppModel

    var body: some View {
        IndexScreen(vm: vm, title: vm.tr("Prezentační grafika"), subtitle: vm.tr("pg_years_sub"), entries: IndexScreen.years(vm, [
            ("pg_year1", "pg_sub", .pgMap),
            ("pg_year2", "pg2_sub", .pg2Map),
            ("pg_year3", "pg3_sub", .pg3Map),
            ("pg_year4", "pg4_sub", .pg4Map),
        ]))
    }
}

struct TgYearsScreen: View {
    @ObservedObject var vm: AppModel

    var body: some View {
        IndexScreen(vm: vm, title: vm.tr("Technická grafika"), subtitle: vm.tr("tg_years_sub"), entries: IndexScreen.years(vm, [
            ("tg_year1", "tg_sub", .tgMap),
            ("tg_year2", "tg2_sub", .tg2Map),
            ("tg_year3", "tg3_sub", .tg3Map),
            ("tg_year4", "tg4_sub", .tg4Map),
        ]))
    }
}

struct FyzYearsScreen: View {
    @ObservedObject var vm: AppModel

    var body: some View {
        IndexScreen(vm: vm, title: vm.tr("Fyzika"), subtitle: vm.tr("fyz_years_sub"), entries: IndexScreen.years(vm, [
            ("fyz_year1", "fyz_sub", .fyzMap),
            ("fyz_year2", "fyz2_sub", .fyz2Map),
            ("fyz_year3", "fyz3_sub", .fyz3Map),
            ("fyz_year4", "fyz4_sub", .fyz4Map),
        ]))
    }
}

struct SciMapScreen: View {
    @ObservedObject var vm: AppModel

    var body: some View {
        IndexScreen(vm: vm, title: vm.tr("Základy přírodních věd"), subtitle: vm.tr("sci_sub"), entries: [
            IndexEntry(title: vm.tr("Chemie"), subtitle: vm.tr("chem_sub"), route: .chemMap, symbol: "flask.fill"),
            IndexEntry(title: vm.tr("Biologie"), subtitle: vm.tr("bio_sub"), route: .bioMap, symbol: "leaf.fill"),
        ])
    }
}

struct CzechMapScreen: View {
    @ObservedObject var vm: AppModel

    var body: some View {
        IndexScreen(vm: vm, title: vm.tr("Český jazyk a literatura"), subtitle: vm.tr("czech_sub"), entries: [
            IndexEntry(title: vm.tr("Literatura"), subtitle: vm.tr("lit_years_sub"), route: .litYears, symbol: "book.closed.fill"),
            IndexEntry(title: vm.tr("Mluvnice"), subtitle: vm.tr("mluv_sub"), route: .mluvnice, symbol: "textformat"),
            IndexEntry(title: vm.tr("Maturitní četba"), subtitle: vm.tr("reading_sub"), route: .readingList, symbol: "books.vertical.fill"),
        ])
    }
}

struct BookListScreen: View {
    @ObservedObject var vm: AppModel
    @Environment(\.palette) private var palette
    @Environment(\.accent) private var accent

    var body: some View {
        let books = vm.content.books
        let read = books.filter { done($0) }.count
        #if os(macOS)
        ScrollView {
            VStack(alignment: .leading, spacing: 24) {
                PageHeader(title: vm.tr("Maturitní četba"), subtitle: vm.tr("reading_sub"), progress: (read, books.count))
                LazyVGrid(columns: [GridItem(.adaptive(minimum: 280), spacing: 16, alignment: .top)], spacing: 16) {
                    ForEach(Array(books.enumerated()), id: \.offset) { _, b in
                        NavigationLink(value: Route.book(b.str("id"))) {
                            bookCard(b)
                        }
                        .buttonStyle(PressScaleStyle())
                    }
                }
            }
            .padding(Page.inset)
            .macPage()
        }
        .pageTitle(vm.tr("Maturitní četba"), subtitle: vm.tr("reading_sub"))
        #else
        List {
            Section {
                ForEach(Array(books.enumerated()), id: \.offset) { _, b in
                    NavigationLink(value: Route.book(b.str("id"))) {
                        bookRow(b)
                    }
                }
                .stepRows()
            } header: {
                ProgressLine(done: read, total: books.count)
                    .textCase(nil)
                    .padding(.bottom, 6)
            }
        }
        .navigationTitle(vm.tr("Maturitní četba"))
        .navigationSubtitle(vm.tr("reading_sub"))
        #endif
    }

    private func done(_ b: J) -> Bool {
        (!b.arr("quiz").isEmpty && vm.progress.bookQuiz(b.str("id")))
            || (!b.arr("plot").isEmpty && vm.progress.bookPlot(b.str("id")))
    }

    private func genre(_ b: J) -> String {
        let genre = b.str("genre")
        return genre.isEmpty ? vm.tr(b.str("subKey")) : genre
    }

    #if os(macOS)
    /// A book as an index card: author over the title, the genre along the bottom edge.
    private func bookCard(_ b: J) -> some View {
        let parts = bookTitleParts(b.str("title"))
        return VStack(alignment: .leading, spacing: 6) {
            if let author = parts.author {
                Text(author)
                    .kicker()
                    .foregroundStyle(accent.text)
                    .lineLimit(1)
            }
            Text(parts.title)
                .display(22, relativeTo: .title3)
                .foregroundStyle(palette.ink)
                .multilineTextAlignment(.leading)
                .lineLimit(3)
            Spacer(minLength: 0)
            HStack(spacing: 8) {
                Text(genre(b))
                    .font(.subheadline)
                    .foregroundStyle(palette.ink2)
                    .lineLimit(1)
                Spacer(minLength: 0)
                if done(b) {
                    Image(systemName: "checkmark.circle.fill")
                        .foregroundStyle(palette.success)
                        .accessibilityLabel(vm.tr("book_done"))
                }
            }
        }
        .padding(20)
        .frame(maxWidth: .infinity, minHeight: 156, alignment: .topLeading)
        .softSurface(cornerRadius: 20)
        .contentShape(RoundedRectangle(cornerRadius: 20, style: .continuous))
    }
    #endif

    #if os(iOS)
    private func bookRow(_ b: J) -> some View {
        let parts = bookTitleParts(b.str("title"))
        return HStack(alignment: .top, spacing: 14) {
            Badge(symbol: done(b) ? "checkmark" : "book.closed.fill", size: 34, filled: done(b))
            VStack(alignment: .leading, spacing: 2) {
                Text(parts.title)
                    .font(.body.weight(.semibold))
                    .foregroundStyle(palette.ink)
                if let author = parts.author {
                    Text(author)
                        .font(.subheadline)
                        .foregroundStyle(palette.ink2)
                }
                Text(genre(b))
                    .font(.caption)
                    .foregroundStyle(palette.ink3)
            }
        }
        .padding(.vertical, 2)
    }
    #endif
}

struct BookScreen: View {
    @ObservedObject var vm: AppModel
    let id: String
    @Environment(\.palette) private var palette
    @Environment(\.accent) private var accent

    var body: some View {
        if let b = vm.content.book(id) {
            let hasQuiz = !b.arr("quiz").isEmpty
            let hasPlot = !b.arr("plot").isEmpty
            let notes = b.arr("notes")
            let parts = bookTitleParts(b.str("title"))
            ScrollView {
                VStack(alignment: .leading, spacing: onMac(32, 24)) {
                    VStack(alignment: .leading, spacing: 8) {
                        // The iPhone already names the book and its author in the navigation bar.
                        #if os(macOS)
                        if let author = parts.author {
                            Text(author)
                                .kicker()
                                .foregroundStyle(accent.text)
                        }
                        Text(parts.title)
                            .display(44, relativeTo: .largeTitle)
                            .foregroundStyle(palette.ink)
                            .fixedSize(horizontal: false, vertical: true)
                        #endif
                        if !b.str("genre").isEmpty {
                            Text(b.str("genre"))
                                .font(onMac(Font.title3, Font.subheadline).weight(.medium))
                                .foregroundStyle(palette.ink2)
                        }
                        Text(vm.tr("book_study_hint"))
                            .font(onMac(Font.title3, Font.subheadline))
                            .foregroundStyle(palette.ink2)
                            .fixedSize(horizontal: false, vertical: true)
                            .padding(.top, 4)
                    }

                    if hasQuiz || hasPlot {
                        VStack(alignment: .leading, spacing: 14) {
                            SectionTitle(text: vm.tr("book_practice"))
                            LazyVGrid(columns: [GridItem(.adaptive(minimum: 300), spacing: 14)], spacing: onMac(14, 10)) {
                                if hasQuiz {
                                    NavigationLink(value: Route.bookQuiz(id)) {
                                        BookActionCard(
                                            title: vm.tr("book_quiz_heading"),
                                            subtitle: vm.tr("lit_quiz_sub"),
                                            systemImage: "questionmark",
                                            done: vm.progress.bookQuiz(id),
                                            doneLabel: vm.tr("book_done")
                                        )
                                    }
                                    .buttonStyle(PressScaleStyle())
                                }
                                if hasPlot {
                                    NavigationLink(value: Route.bookPlot(id)) {
                                        BookActionCard(
                                            title: vm.tr("lit_plot_title"),
                                            subtitle: vm.tr("lit_plot_sub"),
                                            systemImage: "list.number",
                                            done: vm.progress.bookPlot(id),
                                            doneLabel: vm.tr("book_done")
                                        )
                                    }
                                    .buttonStyle(PressScaleStyle())
                                }
                            }
                        }
                    }

                    if !notes.isEmpty {
                        VStack(alignment: .leading, spacing: 14) {
                            SectionTitle(text: vm.tr("book_notes"))
                            LazyVGrid(columns: [GridItem(.adaptive(minimum: 360), spacing: 16, alignment: .top)], alignment: .leading, spacing: onMac(16, 12)) {
                                ForEach(Array(notes.enumerated()), id: \.offset) { _, note in
                                    BookNoteCard(note: note)
                                }
                            }
                        }
                    }
                }
                .padding(Page.inset)
                .padding(.bottom, 24)
                .macPage()
            }
            .pageTitle(parts.title, subtitle: parts.author ?? vm.tr(b.str("subKey")))
            #if os(iOS)
            .navigationBarTitleDisplayMode(.inline)
            #endif
        }
    }
}

private struct BookActionCard: View {
    let title: String
    let subtitle: String
    let systemImage: String
    let done: Bool
    let doneLabel: String
    @Environment(\.palette) private var palette

    var body: some View {
        HStack(alignment: .center, spacing: 16) {
            Badge(symbol: systemImage, size: onMac(48, 44))
            VStack(alignment: .leading, spacing: 3) {
                Text(title)
                    .display(onMac(21, 18), relativeTo: .headline)
                    .foregroundStyle(palette.ink)
                Text(subtitle)
                    .font(onMac(Font.body, Font.subheadline))
                    .foregroundStyle(palette.ink2)
                    .multilineTextAlignment(.leading)
                    .fixedSize(horizontal: false, vertical: true)
            }
            Spacer(minLength: 8)
            if done {
                Image(systemName: "checkmark.circle.fill")
                    .font(onMac(Font.title2, Font.title3))
                    .foregroundStyle(palette.success)
                    .accessibilityLabel(doneLabel)
            } else {
                Image(systemName: "chevron.right")
                    .font(.body.weight(.semibold))
                    .foregroundStyle(palette.ink3)
            }
        }
        .padding(onMac(20, 16))
        .frame(maxWidth: .infinity, maxHeight: .infinity, alignment: .leading)
        .softSurface(cornerRadius: 20)
        .contentShape(RoundedRectangle(cornerRadius: 20, style: .continuous))
    }
}

private struct BookNoteCard: View {
    let note: J
    @Environment(\.palette) private var palette
    @Environment(\.accent) private var accent

    var body: some View {
        GlassCard {
            VStack(alignment: .leading, spacing: 14) {
                HStack(spacing: 10) {
                    Image(systemName: noteSymbol(note.str("icon")))
                        .foregroundStyle(accent.text)
                    Text(note.str("title"))
                        .display(onMac(22, 19), relativeTo: .headline)
                        .foregroundStyle(palette.ink)
                }
                VStack(alignment: .leading, spacing: 10) {
                    ForEach(Array(note.strs("lines").enumerated()), id: \.offset) { _, line in
                        HStack(alignment: .firstTextBaseline, spacing: 10) {
                            Circle().fill(accent.solid).frame(width: 5, height: 5)
                                .alignmentGuide(.firstTextBaseline) { $0[.bottom] + 3 }
                            Text(line)
                                .font(onMac(Font.title3, Font.body))
                                .foregroundStyle(palette.ink)
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

/// A pushed screen in the colour of its subject, on the paper.
struct RouteDestination: View {
    @ObservedObject var vm: AppModel
    let route: Route

    var body: some View {
        RouteContent(vm: vm, route: route)
            .accented(vm.accent(for: route))
            .environment(\.pageKicker, kicker)
            .paper()
    }

    /// The subject's name, for every screen below the subject's own index.
    private var kicker: String? {
        guard route.stack.count > 1, let key = route.subject?.key,
              let entry = vm.content.subjects.first(where: { $0.str("icon") == key }) else { return nil }
        return vm.tr(entry.str("key"))
    }
}

private struct RouteContent: View {
    @ObservedObject var vm: AppModel
    let route: Route

    var body: some View {
        switch route {
        case .welcome, .subjects:
            PracticeHome(vm: vm)
        case .stats:
            StatsScreen(vm: vm)
        case .deHome:
            DeHomeScreen(vm: vm)
        case .deMap(let year):
            LessonListScreen(
                vm: vm,
                title: vm.tr("de_year\(year)"),
                subtitle: vm.tr("de_y\(year)_sub"),
                rows: vm.content.deYear(year).map { l in
                    let id = l.int("id")
                    return LessonRow(
                        id: "d\(year)\(id)",
                        title: vm.tr(l.str("titleKey")),
                        done: vm.progress.deDone(year, id),
                        destination: .deLesson(year, id)
                    )
                }
            )
        case .deLesson(let year, let id):
            if let l = vm.content.deLesson(year, id) {
                SlidesScreen(vm: vm, title: vm.tr(l.str("titleKey")), subtitle: vm.tr(l.str("subKey")), slides: l.arr("slides"), next: .deEx(year, id))
            }
        case .deEx(let year, let id):
            EngExScreen(vm: vm, year: year, id: id, voice: "de-DE", lesson: { $0.content.deLesson($1, $2) }, mark: { $0.markDe($1, $2) })
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
        case .net2Map:
            ItYearList(vm: vm, course: "net2", titleKey: "net_year2", subKey: "net2_sub") { .net2Lesson($0) }
        case .net2Lesson(let id):
            ItSlides(vm: vm, course: "net2", id: id, next: .net2Ex(id))
        case .net2Ex(let id):
            ChoiceLessonScreen(vm: vm, lesson: vm.content.itLesson("net2", id)) { vm.markCourse("net2", id) }
        case .net3Map:
            ItYearList(vm: vm, course: "net3", titleKey: "net_year3", subKey: "net3_sub") { .net3Lesson($0) }
        case .net3Lesson(let id):
            ItSlides(vm: vm, course: "net3", id: id, next: .net3Ex(id))
        case .net3Ex(let id):
            ChoiceLessonScreen(vm: vm, lesson: vm.content.itLesson("net3", id)) { vm.markCourse("net3", id) }
        case .net4Map:
            ItYearList(vm: vm, course: "net4", titleKey: "net_year4", subKey: "net4_sub") { .net4Lesson($0) }
        case .net4Lesson(let id):
            ItSlides(vm: vm, course: "net4", id: id, next: .net4Ex(id))
        case .net4Ex(let id):
            ChoiceLessonScreen(vm: vm, lesson: vm.content.itLesson("net4", id)) { vm.markCourse("net4", id) }
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
        case .hw2Map:
            ItYearList(vm: vm, course: "hw2", titleKey: "hw_year2", subKey: "hw2_sub") { .hw2Lesson($0) }
        case .hw2Lesson(let id):
            ItSlides(vm: vm, course: "hw2", id: id, next: .hw2Ex(id))
        case .hw2Ex(let id):
            ChoiceLessonScreen(vm: vm, lesson: vm.content.itLesson("hw2", id)) { vm.markCourse("hw2", id) }
        case .hw3Map:
            ItYearList(vm: vm, course: "hw3", titleKey: "hw_year3", subKey: "hw3_sub") { .hw3Lesson($0) }
        case .hw3Lesson(let id):
            ItSlides(vm: vm, course: "hw3", id: id, next: .hw3Ex(id))
        case .hw3Ex(let id):
            ChoiceLessonScreen(vm: vm, lesson: vm.content.itLesson("hw3", id)) { vm.markCourse("hw3", id) }
        case .hw4Map:
            ItYearList(vm: vm, course: "hw4", titleKey: "hw_year4", subKey: "hw4_sub") { .hw4Lesson($0) }
        case .hw4Lesson(let id):
            ItSlides(vm: vm, course: "hw4", id: id, next: .hw4Ex(id))
        case .hw4Ex(let id):
            ChoiceLessonScreen(vm: vm, lesson: vm.content.itLesson("hw4", id)) { vm.markCourse("hw4", id) }
        case .onYears:
            OnYearsScreen(vm: vm)
        case .enYears:
            EnYearsScreen(vm: vm)
        case .enMap(let year):
            LessonListScreen(
                vm: vm,
                title: vm.tr("en_year\(year)"),
                subtitle: vm.tr("en_y\(year)_sub"),
                rows: vm.content.enYear(year).map { l in
                    let id = l.int("id")
                    return LessonRow(
                        id: "e\(year)\(id)",
                        title: vm.tr(l.str("titleKey")),
                        done: vm.progress.enDone(year, id),
                        destination: .enLesson(year, id)
                    )
                }
            )
        case .enLesson(let year, let id):
            if let l = vm.content.enLesson(year, id) {
                SlidesScreen(vm: vm, title: vm.tr(l.str("titleKey")), subtitle: vm.tr(l.str("subKey")), slides: l.arr("slides"), next: .enEx(year, id))
            }
        case .enEx(let year, let id):
            EngExScreen(vm: vm, year: year, id: id)
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
        case .matYears:
            MatYearsScreen(vm: vm)
        case .mat0Map:
            LessonListScreen(
                vm: vm,
                title: vm.tr("mat0_year"),
                subtitle: vm.tr("mat0_sub"),
                rows: vm.content.mat0.map { l in
                    let id = l.int("id")
                    return LessonRow(id: "m0\(id)", title: vm.tr(l.str("titleKey")), done: vm.progress.mat0Done(id), destination: .mat0Lesson(id))
                }
            )
        case .mat0Lesson(let id):
            if let l = vm.content.mat0Lesson(id) {
                SlidesScreen(vm: vm, title: vm.tr(l.str("titleKey")), subtitle: vm.tr(l.str("subKey")), slides: l.arr("slides"), next: .mat0Ex(id))
            }
        case .mat0Ex(let id):
            MathPracticeScreen(vm: vm, year: 0, id: id)
        case .matMap:
            LessonListScreen(
                vm: vm,
                title: vm.tr("mat_year1"),
                subtitle: vm.tr("mat_sub"),
                rows: vm.content.mat.map { l in
                    let id = l.int("id")
                    return LessonRow(id: "m1\(id)", title: vm.tr(l.str("titleKey")), done: vm.progress.matDone(id), destination: .matLesson(id))
                }
            )
        case .matLesson(let id):
            if let l = vm.content.matLesson(id) {
                SlidesScreen(vm: vm, title: vm.tr(l.str("titleKey")), subtitle: vm.tr(l.str("subKey")), slides: l.arr("slides"), next: .matEx(id))
            }
        case .matEx(let id):
            MathPracticeScreen(vm: vm, year: 1, id: id)
        case .mat2Map:
            LessonListScreen(
                vm: vm,
                title: vm.tr("mat_year2"),
                subtitle: vm.tr("mat2_sub"),
                rows: vm.content.mat2.map { l in
                    let id = l.int("id")
                    return LessonRow(id: "m2\(id)", title: vm.tr(l.str("titleKey")), done: vm.progress.mat2Done(id), destination: .mat2Lesson(id))
                }
            )
        case .mat2Lesson(let id):
            if let l = vm.content.mat2Lesson(id) {
                SlidesScreen(vm: vm, title: vm.tr(l.str("titleKey")), subtitle: vm.tr(l.str("subKey")), slides: l.arr("slides"), next: .mat2Ex(id))
            }
        case .mat2Ex(let id):
            MathPracticeScreen(vm: vm, year: 2, id: id)
        case .mat3Map:
            LessonListScreen(
                vm: vm,
                title: vm.tr("mat_year3"),
                subtitle: vm.tr("mat3_sub"),
                rows: vm.content.mat3.map { l in
                    let id = l.int("id")
                    return LessonRow(id: "m3\(id)", title: vm.tr(l.str("titleKey")), done: vm.progress.mat3Done(id), destination: .mat3Lesson(id))
                }
            )
        case .mat3Lesson(let id):
            if let l = vm.content.mat3Lesson(id) {
                SlidesScreen(vm: vm, title: vm.tr(l.str("titleKey")), subtitle: vm.tr(l.str("subKey")), slides: l.arr("slides"), next: .mat3Ex(id))
            }
        case .mat3Ex(let id):
            MathPracticeScreen(vm: vm, year: 3, id: id)
        case .mat4Map:
            LessonListScreen(
                vm: vm,
                title: vm.tr("mat_year4"),
                subtitle: vm.tr("mat4_sub"),
                rows: vm.content.mat4.map { l in
                    let id = l.int("id")
                    return LessonRow(id: "m4\(id)", title: vm.tr(l.str("titleKey")), done: vm.progress.mat4Done(id), destination: .mat4Lesson(id))
                }
            )
        case .mat4Lesson(let id):
            if let l = vm.content.mat4Lesson(id) {
                SlidesScreen(vm: vm, title: vm.tr(l.str("titleKey")), subtitle: vm.tr(l.str("subKey")), slides: l.arr("slides"), next: .mat4Ex(id))
            }
        case .mat4Ex(let id):
            MathPracticeScreen(vm: vm, year: 4, id: id)
        case .fyzYears:
            FyzYearsScreen(vm: vm)
        case .fyzMap:
            LessonListScreen(
                vm: vm,
                title: vm.tr("fyz_year1"),
                subtitle: vm.tr("fyz_sub"),
                rows: vm.content.fyz.map { l in
                    let id = l.int("id")
                    return LessonRow(
                        id: "f\(id)",
                        title: vm.tr(l.str("titleKey")),
                        done: vm.progress.fyzDone(id),
                        destination: .fyzLesson(id)
                    )
                }
            )
        case .fyzLesson(let id):
            if let l = vm.content.fyzLesson(id) {
                SlidesScreen(vm: vm, title: vm.tr(l.str("titleKey")), subtitle: vm.tr(l.str("subKey")), slides: l.arr("slides"), next: .fyzEx(id))
            }
        case .fyzEx(let id):
            FyzQuizScreen(vm: vm, year: 1, id: id)
        case .fyz2Map:
            LessonListScreen(
                vm: vm,
                title: vm.tr("fyz_year2"),
                subtitle: vm.tr("fyz2_sub"),
                rows: vm.content.fyz2.map { l in
                    let id = l.int("id")
                    return LessonRow(
                        id: "f2\(id)",
                        title: vm.tr(l.str("titleKey")),
                        done: vm.progress.fyz2Done(id),
                        destination: .fyz2Lesson(id)
                    )
                }
            )
        case .fyz2Lesson(let id):
            if let l = vm.content.fyz2Lesson(id) {
                SlidesScreen(vm: vm, title: vm.tr(l.str("titleKey")), subtitle: vm.tr(l.str("subKey")), slides: l.arr("slides"), next: .fyz2Ex(id))
            }
        case .fyz2Ex(let id):
            FyzQuizScreen(vm: vm, year: 2, id: id)
        case .fyz3Map:
            LessonListScreen(
                vm: vm,
                title: vm.tr("fyz_year3"),
                subtitle: vm.tr("fyz3_sub"),
                rows: vm.content.fyz3.map { l in
                    let id = l.int("id")
                    return LessonRow(
                        id: "f3\(id)",
                        title: vm.tr(l.str("titleKey")),
                        done: vm.progress.fyz3Done(id),
                        destination: .fyz3Lesson(id)
                    )
                }
            )
        case .fyz3Lesson(let id):
            if let l = vm.content.fyz3Lesson(id) {
                SlidesScreen(vm: vm, title: vm.tr(l.str("titleKey")), subtitle: vm.tr(l.str("subKey")), slides: l.arr("slides"), next: .fyz3Ex(id))
            }
        case .fyz3Ex(let id):
            FyzQuizScreen(vm: vm, year: 3, id: id)
        case .fyz4Map:
            LessonListScreen(
                vm: vm,
                title: vm.tr("fyz_year4"),
                subtitle: vm.tr("fyz4_sub"),
                rows: vm.content.fyz4.map { l in
                    let id = l.int("id")
                    return LessonRow(
                        id: "f4\(id)",
                        title: vm.tr(l.str("titleKey")),
                        done: vm.progress.fyz4Done(id),
                        destination: .fyz4Lesson(id)
                    )
                }
            )
        case .fyz4Lesson(let id):
            if let l = vm.content.fyz4Lesson(id) {
                SlidesScreen(vm: vm, title: vm.tr(l.str("titleKey")), subtitle: vm.tr(l.str("subKey")), slides: l.arr("slides"), next: .fyz4Ex(id))
            }
        case .fyz4Ex(let id):
            FyzQuizScreen(vm: vm, year: 4, id: id)
        case .csYears:
            CsYearsScreen(vm: vm)
        case .csMap:
            LessonListScreen(
                vm: vm,
                title: vm.tr("cs_year1"),
                subtitle: vm.tr("cs_sub"),
                rows: vm.content.cs.map { l in
                    let id = l.int("id")
                    return LessonRow(id: "c\(id)", title: vm.tr(l.str("titleKey")), done: vm.progress.csDone(id), destination: .csLesson(id))
                }
            )
        case .csLesson(let id):
            if let l = vm.content.csLesson(id) {
                CodeLessonScreen(vm: vm, lesson: l, onSuccess: { vm.markCs(id) })
            }
        case .cs2Map:
            LessonListScreen(
                vm: vm,
                title: vm.tr("cs_year2"),
                subtitle: vm.tr("cs2_sub"),
                rows: vm.content.cs2.map { l in
                    let id = l.int("id")
                    return LessonRow(id: "c2\(id)", title: vm.tr(l.str("titleKey")), done: vm.progress.cs2Done(id), destination: .cs2Lesson(id))
                }
            )
        case .cs2Lesson(let id):
            if let l = vm.content.cs2Lesson(id) {
                CodeLessonScreen(vm: vm, lesson: l, onSuccess: { vm.markCs2(id) })
            }
        case .cs3Map:
            LessonListScreen(
                vm: vm,
                title: vm.tr("cs_year3"),
                subtitle: vm.tr("cs3_sub"),
                rows: vm.content.cs3.map { l in
                    let id = l.int("id")
                    return LessonRow(id: "c3\(id)", title: vm.tr(l.str("titleKey")), done: vm.progress.cs3Done(id), destination: .cs3Lesson(id))
                }
            )
        case .cs3Lesson(let id):
            if let l = vm.content.cs3Lesson(id) {
                CodeLessonScreen(vm: vm, lesson: l, onSuccess: { vm.markCs3(id) })
            }
        case .cs4Map:
            LessonListScreen(
                vm: vm,
                title: vm.tr("cs_year4"),
                subtitle: vm.tr("cs4_sub"),
                rows: vm.content.cs4.map { l in
                    let id = l.int("id")
                    return LessonRow(id: "c4\(id)", title: vm.tr(l.str("titleKey")), done: vm.progress.cs4Done(id), destination: .cs4Lesson(id))
                }
            )
        case .cs4Lesson(let id):
            if let l = vm.content.cs4Lesson(id) {
                CodeLessonScreen(vm: vm, lesson: l, onSuccess: { vm.markCs4(id) })
            }
        case .swYears:
            SwYearsScreen(vm: vm)
        case .swMap:
            LessonListScreen(
                vm: vm,
                title: vm.tr("sw_year1"),
                subtitle: vm.tr("sw_sub"),
                rows: vm.content.sw.map { l in
                    let id = l.int("id")
                    return LessonRow(id: "s\(id)", title: vm.tr(l.str("titleKey")), done: vm.progress.swDone(id), destination: .swLesson(id))
                }
            )
        case .swLesson(let id):
            if let l = vm.content.swLesson(id) {
                SlidesScreen(vm: vm, title: vm.tr(l.str("titleKey")), subtitle: vm.tr(l.str("subKey")), slides: l.arr("slides"), next: .swEx(id))
            }
        case .swEx(let id):
            SwQuizScreen(vm: vm, year: 1, id: id)
        case .sw2Map:
            LessonListScreen(
                vm: vm,
                title: vm.tr("sw_year2"),
                subtitle: vm.tr("sw2_sub"),
                rows: vm.content.sw2.map { l in
                    let id = l.int("id")
                    return LessonRow(id: "s2\(id)", title: vm.tr(l.str("titleKey")), done: vm.progress.sw2Done(id), destination: .sw2Lesson(id))
                }
            )
        case .sw2Lesson(let id):
            if let l = vm.content.sw2Lesson(id) {
                SlidesScreen(vm: vm, title: vm.tr(l.str("titleKey")), subtitle: vm.tr(l.str("subKey")), slides: l.arr("slides"), next: .sw2Ex(id))
            }
        case .sw2Ex(let id):
            SwQuizScreen(vm: vm, year: 2, id: id)
        case .sw3Map:
            LessonListScreen(
                vm: vm,
                title: vm.tr("sw_year3"),
                subtitle: vm.tr("sw3_sub"),
                rows: vm.content.sw3.map { l in
                    let id = l.int("id")
                    return LessonRow(id: "s3\(id)", title: vm.tr(l.str("titleKey")), done: vm.progress.sw3Done(id), destination: .sw3Lesson(id))
                }
            )
        case .sw3Lesson(let id):
            if let l = vm.content.sw3Lesson(id) {
                SlidesScreen(vm: vm, title: vm.tr(l.str("titleKey")), subtitle: vm.tr(l.str("subKey")), slides: l.arr("slides"), next: .sw3Ex(id))
            }
        case .sw3Ex(let id):
            SwQuizScreen(vm: vm, year: 3, id: id)
        case .sw4Map:
            LessonListScreen(
                vm: vm,
                title: vm.tr("sw_year4"),
                subtitle: vm.tr("sw4_sub"),
                rows: vm.content.sw4.map { l in
                    let id = l.int("id")
                    return LessonRow(id: "s4\(id)", title: vm.tr(l.str("titleKey")), done: vm.progress.sw4Done(id), destination: .sw4Lesson(id))
                }
            )
        case .sw4Lesson(let id):
            if let l = vm.content.sw4Lesson(id) {
                SlidesScreen(vm: vm, title: vm.tr(l.str("titleKey")), subtitle: vm.tr(l.str("subKey")), slides: l.arr("slides"), next: .sw4Ex(id))
            }
        case .sw4Ex(let id):
            SwQuizScreen(vm: vm, year: 4, id: id)
        case .pgYears:
            PgYearsScreen(vm: vm)
        case .pgMap:
            LessonListScreen(
                vm: vm,
                title: vm.tr("pg_year1"),
                subtitle: vm.tr("pg_sub"),
                rows: vm.content.pg.map { l in
                    let id = l.int("id")
                    return LessonRow(id: "p\(id)", title: vm.tr(l.str("titleKey")), done: vm.progress.pgDone(id), destination: .pgLesson(id))
                }
            )
        case .pgLesson(let id):
            if let l = vm.content.pgLesson(id) {
                SlidesScreen(vm: vm, title: vm.tr(l.str("titleKey")), subtitle: vm.tr(l.str("subKey")), slides: l.arr("slides"), next: .pgEx(id))
            }
        case .pgEx(let id):
            PgQuizScreen(vm: vm, year: 1, id: id)
        case .pg2Map:
            LessonListScreen(
                vm: vm,
                title: vm.tr("pg_year2"),
                subtitle: vm.tr("pg2_sub"),
                rows: vm.content.pg2.map { l in
                    let id = l.int("id")
                    return LessonRow(id: "p2\(id)", title: vm.tr(l.str("titleKey")), done: vm.progress.pg2Done(id), destination: .pg2Lesson(id))
                }
            )
        case .pg2Lesson(let id):
            if let l = vm.content.pg2Lesson(id) {
                SlidesScreen(vm: vm, title: vm.tr(l.str("titleKey")), subtitle: vm.tr(l.str("subKey")), slides: l.arr("slides"), next: .pg2Ex(id))
            }
        case .pg2Ex(let id):
            PgQuizScreen(vm: vm, year: 2, id: id)
        case .pg3Map:
            LessonListScreen(
                vm: vm,
                title: vm.tr("pg_year3"),
                subtitle: vm.tr("pg3_sub"),
                rows: vm.content.pg3.map { l in
                    let id = l.int("id")
                    return LessonRow(id: "p3\(id)", title: vm.tr(l.str("titleKey")), done: vm.progress.pg3Done(id), destination: .pg3Lesson(id))
                }
            )
        case .pg3Lesson(let id):
            if let l = vm.content.pg3Lesson(id) {
                SlidesScreen(vm: vm, title: vm.tr(l.str("titleKey")), subtitle: vm.tr(l.str("subKey")), slides: l.arr("slides"), next: .pg3Ex(id))
            }
        case .pg3Ex(let id):
            PgQuizScreen(vm: vm, year: 3, id: id)
        case .pg4Map:
            LessonListScreen(
                vm: vm,
                title: vm.tr("pg_year4"),
                subtitle: vm.tr("pg4_sub"),
                rows: vm.content.pg4.map { l in
                    let id = l.int("id")
                    return LessonRow(id: "p4\(id)", title: vm.tr(l.str("titleKey")), done: vm.progress.pg4Done(id), destination: .pg4Lesson(id))
                }
            )
        case .pg4Lesson(let id):
            if let l = vm.content.pg4Lesson(id) {
                SlidesScreen(vm: vm, title: vm.tr(l.str("titleKey")), subtitle: vm.tr(l.str("subKey")), slides: l.arr("slides"), next: .pg4Ex(id))
            }
        case .pg4Ex(let id):
            PgQuizScreen(vm: vm, year: 4, id: id)
        case .tgYears:
            TgYearsScreen(vm: vm)
        case .tgMap:
            LessonListScreen(
                vm: vm,
                title: vm.tr("tg_year1"),
                subtitle: vm.tr("tg_sub"),
                rows: vm.content.tg.map { l in
                    let id = l.int("id")
                    return LessonRow(id: "t\(id)", title: vm.tr(l.str("titleKey")), done: vm.progress.tgDone(id), destination: .tgLesson(id))
                }
            )
        case .tgLesson(let id):
            if let l = vm.content.tgLesson(id) {
                SlidesScreen(vm: vm, title: vm.tr(l.str("titleKey")), subtitle: vm.tr(l.str("subKey")), slides: l.arr("slides"), next: .tgEx(id))
            }
        case .tgEx(let id):
            TgQuizScreen(vm: vm, year: 1, id: id)
        case .tg2Map:
            LessonListScreen(
                vm: vm,
                title: vm.tr("tg_year2"),
                subtitle: vm.tr("tg2_sub"),
                rows: vm.content.tg2.map { l in
                    let id = l.int("id")
                    return LessonRow(id: "t2\(id)", title: vm.tr(l.str("titleKey")), done: vm.progress.tg2Done(id), destination: .tg2Lesson(id))
                }
            )
        case .tg2Lesson(let id):
            if let l = vm.content.tg2Lesson(id) {
                SlidesScreen(vm: vm, title: vm.tr(l.str("titleKey")), subtitle: vm.tr(l.str("subKey")), slides: l.arr("slides"), next: .tg2Ex(id))
            }
        case .tg2Ex(let id):
            TgQuizScreen(vm: vm, year: 2, id: id)
        case .tg3Map:
            LessonListScreen(
                vm: vm,
                title: vm.tr("tg_year3"),
                subtitle: vm.tr("tg3_sub"),
                rows: vm.content.tg3.map { l in
                    let id = l.int("id")
                    return LessonRow(id: "t3\(id)", title: vm.tr(l.str("titleKey")), done: vm.progress.tg3Done(id), destination: .tg3Lesson(id))
                }
            )
        case .tg3Lesson(let id):
            if let l = vm.content.tg3Lesson(id) {
                SlidesScreen(vm: vm, title: vm.tr(l.str("titleKey")), subtitle: vm.tr(l.str("subKey")), slides: l.arr("slides"), next: .tg3Ex(id))
            }
        case .tg3Ex(let id):
            TgQuizScreen(vm: vm, year: 3, id: id)
        case .tg4Map:
            LessonListScreen(
                vm: vm,
                title: vm.tr("tg_year4"),
                subtitle: vm.tr("tg4_sub"),
                rows: vm.content.tg4.map { l in
                    let id = l.int("id")
                    return LessonRow(id: "t4\(id)", title: vm.tr(l.str("titleKey")), done: vm.progress.tg4Done(id), destination: .tg4Lesson(id))
                }
            )
        case .tg4Lesson(let id):
            if let l = vm.content.tg4Lesson(id) {
                SlidesScreen(vm: vm, title: vm.tr(l.str("titleKey")), subtitle: vm.tr(l.str("subKey")), slides: l.arr("slides"), next: .tg4Ex(id))
            }
        case .tg4Ex(let id):
            TgQuizScreen(vm: vm, year: 4, id: id)
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

struct CodeLessonScreen: View {
    @ObservedObject var vm: AppModel
    let lesson: J
    let onSuccess: () -> Void
    @State private var code = ""
    @State private var stdin = ""
    @State private var output = ""
    @State private var status = ""
    @Environment(\.palette) private var palette

    private var running: Bool { status == "running" }
    private var needsInput: Bool { code.contains("ReadLine") }

    var body: some View {
        GeometryReader { geo in
            if geo.size.width >= 900 {
                HStack(alignment: .top, spacing: 28) {
                    ScrollView {
                        explanation
                            .padding(.vertical, 4)
                    }
                    .frame(width: min(560, geo.size.width * 0.4))
                    workspace(height: geo.size.height)
                }
                .padding(.horizontal, 28)
                .padding(.vertical, 20)
                .frame(maxWidth: 1640)
                .frame(maxWidth: .infinity, maxHeight: .infinity)
            } else {
                ScrollView {
                    VStack(alignment: .leading, spacing: 20) {
                        explanation
                        editorCard.frame(height: 320)
                        if needsInput { inputField }
                        outputCard.frame(height: 220)
                    }
                    .padding(16)
                }
            }
        }
        .detailScreen(vm.tr(lesson.str("titleKey")), subtitle: vm.tr(lesson.str("subKey")))
        .onAppear {
            if code.isEmpty { code = lesson.str("code") }
        }
    }

    private var explanation: some View {
        VStack(alignment: .leading, spacing: 34) {
            ForEach(Array(lesson.arr("slides").enumerated()), id: \.offset) { _, slide in
                SlideBody(slide: slide)
            }
        }
    }

    private func workspace(height: CGFloat) -> some View {
        VStack(spacing: 16) {
            editorCard
                .frame(maxHeight: .infinity)
            if needsInput { inputField }
            outputCard
                .frame(height: max(180, height * 0.32))
        }
        .frame(maxWidth: .infinity, maxHeight: .infinity)
    }

    private var editorCard: some View {
        VStack(spacing: 0) {
            HStack(spacing: 10) {
                Image(systemName: "chevron.left.forwardslash.chevron.right")
                    .foregroundStyle(Color.accentColor)
                Text("Program.cs")
                    .font(.headline.monospaced())
                Spacer(minLength: 8)
                Button {
                    code = lesson.str("code")
                    output = ""
                    status = ""
                } label: {
                    Label(vm.tr("cs_reset"), systemImage: "arrow.counterclockwise")
                }
                .buttonStyle(.bordered)
                .disabled(running)
                Button { run() } label: {
                    Label(vm.tr("cs_run"), systemImage: "play.fill")
                        .padding(.horizontal, 4)
                }
                .buttonStyle(.borderedProminent)
                .keyboardShortcut(.return, modifiers: .command)
                .help("⌘↩")
                .disabled(running)
            }
            #if os(macOS)
            .controlSize(.large)
            #endif
            .padding(.horizontal, 14)
            .padding(.vertical, 10)
            Divider().opacity(0.5)
            editorField
                .frame(maxWidth: .infinity, maxHeight: .infinity)
        }
        .background(codeFill, in: RoundedRectangle(cornerRadius: 16, style: .continuous))
        .overlay {
            RoundedRectangle(cornerRadius: 16, style: .continuous)
                .strokeBorder(palette.line, lineWidth: 1)
        }
    }

    @ViewBuilder
    private var editorField: some View {
        #if os(macOS)
        CodeEditor(text: $code)
        #else
        TextEditor(text: $code)
            .font(.system(size: 15, design: .monospaced))
            .textInputAutocapitalization(.never)
            .autocorrectionDisabled()
            .scrollContentBackground(.hidden)
            .padding(8)
        #endif
    }

    private var inputField: some View {
        VStack(alignment: .leading, spacing: 6) {
            Label(vm.tr("cs_stdin"), systemImage: "keyboard")
                .font(.subheadline)
                .foregroundStyle(.secondary)
            TextField("", text: $stdin, axis: .vertical)
                .font(.system(size: 15, design: .monospaced))
                .lineLimit(1...4)
                .textFieldStyle(.roundedBorder)
        }
    }

    private var outputCard: some View {
        VStack(spacing: 0) {
            HStack(spacing: 10) {
                Image(systemName: "terminal")
                    .foregroundStyle(.secondary)
                Text(vm.tr("cs_output"))
                    .font(.headline)
                Spacer(minLength: 8)
                statusBadge
            }
            .padding(.horizontal, 14)
            .frame(height: 44)
            Divider().opacity(0.5)
            ScrollView {
                Text(output.isEmpty ? vm.tr("cs_empty") : output)
                    .font(.system(size: 15, design: .monospaced))
                    .foregroundStyle(output.isEmpty || running ? .secondary : .primary)
                    .frame(maxWidth: .infinity, alignment: .topLeading)
                    .textSelection(.enabled)
                    .padding(14)
            }
        }
        .background(codeFill, in: RoundedRectangle(cornerRadius: 16, style: .continuous))
        .overlay {
            RoundedRectangle(cornerRadius: 16, style: .continuous)
                .strokeBorder(palette.line, lineWidth: 1)
        }
    }

    @ViewBuilder
    private var statusBadge: some View {
        switch status {
        case "running":
            ProgressView().controlSize(.small)
        case "ok":
            Label(vm.tr("cs_done"), systemImage: "checkmark.circle.fill")
                .font(.subheadline.weight(.semibold))
                .foregroundStyle(vm.palette.success)
        case "fail", "timeout":
            Label(vm.tr("cs_failed"), systemImage: "xmark.octagon.fill")
                .font(.subheadline.weight(.semibold))
                .foregroundStyle(vm.palette.error)
        default:
            EmptyView()
        }
    }

    private var codeFill: Color { palette.surface }

    private func run() {
        guard !running else { return }
        status = "running"
        output = vm.tr("cs_running")
        CSharpRunner.run(code, stdin: needsInput ? stdin : "") { result, text in
            status = result
            switch result {
            case "unavailable":
                output = vm.tr("cs_unavailable")
            case "timeout":
                output = (text + "\n\n" + vm.tr("cs_timeout")).trimmingCharacters(in: .whitespacesAndNewlines)
            default:
                output = text.isEmpty ? vm.tr("cs_empty") : text
            }
            if result == "ok" { onSuccess() }
        }
    }
}

#if os(macOS)
/// Plain-text editor for C#: monospaced, coloured, and without smart quotes or
/// autocorrect, which would turn "text" into “text” and break the program.
struct CodeEditor: NSViewRepresentable {
    @Binding var text: String

    func makeCoordinator() -> Coordinator { Coordinator(text: $text) }

    func makeNSView(context: Context) -> NSScrollView {
        let scroll = NSTextView.scrollableTextView()
        scroll.drawsBackground = false
        scroll.autohidesScrollers = true
        guard let view = scroll.documentView as? NSTextView else { return scroll }
        view.delegate = context.coordinator
        view.isRichText = false
        view.allowsUndo = true
        view.isAutomaticQuoteSubstitutionEnabled = false
        view.isAutomaticDashSubstitutionEnabled = false
        view.isAutomaticTextReplacementEnabled = false
        view.isAutomaticSpellingCorrectionEnabled = false
        view.isContinuousSpellCheckingEnabled = false
        view.isGrammarCheckingEnabled = false
        view.isAutomaticLinkDetectionEnabled = false
        view.smartInsertDeleteEnabled = false
        view.drawsBackground = false
        view.font = CSharpStyle.font
        view.textContainerInset = NSSize(width: 10, height: 12)
        view.typingAttributes = CSharpStyle.base
        view.string = text
        CSharpStyle.highlight(view.textStorage)
        return scroll
    }

    func updateNSView(_ scroll: NSScrollView, context: Context) {
        context.coordinator.text = $text
        guard let view = scroll.documentView as? NSTextView, view.string != text else { return }
        view.string = text
        CSharpStyle.highlight(view.textStorage)
    }

    final class Coordinator: NSObject, NSTextViewDelegate {
        var text: Binding<String>

        init(text: Binding<String>) {
            self.text = text
        }

        func textDidChange(_ notification: Notification) {
            guard let view = notification.object as? NSTextView else { return }
            text.wrappedValue = view.string
            CSharpStyle.highlight(view.textStorage)
            view.typingAttributes = CSharpStyle.base
        }
    }
}

enum CSharpStyle {
    static let font = NSFont.monospacedSystemFont(ofSize: 15, weight: .regular)
    static var base: [NSAttributedString.Key: Any] { [.font: font, .foregroundColor: NSColor.labelColor] }

    private static func adaptive(_ light: Int, _ dark: Int) -> NSColor {
        func color(_ hex: Int) -> NSColor {
            NSColor(
                srgbRed: CGFloat((hex >> 16) & 0xFF) / 255,
                green: CGFloat((hex >> 8) & 0xFF) / 255,
                blue: CGFloat(hex & 0xFF) / 255,
                alpha: 1
            )
        }
        return NSColor(name: nil) { appearance in
            appearance.bestMatch(from: [.aqua, .darkAqua]) == .darkAqua ? color(dark) : color(light)
        }
    }

    private static let keywords = [
        "abstract", "as", "async", "await", "base", "bool", "break", "case", "catch", "char", "class",
        "const", "continue", "decimal", "default", "do", "double", "else", "enum", "false", "finally",
        "float", "for", "foreach", "get", "if", "in", "int", "interface", "internal", "is", "long",
        "namespace", "new", "null", "object", "out", "override", "private", "protected", "public",
        "readonly", "record", "ref", "return", "sealed", "set", "static", "string", "struct", "switch",
        "this", "throw", "true", "try", "using", "var", "virtual", "void", "while",
    ]

    private static let rules: [(NSRegularExpression, NSColor)] = {
        func re(_ pattern: String) -> NSRegularExpression {
            try! NSRegularExpression(pattern: pattern)
        }
        return [
            (re("\\b(?:" + keywords.joined(separator: "|") + ")\\b"), adaptive(0x9B2393, 0xFF7AB2)),
            (re("\\b(?:Console|Math|List|Dictionary|File|Task|Exception|Func|Action|String|DateTime|Random)\\b"), adaptive(0x0B4F79, 0x5DD8FF)),
            (re("\\b\\d+(?:\\.\\d+)?\\b"), adaptive(0x1C00CF, 0xD9C97C)),
            (re("\\$?@?\"(?:\\\\.|[^\"\\\\\\n])*\"|'(?:\\\\.|[^'\\\\\\n])'"), adaptive(0xC41A16, 0xFC6A5D)),
            (re("//[^\\n]*|/\\*[\\s\\S]*?\\*/"), adaptive(0x5D6C79, 0x7F8C98)),
        ]
    }()

    static func highlight(_ storage: NSTextStorage?) {
        guard let storage else { return }
        let text = storage.string
        let full = NSRange(location: 0, length: (text as NSString).length)
        storage.beginEditing()
        storage.setAttributes(base, range: full)
        for (pattern, color) in rules {
            pattern.enumerateMatches(in: text, range: full) { match, _, _ in
                if let range = match?.range {
                    storage.addAttribute(.foregroundColor, value: color, range: range)
                }
            }
        }
        storage.endEditing()
    }
}
#endif

enum CSharpRunner {
    static func run(_ source: String, stdin: String, done: @escaping (String, String) -> Void) {
        #if os(macOS)
        DispatchQueue.global(qos: .userInitiated).async {
            let result = runMac(source, stdin: stdin)
            DispatchQueue.main.async { done(result.0, result.1) }
        }
        #else
        done("unavailable", "")
        #endif
    }

    #if os(macOS)
    private static func runMac(_ source: String, stdin: String) -> (String, String) {
        guard source.count <= 100_000 else { return ("fail", "Program je příliš dlouhý.") }
        guard let dotnet = findDotnet() else { return ("unavailable", "") }
        let dir = URL(fileURLWithPath: NSHomeDirectory()).appendingPathComponent(".graduately/csharp-lab")
        let csproj = dir.appendingPathComponent("Lab.csproj")
        if !FileManager.default.fileExists(atPath: csproj.path) {
            try? FileManager.default.createDirectory(at: dir, withIntermediateDirectories: true)
            let framework = targetFramework(dotnet)
            let xml = """
            <Project Sdk="Microsoft.NET.Sdk">
              <PropertyGroup>
                <OutputType>Exe</OutputType>
                <TargetFramework>\(framework)</TargetFramework>
                <ImplicitUsings>enable</ImplicitUsings>
                <Nullable>disable</Nullable>
                <LangVersion>latest</LangVersion>
              </PropertyGroup>
            </Project>
            """
            try? xml.write(to: csproj, atomically: true, encoding: .utf8)
            let restored = command(dotnet, ["restore", dir.path], dir, "", 90)
            if restored.0 != "ok" { return restored }
        }
        do {
            try source.write(to: dir.appendingPathComponent("Program.cs"), atomically: true, encoding: .utf8)
        } catch {
            return ("fail", error.localizedDescription)
        }
        var args = ["run", "--project", dir.path, "--verbosity", "quiet"]
        if FileManager.default.fileExists(atPath: dir.appendingPathComponent("obj").path) {
            args.append("--no-restore")
        }
        return command(dotnet, args, dir, stdin, 20)
    }

    private static func command(_ dotnet: String, _ args: [String], _ dir: URL, _ stdin: String, _ seconds: Double) -> (String, String) {
        let proc = Process()
        proc.executableURL = URL(fileURLWithPath: dotnet)
        proc.arguments = args
        proc.currentDirectoryURL = dir
        var env = ProcessInfo.processInfo.environment
        let root = (dotnet as NSString).deletingLastPathComponent
        env["PATH"] = [root, "/opt/homebrew/bin", "/usr/local/bin", "/usr/bin", "/bin"].joined(separator: ":")
        env["DOTNET_ROOT"] = root
        env["DOTNET_CLI_TELEMETRY_OPTOUT"] = "1"
        env["DOTNET_NOLOGO"] = "1"
        env["DOTNET_SKIP_FIRST_TIME_EXPERIENCE"] = "1"
        proc.environment = env
        let out = Pipe()
        let input = Pipe()
        proc.standardOutput = out
        proc.standardError = out
        proc.standardInput = input
        var collected = Data()
        let lock = NSLock()
        out.fileHandleForReading.readabilityHandler = { handle in
            let chunk = handle.availableData
            guard !chunk.isEmpty else { return }
            lock.lock()
            collected.append(chunk)
            lock.unlock()
        }
        do { try proc.run() } catch { return ("fail", error.localizedDescription) }
        if let data = stdin.data(using: .utf8) { input.fileHandleForWriting.write(data) }
        try? input.fileHandleForWriting.close()
        let group = DispatchGroup()
        group.enter()
        proc.terminationHandler = { _ in group.leave() }
        if group.wait(timeout: .now() + seconds) == .timedOut {
            proc.terminate()
            kill(proc.processIdentifier, SIGKILL)
            out.fileHandleForReading.readabilityHandler = nil
            return ("timeout", clip(collected, lock))
        }
        out.fileHandleForReading.readabilityHandler = nil
        let rest = out.fileHandleForReading.readDataToEndOfFile()
        lock.lock()
        collected.append(rest)
        lock.unlock()
        let text = clip(collected, lock)
        return proc.terminationStatus == 0 ? ("ok", text) : ("fail", text)
    }

    private static func clip(_ data: Data, _ lock: NSLock) -> String {
        lock.lock()
        let copy = data
        lock.unlock()
        let text = String(data: copy, encoding: .utf8) ?? ""
        let trimmed = text.trimmingCharacters(in: .whitespacesAndNewlines)
        return trimmed.count <= 16_000 ? trimmed : String(trimmed.prefix(16_000)) + "\n…"
    }

    private static func targetFramework(_ dotnet: String) -> String {
        let version = command(dotnet, ["--version"], URL(fileURLWithPath: NSTemporaryDirectory()), "", 20)
        let major = version.1.split(separator: ".").first.map(String.init) ?? ""
        return major.allSatisfy(\.isNumber) && !major.isEmpty ? "net\(major).0" : "net8.0"
    }

    private static func findDotnet() -> String? {
        let names = [
            "/usr/local/share/dotnet/dotnet",
            "/opt/homebrew/share/dotnet/dotnet",
            "/usr/local/bin/dotnet",
            NSHomeDirectory() + "/.dotnet/dotnet",
        ]
        return names.first { FileManager.default.isExecutableFile(atPath: $0) }
    }
    #endif
}
