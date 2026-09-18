import SwiftUI

struct ExerciseScaffold<Content: View>: View {
    @ObservedObject var vm: AppModel
    let title: String
    var sub: String?
    let action: String
    var onAction: () -> Void
    let feedback: String
    let kind: String
    @ViewBuilder var content: () -> Content

    var body: some View {
        let p = vm.palette
        VStack(alignment: .leading, spacing: 0) {
            PageTop(vm: vm, back: { vm.back() }, title: title, subtitle: sub)
            ScrollView {
                VStack(alignment: .leading, spacing: 0) { content() }
            }
            FeedbackLine(text: feedback, kind: kind, palette: p)
            PrimaryButton(label: action, palette: p, action: onAction)
                .padding(.vertical, 12)
        }
        .padding(.horizontal, 16)
    }
}

struct GermanExercise: View {
    @ObservedObject var vm: AppModel
    let unitId: Int
    let ex: Int

    var body: some View {
        if let spec = vm.content.germanUnit(unitId)?.exercise(ex) {
            DispatchExercise(vm: vm, spec: spec, titleFallback: spec.str("title")) {
                vm.markGerman(unitId, ex)
            }
        }
    }
}

struct VocabExercise: View {
    @ObservedObject var vm: AppModel
    let unitId: Int

    var body: some View {
        if let spec = vm.content.germanUnit(unitId)?.obj("vocab") {
            DispatchExercise(vm: vm, spec: spec, titleFallback: vm.tr("Vokabeltraining")) {
                vm.markVocab(unitId)
            }
        }
    }
}

struct MluvExercise: View {
    @ObservedObject var vm: AppModel
    let n: Int

    var body: some View {
        if let spec = vm.content.mluv(n) {
            DispatchExercise(vm: vm, spec: spec, titleFallback: spec.str("title")) {
                vm.markMluv(n)
            }
        }
    }
}

struct DispatchExercise: View {
    @ObservedObject var vm: AppModel
    let spec: J
    let titleFallback: String
    var onDone: () -> Void

    var body: some View {
        let title = spec.str("title").isEmpty ? titleFallback : spec.str("title")
        let sub = spec.strOrNull("sub").map { vm.tr($0) }
        switch spec.str("type") {
        case "choice": ChoiceEx(vm: vm, title: title, sub: sub, spec: spec, onDone: onDone)
        case "assembly": AssemblyEx(vm: vm, title: title, sub: sub, spec: spec, onDone: onDone)
        case "typed", "kw": TypedEx(vm: vm, title: title, sub: sub, spec: spec, onDone: onDone)
        case "free", "profile": FreeEx(vm: vm, title: title, sub: sub, spec: spec, onDone: onDone)
        case "assign": AssignEx(vm: vm, title: title, sub: sub, spec: spec, onDone: onDone)
        case "hangman": HangmanEx(vm: vm, title: title, sub: sub, spec: spec, onDone: onDone)
        case "vocab": VocabEx(vm: vm, title: title, sub: sub, spec: spec, onDone: onDone)
        case "dialog": DialogEx(vm: vm, title: title, sub: sub, spec: spec, onDone: onDone)
        case "number", "count", "seq", "verb", "verbclue", "verb_sections", "ordne", "fill":
            ComboEx(vm: vm, title: title, sub: sub, spec: spec, onDone: onDone)
        case "ex2": Ex2Ex(vm: vm, title: title, sub: sub, spec: spec, onDone: onDone)
        case "letters", "letters_gap": LettersEx(vm: vm, title: title, sub: sub, spec: spec, onDone: onDone)
        case "table": TableEx(vm: vm, title: title, sub: sub, spec: spec, onDone: onDone)
        case "reveal": RevealEx(vm: vm, title: title, sub: sub, spec: spec, onDone: onDone)
        default: Text("?").foregroundStyle(vm.palette.text)
        }
    }
}

private struct ChoiceEx: View {
    @ObservedObject var vm: AppModel
    let title: String
    var sub: String?
    let spec: J
    var onDone: () -> Void
    @State private var picks: [Int]
    @State private var fb = ("", "")
    @State private var revealed = false

