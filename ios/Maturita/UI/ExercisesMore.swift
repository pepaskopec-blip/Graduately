import SwiftUI

struct DialogEx: View {
    @ObservedObject var vm: AppModel
    let title: String
    var sub: String?
    let spec: J
    var onDone: () -> Void

    var body: some View {
        let pool = spec.strs("pool")
        let meanings = spec.strs("meanings")
        let lines = dialogLines()
        ComboRows(vm: vm, title: title, sub: sub, pool: pool, n: lines.count, onDone: onDone) { picks, expected, revealed in
            ForEach(lines.indices, id: \.self) { i in
                let line = lines[i]
                if let name = line.name {
                    Text(name)
                        .font(.subheadline.weight(.semibold))
                        .foregroundStyle(.secondary)
                        .padding(.vertical, 6)
                }
                ComboLine(
                    vm: vm, pool: pool, picks: picks, expected: expected, idx: i,
                    answer: line.answer,
                    prefix: line.prefix,
                    suffix: line.suffix,
                    meaning: i < meanings.count ? meanings[i] : nil,
                    revealed: revealed
                )
            }
        }
    }

    private func dialogLines() -> [(name: String?, prefix: String, answer: String, suffix: String)] {
        var out: [(name: String?, prefix: String, answer: String, suffix: String)] = []
        for d in spec.arr("dialogues") {
            var header: String? = d.str("name")
            for row in d.arr("rows") {
                out.append((
                    header,
                    [row.str("speaker"), row.str("before")].filter { !$0.isEmpty }.joined(separator: " "),
                    row.str("answer"),
                    row.str("after")
                ))
                header = nil
            }
        }
        return out
    }
}

struct ComboEx: View {
    @ObservedObject var vm: AppModel
    let title: String
    var sub: String?
    let spec: J
    var onDone: () -> Void

