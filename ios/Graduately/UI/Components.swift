import SwiftUI

enum Motion {
    static let press = Animation.easeOut(duration: 0.16)
    static let settle = Animation.easeOut(duration: 0.28)
    static let feedback = Animation.spring(response: 0.32, dampingFraction: 0.82)
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

extension View {
    /// One short fade-in. Use on a landing surface, not on every pushed screen.
    func softAppear() -> some View {
        SoftAppear(inner: self)
    }
}

#if os(macOS)
enum MacChrome {
    static let pageInset: CGFloat = 28
    static let cardPadding: CGFloat = 22
    static let listRowMinHeight: CGFloat = 52
    static let homeTileMin: CGFloat = 260
    static let homeTileHeight: CGFloat = 172
    static let pathNode: CGFloat = 84
    /// Pages stop growing here so cards and text keep their shape on wide displays.
    static let contentWidth: CGFloat = 1240
    static let readableWidth: CGFloat = 860
    static let actionWidth: CGFloat = 520
}

/// Fixed-height cards in as many columns as fit, centred and capped in width.
struct MacFillGrid<Content: View>: View {
    var count: Int
    var minWidth: CGFloat = 240
    var minHeight: CGFloat = 148
    var spacing: CGFloat = 16
    var inset: CGFloat = 28
    var maxTileWidth: CGFloat = 340
    @ViewBuilder var content: (Int) -> Content

    var body: some View {
        GeometryReader { geo in
            let available = max(geo.size.width - inset * 2, minWidth)
            let fit = max(1, Int((available + spacing) / (minWidth + spacing)))
            let cols = max(1, min(count, fit))
            let width = min(available, CGFloat(cols) * maxTileWidth + CGFloat(cols - 1) * spacing)
            ScrollView {
                LazyVGrid(
                    columns: Array(repeating: GridItem(.flexible(), spacing: spacing, alignment: .top), count: cols),
                    alignment: .leading,
                    spacing: spacing
                ) {
                    ForEach(0..<count, id: \.self) { i in
                        content(i).frame(height: minHeight)
                    }
                }
                .frame(width: width)
                .padding(inset)
                .frame(maxWidth: .infinity)
            }
        }
        .softAppear()
    }
}

struct MacLinkCard: View {
    let title: String
    var subtitle: String? = nil
    var badge: String? = nil
    var systemImage: String? = nil
    var locked = false
    var progress: (done: Int, total: Int)? = nil

    var body: some View {
        let tint = locked ? Color.secondary : Color.accentColor
        VStack(alignment: .leading, spacing: 12) {
            HStack(alignment: .center) {
                Group {
                    if let systemImage {
                        Image(systemName: systemImage)
                            .font(.title3.weight(.semibold))
                    } else if let badge {
                        Text(badge.hasSuffix(".") ? String(badge.dropLast()) : badge)
                            .font(.title3.weight(.bold))
                            .monospacedDigit()
                    }
                }
                .foregroundStyle(tint)
                .frame(width: 40, height: 40)
                .background(tint.opacity(0.14), in: RoundedRectangle(cornerRadius: 11, style: .continuous))
                Spacer(minLength: 0)
                Image(systemName: locked ? "lock.fill" : "chevron.right")
                    .font(.body.weight(.semibold))
                    .foregroundStyle(.tertiary)
            }
            VStack(alignment: .leading, spacing: 4) {
                Text(title)
                    .font(.title3.weight(.semibold))
                    .foregroundStyle(locked ? .secondary : .primary)
                    .lineLimit(2)
                if let subtitle, !subtitle.isEmpty {
                    Text(subtitle)
                        .font(.body)
                        .foregroundStyle(.secondary)
                        .lineLimit(2)
                        .fixedSize(horizontal: false, vertical: true)
                }
            }
            .multilineTextAlignment(.leading)
            Spacer(minLength: 0)
            if let progress, progress.total > 0 {
                HStack(spacing: 10) {
                    ProgressBar(value: Double(progress.done) / Double(progress.total))
                    Text("\(progress.done) / \(progress.total)")
                        .font(.subheadline.monospacedDigit())
                        .foregroundStyle(.secondary)
                }
            }
        }
        .padding(20)
        .frame(maxWidth: .infinity, maxHeight: .infinity, alignment: .topLeading)
        .softSurface(cornerRadius: 20)
        .contentShape(RoundedRectangle(cornerRadius: 20, style: .continuous))
    }
}
#endif

/// Flat capsule bar. Unlike ProgressView it shows nothing at zero.
struct ProgressBar: View {
    let value: Double
    var height: CGFloat = 8

