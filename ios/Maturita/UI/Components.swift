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
    let palette: Palette
    var action: () -> Void

    var body: some View {
        Button(action: action) {
            Text(label)
                .font(.system(size: 15, weight: .medium))
                .foregroundStyle(palette.onAccent)
                .padding(.horizontal, 22)
                .padding(.vertical, 12)
                .background(palette.accent, in: Capsule())
        }
        .buttonStyle(.plain)
    }
}

struct PillButton: View {
    let label: String
    let palette: Palette
    var action: () -> Void

    var body: some View {
        Button(action: action) {
            Text(label)
                .font(.system(size: 13, weight: .medium))
                .foregroundStyle(palette.text)
                .padding(.horizontal, 16)
                .padding(.vertical, 8)
                .background(palette.text.opacity(0.06), in: Capsule())
        }
        .buttonStyle(.plain)
    }
}

struct IconHit<Content: View>: View {
    var action: () -> Void
    @ViewBuilder var content: () -> Content

    var body: some View {
        Button(action: action) {
            content()
                .frame(width: 40, height: 40)
        }
        .buttonStyle(.plain)
    }
}

struct PageTop: View {
    @ObservedObject var vm: AppModel
    var back: () -> Void
    let title: String
    var subtitle: String?

    var body: some View {
        let p = vm.palette
        VStack(spacing: 6) {
            ZStack {
                Text(title)
                    .font(.system(size: 22, weight: .medium))
                    .foregroundStyle(p.text)
                    .multilineTextAlignment(.center)
                    .padding(.horizontal, 48)
                HStack {
                    IconHit(action: back) { BackIcon(color: p.text) }
                    Spacer()
                }
            }
            if let subtitle, !subtitle.isEmpty {
                Text(subtitle)
                    .font(.system(size: 14))
                    .foregroundStyle(p.subtext)
                    .multilineTextAlignment(.center)
            }
        }
        .padding(.top, 4)
        .padding(.bottom, 14)
    }
}

struct CardBox<Content: View>: View {
    let palette: Palette
    @ViewBuilder var content: () -> Content

    var body: some View {
        content()
            .padding(20)
            .frame(maxWidth: .infinity, alignment: .leading)
            .background(palette.mantle, in: RoundedRectangle(cornerRadius: 20, style: .continuous))
            .overlay(
                RoundedRectangle(cornerRadius: 20, style: .continuous)
                    .stroke(palette.text.opacity(0.07), lineWidth: 1)
            )
    }
}

struct FeedbackLine: View {
    let text: String
    let kind: String
    let palette: Palette

    var body: some View {
        if !text.isEmpty {
            Text(text)
                .font(.system(size: 15, weight: .semibold))
                .foregroundStyle(kind == "ok" ? palette.success : kind == "warn" ? palette.warning : palette.error)
                .padding(.top, 8)
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
                .font(.system(size: 13))
                .foregroundStyle(palette.subtext)
                .padding(.bottom, 8)
        }
    }
}

struct WordField: View {
    @Binding var value: String
    let palette: Palette
    var placeholder: String
    var mark: Bool?

    var body: some View {
        let border: Color = {
            if mark == true { return palette.success }
            if mark == false { return palette.error }
            return palette.text.opacity(0.12)
        }()
        ZStack(alignment: .leading) {
            if value.isEmpty {
                Text(placeholder).foregroundStyle(palette.overlay).font(.system(size: 15))
            }
            TextField("", text: $value)
                .font(.system(size: 15))
                .foregroundStyle(palette.text)
                .tint(palette.accent)
        }
        .padding(.horizontal, 14)
        .padding(.vertical, 12)
        .background(palette.surface0, in: RoundedRectangle(cornerRadius: 14, style: .continuous))
        .overlay(RoundedRectangle(cornerRadius: 14, style: .continuous).stroke(border, lineWidth: 1))
    }
}

struct SegChip: View {
    let label: String
    let selected: Bool
    let palette: Palette
    var action: () -> Void

    var body: some View {
        Button(action: action) {
            Text(label)
                .font(.system(size: 14, weight: .semibold))
                .foregroundStyle(palette.text)
                .frame(maxWidth: .infinity)
                .padding(.vertical, 8)
                .background(selected ? palette.mantle : Color.clear, in: Capsule())
        }
        .buttonStyle(.plain)
    }
}

struct Hint: View {
    let text: String
    let palette: Palette
    var body: some View {
        Text(text).font(.system(size: 13)).foregroundStyle(palette.subtext).padding(.bottom, 8)
    }
}

struct Prompt: View {
    let text: String
    let palette: Palette
    var body: some View {
        Text(text).font(.system(size: 16, weight: .medium)).foregroundStyle(palette.text)
    }
}

struct WordChip: View {
    let text: String
    let palette: Palette
    var action: () -> Void

    var body: some View {
        Button(action: action) {
            Text(text)
                .font(.system(size: 15, weight: .medium))
                .foregroundStyle(palette.text)
                .padding(.horizontal, 12)
                .padding(.vertical, 8)
                .background(palette.mantle, in: Capsule())
                .overlay(Capsule().stroke(palette.text.opacity(0.1), lineWidth: 1))
        }
        .buttonStyle(.plain)
    }
}

struct OptionChip: View {
    let text: String
    let selected: Bool
    let mark: Bool?
    let palette: Palette
    var action: () -> Void

    var body: some View {
        let border: Color = {
            if mark == true { return palette.success }
            if mark == false { return palette.error }
            return selected ? palette.accent : palette.text.opacity(0.1)
        }()
        Button(action: action) {
            Text(text)
                .font(.system(size: 15))
                .foregroundStyle(palette.text)
                .frame(maxWidth: .infinity, alignment: .leading)
                .padding(12)
                .background(selected ? palette.surface1 : palette.mantle, in: RoundedRectangle(cornerRadius: 14, style: .continuous))
                .overlay(RoundedRectangle(cornerRadius: 14, style: .continuous).stroke(border, lineWidth: 1))
        }
        .buttonStyle(.plain)
        .padding(.bottom, 6)
    }
}