    init(vm: AppModel, title: String, sub: String?, spec: J, onDone: @escaping () -> Void) {
        self.vm = vm
        self.title = title
        self.sub = sub
        self.spec = spec
        self.onDone = onDone
        _picks = State(initialValue: Array(repeating: -1, count: spec.arr("questions").count))
    }

    var body: some View {
        let qs = spec.arr("questions")
        let meanings = spec.strs("meanings")
        let expls = spec.strs("expls")
        let p = vm.palette
        ExerciseScaffold(vm: vm, title: title, sub: sub, action: vm.tr("check"), onAction: {
            let ok = qs.indices.filter { picks[$0] == qs[$0].int("correct") }.count
            if ok == qs.count { fb = (vm.tr("feedback_ok"), "ok"); onDone() }
            else { fb = (vm.tr("feedback_retry_short"), "err") }
            revealed = true
        }, feedback: fb.0, kind: fb.1) {
            ForEach(qs.indices, id: \.self) { i in
                Text("\(i + 1). \(qs[i].str("prompt"))")
                    .font(.system(size: 16, weight: .semibold))
                    .foregroundStyle(p.text)
                    .padding(.bottom, 6)
                ForEach(Array(qs[i].strs("options").enumerated()), id: \.offset) { o, opt in
                    let sel = picks[i] == o
                    let mark: Bool? = revealed && sel ? picks[i] == qs[i].int("correct") : nil
                    OptionChip(text: opt, selected: sel, mark: mark, palette: p) { picks[i] = o }
                }
                if revealed {
                    if i < meanings.count { Meaning(text: vm.tr(meanings[i]), palette: p, visible: true) }
                    if i < expls.count { Meaning(text: expls[i], palette: p, visible: true) }
                }
                Spacer().frame(height: 10)
            }
        }
    }
}

private struct AssemblyEx: View {
    @ObservedObject var vm: AppModel
    let title: String
    var sub: String?
    let spec: J
    var onDone: () -> Void
    @State private var placed: [[Int]]
    @State private var pools: [[Int]]
    @State private var fb = ("", "")
    @State private var revealed = false

    init(vm: AppModel, title: String, sub: String?, spec: J, onDone: @escaping () -> Void) {
        self.vm = vm
        self.title = title
        self.sub = sub
        self.spec = spec
        self.onDone = onDone
        let items = spec.arr("items")
        _placed = State(initialValue: items.map { _ in [] })
        _pools = State(initialValue: items.map { item in
            Array(0..<item.strs("words").count).shuffled()
        })
    }

    var body: some View {
        let items = spec.arr("items")
        let meanings = spec.strs("meanings")
        let p = vm.palette
        ExerciseScaffold(vm: vm, title: title, sub: sub, action: vm.tr("check"), onAction: {
            let ok = items.indices.allSatisfy { i in
                let words = items[i].strs("words")
                return placed[i].count == words.count && placed[i].map { words[$0] } == words
            }
            if ok { fb = (vm.tr("feedback_ok"), "ok"); onDone() }
            else { fb = (vm.tr("feedback_sentences"), "err") }
            revealed = true
        }, feedback: fb.0, kind: fb.1) {
            ForEach(items.indices, id: \.self) { i in
                if let prompt = items[i].strOrNull("prompt") { Hint(text: prompt, palette: p) }
                FlowLayout {
                    if placed[i].isEmpty { Text("…").foregroundStyle(p.overlay) }
                    ForEach(placed[i], id: \.self) { idx in
                        if idx < items[i].strs("words").count {
                            WordChip(text: items[i].strs("words")[idx], palette: p) {
                                placed[i].removeAll { $0 == idx }
                                pools[i].append(idx)
                            }
                        }
                    }
                }
                .padding(8)
                .frame(maxWidth: .infinity, alignment: .leading)
                .background(p.surface0, in: RoundedRectangle(cornerRadius: 14, style: .continuous))
                .padding(.bottom, 6)
                FlowLayout(spacing: 6) {
                    ForEach(pools[i], id: \.self) { idx in
                        if idx < items[i].strs("words").count {
                            WordChip(text: items[i].strs("words")[idx], palette: p) {
                                pools[i].removeAll { $0 == idx }
                                placed[i].append(idx)
                            }
                        }
                    }
                }
                if revealed, i < meanings.count {
                    Meaning(text: vm.tr(meanings[i]), palette: p, visible: true)
                }
                Spacer().frame(height: 12)
            }
        }
    }
}

