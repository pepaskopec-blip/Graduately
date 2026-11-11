import SwiftUI

enum Motion {
    static let press = Animation.easeOut(duration: 0.16)
    static let settle = Animation.easeOut(duration: 0.28)
    static let feedback = Animation.spring(response: 0.32, dampingFraction: 0.82)
}

/// Text and controls run one step larger on the Mac than on the iPhone.
func onMac<T>(_ mac: T, _ phone: T) -> T {
    #if os(macOS)
    mac
    #else
    phone
    #endif
}

/// Slight press-in for custom cards and map nodes. System buttons keep their own style.
struct PressScaleStyle: ButtonStyle {
    @Environment(\.accessibilityReduceMotion) private var reduceMotion

    func makeBody(configuration: Configuration) -> some View {
        configuration.label
            .scaleEffect(reduceMotion || !configuration.isPressed ? 1 : 0.98)
            .animation(reduceMotion ? nil : Motion.press, value: configuration.isPressed)
    }
}

/// Named so it does not collide with the app's `Content` type, which would
/// otherwise hide `ViewModifier.Content` and fail the build.
private struct SoftAppear<Inner: View>: View {
    @Environment(\.accessibilityReduceMotion) private var reduceMotion
    @State private var shown = false
    var inner: Inner

    var body: some View {
        inner
            .opacity(reduceMotion || shown ? 1 : 0)
            .offset(y: reduceMotion || shown ? 0 : 8)
            .onAppear {
                guard !reduceMotion else {
                    shown = true
                    return
                }
                withAnimation(Motion.settle) { shown = true }
            }
    }
}

private struct Surface<Inner: View>: View {
    @Environment(\.palette) private var palette
    let radius: CGFloat
    var inner: Inner

    var body: some View {
        inner
            .background(palette.surface, in: RoundedRectangle(cornerRadius: radius, style: .continuous))
            .overlay {
                RoundedRectangle(cornerRadius: radius, style: .continuous)
                    .strokeBorder(palette.line, lineWidth: 1)
            }
    }
}

private struct Paper<Inner: View>: View {
    @Environment(\.palette) private var palette
    var inner: Inner

    var body: some View {
        inner
            .scrollContentBackground(.hidden)
            .background(palette.paper.ignoresSafeArea())
    }
}

extension View {
    /// One short fade-in. Use on a landing surface, not on every pushed screen.
    func softAppear() -> some View {
        SoftAppear(inner: self)
    }

    /// White card on the paper, outlined by a hairline instead of a shadow.
    func softSurface(cornerRadius: CGFloat) -> some View {
        Surface(radius: cornerRadius, inner: self)
    }

    /// The warm page every screen sits on; lists and forms drop their own grey.
    func paper() -> some View {
        Paper(inner: self)
    }

    /// List rows as white cards on the paper.
    func stepRows() -> some View {
        StepRows(inner: self)
    }

    /// The Mac sets a page's title in its content (see `PageHeader`), so the
    /// toolbar drops it; the iPhone keeps title and subtitle in the bar.
    @ViewBuilder
    func pageTitle(_ title: String, subtitle: String? = nil) -> some View {
        #if os(macOS)
        navigationTitle(title)
            .toolbar(removing: .title)
        #else
        navigationTitle(title)
            .navSubtitle(subtitle)
        #endif
    }

    /// Small tracked capitals above a title.
    func kicker() -> some View {
        font(onMac(Font.subheadline, Font.caption).weight(.semibold))
            .tracking(1.2)
            .textCase(.uppercase)
    }
}

enum Page {
    static let inset: CGFloat = onMac(32, 16)
}

/// Heading of a block inside a page, in the display serif.
struct SectionTitle: View {
    let text: String
    @Environment(\.palette) private var palette

    var body: some View {
        Text(text)
            .display(onMac(28, 22), relativeTo: .title2)
            .foregroundStyle(palette.ink)
            .accessibilityAddTraits(.isHeader)
    }
}

/// Mac pages carry their title in the content, set large in the display serif;
/// the toolbar keeps only its controls. The iPhone shows titles in the navigation bar.
struct PageHeader: View {
    let title: String
    var subtitle: String? = nil
    var progress: (done: Int, total: Int)? = nil
    @Environment(\.palette) private var palette
    @Environment(\.accent) private var accent
    @Environment(\.pageKicker) private var kicker

