import SwiftUI

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
        Button(label, action: action)
            .buttonStyle(.borderedProminent)
            .controlSize(.large)
    }
}

struct PillButton: View {
    let label: String
    var action: () -> Void

    var body: some View {
        Button(label, action: action)
            .buttonStyle(.bordered)
    }
}

struct GlassCard<Content: View>: View {
    @ViewBuilder var content: () -> Content

    var body: some View {
        content()
            .padding(16)
            .frame(maxWidth: .infinity, alignment: .leading)
            .glassEffect(.regular, in: RoundedRectangle(cornerRadius: 22, style: .continuous))
    }
}

struct FeedbackLine: View {
    let text: String
    let kind: String
    let palette: Palette

    var body: some View {
        if !text.isEmpty {
            Text(text)
                .font(.subheadline.weight(.semibold))
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
                .font(.footnote)
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
        TextField(placeholder, text: $value)
            .textInputAutocapitalization(.never)
            .autocorrectionDisabled()
            .padding(12)
            .background(.thinMaterial, in: RoundedRectangle(cornerRadius: 14, style: .continuous))
            .overlay(
                RoundedRectangle(cornerRadius: 14, style: .continuous)
                    .stroke(border, lineWidth: mark == nil ? 0 : 1.5)
            )
    }

    private var border: Color {
        if mark == true { return palette?.success ?? .green }
        if mark == false { return palette?.error ?? .red }
        return .clear
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
    }
}

struct Hint: View {
    let text: String
    var body: some View {
        Text(text).font(.footnote).foregroundStyle(.secondary).padding(.bottom, 6)
    }
}

struct Prompt: View {
    let text: String
    var body: some View {
        Text(text).font(.body.weight(.medium))
    }
}

struct WordChip: View {
    let text: String
    var action: () -> Void

    var body: some View {
        Button(text, action: action)
            .buttonStyle(.bordered)
            .controlSize(.small)
    }
}

struct OptionChip: View {
    let text: String
    let selected: Bool
    let mark: Bool?
    var action: () -> Void

    var body: some View {
        Button(action: action) {
            HStack {
                Text(text).frame(maxWidth: .infinity, alignment: .leading)
                if selected { Image(systemName: "checkmark") }
            }
        }
        .buttonStyle(.bordered)
        .tint(tint)
        .padding(.bottom, 4)
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