private struct TypedEx: View {
    @ObservedObject var vm: AppModel
    let title: String
    var sub: String?
    let spec: J
    var onDone: () -> Void
    @State private var fields: [[String]]
    @State private var fb = ("", "")
    @State private var revealed = false

    init(vm: AppModel, title: String, sub: String?, spec: J, onDone: @escaping () -> Void) {
        self.vm = vm
        self.title = title
        self.sub = sub
        self.spec = spec
        self.onDone = onDone
        let items = spec.arr("rows").isEmpty ? spec.arr("items") : spec.arr("rows")
        _fields = State(initialValue: items.map { q in
            Array(repeating: "", count: max(1, q.strs("hints").count, q.answerList().count))
        })
    }

    var body: some View {
        let items = spec.arr("rows").isEmpty ? spec.arr("items") : spec.arr("rows")
        let p = vm.palette
        let umlaut = items.contains { q in
            q.answerList().contains { hasUmlaut($0) } || hasUmlaut(q.str("prompt"))
        }
        ExerciseScaffold(vm: vm, title: title, sub: sub, action: vm.tr("check"), onAction: {
            var ok = 0, total = 0, incomplete = false
            for (i, q) in items.enumerated() {
                let answers = q.answerList()
                for (j, value) in fields[i].enumerated() {
                    total += 1
                    let ans = j < answers.count ? answers[j] : ""
                    let norm = normalizeAnswer(value)
                    if answerAccepts(norm, ans) { ok += 1 }
                    else if answerIncomplete(norm, ans) { incomplete = true }
                }
            }
            if total > 0 && ok == total { fb = (vm.tr("feedback_ok"), "ok"); onDone() }
            else if incomplete { fb = (vm.tr("trans_incomplete"), "warn") }
            else { fb = (vm.tr("feedback_retry"), "err") }
            revealed = true
        }, feedback: fb.0, kind: fb.1) {
            if let note = spec.strOrNull("note") { Hint(text: note, palette: p) }
            if let sample = spec.strOrNull("sample") { Hint(text: sample, palette: p) }
            if let bank = spec.strOrNull("bank") { Hint(text: "\(vm.tr("wordbank")) \(bank)", palette: p) }
            if umlaut { Hint(text: vm.tr("hint_umlauts"), palette: p) }
            ForEach(items.indices, id: \.self) { i in
                let q = items[i]
                Prompt(text: q.str("prompt"), palette: p)
                let answers = q.answerList()
                let hints = q.strs("hints")
                ForEach(fields[i].indices, id: \.self) { j in
                    let ans = j < answers.count ? answers[j] : ""
                    WordField(
                        value: Binding(get: { fields[i][j] }, set: { fields[i][j] = $0 }),
                        palette: p,
                        placeholder: j < hints.count ? hints[j] : vm.tr("your_answer"),
                        mark: revealed ? answerAccepts(normalizeAnswer(fields[i][j]), ans) : nil
                    )
                    .padding(.bottom, 4)
                }
                if revealed {
                    if let m = q.strOrNull("meaning") { Meaning(text: vm.tr(m), palette: p, visible: true) }
                    if let m = q.strOrNull("mean") { Meaning(text: vm.tr(m), palette: p, visible: true) }
                    if let g = q.strOrNull("german") { Meaning(text: g, palette: p, visible: true) }
                    if let s = q.strOrNull("shown") { Meaning(text: s, palette: p, visible: true) }
                }
                Spacer().frame(height: 10)
            }
        }
    }
}

private struct FreeEx: View {
    @ObservedObject var vm: AppModel
    let title: String
    var sub: String?
    let spec: J
    var onDone: () -> Void
    @State private var shown: [Bool]
    @State private var values: [String]
    @State private var fb = ("", "")

    init(vm: AppModel, title: String, sub: String?, spec: J, onDone: @escaping () -> Void) {
        self.vm = vm
        self.title = title
        self.sub = sub
        self.spec = spec
        self.onDone = onDone
        let qs = spec.arr("questions").isEmpty ? spec.arr("rows") : spec.arr("questions")
        _shown = State(initialValue: Array(repeating: false, count: qs.count))
        _values = State(initialValue: Array(repeating: "", count: qs.count))
    }