    var body: some View {
        HStack(alignment: .bottom, spacing: 24) {
            VStack(alignment: .leading, spacing: 6) {
                if let kicker {
                    Text(kicker)
                        .kicker()
                        .foregroundStyle(accent.text)
                }
                Text(title)
                    .display(40, relativeTo: .largeTitle)
                    .foregroundStyle(palette.ink)
                    .fixedSize(horizontal: false, vertical: true)
                    .accessibilityAddTraits(.isHeader)
                if let subtitle, !subtitle.isEmpty {
                    Text(subtitle)
                        .font(.title3)
                        .foregroundStyle(palette.ink2)
                        .fixedSize(horizontal: false, vertical: true)
                }
            }
            Spacer(minLength: 0)
            if let progress, progress.total > 0 {
                VStack(alignment: .trailing, spacing: 8) {
                    Text("\(progress.done) / \(progress.total)")
                        .display(28, relativeTo: .title)
                        .monospacedDigit()
                        .foregroundStyle(palette.ink)
                    ProgressBar(value: Double(progress.done) / Double(progress.total), height: 6)
                        .frame(width: 160)
                }
            }
        }
        .frame(maxWidth: .infinity, alignment: .leading)
    }
}

/// One step of a lesson path on the iPhone: done, next up, still ahead, or locked.
struct StepRow: View {
    let index: Int?
    let title: String
    var done = false
    var current = false
    var locked = false
    var symbol: String? = nil
    @Environment(\.palette) private var palette

    var body: some View {
        HStack(spacing: 14) {
            Badge(
                text: index.map { "\($0)" },
                symbol: locked ? "lock.fill" : done ? "checkmark" : symbol,
                size: 34,
                filled: done || current
            )
            Text(title)
                .font(.body.weight(current ? .semibold : .regular))
                .foregroundStyle(locked ? palette.ink3 : palette.ink)
        }
        .padding(.vertical, 2)
    }
}

private struct StepRows<Inner: View>: View {
    @Environment(\.palette) private var palette
    var inner: Inner

    var body: some View {
        inner
            .listRowBackground(palette.surface)
            .listRowSeparatorTint(palette.line)
    }
}

#if os(macOS)
enum MacChrome {
    static let pageInset: CGFloat = Page.inset
    static let cardPadding: CGFloat = 22
    static let listRowMinHeight: CGFloat = 52
    static let homeTileMin: CGFloat = 250
    static let pathNode: CGFloat = 84
    /// Pages stop growing here so cards and text keep their shape on wide displays.
    static let contentWidth: CGFloat = 1240
    static let readableWidth: CGFloat = 820
    static let actionWidth: CGFloat = 520
}
#endif

/// Flat capsule bar. Unlike ProgressView it shows nothing at zero.
struct ProgressBar: View {
    let value: Double
    var height: CGFloat = 8
    var fill: Color? = nil
    var track: Color? = nil
    @Environment(\.palette) private var palette
    @Environment(\.accent) private var accent

    var body: some View {
        GeometryReader { geo in
            ZStack(alignment: .leading) {
                Capsule().fill(track ?? palette.sunken)
                if value > 0 {
                    Capsule()
                        .fill(fill ?? accent.solid)
                        .frame(width: max(height, geo.size.width * min(value, 1)))
                }
            }
        }
        .frame(height: height)
        .animation(Motion.settle, value: value)
    }
}

struct ProgressRing<Label: View>: View {
    let value: Double
    var lineWidth: CGFloat = 12
    @ViewBuilder var label: () -> Label
    @Environment(\.palette) private var palette
    @Environment(\.accent) private var accent

    var body: some View {
        ZStack {
            Circle().stroke(palette.sunken, lineWidth: lineWidth)
            if value > 0 {
                Circle()
                    .trim(from: 0, to: min(value, 1))
                    .stroke(accent.solid, style: StrokeStyle(lineWidth: lineWidth, lineCap: .round))
                    .rotationEffect(.degrees(-90))
            }
            label()
        }
        .animation(Motion.settle, value: value)
    }
}

struct FlowLayout: Layout {
    var spacing: CGFloat = 6

    func sizeThatFits(proposal: ProposedViewSize, subviews: Subviews, cache: inout ()) -> CGSize {
        arrange(width: proposal.width ?? .infinity, subviews: subviews).size
    }

    func placeSubviews(in bounds: CGRect, proposal: ProposedViewSize, subviews: Subviews, cache: inout ()) {
        let result = arrange(width: bounds.width, subviews: subviews)
        for (i, frame) in result.frames.enumerated() {
            subviews[i].place(
                at: CGPoint(x: bounds.minX + frame.minX, y: bounds.minY + frame.minY),
                proposal: ProposedViewSize(frame.size)
            )
        }
    }

