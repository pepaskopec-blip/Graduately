import SwiftUI

struct MapNode: Identifiable {
    let id: String
    let label: String
    let locked: Bool
    let done: Bool
    let current: Bool
    var finish: Bool = false
    var destination: Route?
    /// Glyph for a node that has no number, such as a side branch.
    var symbol: String? = nil
}

private struct NodePos {
    var x: CGFloat
    var y: CGFloat
    var row: Int
}

private struct Serpentine {
    let points: [NodePos]
    let width: CGFloat
    let height: CGFloat
    let spacing: CGFloat
}

/// Keep enough room under/above each node for a two-line caption.
#if os(macOS)
private let labelBand: CGFloat = 56
#else
private let labelBand: CGFloat = 44
#endif

private func serpentine(
    count n: Int,
    avail: CGFloat,
    mx: CGFloat,
    my: CGFloat,
    spac: CGFloat,
    gap: CGFloat,
    wave: CGFloat,
    nodeSize: CGFloat,
    lift: CGFloat = 0
) -> Serpentine {
    guard n > 0 else {
        return Serpentine(points: [], width: avail, height: my * 2, spacing: spac)
    }

    // Pick the fewest rows that fit the preferred spacing in the available width.
    // Do NOT pre-scale as if every node were on one row — that crushed tall maps.
    var rows = 1
    var cols = n
    while rows <= n {
        cols = max(1, (n + rows - 1) / rows)
        if 2 * mx + CGFloat(cols - 1) * spac <= avail + 1 { break }
        rows += 1
    }
    rows = min(rows, n)
    cols = max(1, (n + rows - 1) / rows)

    let needed = 2 * mx + CGFloat(cols - 1) * spac
    let scale = needed > avail ? max(0.72, avail / needed) : 1

    let smx = mx * scale
    let smy = max(my * scale, nodeSize / 2 + labelBand + 8)
    let sspac = max(spac * scale, nodeSize + 36)
    // Gap must clear labels that sit between rows (outward placement still
    // needs space when wave lifts nodes toward each other).
    let sgap = max(gap * scale, nodeSize + labelBand * 2 + 28)
    let swave = min(wave * scale, sgap * 0.12)
    let slift = lift * scale

    let cw = 2 * smx + CGFloat(cols - 1) * sspac
    let ch = 2 * smy + CGFloat(rows - 1) * sgap + slift + labelBand
    let phase = CGFloat(2 * Double.pi * 1.7 / Double(max(n - 1, 1)))

    var pts = Array(repeating: NodePos(x: 0, y: 0, row: 0), count: n)
    for r in 0..<rows {
        let base = r * cols
        let len = min(cols, n - base)
        let fwd = r % 2 == 0
        for c in 0..<len {
            let i = base + c
            let cc = fwd ? c : cols - 1 - c
            pts[i] = NodePos(
                x: smx + CGFloat(cc) * sspac,
                y: smy + slift + CGFloat(r) * sgap + swave * sin(CGFloat(i) * phase),
                row: r
            )
        }
    }
    return Serpentine(points: pts, width: cw, height: ch, spacing: sspac)
}

private func catmull(_ pts: [NodePos], _ t: CGFloat) -> NodePos {
    let n = pts.count
    guard n > 1 else { return pts.first ?? NodePos(x: 0, y: 0, row: 0) }
    var k = Int(t)
    var u = t - CGFloat(k)
    if k < 0 { k = 0; u = 0 }
    if k >= n - 1 { k = n - 2; u = 1 }
    let p1 = pts[k]
    let p2 = pts[k + 1]
    let p0 = k - 1 >= 0
        ? pts[k - 1]
        : NodePos(x: p1.x - (p2.x - p1.x), y: p1.y - (p2.y - p1.y), row: p1.row)
    let p3 = k + 2 < n
        ? pts[k + 2]
        : NodePos(x: p2.x + (p2.x - p1.x), y: p2.y + (p2.y - p1.y), row: p2.row)
    let u2 = u * u
    let u3 = u2 * u
    let x = 0.5 * (2 * p1.x + (-p0.x + p2.x) * u
        + (2 * p0.x - 5 * p1.x + 4 * p2.x - p3.x) * u2
        + (-p0.x + 3 * p1.x - 3 * p2.x + p3.x) * u3)
    let y = 0.5 * (2 * p1.y + (-p0.y + p2.y) * u
        + (2 * p0.y - 5 * p1.y + 4 * p2.y - p3.y) * u2
        + (-p0.y + 3 * p1.y - 3 * p2.y + p3.y) * u3)
    return NodePos(x: x, y: y, row: p1.row)
}