    var body: some View {
        let pool = spec.strs("pool")
        let rows = spec.arr("rows")
        let meanings = spec.strs("meanings")
        let type = spec.str("type")
        if type == "verb_sections" {
            let sections = spec.arr("sections")
            let allRows = sections.flatMap { $0.arr("rows") }
            ComboRows(vm: vm, title: title, sub: sub, pool: spec.strs("pool"), n: allRows.count, onDone: onDone, revealMean: spec.bool("transUpfront")) { picks, expected, revealed in
                ForEach(Array(verbSectionLines().enumerated()), id: \.offset) { idx, line in
                    if let title = line.title {
                        Text(title).font(.subheadline.weight(.semibold)).foregroundStyle(.secondary)
                    }
                    ComboLine(
                        vm: vm, pool: spec.strs("pool"), picks: picks, expected: expected, idx: idx,
                        answer: line.answer, prefix: line.before, suffix: line.after,
                        meaning: line.meaning,
                        revealed: revealed || spec.bool("transUpfront")
                    )
                }
            }
        } else if type == "ordne" {
            ComboRows(vm: vm, title: title, sub: sub, pool: [], n: rows.count * 2, onDone: onDone) { picks, expected, revealed in
                ForEach(rows.indices, id: \.self) { i in
                    ComboLine(vm: vm, pool: spec.strs("fwPool"), picks: picks, expected: expected, idx: i * 2, answer: rows[i].str("fw"), prefix: "\(i + 1). ", suffix: rows[i].str("mid"), revealed: revealed)
                    ComboLine(vm: vm, pool: spec.strs("ansPool"), picks: picks, expected: expected, idx: i * 2 + 1, answer: rows[i].str("ans"), prefix: "→ ", meaning: i < meanings.count ? meanings[i] : nil, revealed: revealed)
                }
            }
        } else if type == "fill" {
            let blanks = rows.reduce(0) { $0 + $1.answerList().count }
            ComboRows(vm: vm, title: title, sub: sub, pool: pool, n: blanks, onDone: onDone, revealMean: spec.bool("preMeaning")) { picks, expected, revealed in
                if let sample = spec.strOrNull("sample") { Hint(text: sample) }
                ForEach(Array(fillRows().enumerated()), id: \.offset) { _, block in
                    FlowLayout(spacing: 4) {
                        if let num = block.num {
                            Text(num).font(.body.weight(.semibold)).foregroundStyle(.primary)
                        }
                        ForEach(Array(block.parts.enumerated()), id: \.offset) { _, part in
                            switch part {
                            case .text(let s):
                                Text(s).foregroundStyle(.primary)
                            case .blank(let idx, let ans):
                                ComboLine(vm: vm, pool: pool, picks: picks, expected: expected, idx: idx, answer: ans, revealed: revealed, inline: true)
                            }
                        }
                        if let mean = block.meaning {
                            Text(vm.tr(mean)).font(.caption).foregroundStyle(.secondary)
                        }
                    }
                    .padding(.bottom, 6)
                    if revealed || spec.bool("preMeaning") || spec.bool("showGerman"), let mean = block.rowMean {
                        Meaning(text: vm.tr(mean), palette: vm.palette, visible: true)
                    }
                }
            }
        } else {
            ComboRows(vm: vm, title: title, sub: sub, pool: pool, n: rows.count, onDone: onDone, revealMean: spec.bool("preMeaning")) { picks, expected, revealed in
                if let sample = spec.strOrNull("sample") { Hint(text: sample) }
                ForEach(rows.indices, id: \.self) { i in
                    let r = rows[i]
                    let prefix: String = {
                        switch type {
                        case "number": return "\(r.str("digits")) = "
                        case "count": return "\(Array(repeating: r.str("emoji"), count: r.int("count")).joined(separator: " ")) \(r.str("noun")) "
                        case "seq": return r.str("before")
                        case "verbclue": return "\(r.str("emoji")) \(r.str("before"))"
                        default: return r.str("before")
                        }
                    }()
                    let suffix: String = {
                        switch type {
                        case "seq", "verbclue", "verb": return r.str("after")
                        default: return ""
                        }
                    }()
                    ComboLine(
                        vm: vm, pool: pool, picks: picks, expected: expected, idx: i, answer: r.str("answer"),
                        prefix: prefix, suffix: suffix,
                        meaning: i < meanings.count ? meanings[i] : r.strOrNull("mean"),
                        revealed: revealed || spec.bool("preMeaning")
                    )
                }
            }
        }
    }

    private func verbSectionLines() -> [(title: String?, before: String, answer: String, after: String, meaning: String?)] {
        var out: [(title: String?, before: String, answer: String, after: String, meaning: String?)] = []
        for sec in spec.arr("sections") {
            var header: String? = sec.str("title")
            let meanings = sec.strs("meanings")
            for (ri, row) in sec.arr("rows").enumerated() {
                out.append((
                    header,
                    row.str("before"),
                    row.str("answer"),
                    row.str("after"),
                    ri < meanings.count ? meanings[ri] : nil
                ))
                header = nil
            }
        }
        return out
    }

    private enum FillPart {
        case text(String)
        case blank(Int, String)
    }

    private func fillRows() -> [(num: String?, parts: [FillPart], meaning: String?, rowMean: String?)] {
        let meanings = spec.strs("meanings")
        var i = 0
        return spec.arr("rows").enumerated().map { ri, r in
            var parts: [FillPart] = []
            let segs = r.strsOrEmpty("segs")
            let ans = r.answerList()
            for si in segs.indices {
                if let seg = segs[si], !seg.isEmpty { parts.append(.text(seg)) }
                if si < ans.count {
                    parts.append(.blank(i, ans[si]))
                    i += 1
                }
            }
            return (
                r.strOrNull("num"),
                parts,
                ri < meanings.count ? meanings[ri] : nil,
                r.strOrNull("mean")
            )
        }
    }
}

private struct ComboRows<Body: View>: View {
    @ObservedObject var vm: AppModel
    let title: String
    var sub: String?
    let pool: [String]
    let n: Int
    var onDone: () -> Void
    var revealMean = false
    @ViewBuilder var bodyContent: (Binding<[String]>, Binding<[String]>, Bool) -> Body
    @State private var picks: [String]
    @State private var expected: [String]
    @State private var fb = ("", "")
    @State private var revealed: Bool