    /// Rows wrap at `width`; a view wider than a whole row is wrapped inside
    /// itself, and views of different heights are centred on their row.
    private func arrange(width: CGFloat, subviews: Subviews) -> (size: CGSize, frames: [CGRect]) {
        var frames: [CGRect] = []
        var x: CGFloat = 0
        var y: CGFloat = 0
        var rowH: CGFloat = 0
        var rowStart = 0
        var maxW: CGFloat = 0
        func centreRow() {
            for i in rowStart..<frames.count {
                frames[i].origin.y += (rowH - frames[i].height) / 2
            }
        }
        for view in subviews {
            var size = view.sizeThatFits(.unspecified)
            if size.width > width {
                size = view.sizeThatFits(ProposedViewSize(width: width, height: nil))
            }
            if x > 0 && x + size.width > width {
                centreRow()
                x = 0
                y += rowH + spacing
                rowH = 0
                rowStart = frames.count
            }
            frames.append(CGRect(origin: CGPoint(x: x, y: y), size: size))
            x += size.width + spacing
            rowH = max(rowH, size.height)
            maxW = max(maxW, x - spacing)
        }
        centreRow()
        return (CGSize(width: max(maxW, 0), height: y + rowH), frames)
    }
}

/// The one filled button of a screen, in the colour of the subject it belongs to.
/// `quiet` is its outlined companion of the same height, such as "back" beside "next".
struct SolidButtonStyle: ButtonStyle {
    var wide = true
    var quiet = false
    @Environment(\.palette) private var palette
    @Environment(\.accent) private var accent
    @Environment(\.isEnabled) private var enabled
    @Environment(\.accessibilityReduceMotion) private var reduceMotion

    func makeBody(configuration: Configuration) -> some View {
        let shape = RoundedRectangle(cornerRadius: 14, style: .continuous)
        configuration.label
            .font(onMac(Font.title3, Font.body).weight(.semibold))
            .foregroundStyle(quiet ? palette.ink : accent.onSolid)
            .frame(maxWidth: wide ? .infinity : nil)
            .padding(.horizontal, 20)
            .padding(.vertical, onMac(12, 15))
            .background(quiet ? palette.surface : accent.solid, in: shape)
            .overlay {
                if quiet { shape.strokeBorder(palette.line, lineWidth: 1) }
            }
            .opacity(enabled ? (configuration.isPressed ? 0.86 : 1) : 0.45)
            .scaleEffect(reduceMotion || !configuration.isPressed ? 1 : 0.985)
            .animation(reduceMotion ? nil : Motion.press, value: configuration.isPressed)
            .contentShape(shape)
    }
}

/// Outlined button for everything that is not the primary action: word chips,
/// segment switches, "show the answer".
struct ChipStyle: ButtonStyle {
    var selected = false
    var compact = false
    var tone: Color? = nil
    @Environment(\.palette) private var palette
    @Environment(\.accent) private var accent
    @Environment(\.isEnabled) private var enabled
    @Environment(\.accessibilityReduceMotion) private var reduceMotion

    func makeBody(configuration: Configuration) -> some View {
        let edge = tone ?? (selected ? accent.solid : palette.line)
        configuration.label
            .font(onMac(Font.title3, compact ? Font.subheadline : Font.body).weight(.medium))
            .foregroundStyle(tone ?? (selected ? accent.text : palette.ink))
            .padding(.horizontal, compact ? 12 : 16)
            .padding(.vertical, compact ? 7 : 10)
            .background(selected ? accent.soft : palette.surface, in: RoundedRectangle(cornerRadius: 11, style: .continuous))
            .overlay {
                RoundedRectangle(cornerRadius: 11, style: .continuous)
                    .strokeBorder(edge, lineWidth: selected || tone != nil ? 1.5 : 1)
            }
            .opacity(enabled ? (configuration.isPressed ? 0.7 : 1) : 0.5)
            .scaleEffect(reduceMotion || !configuration.isPressed ? 1 : 0.97)
            .animation(reduceMotion ? nil : Motion.press, value: configuration.isPressed)
            .contentShape(RoundedRectangle(cornerRadius: 11, style: .continuous))
    }
}

struct PrimaryButton: View {
    let label: String
    var action: () -> Void

    var body: some View {
        Button(label, action: action)
            .buttonStyle(SolidButtonStyle())
    }
}

struct PillButton: View {
    let label: String
    var action: () -> Void

    var body: some View {
        Button(label, action: action)
            .buttonStyle(ChipStyle())
    }
}

struct GlassCard<Content: View>: View {
    @ViewBuilder var content: () -> Content

    var body: some View {
        content()
            .padding(onMac(22, 16))
            .frame(maxWidth: .infinity, alignment: .leading)
            .softSurface(cornerRadius: 20)
    }
}

struct FeedbackLine: View {
    let text: String
    let kind: String
    let palette: Palette
    @Environment(\.accessibilityReduceMotion) private var reduceMotion

