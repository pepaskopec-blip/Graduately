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

private struct SoftAppear: ViewModifier {
    @Environment(\.accessibilityReduceMotion) private var reduceMotion
    @State private var shown = false

    func body(content: Content) -> some View {
        content
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
        modifier(SoftAppear())
    }
}

#if os(macOS)
enum MacChrome {
    static let pageInset: CGFloat = 28
    static let cardPadding: CGFloat = 22
    static let listRowMinHeight: CGFloat = 52
    static let homeTileMin: CGFloat = 240
    static let homeTileHeight: CGFloat = 168
    static let pathNode: CGFloat = 84
}

enum MacGrid {
    /// Picks a column count that fills the width and keeps tiles wider than they are tall.
    static func columns(width: CGFloat, height: CGFloat, count: Int, minWidth: CGFloat, minHeight: CGFloat, spacing: CGFloat) -> Int {
        guard count > 0, width > 0 else { return 1 }
        let maxCols = max(1, min(count, Int((width + spacing) / (minWidth + spacing))))
        var best = maxCols
        var bestScore = CGFloat.greatestFiniteMagnitude
        let offeredH = height > 1 ? height : minHeight * CGFloat(count)
        for cols in 1...maxCols {
            let rows = Int(ceil(Double(count) / Double(cols)))
            let tileW = (width - spacing * CGFloat(cols - 1)) / CGFloat(cols)
            if tileW + 0.5 < minWidth { continue }
            let needed = CGFloat(rows) * minHeight + spacing * CGFloat(max(rows - 1, 0))
            let tileH = max(minHeight, (offeredH - spacing * CGFloat(max(rows - 1, 0))) / CGFloat(rows))
            let aspect = tileH / max(tileW, 1)
            let rem = count % cols
            let ragged = rem == 0 ? 0 : CGFloat(1.5)
            let lonely = rem == 1 ? CGFloat(1.4) : 0
            let tooTall = max(0, aspect - 1.05) * 3
            let overflow = max(0, needed - offeredH)
            let score = abs(aspect - 0.78) * 2 + ragged + lonely + tooTall + overflow / 180
            if score < bestScore {
                bestScore = score
                best = cols
            }
        }
        return best
    }

    static func contentHeight(width: CGFloat, height: CGFloat, count: Int, minWidth: CGFloat, minHeight: CGFloat, spacing: CGFloat) -> CGFloat {
        let cols = columns(width: width, height: height, count: count, minWidth: minWidth, minHeight: minHeight, spacing: spacing)
        let rows = max(1, Int(ceil(Double(max(count, 1)) / Double(cols))))
        return CGFloat(rows) * minHeight + spacing * CGFloat(max(rows - 1, 0))
    }
}

struct MacHeaderHeight: PreferenceKey {
    static var defaultValue: CGFloat = 0
    static func reduce(value: inout CGFloat, nextValue: () -> CGFloat) {
        value = max(value, nextValue())
    }
}

/// Card grid that grows into the space it is given and scrolls only when the
/// minimum tile size no longer fits.
struct MacFillGrid<Content: View>: View {
    var count: Int
    var minWidth: CGFloat = 240
    var minHeight: CGFloat = 148
    var spacing: CGFloat = 16
    var inset: CGFloat = 28
    /// Home embeds the grid in its own scroll view. A second scroller here
    /// swallows the wheel once a banner (the changelog) makes the page taller.
    var scrolls = true
    @ViewBuilder var content: (Int) -> Content

    var body: some View {
        GeometryReader { geo in
            let innerW = max(geo.size.width - inset * 2, minWidth)
            let available = max(geo.size.height - inset * 2, minHeight)
            let cols = MacGrid.columns(width: innerW, height: available, count: count, minWidth: minWidth, minHeight: minHeight, spacing: spacing)
            let rows = max(1, Int(ceil(Double(max(count, 1)) / Double(cols))))
            let floorH = CGFloat(rows) * minHeight + spacing * CGFloat(max(rows - 1, 0))
            let gridH = max(available, floorH)
            let tileH = max(minHeight, (gridH - spacing * CGFloat(max(rows - 1, 0))) / CGFloat(rows))
            let tileW = (innerW - spacing * CGFloat(max(cols - 1, 0))) / CGFloat(max(cols, 1))
            let lastCount = count - (rows - 1) * cols
            let tiles = VStack(spacing: spacing) {
                ForEach(0..<rows, id: \.self) { row in
                    let stretch = row < rows - 1 || lastCount > 1
                    HStack(spacing: spacing) {
                        ForEach(0..<cols, id: \.self) { col in
                            let index = row * cols + col
                            if index < count {
                                content(index)
                                    .frame(
                                        maxWidth: stretch ? .infinity : tileW,
                                        minHeight: tileH,
                                        maxHeight: tileH,
                                        alignment: .topLeading
                                    )
                            }
                        }
                    }
                    .frame(maxWidth: .infinity, alignment: .leading)
                }
            }
            .frame(height: gridH, alignment: .top)
            .padding(inset)
            .frame(maxWidth: .infinity, minHeight: scrolls ? geo.size.height : nil, alignment: .top)

            if scrolls {
                ScrollView { tiles }
                    .scrollDisabled(floorH + inset * 2 <= geo.size.height + 1)
            } else {
                tiles
            }
        }
    }
}

struct MacLinkCard: View {
    let title: String
    var subtitle: String? = nil
    var badge: String? = nil
    var systemImage: String? = nil
    var locked = false

    var body: some View {
        VStack(alignment: .leading, spacing: 10) {
            HStack(alignment: .center) {
                if let badge {
                    Text(badge)
                        .font(.title.weight(.bold))
                        .foregroundStyle(locked ? Color.secondary : Color.accentColor)
                }
                if let systemImage {
                    Image(systemName: systemImage)
                        .font(.title2.weight(.semibold))
                        .foregroundStyle(locked ? Color.secondary : Color.accentColor)
                }
                Spacer(minLength: 0)
                if locked {
                    Image(systemName: "lock.fill")
                        .foregroundStyle(.secondary)
                }
            }
            Text(title)
                .font(.title2.weight(.semibold))
                .foregroundStyle(locked ? .secondary : .primary)
                .fixedSize(horizontal: false, vertical: true)
            if let subtitle, !subtitle.isEmpty {
                Text(subtitle)
                    .font(.body)
                    .foregroundStyle(.secondary)
                    .fixedSize(horizontal: false, vertical: true)
            }
            Spacer(minLength: 0)
        }
        .padding(22)
        .frame(maxWidth: .infinity, maxHeight: .infinity, alignment: .topLeading)
        .softSurface(cornerRadius: 22)
    }
}
#endif

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

    /// Full-width, leading-aligned content for exercise screens.
    func exerciseContent() -> some View {
        frame(maxWidth: .infinity, alignment: .leading)
            #if os(macOS)
            .padding(.horizontal, MacChrome.pageInset)
            .padding(.vertical, 20)
            #else
            .padding()
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
        .padding(.bottom, 10)
        .frame(maxWidth: .infinity)
        #else
        .padding(.top, 8)
        .padding(.bottom, 4)
        #endif
    }
}

/// Primary + secondary text sized for desktop lists.
struct ListRowLabel: View {
    let title: String
    var subtitle: String? = nil

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