    init(
        vm: AppModel,
        title: String,
        sub: String?,
        pool: [String],
        n: Int,
        onDone: @escaping () -> Void,
        revealMean: Bool = false,
        @ViewBuilder bodyContent: @escaping (Binding<[String]>, Binding<[String]>, Bool) -> Body
    ) {
        self.vm = vm
        self.title = title
        self.sub = sub
        self.pool = pool
        self.n = n
        self.onDone = onDone
        self.revealMean = revealMean
        self.bodyContent = bodyContent
        _picks = State(initialValue: Array(repeating: "", count: n))
        _expected = State(initialValue: Array(repeating: "", count: n))
        _revealed = State(initialValue: revealMean)
    }

    var body: some View {
        ExerciseScaffold(vm: vm, title: title, sub: sub, action: vm.tr("check"), onAction: {
            let ok = expected.indices.allSatisfy { i in
                let exp = expected[i]
                return exp.isEmpty || answerAccepts(normalizeAnswer(picks[i]), exp)
            }
            if ok { fb = (vm.tr("feedback_ok"), "ok"); onDone() }
            else { fb = (vm.tr("feedback_retry"), "err") }
            revealed = true
        }, feedback: fb.0, kind: fb.1) {
            bodyContent($picks, $expected, revealed)
        }
    }
}

private struct ComboLine: View {
    @ObservedObject var vm: AppModel
    let pool: [String]
    @Binding var picks: [String]
    @Binding var expected: [String]
    let idx: Int
    let answer: String
    var prefix: String = ""
    var suffix: String = ""
    var meaning: String?
    var revealed: Bool
    var inline = false
    @State private var open = false

    var body: some View {
        let p = vm.palette
        let pick = idx < picks.count ? picks[idx] : ""
        let mark: Bool? = {
            if revealed && !pick.isEmpty { return answerAccepts(normalizeAnswer(pick), answer) }
            return nil
        }()
        let chip = Button {
            open = true
        } label: {
            HStack(spacing: 4) {
                Text(pick.isEmpty ? "…" : pick)
                    .foregroundStyle(pick.isEmpty ? Color.secondary : Color.primary)
                    .frame(minWidth: 36)
                Image(systemName: mark == true ? "checkmark.circle.fill" : mark == false ? "xmark.circle.fill" : "chevron.up.chevron.down")
                    .font(.caption2)
                    .foregroundStyle(mark == true ? p.success : mark == false ? p.error : Color.secondary)
            }
        }
        .buttonStyle(.bordered)
        .controlSize(.small)
        .tint(mark == true ? p.success : mark == false ? p.error : .secondary)

        Group {
            if inline {
                chip
            } else {
                FlowLayout(spacing: 6) {
                    if !prefix.isEmpty { Text(prefix) }
                    chip
                    if !suffix.isEmpty { Text(suffix) }
                }
                .frame(maxWidth: .infinity, alignment: .leading)
                .padding(.bottom, 8)
            }
        }
        .onAppear {
            if idx >= 0 && idx < expected.count { expected[idx] = answer }
        }
        .onChange(of: answer) { _, new in
            if idx >= 0 && idx < expected.count { expected[idx] = new }
        }
        .sheet(isPresented: $open) {
            WordPicker(palette: p, pool: pool) { word in
                if let word, idx < picks.count { picks[idx] = word }
                open = false
            }
            .sheetDetents()
        }
        if revealed, let meaning, !meaning.isEmpty {
            Meaning(text: vm.tr(meaning), palette: p, visible: true)
        }
    }
}

struct WordPicker: View {
    let palette: Palette
    let pool: [String]
    var onPick: (String?) -> Void