    var body: some View {
        Group {
            if !text.isEmpty {
                Label {
                    Text(text)
                } icon: {
                    Image(systemName: kind == "ok" ? "checkmark.circle.fill" : kind == "warn" ? "exclamationmark.triangle.fill" : "xmark.circle.fill")
                }
                .font(onMac(Font.body, Font.subheadline).weight(.semibold))
                .foregroundStyle(kind == "ok" ? palette.success : kind == "warn" ? palette.warning : palette.error)
                .padding(.horizontal, 14)
                .padding(.vertical, 10)
                .frame(maxWidth: .infinity, alignment: .leading)
                .background(
                    kind == "ok" ? palette.successSoft : kind == "warn" ? palette.warningSoft : palette.errorSoft,
                    in: RoundedRectangle(cornerRadius: 12, style: .continuous)
                )
                .transition(.opacity.combined(with: .scale(scale: 0.96, anchor: .leading)))
            }
        }
        .animation(reduceMotion ? nil : Motion.feedback, value: text)
        .animation(reduceMotion ? nil : Motion.feedback, value: kind)
    }
}

/// Translation or explanation shown under an answer: a quiet note with a rule on its left.
struct Meaning: View {
    let text: String
    let palette: Palette
    let visible: Bool

    @Environment(\.accessibilityReduceMotion) private var reduceMotion

    var body: some View {
        Group {
            if visible && !text.isEmpty {
                Text(text)
                    .font(onMac(Font.body, Font.footnote))
                    .foregroundStyle(palette.ink2)
                    .fixedSize(horizontal: false, vertical: true)
                    .padding(.leading, 12)
                    .overlay(alignment: .leading) {
                        Capsule().fill(palette.line).frame(width: 3)
                    }
                    .padding(.bottom, 6)
                    .transition(.opacity)
            }
        }
        .animation(reduceMotion ? nil : Motion.settle, value: visible)
    }
}

struct WordField: View {
    @Binding var value: String
    var placeholder: String
    var mark: Bool?
    var palette: Palette?
    /// A short blank inside a sentence or a table cell.
    var compact = false
    @Environment(\.palette) private var theme
    @Environment(\.accent) private var accent
    @Environment(\.accessibilityReduceMotion) private var reduceMotion
    @FocusState private var focused: Bool

    var body: some View {
        HStack(spacing: 8) {
            TextField(placeholder, text: $value)
                .textFieldStyle(.plain)
                .textInputAutocapitalization(.never)
                .autocorrectionDisabled()
                .focused($focused)
                .multilineTextAlignment(compact ? .center : .leading)
                #if os(macOS)
                .font(.title3)
                #endif
            if let mark, !compact {
                Image(systemName: mark ? "checkmark.circle.fill" : "xmark.circle.fill")
                    .foregroundStyle(mark ? theme.success : theme.error)
            }
        }
        .padding(.horizontal, compact ? 8 : 14)
        .padding(.vertical, compact ? 7 : onMac(11, 12))
        .background(fill, in: RoundedRectangle(cornerRadius: compact ? 9 : 12, style: .continuous))
        .overlay(
            RoundedRectangle(cornerRadius: compact ? 9 : 12, style: .continuous)
                .strokeBorder(border, lineWidth: mark == nil && !focused ? 1 : 1.5)
        )
        .animation(reduceMotion ? nil : Motion.settle, value: mark)
        .animation(reduceMotion ? nil : Motion.press, value: focused)
    }

    private var fill: Color {
        if mark == true { return theme.successSoft }
        if mark == false { return theme.errorSoft }
        // Fields and options are set into the white task card, so they take the page's tone.
        return theme.paper
    }

    private var border: Color {
        if mark == true { return theme.success }
        if mark == false { return theme.error }
        return focused ? accent.solid : theme.line
    }
}

struct SegChip: View {
    let label: String
    let selected: Bool
    var action: () -> Void

    var body: some View {
        Button(label, action: action)
            .buttonStyle(ChipStyle(selected: selected))
    }
}

struct Hint: View {
    let text: String
    @Environment(\.palette) private var palette

    var body: some View {
        Text(text)
            .font(onMac(Font.body, Font.footnote))
            .foregroundStyle(palette.ink2)
            .padding(.bottom, 6)
    }
}

/// An exercise's page: its title and instruction, then the tasks in a readable column.
struct ExercisePage<Inner: View>: View {
    let title: String
    var sub: String? = nil
    /// False when something is pinned above the tasks; the title then stays in the bar.
    var header = true
    @ViewBuilder var content: () -> Inner
    @Environment(\.palette) private var palette
    @Environment(\.accent) private var accent
    @Environment(\.pageKicker) private var kicker

