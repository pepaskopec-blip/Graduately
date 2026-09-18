import SwiftUI

struct MapNode {
    let id: String
    let label: String
    let locked: Bool
    let done: Bool
    let current: Bool
    var finish: Bool = false
    var onClick: (() -> Void)?
}

struct PathMap: View {
    let nodes: [MapNode]
    let palette: Palette

    var body: some View {
        ScrollView {
            LazyVStack(spacing: 10) {
                ForEach(Array(nodes.enumerated()), id: \.element.id) { i, node in
                    Button {
                        if !node.locked { node.onClick?() }
                    } label: {
                        CardBox(palette: palette) {
                            HStack(spacing: 12) {
                                let bg: Color = {
                                    if node.locked { return palette.lockedBg }
                                    if node.current || node.done { return palette.accent }
                                    return palette.node
                                }()
                                let fg: Color = {
                                    if node.locked { return palette.overlay }
                                    if node.current || node.done { return palette.onAccent }
                                    return palette.text
                                }()
                                ZStack {
                                    Circle().fill(bg).frame(width: 44, height: 44)
                                    if node.locked {
                                        LockIcon(color: palette.overlay, size: 16)
                                    } else if node.done {
                                        CheckIcon(color: palette.onAccent, size: 16)
                                    } else {
                                        Text("\(i + 1)")
                                            .font(.system(size: 16, weight: .heavy))
                                            .foregroundStyle(fg)
                                    }
                                }
                                Text(node.label)
                                    .font(.system(size: 16, weight: node.current ? .bold : .semibold))
                                    .foregroundStyle(node.locked ? palette.overlay : palette.text)
                                    .frame(maxWidth: .infinity, alignment: .leading)
                            }
                        }
                    }
                    .buttonStyle(.plain)
                    .disabled(node.locked || node.onClick == nil)
                    .opacity(node.locked ? 0.48 : 1)
                }
            }
            .padding(.bottom, 24)
        }
    }
}