    var body: some View {
        NavigationStack {
            List {
                ForEach(Array(pool.enumerated()), id: \.offset) { _, w in
                    Button {
                        onPick(w)
                    } label: {
                        Text(w)
                            .foregroundStyle(.primary)
                            .frame(maxWidth: .infinity, alignment: .leading)
                            .contentShape(Rectangle())
                    }
                    .buttonStyle(.plain)
                }
            }
            .listStyle(.plain)
            .scrollContentBackground(.visible)
            .navigationTitle("Vyberte slovo")
            .navigationBarTitleDisplayMode(.inline)
            .toolbar {
                ToolbarItem(placement: .cancellationAction) {
                    Button(role: .cancel) { onPick(nil) } label: { Image(systemName: "xmark") }
                }
            }
        }
        .sheetDetents()
    }
}

struct Ex2Ex: View {
    @ObservedObject var vm: AppModel
    let title: String
    var sub: String?
    let spec: J
    var onDone: () -> Void

    var body: some View {
        let items = spec.arr("items")
        let pool = spec.strs("pool")
        let blanks = items.reduce(0) { acc, item in
            acc + item.arr("rows").reduce(0) { $0 + $1.strs("answers").count }
        }
        ComboRows(vm: vm, title: title, sub: sub, pool: pool, n: blanks, onDone: onDone) { picks, expected, revealed in
            ForEach(Array(ex2Blocks().enumerated()), id: \.offset) { _, block in
                Meaning(text: block.czech, palette: vm.palette, visible: true)
                ForEach(Array(block.rows.enumerated()), id: \.offset) { _, row in
                    FlowLayout(spacing: 4) {
                        if let num = row.num {
                            Text(num).foregroundStyle(.primary).padding(.trailing, 6)
                        }
                        ForEach(Array(row.parts.enumerated()), id: \.offset) { _, part in
                            switch part {
                            case .text(let s):
                                Text(s).foregroundStyle(.primary)
                            case .blank(let idx, let ans):
                                ComboLine(vm: vm, pool: pool, picks: picks, expected: expected, idx: idx, answer: ans, revealed: revealed, inline: true)
                            }
                        }
                    }
                }
            }
        }
    }

    private enum Part {
        case text(String)
        case blank(Int, String)
    }

    private func ex2Blocks() -> [(czech: String, rows: [(num: String?, parts: [Part])])] {
        var i = 0
        return spec.arr("items").map { item in
            let rows = item.arr("rows").map { row -> (num: String?, parts: [Part]) in
                var parts: [Part] = []
                let segs = row.strsOrEmpty("segs")
                let ans = row.strs("answers")
                for si in segs.indices {
                    if let seg = segs[si], !seg.isEmpty { parts.append(.text(seg)) }
                    if si < ans.count {
                        parts.append(.blank(i, ans[si]))
                        i += 1
                    }
                }
                return (row.strOrNull("num"), parts)
            }
            return (item.str("czech"), rows)
        }
    }
}

struct LettersEx: View {
    @ObservedObject var vm: AppModel
    let title: String
    var sub: String?
    let spec: J
    var onDone: () -> Void
    @State private var fields: [String]
    @State private var fb = ("", "")
    @State private var revealed = false

    init(vm: AppModel, title: String, sub: String?, spec: J, onDone: @escaping () -> Void) {
        self.vm = vm
        self.title = title
        self.sub = sub
        self.spec = spec
        self.onDone = onDone
        let n = spec.arr("rows").flatMap { $0.strs("answers") }.count
        _fields = State(initialValue: Array(repeating: "", count: n))
    }

    var body: some View {
        let rows = spec.arr("rows")
        ExerciseScaffold(vm: vm, title: title, sub: sub, action: vm.tr("check"), onAction: {
            var i = 0, ok = 0, total = 0
            for r in rows {
                for ans in r.strs("answers") {
                    total += 1
                    if i < fields.count && answerAccepts(normalizeAnswer(fields[i]), ans) { ok += 1 }
                    i += 1
                }
            }
            if ok == total { fb = (vm.tr("feedback_ok"), "ok"); onDone() }
            else { fb = (vm.tr("feedback_retry"), "err") }
            revealed = true
        }, feedback: fb.0, kind: fb.1) {
            if rows.contains(where: { $0.strs("answers").contains { hasUmlaut($0) } }) {
                Hint(text: vm.tr("hint_umlauts"))
            }
            LettersRows(vm: vm, rows: rows, fields: $fields, revealed: revealed)
        }
    }
}