    var body: some View {
        titled(
            ScrollView {
                VStack(alignment: .leading, spacing: onMac(14, 12)) {
                    if header { heading }
                    content()
                }
                .exerciseContent()
            }
            .scrollDismissesKeyboard(.interactively)
        )
    }

    /// The Mac sets the title in the page. The iPhone has it in the navigation
    /// bar, so only the instruction is repeated at reading size.
    @ViewBuilder
    private var heading: some View {
        #if os(macOS)
        VStack(alignment: .leading, spacing: 6) {
            if let kicker {
                Text(kicker)
                    .kicker()
                    .foregroundStyle(accent.text)
            }
            Text(title)
                .display(34, relativeTo: .largeTitle)
                .foregroundStyle(palette.ink)
                .fixedSize(horizontal: false, vertical: true)
                .accessibilityAddTraits(.isHeader)
            if let sub, !sub.isEmpty {
                Text(sub)
                    .font(.title3)
                    .foregroundStyle(palette.ink2)
                    .fixedSize(horizontal: false, vertical: true)
            }
        }
        .padding(.bottom, 10)
        #else
        if let sub, !sub.isEmpty {
            Text(sub)
                .font(.subheadline)
                .foregroundStyle(palette.ink2)
                .fixedSize(horizontal: false, vertical: true)
                .padding(.bottom, 2)
        }
        #endif
    }

    @ViewBuilder
    private func titled<Page: View>(_ page: Page) -> some View {
        #if os(macOS)
        if header {
            page.pageTitle(title)
        } else {
            page.detailScreen(title, subtitle: sub)
        }
        #else
        page.detailScreen(title, subtitle: header ? nil : sub)
        #endif
    }
}

/// One task of an exercise on its own card: its number, the prompt, then the answer controls.
struct QuestionCard<Inner: View>: View {
    var number: Int? = nil
    var prompt: String? = nil
    /// Outlines the card green or red once the answer is checked.
    var mark: Bool? = nil
    @ViewBuilder var content: () -> Inner
    @Environment(\.palette) private var palette
    @Environment(\.accent) private var accent

    var body: some View {
        let shape = RoundedRectangle(cornerRadius: 18, style: .continuous)
        VStack(alignment: .leading, spacing: 12) {
            if number != nil || !(prompt ?? "").isEmpty {
                HStack(alignment: .firstTextBaseline, spacing: 12) {
                    if let number {
                        Text("\(number)")
                            .display(onMac(21, 19), relativeTo: .headline)
                            .foregroundStyle(accent.text)
                            .frame(minWidth: 20, alignment: .leading)
                    }
                    if let prompt, !prompt.isEmpty {
                        Text(prompt)
                            .font(onMac(Font.title3, Font.body).weight(.semibold))
                            .foregroundStyle(palette.ink)
                            .fixedSize(horizontal: false, vertical: true)
                    }
                }
            }
            content()
        }
        .font(onMac(Font.title3, Font.body))
        .foregroundStyle(palette.ink)
        .padding(onMac(20, 16))
        .frame(maxWidth: .infinity, alignment: .leading)
        .background(palette.surface, in: shape)
        .overlay {
            shape.strokeBorder(
                mark == true ? palette.success : mark == false ? palette.error : palette.line,
                lineWidth: mark == nil ? 1 : 1.5
            )
        }
        .animation(Motion.settle, value: mark)
    }
}

struct WordChip: View {
    let text: String
    var action: () -> Void

    var body: some View {
        Button(text, action: action)
            .buttonStyle(ChipStyle(compact: onMac(false, true)))
    }
}

/// Drop zone for word ordering: visible even when empty.
struct WordSlot<Content: View>: View {
    let empty: Bool
    @ViewBuilder var content: () -> Content
    @Environment(\.palette) private var palette

    var body: some View {
        content()
            .frame(maxWidth: .infinity, minHeight: onMac(56, 44), alignment: .leading)
            .padding(onMac(12, 8))
            .background(palette.sunken, in: RoundedRectangle(cornerRadius: 14, style: .continuous))
            .overlay(
                RoundedRectangle(cornerRadius: 14, style: .continuous)
                    .strokeBorder(style: StrokeStyle(lineWidth: empty ? 1.5 : 0, dash: [5, 4]))
                    .foregroundStyle(palette.ink3.opacity(0.6))
            )
    }
}

/// One answer of a multiple choice: a card that takes the subject's colour when
/// picked, then turns green or red once the answer is checked.
struct OptionChip: View {
    let text: String
    let selected: Bool
    let mark: Bool?
    var action: () -> Void
    @Environment(\.palette) private var palette
    @Environment(\.accent) private var accent
    @Environment(\.accessibilityReduceMotion) private var reduceMotion