    var body: some View {
        let qs = spec.arr("questions").isEmpty ? spec.arr("rows") : spec.arr("questions")
        let p = vm.palette
        ExerciseScaffold(vm: vm, title: title, sub: sub, action: vm.tr("finish"), onAction: {
            fb = (vm.tr("feedback_ok"), "ok")
            shown = shown.map { _ in true }
            onDone()
        }, feedback: fb.0, kind: fb.1) {
            if let tip = spec.strOrNull("tip") { Hint(text: vm.tr(tip), palette: p) }
            ForEach(qs.indices, id: \.self) { i in
                let q = qs[i]
                Prompt(text: q.str("question").isEmpty ? q.str("stem") : q.str("question"), palette: p)
                WordField(value: Binding(get: { values[i] }, set: { values[i] = $0 }), palette: p, placeholder: vm.tr("your_answer"))
                PillButton(label: vm.tr("show_sample"), palette: p) { shown[i].toggle() }
                    .padding(.vertical, 6)
                if shown[i] {
                    let sample = q.str("sample")
                    let qcs = q.str("qCs")
                    let acs = q.str("aCs")
                    Meaning(
                        text: qcs.isEmpty ? sample : vm.fmt("sample_fmt", vm.tr(qcs), sample, vm.tr(acs)),
                        palette: p,
                        visible: true
                    )
                }
                Spacer().frame(height: 10)
            }
        }
    }
}

private struct AssignEx: View {
    @ObservedObject var vm: AppModel
    let title: String
    var sub: String?
    let spec: J
    var onDone: () -> Void
    @State private var active = 0
    @State private var loc: [Int]
    @State private var fb = ("", "")
    @State private var revealed = false

    init(vm: AppModel, title: String, sub: String?, spec: J, onDone: @escaping () -> Void) {
        self.vm = vm
        self.title = title
        self.sub = sub
        self.spec = spec
        self.onDone = onDone
        _loc = State(initialValue: Array(repeating: -1, count: spec.arr("items").count))
    }

    var body: some View {
        let items = spec.arr("items")
        let groups = spec.strs("groups")
        let meanings = spec.strs("meanings")
        let p = vm.palette
        ExerciseScaffold(vm: vm, title: title, sub: sub, action: vm.tr("check"), onAction: {
            let ok = items.indices.allSatisfy { loc[$0] == items[$0].int("group") }
            if ok { fb = (vm.tr("feedback_ok"), "ok"); onDone() }
            else { fb = (vm.tr("feedback_retry"), "err") }
            revealed = true
        }, feedback: fb.0, kind: fb.1) {
            Hint(text: vm.tr("assign_hint"), palette: p)
            FlowLayout(spacing: 8) {
                ForEach(groups.indices, id: \.self) { i in
                    SegChip(label: groups[i], selected: active == i, palette: p) { active = i }
                }
            }
            Text(vm.tr("wordbank")).font(.system(size: 12)).foregroundStyle(p.subtext).padding(.top, 10)
            FlowLayout(spacing: 6) {
                ForEach(items.indices, id: \.self) { i in
                    if loc[i] == -1 {
                        let label = [items[i].strOrNull("emoji"), items[i].str("label")].compactMap { $0 }.joined(separator: " ")
                        WordChip(text: label, palette: p) { loc[i] = active }
                    }
                }
            }
            ForEach(groups.indices, id: \.self) { g in
                Text(groups[g]).font(.system(size: 12, weight: .semibold)).foregroundStyle(p.subtext).padding(.top, 10)
                FlowLayout(spacing: 6) {
                    ForEach(items.indices, id: \.self) { i in
                        if loc[i] == g {
                            let label = [items[i].strOrNull("emoji"), items[i].str("label")].compactMap { $0 }.joined(separator: " ")
                            WordChip(text: label, palette: p) { loc[i] = -1 }
                        }
                    }
                }
            }
            if revealed {
                ForEach(items.indices, id: \.self) { i in
                    if i < meanings.count { Meaning(text: vm.tr(meanings[i]), palette: p, visible: true) }
                }
            }
        }
    }
}