private struct LettersRows: View {
    @ObservedObject var vm: AppModel
    let rows: [J]
    @Binding var fields: [String]
    let revealed: Bool

    var body: some View {
        let p = vm.palette
        let pairs = build()
        VStack(alignment: .leading, spacing: 8) {
            ForEach(pairs.indices, id: \.self) { ri in
                let row = pairs[ri]
                if let num = row.num { Text(num).foregroundStyle(.secondary) }
                FlowLayout {
                    ForEach(row.parts.indices, id: \.self) { pi in
                        switch row.parts[pi] {
                        case .text(let s):
                            Text(s).foregroundStyle(.primary)
                        case .field(let idx, let ans):
                            WordField(
                                value: Binding(get: { idx < fields.count ? fields[idx] : "" }, set: { if idx < fields.count { fields[idx] = $0 } }),
                                placeholder: "",
                                mark: revealed ? answerAccepts(normalizeAnswer(idx < fields.count ? fields[idx] : ""), ans) : nil,
                                palette: p
                            )
                            .frame(width: 72)
                        }
                    }
                }
                if revealed, let czech = row.czech { Meaning(text: czech, palette: p, visible: true) }
                if revealed, let mean = row.mean { Meaning(text: mean, palette: p, visible: true) }
            }
        }
    }

    private enum Part {
        case text(String)
        case field(Int, String)
    }

    private struct RowModel {
        var num: String?
        var parts: [Part]
        var czech: String?
        var mean: String?
    }

    private func build() -> [RowModel] {
        var i = 0
        return rows.map { r in
            var parts: [Part] = []
            let segs = r.strsOrEmpty("segs")
            let ans = r.strs("answers")
            for si in segs.indices {
                if let seg = segs[si], !seg.isEmpty { parts.append(.text(seg)) }
                if si < ans.count {
                    parts.append(.field(i, ans[si]))
                    i += 1
                }
            }
            return RowModel(num: r.strOrNull("num"), parts: parts, czech: r.strOrNull("czech"), mean: r.strOrNull("mean"))
        }
    }
}

struct TableEx: View {
    @ObservedObject var vm: AppModel
    let title: String
    var sub: String?
    let spec: J
    var onDone: () -> Void
    @State private var fields: [String]
    @State private var fb = ("", "")
    @State private var revealed = false

    init(vm: AppModel, title: String, sub: String?, spec: J, onDone: @escaping () -> Void) {
        self.vm = vm
        self.title = title
        self.sub = sub
        self.spec = spec
        self.onDone = onDone
        _fields = State(initialValue: Array(repeating: "", count: spec.strs("nouns").count * spec.strs("persons").count))
    }

    var body: some View {
        let persons = spec.strs("persons")
        let nouns = spec.strs("nouns")
        let answers = spec.intRows("answers")
        let p = vm.palette
        ExerciseScaffold(vm: vm, title: title, sub: sub, action: vm.tr("check"), onAction: {
            var ok = 0
            for (r, row) in answers.enumerated() {
                for (c, ans) in row.enumerated() {
                    let i = r * persons.count + c
                    if i < fields.count && normalizeAnswer(fields[i]) == normalizeAnswer(ans) { ok += 1 }
                }
            }
            if ok == fields.count { fb = (vm.tr("feedback_ok"), "ok"); onDone() }
            else { fb = (vm.tr("feedback_retry"), "err") }
            revealed = true
        }, feedback: fb.0, kind: fb.1) {
            ForEach(nouns.indices, id: \.self) { r in
                Text(nouns[r]).font(.body.weight(.bold)).foregroundStyle(.primary)
                ForEach(persons.indices, id: \.self) { c in
                    let i = r * persons.count + c
                    Text(persons[c]).font(.caption).foregroundStyle(.secondary)
                    WordField(value: Binding(get: { i < fields.count ? fields[i] : "" }, set: { if i < fields.count { fields[i] = $0 } }), placeholder: "", palette: p)
                }
                Spacer().frame(height: 8)
            }
        }
    }
}