    var body: some View {
        Button(action: action) {
            HStack(spacing: 12) {
                Image(systemName: symbol)
                    .font(.title3)
                    .foregroundStyle(edge)
                Text(text)
                    .font(onMac(Font.title3, Font.body))
                    .foregroundStyle(palette.ink)
                    .multilineTextAlignment(.leading)
                    .frame(maxWidth: .infinity, alignment: .leading)
            }
            .padding(.horizontal, 14)
            .padding(.vertical, onMac(12, 13))
            .background(fill, in: RoundedRectangle(cornerRadius: 14, style: .continuous))
            .overlay {
                RoundedRectangle(cornerRadius: 14, style: .continuous)
                    .strokeBorder(selected || mark != nil ? edge : palette.line, lineWidth: selected || mark != nil ? 1.5 : 1)
            }
            .contentShape(RoundedRectangle(cornerRadius: 14, style: .continuous))
        }
        .buttonStyle(PressScaleStyle())
        .animation(reduceMotion ? nil : Motion.settle, value: mark)
        .animation(reduceMotion ? nil : Motion.press, value: selected)
    }

    private var symbol: String {
        if mark == true { return "checkmark.circle.fill" }
        if mark == false { return "xmark.circle.fill" }
        return selected ? "largecircle.fill.circle" : "circle"
    }

    private var edge: Color {
        if mark == true { return palette.success }
        if mark == false { return palette.error }
        return selected ? accent.solid : palette.ink3
    }

    private var fill: Color {
        if mark == true { return palette.successSoft }
        if mark == false { return palette.errorSoft }
        return selected ? accent.soft : palette.paper
    }
}

extension View {
    @ViewBuilder
    func navSubtitle(_ text: String?) -> some View {
        if let text, !text.isEmpty {
            navigationSubtitle(text)
        } else {
            self
        }
    }

    /// Detail screens with their own bottom action hide the tab bar so the
    /// primary button is never covered by it.
    @ViewBuilder
    func detailScreen(_ title: String, subtitle: String? = nil) -> some View {
        #if os(iOS)
        navigationTitle(title)
            .navigationBarTitleDisplayMode(.inline)
            .navSubtitle(subtitle)
            .toolbar(.hidden, for: .tabBar)
        #else
        navigationTitle(title)
            .navSubtitle(subtitle)
        #endif
    }

    @ViewBuilder
    func sheetDetents() -> some View {
        #if os(iOS)
        presentationDetents([.medium, .large])
            .presentationDragIndicator(.visible)
            // Liquid Glass sheets refract the exercise chrome underneath into a
            // bright tinted band across the word list; keep the sheet opaque.
            .presentationBackground(.background)
        #else
        self
        #endif
    }

    /// Leading-aligned content for exercise screens; on the Mac a centred readable column.
    func exerciseContent() -> some View {
        #if os(macOS)
        frame(maxWidth: MacChrome.readableWidth, alignment: .leading)
            .padding(.horizontal, MacChrome.pageInset)
            .padding(.vertical, 28)
            .frame(maxWidth: .infinity)
        #else
        frame(maxWidth: .infinity, alignment: .leading)
            .padding()
        #endif
    }

    /// Caps a Mac page at [width] and centres it; no-op on iOS.
    @ViewBuilder
    func macPage(_ width: CGFloat? = nil) -> some View {
        #if os(macOS)
        frame(maxWidth: width ?? MacChrome.contentWidth)
            .frame(maxWidth: .infinity)
        #else
        self
        #endif
    }

    /// macOS content lists use the inset style so they sit under the floating
    /// glass sidebar instead of looking like a second source list.
    @ViewBuilder
    func appListStyle() -> some View {
        #if os(macOS)
        listStyle(.inset)
            .environment(\.defaultMinListRowHeight, MacChrome.listRowMinHeight)
            .scrollContentBackground(.hidden)
            .padding(.horizontal, 8)
        #else
        scrollContentBackground(.hidden)
        #endif
    }

    @ViewBuilder
    func appFormStyle() -> some View {
        #if os(macOS)
        formStyle(.grouped)
            .scenePadding()
        #else
        scrollContentBackground(.hidden)
        #endif
    }

    /// Pin a primary action above scrolling content using the system scroll
    /// edge effect instead of an opaque bar.
    func glassBottomBar<Bar: View>(@ViewBuilder _ bar: () -> Bar) -> some View {
        safeAreaBar(edge: .bottom, spacing: 8, content: bar)
    }
}

/// Bottom action area used by exercises and lesson slides.
struct BottomAction<Extra: View>: View {
    let label: String
    var action: () -> Void
    @ViewBuilder var extra: () -> Extra