struct PathMap: View {
    let nodes: [MapNode]
    let palette: Palette
    var litUntil: CGFloat = 0
    var nodeSize: CGFloat = 64
    var mx: CGFloat = 90
    var my: CGFloat = 110
    var spac: CGFloat = 170
    var gap: CGFloat = 220
    var wave: CGFloat = 18
    var branch: (index: Int, node: MapNode)? = nil
    /// Title, subtitle and progress set above the path; they scroll away with it.
    var heading: (title: String, subtitle: String, done: Int, total: Int)? = nil
    @Environment(\.accent) private var accent

    var body: some View {
        GeometryReader { geo in
            let layout = serpentine(
                count: nodes.count,
                avail: max(geo.size.width - 24, 480),
                mx: mx,
                my: my,
                spac: spac,
                gap: gap,
                wave: wave,
                nodeSize: nodeSize,
                lift: branch == nil ? 0 : 56
            )
            let labelWidth = min(140, max(88, layout.spacing - 16))
            let canvasH = layout.height + labelBand + 24
            ScrollView([.horizontal, .vertical]) {
                VStack(alignment: .leading, spacing: 0) {
                if let heading {
                    PageHeader(title: heading.title, subtitle: heading.subtitle, progress: (heading.done, heading.total))
                        .padding([.horizontal, .top], Page.inset)
                        .frame(width: geo.size.width)
                }
                ZStack(alignment: .topLeading) {
                    railCanvas(points: layout.points)
                        .frame(width: layout.width, height: canvasH)

                    ForEach(Array(nodes.enumerated()), id: \.element.id) { i, node in
                        let pt = layout.points[i]
                        let labelBelow = pt.row % 2 == 0
                        nodeView(node, index: i)
                            .position(x: pt.x, y: pt.y)
                        Text(node.label)
                            .font(onMac(Font.callout, Font.caption2).weight(node.current ? .bold : .medium))
                            .foregroundStyle(node.locked ? palette.ink3 : palette.ink)
                            .multilineTextAlignment(.center)
                            .lineLimit(2)
                            .minimumScaleFactor(0.85)
                            // Keeps the caption readable where the rail turns behind it.
                            .padding(.horizontal, 5)
                            .padding(.vertical, 2)
                            .background(palette.paper.opacity(0.88), in: RoundedRectangle(cornerRadius: 6, style: .continuous))
                            .frame(width: labelWidth + 10, height: labelBand, alignment: labelBelow ? .top : .bottom)
                            .position(
                                x: pt.x,
                                y: labelBelow
                                    ? pt.y + nodeSize / 2 + labelBand / 2 + 6
                                    : pt.y - nodeSize / 2 - labelBand / 2 - 6
                            )
                    }

                    if let branch, branch.index < layout.points.count {
                        let base = layout.points[branch.index]
                        let tip = NodePos(x: base.x + 64, y: base.y - 128, row: base.row)
                        Path { p in
                            p.move(to: CGPoint(x: base.x, y: base.y - nodeSize / 2))
                            p.addQuadCurve(
                                to: CGPoint(x: tip.x, y: tip.y + nodeSize / 2),
                                control: CGPoint(x: base.x + 14, y: tip.y + 28)
                            )
                        }
                        .stroke(palette.line, style: StrokeStyle(lineWidth: 8, lineCap: .round))
                        nodeView(branch.node, index: nil)
                            .position(x: tip.x, y: tip.y)
                        Text(branch.node.label)
                            .font(onMac(Font.callout, Font.caption2).weight(.medium))
                            .foregroundStyle(palette.ink)
                            .multilineTextAlignment(.center)
                            .lineLimit(2)
                            .frame(width: labelWidth, height: labelBand, alignment: .bottom)
                            .position(x: tip.x, y: tip.y - nodeSize / 2 - labelBand / 2 - 6)
                    }
                }
                .frame(width: layout.width + 24, height: canvasH, alignment: .topLeading)
                .frame(minWidth: geo.size.width, alignment: layout.width + 24 < geo.size.width ? .center : .leading)
                }
            }
        }
        #if os(macOS)
        .frame(minHeight: 520)
        #else
        .frame(minHeight: 420)
        #endif
        .softAppear()
    }