private struct HangmanEx: View {
    @ObservedObject var vm: AppModel
    let title: String
    var sub: String?
    let spec: J
    var onDone: () -> Void
    @State private var word = 0
    @State private var misses = 0
    @State private var guessed: [Bool]
    @State private var fb = ("", "")
    @State private var finished = false

    init(vm: AppModel, title: String, sub: String?, spec: J, onDone: @escaping () -> Void) {
        self.vm = vm
        self.title = title
        self.sub = sub
        self.spec = spec
        self.onDone = onDone
        _guessed = State(initialValue: Array(repeating: false, count: spec.strs("letters").count))
    }

    var body: some View {
        let words = spec.strs("words")
        let tips = spec.strs("tips")
        let letters = spec.strs("letters")
        let p = vm.palette
        let cur = word < words.count ? words[word].uppercased() : ""
        VStack(alignment: .leading, spacing: 8) {
            PageTop(vm: vm, back: { vm.back() }, title: title, subtitle: sub)
            Text(vm.fmt("hm_progress", word + 1, words.count)).foregroundStyle(p.subtext)
            if word < tips.count { Hint(text: "\(vm.tr("hm_hint")): \(vm.tr(tips[word]))", palette: p) }
            Canvas { ctx, size in
                var gallows = Path()
                gallows.move(to: CGPoint(x: 40, y: size.height - 10))
                gallows.addLine(to: CGPoint(x: size.width * 0.55, y: size.height - 10))
                gallows.move(to: CGPoint(x: 70, y: size.height - 10))
                gallows.addLine(to: CGPoint(x: 70, y: 16))
                gallows.addLine(to: CGPoint(x: size.width * 0.3 + 40, y: 16))
                ctx.stroke(gallows, with: .color(p.text), style: StrokeStyle(lineWidth: 6, lineCap: .round))
                let x = size.width * 0.3 + 40
                if misses > 0 {
                    var rope = Path(); rope.move(to: CGPoint(x: x, y: 16)); rope.addLine(to: CGPoint(x: x, y: 36))
                    ctx.stroke(rope, with: .color(p.text), lineWidth: 4)
                }
                if misses > 1 {
                    ctx.stroke(Path(ellipseIn: CGRect(x: x - 16, y: 36, width: 32, height: 32)), with: .color(p.text), lineWidth: 4)
                }
                if misses > 2 {
                    var body = Path(); body.move(to: CGPoint(x: x, y: 68)); body.addLine(to: CGPoint(x: x, y: 110))
                    ctx.stroke(body, with: .color(p.text), lineWidth: 4)
                }
                if misses > 3 {
                    var a = Path(); a.move(to: CGPoint(x: x, y: 80)); a.addLine(to: CGPoint(x: x - 22, y: 100))
                    ctx.stroke(a, with: .color(p.text), lineWidth: 4)
                }
                if misses > 4 {
                    var a = Path(); a.move(to: CGPoint(x: x, y: 80)); a.addLine(to: CGPoint(x: x + 22, y: 100))
                    ctx.stroke(a, with: .color(p.text), lineWidth: 4)
                }
                if misses > 5 {
                    var a = Path(); a.move(to: CGPoint(x: x, y: 110)); a.addLine(to: CGPoint(x: x - 22, y: 140))
                    ctx.stroke(a, with: .color(p.text), lineWidth: 4)
                }
            }
            .frame(height: 160)
            HStack(spacing: 6) {
                ForEach(Array(cur.enumerated()), id: \.offset) { _, ch in
                    let idx = letters.firstIndex { $0.compare(String(ch), options: .caseInsensitive) == .orderedSame }
                    let show = idx.map { guessed[$0] } ?? false
                    Text(show ? String(ch) : "_")
                        .font(.system(size: 26, weight: .bold))
                        .foregroundStyle(p.text)
                }
            }
            ForEach(Array(letters.chunked(10).enumerated()), id: \.offset) { _, row in
                HStack(spacing: 4) {
                    ForEach(row, id: \.self) { letter in
                        if let i = letters.firstIndex(of: letter) {
                            Button {
                                guessed[i] = true
                                if !cur.localizedCaseInsensitiveContains(letter) {
                                    misses += 1
                                    if misses >= 6 { fb = (vm.fmt("hm_fail", cur), "err") }
                                } else if cur.allSatisfy({ ch in
                                    let idx = letters.firstIndex { $0.compare(String(ch), options: .caseInsensitive) == .orderedSame }
                                    return idx.map { guessed[$0] } ?? false
                                }) {
                                    if word + 1 >= words.count {
                                        finished = true
                                        fb = (vm.tr("hm_done"), "ok")
                                        onDone()
                                    } else {
                                        fb = (vm.tr("hm_wrong"), "ok")
                                        word += 1
                                        misses = 0
                                        guessed = guessed.map { _ in false }
                                    }
                                }
                            } label: {
                                Text(letter)
                                    .font(.system(size: 15, weight: .bold))
                                    .foregroundStyle(p.text)
                                    .padding(.horizontal, 10)
                                    .padding(.vertical, 8)
                                    .background(guessed[i] ? p.surface1 : p.mantle, in: RoundedRectangle(cornerRadius: 8))
                            }
                            .buttonStyle(.plain)
                            .disabled(guessed[i] || finished)
                        }
                    }
                }
            }
            FeedbackLine(text: fb.0, kind: fb.1, palette: p)
            if misses >= 6 && !finished {
                PillButton(label: vm.tr("hm_retry"), palette: p) {
                    misses = 0
                    guessed = guessed.map { _ in false }
                    fb = ("", "")
                }
            }
            Spacer()
        }
        .padding(.horizontal, 16)
    }
}

