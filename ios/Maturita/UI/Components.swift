import SwiftUI

#if os(macOS)
enum MacChrome {
    static let contentMaxWidth: CGFloat = 1080
    static let exerciseMaxWidth: CGFloat = 900
    static let cardPadding: CGFloat = 24
    static let listRowMinHeight: CGFloat = 52
    static let homeTileMin: CGFloat = 260
    static let homeTileHeight: CGFloat = 176
    static let pathNode: CGFloat = 84
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

    var body: some View {
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
        }
    }
}

struct Meaning: View {
    let text: String
    let palette: Palette
    let visible: Bool

    var body: some View {
        if visible && !text.isEmpty {
            Text(text)
                #if os(macOS)
                .font(.body)
                #else
                .font(.footnote)
                #endif
                .foregroundStyle(.secondary)
                .padding(.bottom, 4)
        }
    }
}

struct WordField: View {
    @Binding var value: String
    var placeholder: String
    var mark: Bool?
    var palette: Palette?

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
        #else
        self
        #endif
    }

    /// Full-width, leading-aligned content for exercise screens.
    func exerciseContent() -> some View {
        #if os(macOS)
        frame(maxWidth: MacChrome.exerciseMaxWidth, alignment: .leading)
            .padding(28)
            .frame(maxWidth: .infinity, alignment: .leading)
        #else
        frame(maxWidth: .infinity, alignment: .leading)
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
        .padding(.horizontal)
        #if os(macOS)
        .padding(.top, 12)
        .padding(.bottom, 10)
        .frame(maxWidth: MacChrome.exerciseMaxWidth)
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