    @ViewBuilder
    private func railCanvas(points: [NodePos]) -> some View {
        Canvas { ctx, _ in
            guard points.count >= 2 else { return }
            func stroke(_ t0: CGFloat, _ t1: CGFloat, color: Color, width: CGFloat, dashed: Bool = false) {
                var path = Path()
                let steps = max(1, Int((t1 - t0) * 48))
                let start = catmull(points, t0)
                path.move(to: CGPoint(x: start.x, y: start.y))
                for s in 1...steps {
                    let t = t0 + (t1 - t0) * CGFloat(s) / CGFloat(steps)
                    let pt = catmull(points, t)
                    path.addLine(to: CGPoint(x: pt.x, y: pt.y))
                }
                var style = StrokeStyle(lineWidth: width, lineCap: .round, lineJoin: .round)
                if dashed { style.dash = [1, 18] }
                ctx.stroke(path, with: .color(color), style: style)
            }
            let end = CGFloat(points.count - 1)
            stroke(0, end, color: palette.line, width: 8)
            if litUntil > 0 {
                stroke(0, min(litUntil, end), color: accent.solid, width: 8)
            }
        }
    }

    /// Done steps are filled with a tick, the next one is filled and ringed,
    /// the rest wait as outlines.
    @ViewBuilder
    private func nodeView(_ node: MapNode, index: Int?) -> some View {
        let lit = !node.locked && (node.current || node.done)
        let ink = node.locked ? palette.ink3 : lit ? accent.onSolid : palette.ink
        let content = ZStack {
            if node.current {
                Circle()
                    .strokeBorder(accent.solid.opacity(0.28), lineWidth: 5)
                    .frame(width: nodeSize + 18, height: nodeSize + 18)
            }
            Circle()
                .fill(node.locked ? palette.sunken : lit ? accent.solid : palette.surface)
                .overlay {
                    if !lit {
                        Circle().strokeBorder(node.locked ? palette.line : accent.solid.opacity(0.6), lineWidth: node.locked ? 1 : 2.5)
                    }
                }
                .frame(width: nodeSize, height: nodeSize)
            VStack(spacing: 1) {
                if let index {
                    Text("\(index + 1)")
                        .display(onMac(28, 20), relativeTo: .title2)
                } else if let symbol = node.symbol {
                    Image(systemName: symbol)
                        .font(onMac(Font.title2, Font.headline).weight(.semibold))
                }
                if let mark = node.finish ? "flag.checkered" : node.locked ? "lock.fill" : node.done ? "checkmark" : nil {
                    Image(systemName: mark)
                        .font(onMac(Font.caption, Font.caption2).weight(.bold))
                }
            }
            .foregroundStyle(ink)
        }
        .frame(width: nodeSize + 18, height: nodeSize + 18)

        if let dest = node.destination, !node.locked {
            NavigationLink(value: dest) { content }
                .buttonStyle(PressScaleStyle())
        } else {
            content
        }
    }
}