struct RevealEx: View {
    @ObservedObject var vm: AppModel
    let title: String
    var sub: String?
    let spec: J
    var onDone: () -> Void
    @State private var shown: [Bool]
    @State private var fb = ("", "")

    init(vm: AppModel, title: String, sub: String?, spec: J, onDone: @escaping () -> Void) {
        self.vm = vm
        self.title = title
        self.sub = sub
        self.spec = spec
        self.onDone = onDone
        _shown = State(initialValue: Array(repeating: false, count: spec.arr("items").count))
    }

    var body: some View {
        let items = spec.arr("items")
        let p = vm.palette
        ExerciseScaffold(vm: vm, title: title, sub: sub, action: vm.tr("finish"), onAction: {
            shown = shown.map { _ in true }
            fb = (vm.tr("feedback_ok"), "ok")
            onDone()
        }, feedback: fb.0, kind: fb.1) {
            if let note = spec.strOrNull("note") { Hint(text: note) }
            ForEach(items.indices, id: \.self) { i in
                Prompt(text: items[i].str("prompt"))
                PillButton(label: vm.tr("show_sample")) { shown[i].toggle() }
                    .padding(.vertical, 6)
                if shown[i] { Meaning(text: items[i].str("solution"), palette: p, visible: true) }
                Spacer().frame(height: 8)
            }
        }
    }
}

struct NetQuizScreen: View {
    @ObservedObject var vm: AppModel
    let id: Int

    var body: some View {
        if let lesson = vm.content.netLesson(id) {
            if id == 1 {
                VlsmEx(vm: vm, lesson: lesson)
            } else {
                let spec = J([
                    "type": "choice",
                    "title": vm.tr(lesson.str("exTitleKey")),
                    "sub": "net_quiz_intro",
                    "questions": lesson.rawArray("quiz"),
                ])
                DispatchExercise(vm: vm, spec: spec, titleFallback: vm.tr(lesson.str("exTitleKey"))) {
                    vm.markNet(id)
                }
            }
        }
    }
}

struct HwQuizScreen: View {
    @ObservedObject var vm: AppModel
    let id: Int

    var body: some View {
        if let lesson = vm.content.hwLesson(id) {
            let spec = J([
                "type": "choice",
                "title": vm.tr(lesson.str("exTitleKey")),
                "sub": lesson.str("quizHeadKey"),
                "questions": lesson.rawArray("quiz"),
            ])
            DispatchExercise(vm: vm, spec: spec, titleFallback: vm.tr(lesson.str("exTitleKey"))) {
                vm.markHw(id)
            }
        }
    }
}

private struct VlsmRow {
    var pfx = ""
    var net = ""
    var bc = ""
    var lo = ""
    var hi = ""
}

private struct VlsmEx: View {
    @ObservedObject var vm: AppModel
    let lesson: J
    @State private var done: [Bool]
    @State private var rows: [[VlsmRow]]
    @State private var fb = ("", "")

    init(vm: AppModel, lesson: J) {
        self.vm = vm
        self.lesson = lesson
        let answers = vm.content.netAnswers
        _done = State(initialValue: Array(repeating: false, count: vm.content.netTasks.count))
        _rows = State(initialValue: answers.map { block in Array(repeating: VlsmRow(), count: block.count) })
    }