private extension Array {
    func chunked(_ size: Int) -> [[Element]] {
        stride(from: 0, to: count, by: size).map { Array(self[$0..<Swift.min($0 + size, count)]) }
    }
}

private struct VocabEx: View {
    @ObservedObject var vm: AppModel
    let title: String
    var sub: String?
    let spec: J
    var onDone: () -> Void
    @State private var cards: [(String, String, String)]
    @State private var index = 0
    @State private var done = 0
    @State private var value = ""
    @State private var fb = ("", "")
    let total: Int

    init(vm: AppModel, title: String, sub: String?, spec: J, onDone: @escaping () -> Void) {
        self.vm = vm
        self.title = title
        self.sub = sub
        self.spec = spec
        self.onDone = onDone
        let list = spec.arr("sections").flatMap { sec in
            sec.arr("rows").map { (sec.str("header"), $0.str("prompt"), $0.str("answers")) }
        }
        _cards = State(initialValue: list)
        total = list.count
    }

    var body: some View {
        let p = vm.palette
        VStack(alignment: .leading, spacing: 8) {
            PageTop(vm: vm, back: { vm.back() }, title: title, subtitle: sub)
            Text(vm.fmt("trans_progress", done, total)).foregroundStyle(p.subtext)
            if cards.isEmpty {
                Text(vm.tr("trans_done")).font(.system(size: 16, weight: .bold)).foregroundStyle(p.success)
            } else if index < cards.count {
                let cur = cards[index]
                Hint(text: cur.0, palette: p)
                Prompt(text: cur.1, palette: p)
                if hasUmlaut(cur.2) { Hint(text: vm.tr("hint_umlauts"), palette: p) }
                WordField(value: $value, palette: p, placeholder: vm.tr("your_answer"))
                PrimaryButton(label: vm.tr("check"), palette: p) {
                    let norm = normalizeAnswer(value)
                    if answerAccepts(norm, cur.2) {
                        done += 1
                        cards.remove(at: index)
                        if cards.isEmpty { fb = (vm.tr("trans_done"), "ok"); onDone() }
                        else { if index >= cards.count { index = 0 }; value = ""; fb = ("", "") }
                    } else if answerIncomplete(norm, cur.2) {
                        fb = (vm.tr("trans_incomplete"), "warn")
                    } else {
                        fb = (vm.fmt("trans_wrong", cur.2.split(separator: "|").first.map(String.init) ?? ""), "err")
                        let item = cards.remove(at: index)
                        cards.append(item)
                        if index >= cards.count { index = 0 }
                        value = ""
                    }
                }
                FeedbackLine(text: fb.0, kind: fb.1, palette: p)
            }
            Spacer()
        }
        .padding(.horizontal, 16)
    }
}