    var body: some View {
        GeometryReader { geo in
            ZStack(alignment: .leading) {
                Capsule().fill(Color.primary.opacity(0.08))
                if value > 0 {
                    Capsule()
                        .fill(Color.accentColor)
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

    var body: some View {
        ZStack {
            Circle().stroke(Color.primary.opacity(0.08), lineWidth: lineWidth)
            if value > 0 {
                Circle()
                    .trim(from: 0, to: min(value, 1))
                    .stroke(Color.accentColor, style: StrokeStyle(lineWidth: lineWidth, lineCap: .round))
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

    private func arrange(width: CGFloat, subviews: Subviews) -> (size: CGSize, frames: [CGRect]) {
        var frames: [CGRect] = []
        var x: CGFloat = 0
        var y: CGFloat = 0
        var rowH: CGFloat = 0
        var maxW: CGFloat = 0
        for view in subviews {
            let size = view.sizeThatFits(.unspecified)
            if x > 0 && x + size.width > width {
                x = 0
                y += rowH + spacing
                rowH = 0
            }
            frames.append(CGRect(origin: CGPoint(x: x, y: y), size: size))
            x += size.width + spacing
            rowH = max(rowH, size.height)
            maxW = max(maxW, x - spacing)
        }
        return (CGSize(width: max(maxW, 0), height: y + rowH), frames)
    }
}

struct PrimaryButton: View {
    let label: String
    var action: () -> Void

    var body: some View {
        Button(action: action) {
            Text(label)
                .font(.body.weight(.semibold))
                .frame(maxWidth: .infinity)
                #if os(macOS)
                .padding(.vertical, 4)
                #endif
        }
        #if os(macOS)
        .buttonStyle(.borderedProminent)
        .controlSize(.extraLarge)
        #else
        .buttonStyle(.glassProminent)
        .controlSize(.large)
        #endif
    }
}

struct PillButton: View {
    let label: String
    var action: () -> Void

    var body: some View {
        Button(label, action: action)
            .buttonStyle(.bordered)
            #if os(macOS)
            .controlSize(.large)
            #endif
    }
}

struct GlassCard<Content: View>: View {
    @ViewBuilder var content: () -> Content

    var body: some View {
        content()
            #if os(macOS)
            .padding(MacChrome.cardPadding)
            #else
            .padding(16)
            #endif
            .frame(maxWidth: .infinity, alignment: .leading)
            .softSurface(cornerRadius: 22)
    }
}

extension View {
    /// Soft card surface without Liquid Glass refraction (which reads as a mirror
    /// on large macOS panels over the wallpaper / sidebar extension).
    func softSurface(cornerRadius: CGFloat) -> some View {
        background {
            RoundedRectangle(cornerRadius: cornerRadius, style: .continuous)
                .fill(.background.secondary)
        }
        .overlay {
            RoundedRectangle(cornerRadius: cornerRadius, style: .continuous)
                .strokeBorder(.separator.opacity(0.35), lineWidth: 0.5)
        }
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
                #if os(macOS)
                .font(.body.weight(.semibold))
                #else
                .font(.subheadline.weight(.semibold))
                #endif
                .foregroundStyle(kind == "ok" ? palette.success : kind == "warn" ? palette.warning : palette.error)
                .transition(.opacity.combined(with: .scale(scale: 0.96, anchor: .leading)))
            }
        }
        .animation(reduceMotion ? nil : Motion.feedback, value: text)
        .animation(reduceMotion ? nil : Motion.feedback, value: kind)
    }
}

struct Meaning: View {
    let text: String
    let palette: Palette
    let visible: Bool

    @Environment(\.accessibilityReduceMotion) private var reduceMotion

    var body: some View {
        Group {
            if visible && !text.isEmpty {
                Text(text)
                    #if os(macOS)
                    .font(.body)
                    #else
                    .font(.footnote)
                    #endif
                    .foregroundStyle(.secondary)
                    .padding(.bottom, 4)
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
    @Environment(\.accessibilityReduceMotion) private var reduceMotion

    var body: some View {
        HStack(spacing: 8) {
            TextField(placeholder, text: $value)
                .textInputAutocapitalization(.never)
                .autocorrectionDisabled()
                #if os(macOS)
                .font(.title3)
                #endif
            if let mark {
                Image(systemName: mark ? "checkmark.circle.fill" : "xmark.circle.fill")
                    .foregroundStyle(mark ? (palette?.success ?? .green) : (palette?.error ?? .red))
            }
        }
        .padding(.horizontal, 12)
        #if os(macOS)
        .padding(.vertical, 14)
        #else
        .padding(.vertical, 10)
        #endif
        .background(Color(.secondarySystemGroupedBackground), in: RoundedRectangle(cornerRadius: 12, style: .continuous))
        .overlay(
            RoundedRectangle(cornerRadius: 12, style: .continuous)
                .stroke(border, lineWidth: mark == nil ? 0.5 : 1.5)
        )
        .animation(reduceMotion ? nil : Motion.settle, value: mark)
    }

    private var border: Color {
        if mark == true { return palette?.success ?? .green }
        if mark == false { return palette?.error ?? .red }
        return Color(.separator)
    }
}

struct SegChip: View {
    let label: String
    let selected: Bool
    var action: () -> Void

    var body: some View {
        Button(label, action: action)
            .buttonStyle(.bordered)
            .tint(selected ? .accentColor : .secondary)
            #if os(macOS)
            .controlSize(.large)
            #endif
    }
}

struct Hint: View {
    let text: String
    var body: some View {
        Text(text)
            #if os(macOS)
            .font(.body)
            #else
            .font(.footnote)
            #endif
            .foregroundStyle(.secondary)
            .padding(.bottom, 6)
    }
}

struct Prompt: View {
    let text: String
    var body: some View {
        Text(text)
            #if os(macOS)
            .font(.title3.weight(.medium))
            #else
            .font(.body.weight(.medium))
            #endif
    }
}

struct WordChip: View {
    let text: String
    var action: () -> Void

    var body: some View {
        Button(text, action: action)
            .buttonStyle(.bordered)
            .tint(.primary)
            #if os(macOS)
            .controlSize(.large)
            #else
            .controlSize(.small)
            #endif
    }
}

/// Drop zone for word ordering: visible even when empty.
struct WordSlot<Content: View>: View {
    let empty: Bool
    @ViewBuilder var content: () -> Content

    var body: some View {
        content()
            #if os(macOS)
            .frame(maxWidth: .infinity, minHeight: 56, alignment: .leading)
            .padding(12)
            #else
            .frame(maxWidth: .infinity, minHeight: 44, alignment: .leading)
            .padding(8)
            #endif
            .background(Color(.tertiarySystemFill), in: RoundedRectangle(cornerRadius: 14, style: .continuous))
            .overlay(
                RoundedRectangle(cornerRadius: 14, style: .continuous)
                    .strokeBorder(style: StrokeStyle(lineWidth: empty ? 1 : 0, dash: [5, 4]))
                    .foregroundStyle(.tertiary)
            )
    }
}

struct OptionChip: View {
    let text: String
    let selected: Bool
    let mark: Bool?
    var action: () -> Void
    @Environment(\.accessibilityReduceMotion) private var reduceMotion

    var body: some View {
        Button(action: action) {
            HStack(spacing: 10) {
                Image(systemName: symbol)
                    .font(.title3)
                    .foregroundStyle(symbolColor)
                Text(text)
                    #if os(macOS)
                    .font(.title3)
                    #endif
                    .foregroundStyle(.primary)
                    .multilineTextAlignment(.leading)
                    .frame(maxWidth: .infinity, alignment: .leading)
            }
            #if os(macOS)
            .padding(.vertical, 8)
            #else
            .padding(.vertical, 4)
            #endif
        }
        .buttonStyle(.bordered)
        .tint(tint)
        #if os(macOS)
        .controlSize(.large)
        #endif
        .padding(.bottom, 4)
        .animation(reduceMotion ? nil : Motion.settle, value: mark)
        .animation(reduceMotion ? nil : Motion.press, value: selected)
    }

    private var symbol: String {
        if mark == true { return "checkmark.circle.fill" }
        if mark == false { return "xmark.circle.fill" }
        return selected ? "largecircle.fill.circle" : "circle"
    }

    private var symbolColor: Color {
        if mark == true { return .green }
        if mark == false { return .red }
        return selected ? .accentColor : .secondary
    }

    private var tint: Color {
        if mark == true { return .green }
        if mark == false { return .red }
        return selected ? .accentColor : .secondary
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
        self
        #endif
    }

    @ViewBuilder
    func appFormStyle() -> some View {
        #if os(macOS)
        formStyle(.grouped)
            .scenePadding()
        #else
        self
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

/// Primary + secondary text sized for desktop lists, with optional exercise progress.
struct ListRowLabel: View {
    let title: String
    var subtitle: String? = nil
    var progress: (done: Int, total: Int)? = nil

    var body: some View {
        VStack(alignment: .leading, spacing: 3) {
            Text(title)
                #if os(macOS)
                .font(.title3)
                #endif
            if let subtitle, !subtitle.isEmpty {
                Text(subtitle)
                    #if os(macOS)
                    .font(.body)
                    #else
                    .font(.caption)
                    #endif
                    .foregroundStyle(.secondary)
            }
            if let progress, progress.total > 0 {
                ProgressLine(done: progress.done, total: progress.total)
                    .padding(.top, 4)
            }
        }
    }
}

/// Slim bar with "done / total" beside it, for list rows.
struct ProgressLine: View {
    let done: Int
    let total: Int

    var body: some View {
        HStack(spacing: 8) {
            ProgressBar(value: total == 0 ? 0 : Double(done) / Double(total), height: 5)
            Text("\(done) / \(total)")
                .font(.caption.monospacedDigit())
                .foregroundStyle(.secondary)
        }
    }
}

func subjectSymbol(_ icon: String) -> String {
    switch icon {
    case "de": return "character.book.closed.fill"
    case "wifi": return "wifi"
    case "chip": return "cpu"
    case "cz": return "text.book.closed.fill"
    case "people": return "person.2.fill"
    case "flask": return "flask.fill"
    case "bolt": return "bolt.fill"
    case "function": return "function"
    case "ruler": return "ruler.fill"
    case "code": return "chevron.left.forwardslash.chevron.right"
    case "brush": return "paintbrush.fill"
    default: return "lock.fill"
    }
}

/// Subject glyph for list rows: real flags for the languages, SF Symbols otherwise.
struct SubjectIcon: View {
    let icon: String
    var open = true
    var size: CGFloat = 26

    var body: some View {
        Group {
            switch icon {
            case "de": GermanFlag(size: size)
            case "cz": CzechFlag(size: size)
            case "uk": UkFlag(size: size)
            default:
                Image(systemName: subjectSymbol(icon))
                    .font(.system(size: size * 0.72, weight: .semibold))
                    .foregroundStyle(open ? AnyShapeStyle(Color.accentColor) : AnyShapeStyle(.tertiary))
            }
        }
        .frame(width: size, height: size)
        .opacity(open ? 1 : 0.6)
    }
}