    var body: some View {
        VStack(spacing: 10) {
            extra()
            PrimaryButton(label: label, action: action)
        }
        .padding(.horizontal, {
            #if os(macOS)
            MacChrome.pageInset
            #else
            16
            #endif
        }())
        #if os(macOS)
        .padding(.top, 12)
        .padding(.bottom, 14)
        .frame(maxWidth: MacChrome.actionWidth)
        .frame(maxWidth: .infinity)
        #else
        .padding(.top, 8)
        .padding(.bottom, 4)
        #endif
    }
}

/// Slim bar with "done / total" beside it.
struct ProgressLine: View {
    let done: Int
    let total: Int
    @Environment(\.palette) private var palette

    var body: some View {
        HStack(spacing: 10) {
            ProgressBar(value: total == 0 ? 0 : Double(done) / Double(total), height: 6)
            Text("\(done) / \(total)")
                .font(onMac(Font.subheadline, Font.caption).monospacedDigit())
                .foregroundStyle(palette.ink2)
        }
    }
}

/// Number or glyph on the subject's colour: the year of a course, a lesson's
/// place on the path, a section of a subject.
struct Badge: View {
    var text: String? = nil
    var symbol: String? = nil
    var size: CGFloat = 44
    /// Filled for what is done or current, outlined for what is still ahead.
    var filled = true
    @Environment(\.palette) private var palette
    @Environment(\.accent) private var accent

    var body: some View {
        let shape = RoundedRectangle(cornerRadius: size * 0.3, style: .continuous)
        Group {
            if let symbol {
                Image(systemName: symbol)
                    .font(.system(size: size * 0.42, weight: .semibold))
            } else if let text {
                Text(text)
                    .display(size * 0.5, relativeTo: .title3)
                    .minimumScaleFactor(0.6)
                    .lineLimit(1)
            }
        }
        .foregroundStyle(filled ? accent.onSolid : accent.text)
        .frame(width: size, height: size)
        .background(filled ? accent.solid : accent.soft, in: shape)
    }
}

/// One tappable card of a subject's index: a year, a branch such as grammar,
/// or a book. Grid cell on the Mac, full-width row on the iPhone.
struct IndexCard: View {
    let title: String
    var subtitle: String? = nil
    var badge: String? = nil
    var symbol: String? = nil
    var progress: (done: Int, total: Int)? = nil
    @Environment(\.palette) private var palette

    var body: some View {
        #if os(macOS)
        VStack(alignment: .leading, spacing: 14) {
            HStack(alignment: .top) {
                Badge(text: badge, symbol: symbol, size: 48)
                Spacer(minLength: 0)
                Image(systemName: "arrow.up.right")
                    .font(.body.weight(.semibold))
                    .foregroundStyle(palette.ink3)
            }
            texts
            Spacer(minLength: 0)
            if let progress, progress.total > 0 {
                ProgressLine(done: progress.done, total: progress.total)
            }
        }
        .padding(20)
        .frame(maxWidth: .infinity, maxHeight: .infinity, alignment: .topLeading)
        .softSurface(cornerRadius: 22)
        .contentShape(RoundedRectangle(cornerRadius: 22, style: .continuous))
        #else
        HStack(alignment: .top, spacing: 14) {
            Badge(text: badge, symbol: symbol, size: 44)
            VStack(alignment: .leading, spacing: 10) {
                texts
                if let progress, progress.total > 0 {
                    ProgressLine(done: progress.done, total: progress.total)
                }
            }
            .frame(maxWidth: .infinity, alignment: .leading)
            Image(systemName: "chevron.right")
                .font(.footnote.weight(.semibold))
                .foregroundStyle(palette.ink3)
                .padding(.top, 14)
        }
        .padding(16)
        .softSurface(cornerRadius: 20)
        .contentShape(RoundedRectangle(cornerRadius: 20, style: .continuous))
        #endif
    }

    private var texts: some View {
        VStack(alignment: .leading, spacing: 4) {
            Text(title)
                .display(onMac(22, 19), relativeTo: .title3)
                .foregroundStyle(palette.ink)
                .lineLimit(2)
            if let subtitle, !subtitle.isEmpty {
                Text(subtitle)
                    .font(onMac(Font.body, Font.subheadline))
                    .foregroundStyle(palette.ink2)
                    .lineLimit(3)
                    .fixedSize(horizontal: false, vertical: true)
            }
        }
        .multilineTextAlignment(.leading)
    }
}

/// A screen of `IndexCard`s: as many columns as fit on the Mac, under the
/// page's title; one column on the iPhone.
struct IndexGrid<Cell: View>: View {
    let count: Int
    var title = ""
    var subtitle = ""
    @ViewBuilder var cell: (Int) -> Cell