    var body: some View {
        let tasks = vm.content.netTasks
        let answers = vm.content.netAnswers
        let p = vm.palette
        ScrollView {
                VStack(alignment: .leading, spacing: 12) {
                    ForEach(tasks.indices, id: \.self) { ti in
                        let task = tasks[ti]
                        let block = ti < answers.count ? answers[ti] : []
                        let infeasible = block.isEmpty || block.allSatisfy { $0.int("pfx") <= 0 }
                        GlassCard {
                            Label {
                                Text(task.str("prompt"))
                            } icon: {
                                Image(systemName: done[ti] ? "checkmark.circle.fill" : "\(ti + 1).circle")
                                    .foregroundStyle(done[ti] ? p.success : .secondary)
                            }
                        }
                        .padding(.vertical, 4)
                        if infeasible {
                            Hint(text: vm.tr("net_infeasible"))
                        } else {
                            ForEach(block.indices, id: \.self) { si in
                                Text("Podsíť \(String(UnicodeScalar(65 + si)!))")
                                    .font(.subheadline.weight(.semibold))
                                    .foregroundStyle(.secondary)
                                    .padding(.top, 6)
                                PrefixMenu(value: Binding(
                                    get: { rows[ti][si].pfx },
                                    set: { rows[ti][si].pfx = $0 }
                                ), palette: p)
                                WordField(value: Binding(get: { rows[ti][si].net }, set: { rows[ti][si].net = $0 }), placeholder: "síť", palette: p)
                                    .keyboardType(.decimalPad)
                                WordField(value: Binding(get: { rows[ti][si].bc }, set: { rows[ti][si].bc = $0 }), placeholder: "broadcast", palette: p)
                                    .keyboardType(.decimalPad)
                                WordField(value: Binding(get: { rows[ti][si].lo }, set: { rows[ti][si].lo = $0 }), placeholder: "první uzel", palette: p)
                                    .keyboardType(.decimalPad)
                                WordField(value: Binding(get: { rows[ti][si].hi }, set: { rows[ti][si].hi = $0 }), placeholder: "poslední uzel", palette: p)
                                    .keyboardType(.decimalPad)
                            }
                            PillButton(label: vm.tr("check")) {
                                let ok = block.indices.allSatisfy { si in
                                    let a = block[si]
                                    let field = rows[ti][si]
                                    return Int(field.pfx.replacingOccurrences(of: "/", with: "")) == a.int("pfx")
                                        && netTxtEq(field.net, a.str("net"))
                                        && netTxtEq(field.bc, a.str("bcast"))
                                        && netTxtEq(field.lo, a.str("lo"))
                                        && netTxtEq(field.hi, a.str("hi"))
                                }
                                if ok {
                                    done[ti] = true
                                    fb = (vm.tr("feedback_ok"), "ok")
                                    maybeFinish()
                                } else {
                                    fb = (vm.tr("feedback_retry"), "err")
                                }
                            }
                            .padding(.vertical, 6)
                        }
                        PillButton(label: vm.tr("net_solution")) {
                            if !infeasible {
                                for si in block.indices {
                                    rows[ti][si].pfx = "/\(block[si].int("pfx"))"
                                    rows[ti][si].net = block[si].str("net")
                                    rows[ti][si].bc = block[si].str("bcast")
                                    rows[ti][si].lo = block[si].str("lo")
                                    rows[ti][si].hi = block[si].str("hi")
                                }
                            }
                            done[ti] = true
                            fb = ("", "")
                            maybeFinish()
                        }
                        if done[ti] { Meaning(text: task.str("solution"), palette: p, visible: true) }
                        Divider().padding(.vertical, 8)
                    }
                }
                .exerciseContent()
        }
        .scrollDismissesKeyboard(.interactively)
        .detailScreen(vm.tr("net_ex_title"))
        .glassBottomBar {
            if !fb.0.isEmpty {
                FeedbackLine(text: fb.0, kind: fb.1, palette: p)
                    .padding(.horizontal)
                    .padding(.vertical, 8)
                    .frame(maxWidth: .infinity, alignment: .leading)
            }
        }
        .animation(.default, value: fb.0)
    }

    private func maybeFinish() {
        if !done.isEmpty && done.allSatisfy({ $0 }) { vm.markNet(1) }
    }
}

private struct PrefixMenu: View {
    @Binding var value: String
    let palette: Palette
    @State private var open = false

    var body: some View {
        Button { open = true } label: {
            HStack {
                Text("Prefix").foregroundStyle(.secondary)
                Spacer()
                Text(value.isEmpty ? "/…" : value)
                    .foregroundStyle(value.isEmpty ? Color.secondary : Color.primary)
                    .monospacedDigit()
                Image(systemName: "chevron.up.chevron.down").font(.caption2).foregroundStyle(.secondary)
            }
        }
        .buttonStyle(.bordered)
        .tint(.secondary)
        .sheet(isPresented: $open) {
            WordPicker(palette: palette, pool: (8...30).map { "/\($0)" }) { word in
                if let word { value = word }
                open = false
            }
            .sheetDetents()
        }
    }
}

