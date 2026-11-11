import SwiftUI

struct ExerciseScaffold<Content: View>: View {
    @ObservedObject var vm: AppModel
    let title: String
    var sub: String?
    let action: String
    var onAction: () -> Void
    let feedback: String
    let kind: String
    var header = true
    @ViewBuilder var content: () -> Content

    var body: some View {
        let p = vm.palette
        ExercisePage(title: title, sub: sub, header: header, content: content)
        .glassBottomBar {
            BottomAction(label: action, action: onAction) {
                if !feedback.isEmpty {
                    FeedbackLine(text: feedback, kind: kind, palette: p)
                        .frame(maxWidth: .infinity, alignment: .leading)
                        .transition(.opacity.combined(with: .scale(scale: 0.96, anchor: .leading)))
                }
            }
        }
        .animation(Motion.feedback, value: feedback)
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
        default: Text("?").foregroundStyle(.primary)
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
                QuestionCard(number: i + 1, prompt: qs[i].str("prompt")) {
                    choiceOptions(qs[i], index: i)
                    if revealed {
                        if i < meanings.count { Meaning(text: vm.tr(meanings[i]), palette: p, visible: true) }
                        if i < expls.count { Meaning(text: expls[i], palette: p, visible: true) }
                        if let expl = qs[i].strOrNull("expl") {
                            Meaning(text: expl, palette: p, visible: true)
                        }
                    }
                }
            }
        }
    }

    @ViewBuilder
    private func choiceOptions(_ question: J, index: Int) -> some View {
        let options = question.strs("options")
        #if os(macOS)
        LazyVGrid(
            columns: [
                GridItem(.flexible(), spacing: 12, alignment: .top),
                GridItem(.flexible(), spacing: 12, alignment: .top),
            ],
            spacing: 10
        ) {
            optionChips(options, index: index, correct: question.int("correct"))
        }
        #else
        VStack(spacing: 8) {
            optionChips(options, index: index, correct: question.int("correct"))
        }
        #endif
    }

    @ViewBuilder
    private func optionChips(_ options: [String], index: Int, correct: Int) -> some View {
        ForEach(Array(options.enumerated()), id: \.offset) { o, opt in
            let sel = picks[index] == o
            let mark: Bool? = revealed && sel ? picks[index] == correct : nil
            OptionChip(text: opt, selected: sel, mark: mark) { picks[index] = o }
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
                QuestionCard(number: i + 1, prompt: items[i].strOrNull("prompt")) {
                WordSlot(empty: placed[i].isEmpty) {
                    FlowLayout {
                        ForEach(placed[i], id: \.self) { idx in
                            if idx < items[i].strs("words").count {
                                WordChip(text: items[i].strs("words")[idx]) {
                                    placed[i].removeAll { $0 == idx }
                                    pools[i].append(idx)
                                }
                            }
                        }
                    }
                }
                FlowLayout(spacing: 6) {
                    ForEach(pools[i], id: \.self) { idx in
                        if idx < items[i].strs("words").count {
                            WordChip(text: items[i].strs("words")[idx]) {
                                pools[i].removeAll { $0 == idx }
                                placed[i].append(idx)
                            }
                        }
                    }
                }
                if revealed, i < meanings.count {
                    Meaning(text: vm.tr(meanings[i]), palette: p, visible: true)
                }
                }
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
            if let note = spec.strOrNull("note") { Hint(text: note) }
            if let sample = spec.strOrNull("sample") { Hint(text: sample) }
            if let bank = spec.strOrNull("bank") { Hint(text: "\(vm.tr("wordbank")) \(bank)") }
            if umlaut { Hint(text: vm.tr("hint_umlauts")) }
            ForEach(items.indices, id: \.self) { i in
                let q = items[i]
                let answers = q.answerList()
                let hints = q.strs("hints")
                QuestionCard(number: i + 1, prompt: q.str("prompt")) {
                ForEach(fields[i].indices, id: \.self) { j in
                    let ans = j < answers.count ? answers[j] : ""
                    WordField(
                        value: Binding(get: { fields[i][j] }, set: { fields[i][j] = $0 }),
                        placeholder: j < hints.count ? hints[j] : vm.tr("your_answer"),
                        mark: revealed ? answerAccepts(normalizeAnswer(fields[i][j]), ans) : nil,
                        palette: p
                    )
                }
                if revealed {
                    if let m = q.strOrNull("meaning") { Meaning(text: vm.tr(m), palette: p, visible: true) }
                    if let m = q.strOrNull("mean") { Meaning(text: vm.tr(m), palette: p, visible: true) }
                    if let g = q.strOrNull("german") { Meaning(text: g, palette: p, visible: true) }
                    if let s = q.strOrNull("shown") { Meaning(text: s, palette: p, visible: true) }
                }
                }
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
            if let tip = spec.strOrNull("tip") { Hint(text: vm.tr(tip)) }
            ForEach(qs.indices, id: \.self) { i in
                let q = qs[i]
                QuestionCard(number: i + 1, prompt: q.str("question").isEmpty ? q.str("stem") : q.str("question")) {
                WordField(value: Binding(get: { values[i] }, set: { values[i] = $0 }), placeholder: vm.tr("your_answer"), palette: p)
                PillButton(label: vm.tr("show_sample")) { shown[i].toggle() }
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
                }
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
    @Environment(\.accent) private var accent

    private func label(_ item: J) -> String {
        [item.strOrNull("emoji"), item.str("label")].compactMap { $0 }.joined(separator: " ")
    }

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
            Hint(text: vm.tr("assign_hint"))
            QuestionCard {
                Text(vm.tr("wordbank")).kicker().foregroundStyle(p.ink3)
                FlowLayout(spacing: 8) {
                    ForEach(items.indices, id: \.self) { i in
                        if loc[i] == -1 {
                            WordChip(text: label(items[i])) { loc[i] = active }
                        }
                    }
                }
                .frame(minHeight: 36, alignment: .topLeading)
            }
            // Each group is a tray: tap it to make it the target, tap a word in it to send it back.
            LazyVGrid(
                columns: Array(repeating: GridItem(.flexible(), spacing: 12, alignment: .top), count: max(1, min(groups.count, onMac(3, 1)))),
                spacing: 12
            ) {
                ForEach(groups.indices, id: \.self) { g in
                    let selected = active == g
                    VStack(alignment: .leading, spacing: 10) {
                        Label(groups[g], systemImage: selected ? "largecircle.fill.circle" : "circle")
                            .font(onMac(Font.title3, Font.body).weight(.semibold))
                            .foregroundStyle(selected ? accent.text : p.ink)
                        FlowLayout(spacing: 8) {
                            ForEach(items.indices, id: \.self) { i in
                                if loc[i] == g {
                                    WordChip(text: label(items[i])) { loc[i] = -1 }
                                }
                            }
                        }
                    }
                    .padding(onMac(18, 14))
                    .frame(maxWidth: .infinity, minHeight: 110, alignment: .topLeading)
                    .background(selected ? accent.soft : p.surface, in: RoundedRectangle(cornerRadius: 18, style: .continuous))
                    .overlay {
                        RoundedRectangle(cornerRadius: 18, style: .continuous)
                            .strokeBorder(selected ? accent.solid : p.line, lineWidth: selected ? 1.5 : 1)
                    }
                    .contentShape(RoundedRectangle(cornerRadius: 18, style: .continuous))
                    .onTapGesture { active = g }
                    .accessibilityAddTraits(selected ? [.isButton, .isSelected] : .isButton)
                }
            }
            .animation(Motion.press, value: active)
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
    @Environment(\.accent) private var accent

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
        ExercisePage(title: title, sub: sub) {
            QuestionCard {
            HStack {
                Text(vm.fmt("hm_progress", word + 1, words.count)).kicker().foregroundStyle(p.ink3)
                Spacer()
                Text("\(misses) / 6").font(.subheadline.monospacedDigit()).foregroundStyle(misses >= 4 ? p.error : p.ink3)
            }
            if word < tips.count {
                Text("\(vm.tr("hm_hint")): \(vm.tr(tips[word]))")
                    .font(onMac(Font.title3, Font.body).weight(.semibold))
            }
            Canvas { ctx, size in
                var gallows = Path()
                gallows.move(to: CGPoint(x: 40, y: size.height - 10))
                gallows.addLine(to: CGPoint(x: size.width * 0.55, y: size.height - 10))
                gallows.move(to: CGPoint(x: 70, y: size.height - 10))
                gallows.addLine(to: CGPoint(x: 70, y: 16))
                gallows.addLine(to: CGPoint(x: size.width * 0.3 + 40, y: 16))
                ctx.stroke(gallows, with: .color(p.ink), style: StrokeStyle(lineWidth: 6, lineCap: .round))
                let x = size.width * 0.3 + 40
                if misses > 0 {
                    var rope = Path(); rope.move(to: CGPoint(x: x, y: 16)); rope.addLine(to: CGPoint(x: x, y: 36))
                    ctx.stroke(rope, with: .color(p.ink), lineWidth: 4)
                }
                if misses > 1 {
                    ctx.stroke(Path(ellipseIn: CGRect(x: x - 16, y: 36, width: 32, height: 32)), with: .color(p.ink), lineWidth: 4)
                }
                if misses > 2 {
                    var body = Path(); body.move(to: CGPoint(x: x, y: 68)); body.addLine(to: CGPoint(x: x, y: 110))
                    ctx.stroke(body, with: .color(p.ink), lineWidth: 4)
                }
                if misses > 3 {
                    var a = Path(); a.move(to: CGPoint(x: x, y: 80)); a.addLine(to: CGPoint(x: x - 22, y: 100))
                    ctx.stroke(a, with: .color(p.ink), lineWidth: 4)
                }
                if misses > 4 {
                    var a = Path(); a.move(to: CGPoint(x: x, y: 80)); a.addLine(to: CGPoint(x: x + 22, y: 100))
                    ctx.stroke(a, with: .color(p.ink), lineWidth: 4)
                }
                if misses > 5 {
                    var a = Path(); a.move(to: CGPoint(x: x, y: 110)); a.addLine(to: CGPoint(x: x - 22, y: 140))
                    ctx.stroke(a, with: .color(p.ink), lineWidth: 4)
                }
            }
            .frame(width: 300, height: 160)
            .frame(maxWidth: .infinity)
            HStack(spacing: 8) {
                ForEach(Array(cur.enumerated()), id: \.offset) { _, ch in
                    let idx = letters.firstIndex { $0.compare(String(ch), options: .caseInsensitive) == .orderedSame }
                    let show = idx.map { guessed[$0] } ?? false
                    Text(show ? String(ch) : " ")
                        .display(onMac(30, 24), relativeTo: .title)
                        .frame(width: onMac(34, 26), height: onMac(44, 36))
                        .overlay(alignment: .bottom) {
                            Capsule().fill(show ? accent.solid : p.ink3).frame(height: 3)
                        }
                }
            }
            .frame(maxWidth: .infinity)
            .padding(.vertical, 8)
            }
            FlowLayout(spacing: 8) {
                    ForEach(letters, id: \.self) { letter in
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
                                    .font(.body.weight(.semibold))
                                    .frame(minWidth: 22)
                            }
                            .buttonStyle(ChipStyle(compact: true))
                            .disabled(guessed[i] || finished)
                        }
                    }
            }
            FeedbackLine(text: fb.0, kind: fb.1, palette: p)
            if misses >= 6 && !finished {
                PillButton(label: vm.tr("hm_retry")) {
                    misses = 0
                    guessed = guessed.map { _ in false }
                    fb = ("", "")
                }
            }
        }
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
        ExercisePage(title: title, sub: sub) {
            HStack(spacing: 12) {
                ProgressBar(value: Double(done) / Double(max(total, 1)))
                Text(vm.fmt("trans_progress", done, total)).font(.subheadline.monospacedDigit()).foregroundStyle(p.ink2)
            }
            if cards.isEmpty {
                Label(vm.tr("trans_done"), systemImage: "checkmark.seal.fill")
                    .font(.headline)
                    .foregroundStyle(p.success)
                    .padding(.top, 8)
            } else if index < cards.count {
                let cur = cards[index]
                QuestionCard {
                    Text(cur.0).kicker().foregroundStyle(p.ink3)
                    Text(cur.1).display(onMac(34, 28), relativeTo: .title2)
                        .padding(.bottom, 4)
                    WordField(value: $value, placeholder: vm.tr("your_answer"), palette: p)
                        .submitLabel(.done)
                        .onSubmit { check(cur) }
                    if hasUmlaut(cur.2) { Hint(text: vm.tr("hint_umlauts")) }
                }
                FeedbackLine(text: fb.0, kind: fb.1, palette: p)
            }
        }
        .glassBottomBar {
            if index < cards.count {
                BottomAction(label: vm.tr("check"), action: { check(cards[index]) }) { EmptyView() }
            }
        }
    }

    private func check(_ cur: (String, String, String)) {
        let norm = normalizeAnswer(value)
        if answerAccepts(norm, cur.2) {
            done += 1
            cards.remove(at: index)
            if cards.isEmpty {
                fb = (vm.tr("trans_done"), "ok")
                onDone()
            } else {
                if index >= cards.count { index = 0 }
                value = ""
                fb = ("", "")
            }
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
}