    var body: some View {
        ScrollView {
            #if os(macOS)
            VStack(alignment: .leading, spacing: 28) {
                PageHeader(title: title, subtitle: subtitle)
                LazyVGrid(
                    columns: [GridItem(.adaptive(minimum: 260, maximum: 380), spacing: 16, alignment: .top)],
                    alignment: .leading,
                    spacing: 16
                ) {
                    ForEach(0..<count, id: \.self) { i in
                        cell(i).frame(height: 196)
                    }
                }
            }
            .padding(Page.inset)
            .macPage()
            #else
            LazyVStack(spacing: 12) {
                ForEach(0..<count, id: \.self) { i in cell(i) }
            }
            .padding(Page.inset)
            #endif
        }
        .softAppear()
    }
}

/// A subject as a book cover: its colour, its glyph, and its timetable
/// abbreviation set large behind the title.
struct SubjectCover: View {
    let title: String
    let symbol: String
    let mark: String
    let accent: Accent
    var done = 0
    var total = 0

    var body: some View {
        VStack(alignment: .leading, spacing: 0) {
            Image(systemName: symbol)
                .font(.system(size: onMac(19, 17), weight: .semibold))
                .frame(width: onMac(42, 38), height: onMac(42, 38))
                .background(accent.onSolid.opacity(0.16), in: Circle())
            Spacer(minLength: 10)
            Text(title)
                .display(onMac(21, 17), relativeTo: .headline)
                .lineLimit(2)
                .multilineTextAlignment(.leading)
                .fixedSize(horizontal: false, vertical: true)
            if total > 0 {
                HStack(spacing: 10) {
                    ProgressBar(
                        value: Double(done) / Double(total),
                        height: 5,
                        fill: accent.onSolid,
                        track: accent.onSolid.opacity(0.24)
                    )
                    Text("\(done) / \(total)")
                        .font(.caption.weight(.medium).monospacedDigit())
                        .opacity(0.85)
                }
                .padding(.top, 12)
            }
        }
        .foregroundStyle(accent.onSolid)
        .padding(onMac(20, 16))
        .frame(maxWidth: .infinity, alignment: .topLeading)
        .frame(height: onMac(184, 168))
        .background(alignment: .topTrailing) {
            // Three letters are set smaller so the mark stays clear of the glyph.
            Text(mark)
                .displayItalic(mark.count > 2 ? onMac(70, 58) : onMac(92, 76))
                .foregroundStyle(accent.onSolid.opacity(0.16))
                .fixedSize()
                .offset(x: onMac(6, 4), y: mark.count > 2 ? onMac(-10, -8) : onMac(-26, -20))
                .accessibilityHidden(true)
        }
        .background(accent.solid)
        .clipShape(RoundedRectangle(cornerRadius: 24, style: .continuous))
        .contentShape(RoundedRectangle(cornerRadius: 24, style: .continuous))
    }
}

/// All subjects on one line, each as wide as its share of the exercises and
/// filled as far as it is done. The app's progress at a glance.
struct SpectrumBar: View {
    let parts: [(accent: Accent, done: Int, total: Int)]
    var height: CGFloat = 12
    var track: Color

    var body: some View {
        let total = max(parts.reduce(0) { $0 + $1.total }, 1)
        GeometryReader { geo in
            let gap: CGFloat = 3
            let usable = max(geo.size.width - gap * CGFloat(max(parts.count - 1, 0)), 0)
            HStack(spacing: gap) {
                ForEach(Array(parts.enumerated()), id: \.offset) { _, part in
                    let width = usable * CGFloat(part.total) / CGFloat(total)
                    ZStack(alignment: .leading) {
                        Capsule().fill(track)
                        if part.done > 0 {
                            Capsule()
                                .fill(part.accent.solid)
                                .frame(width: max(height, width * CGFloat(part.done) / CGFloat(max(part.total, 1))))
                        }
                    }
                    .frame(width: width)
                }
            }
        }
        .frame(height: height)
    }
}

func subjectSymbol(_ icon: String) -> String {
    subjectStyle(icon)?.symbol ?? "lock.fill"
}

/// Subject glyph on its own colour, for rows and tables.
struct SubjectIcon: View {
    let icon: String
    var open = true
    var size: CGFloat = 26
    var mode: ColorMode = .light
    @Environment(\.palette) private var palette

    var body: some View {
        let accent = subjectStyle(icon)?.accent(mode)
        Image(systemName: open ? subjectSymbol(icon) : "lock.fill")
            .font(.system(size: size * 0.46, weight: .semibold))
            .foregroundStyle(open ? (accent?.onSolid ?? palette.ink) : palette.ink3)
            .frame(width: size, height: size)
            .background(
                open ? (accent?.solid ?? palette.sunken) : palette.sunken,
                in: RoundedRectangle(cornerRadius: size * 0.3, style: .continuous)
            )
    }
}