struct LitQuizScreen: View {
    @ObservedObject var vm: AppModel
    let id: String
    @State private var idx = 0
    @State private var score = 0
    @State private var answered = false
    @State private var finished = false
    @State private var picked = -1

    var body: some View {
        if let book = vm.content.book(id) {
            let qs = book.arr("quiz")
            let p = vm.palette
            ScrollView {
                VStack(alignment: .leading, spacing: 14) {
                    if finished {
                        GlassCard {
                            VStack(alignment: .leading, spacing: 10) {
                                Label(vm.tr("lit_finished"), systemImage: "rosette")
                                    #if os(macOS)
                                    .font(.largeTitle.weight(.bold))
                                    #else
                                    .font(.title2.weight(.bold))
                                    #endif
                                Text(vm.tr("lit_finished_text"))
                                    #if os(macOS)
                                    .font(.title3)
                                    #endif
                                    .foregroundStyle(.secondary)
                                Text(vm.fmt("lit_score_fmt", score, qs.count))
                                    #if os(macOS)
                                    .font(.title2.weight(.semibold))
                                    #else
                                    .font(.headline)
                                    #endif
                            }
                        }
                    } else if idx < qs.count {
                        let q = qs[idx]
                        ProgressView(value: Double(idx + (answered ? 1 : 0)), total: Double(qs.count))
                        HStack {
                            Text(vm.fmt("lit_question_fmt", idx + 1, qs.count))
                            Spacer()
                            Text(vm.fmt("lit_score_fmt", score, qs.count))
                        }
                        #if os(macOS)
                        .font(.body.weight(.semibold))
                        #else
                        .font(.footnote)
                        #endif
                        .foregroundStyle(.secondary)
                        Text(q.str("prompt"))
                            #if os(macOS)
                            .font(.title.weight(.semibold))
                            #else
                            .font(.title3.weight(.semibold))
                            #endif
                            .padding(.vertical, 12)
                        ForEach(Array(q.strs("options").enumerated()), id: \.offset) { i, opt in
                            let correct = i == q.int("correct")
                            let chosen = answered && picked == i
                            OptionChip(
                                text: opt,
                                selected: chosen,
                                mark: answered ? (correct ? true : (chosen ? false : nil)) : nil
                            ) {
                                guard !answered else { return }
                                picked = i
                                answered = true
                                if correct { score += 1 }
                            }
                        }
                        if answered {
                            Meaning(text: q.str("expl"), palette: p, visible: true)
                        }
                    }
                }
                .exerciseContent()
            }
            .detailScreen(vm.tr(book.str("quizTitle")), subtitle: vm.tr(book.str("quizSub")))
            .glassBottomBar {
                if finished {
                    BottomAction(label: vm.tr("lit_again"), action: {
                        idx = 0; score = 0; answered = false; finished = false; picked = -1
                    }) { EmptyView() }
                } else if answered {
                    BottomAction(label: idx + 1 >= qs.count ? vm.tr("lit_show_result") : vm.tr("lit_next"), action: {
                        if idx + 1 >= qs.count {
                            finished = true
                            vm.markBookQuiz(id)
                        } else {
                            idx += 1
                            answered = false
                            picked = -1
                        }
                    }) { EmptyView() }
                }
            }
            .animation(.default, value: answered)
        }
    }
}

struct PlotScreen: View {
    @ObservedObject var vm: AppModel
    let id: String

    var body: some View {
        if let book = vm.content.book(id) {
            let spec = J([
                "type": "assembly",
                "title": vm.tr(book.str("plotTitle")),
                "sub": book.str("plotSub"),
                "items": book.rawArray("plot"),
                "meanings": book.rawArray("plotMeaning"),
            ])
            DispatchExercise(vm: vm, spec: spec, titleFallback: vm.tr(book.str("plotTitle"))) {
                vm.markBookPlot(id)
            }
        }
    }
}

