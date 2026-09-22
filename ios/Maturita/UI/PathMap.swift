import SwiftUI

struct MapNode: Identifiable {
    let id: String
    let label: String
    let locked: Bool
    let done: Bool
    let current: Bool
    var finish: Bool = false
    var destination: Route?
}

private struct NodePos {
    var x: CGFloat
    var y: CGFloat
}

private struct Serpentine {
    let points: [NodePos]
    let width: CGFloat
    let height: CGFloat
}

private func serpentine(
    count n: Int,
    avail: CGFloat,
    mx: CGFloat,
    my: CGFloat,
    spac: CGFloat,
    gap: CGFloat,
    wave: CGFloat,
    lift: CGFloat = 0
) -> Serpentine {
    guard n > 0 else { return Serpentine(points: [], width: avail, height: my * 2) }
    let scale = min(1, max(0.55, avail / (2 * mx + CGFloat(max(n - 1, 0)) * spac)))
    let smx = mx * scale
    let smy = my * scale
    let sspac = spac * scale
    let sgap = gap * scale
    let swave = wave * scale
    let slift = lift * scale

    var rows = 1
    while rows <= n {
        let cols = (n + rows - 1) / rows
        if 2 * smx + CGFloat(cols - 1) * sspac <= avail + 1 { break }
        rows += 1
    }
    rows = min(rows, n)
    let cols = max(1, (n + rows - 1) / rows)
    let cw = 2 * smx + CGFloat(cols - 1) * sspac
    let ch = 2 * smy + CGFloat(rows - 1) * sgap + slift
    let phase = CGFloat(2 * Double.pi * 1.7 / Double(max(n - 1, 1)))

    var pts = Array(repeating: NodePos(x: 0, y: 0), count: n)
    for r in 0..<rows {
        let base = r * cols
        let len = min(cols, n - base)
        let fwd = r % 2 == 0
        for c in 0..<len {
            let i = base + c
            let cc = fwd ? c : cols - 1 - c
            pts[i] = NodePos(
                x: smx + CGFloat(cc) * sspac,
                y: smy + slift + CGFloat(r) * sgap + swave * sin(CGFloat(i) * phase)
            )
        }
    }
    return Serpentine(points: pts, width: cw, height: ch)
}

private func catmull(_ pts: [NodePos], _ t: CGFloat) -> NodePos {
    let n = pts.count
    guard n > 1 else { return pts.first ?? NodePos(x: 0, y: 0) }
    var k = Int(t)
    var u = t - CGFloat(k)
    if k < 0 { k = 0; u = 0 }
    if k >= n - 1 { k = n - 2; u = 1 }
    let p1 = pts[k]
    let p2 = pts[k + 1]
    let p0 = k - 1 >= 0
        ? pts[k - 1]
        : NodePos(x: p1.x - (p2.x - p1.x), y: p1.y - (p2.y - p1.y))
    let p3 = k + 2 < n
        ? pts[k + 2]
        : NodePos(x: p2.x + (p2.x - p1.x), y: p2.y + (p2.y - p1.y))
    let u2 = u * u
    let u3 = u2 * u
    let x = 0.5 * (2 * p1.x + (-p0.x + p2.x) * u
        + (2 * p0.x - 5 * p1.x + 4 * p2.x - p3.x) * u2
        + (-p0.x + 3 * p1.x - 3 * p2.x + p3.x) * u3)
    let y = 0.5 * (2 * p1.y + (-p0.y + p2.y) * u
        + (2 * p0.y - 5 * p1.y + 4 * p2.y - p3.y) * u2
        + (-p0.y + 3 * p1.y - 3 * p2.y + p3.y) * u3)
    return NodePos(x: x, y: y)
}

struct PathMap: View {
    let nodes: [MapNode]
    let palette: Palette
    var litUntil: CGFloat = 0
    var nodeSize: CGFloat = 72
    var mx: CGFloat = 90
    var my: CGFloat = 110
    var spac: CGFloat = 170
    var gap: CGFloat = 170
    var wave: CGFloat = 28
    var branch: (index: Int, node: MapNode)? = nil

    var body: some View {
        GeometryReader { geo in
            let layout = serpentine(
                count: nodes.count,
                avail: max(geo.size.width, 320),
                mx: mx,
                my: my,
                spac: spac,
                gap: gap,
                wave: wave,
                lift: branch == nil ? 0 : 40
            )
            ScrollView([.horizontal, .vertical]) {
                ZStack(alignment: .topLeading) {
                    railCanvas(points: layout.points)
                        .frame(width: layout.width, height: layout.height + 96)

                    ForEach(Array(nodes.enumerated()), id: \.element.id) { i, node in
                        let pt = layout.points[i]
                        nodeView(node, index: i)
                            .position(x: pt.x, y: pt.y)
                        Text(node.label)
                            .font(.caption.weight(.semibold))
                            .foregroundStyle(node.locked ? palette.overlay : palette.text)
                            .multilineTextAlignment(.center)
                            .frame(width: 140)
                            .position(x: pt.x, y: pt.y + nodeSize / 2 + 18)
                    }

                    if let branch, branch.index < layout.points.count {
                        let base = layout.points[branch.index]
                        let tip = NodePos(x: base.x + 48, y: base.y - 96)
                        Path { p in
                            p.move(to: CGPoint(x: base.x, y: base.y - nodeSize / 2))
                            p.addQuadCurve(
                                to: CGPoint(x: tip.x, y: tip.y + nodeSize / 2),
                                control: CGPoint(x: base.x + 10, y: tip.y + 20)
                            )
                        }
                        .stroke(palette.rail, style: StrokeStyle(lineWidth: 8, lineCap: .round))
                        nodeView(branch.node, index: nil)
                            .position(x: tip.x, y: tip.y)
                        Text(branch.node.label)
                            .font(.caption.weight(.semibold))
                            .foregroundStyle(palette.text)
                            .multilineTextAlignment(.center)
                            .frame(width: 140)
                            .position(x: tip.x, y: tip.y + nodeSize / 2 + 18)
                    }
                }
                .frame(width: max(layout.width, geo.size.width), height: layout.height + 96, alignment: .topLeading)
            }
        }
        .frame(minHeight: 360)
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
            stroke(0, end, color: palette.rail, width: 12)
            if litUntil > 0 {
                stroke(0, min(litUntil, end), color: palette.accent, width: 12)
            }
            let mutedStart = litUntil > 0 ? litUntil : 0
            if mutedStart < end {
                stroke(mutedStart, end, color: palette.subtext.opacity(0.22), width: 12, dashed: true)
            }
        }
    }

    @ViewBuilder
    private func nodeView(_ node: MapNode, index: Int?) -> some View {
        let content = ZStack {
            Circle()
                .fill(nodeFill(node))
                .overlay {
                    if node.locked {
                        Circle().strokeBorder(palette.lockedBorder, lineWidth: 1)
                    }
                }
            if node.finish {
                VStack(spacing: 2) {
                    if let index { Text("\(index + 1)").font(.title3.weight(.black)) }
                    FinishFlagBadge(palette: palette)
                }
                .foregroundStyle(palette.overlay)
            } else if node.locked {
                VStack(spacing: 2) {
                    if let index { Text("\(index + 1)").font(.title3.weight(.black)) }
                    Image(systemName: "lock.fill").font(.caption.weight(.bold))
                }
                .foregroundStyle(palette.overlay)
            } else if node.done {
                VStack(spacing: 2) {
                    if let index { Text("\(index + 1)").font(.title3.weight(.black)) }
                    Image(systemName: "checkmark").font(.caption.weight(.bold))
                }
                .foregroundStyle(palette.onAccent)
            } else {
                if let index {
                    Text("\(index + 1)")
                        .font(.title3.weight(.black))
                        .foregroundStyle(node.current ? palette.onAccent : palette.text)
                }
            }
        }
        .frame(width: nodeSize, height: nodeSize)
        .shadow(color: .black.opacity(node.locked ? 0 : 0.12), radius: 6, y: 2)

        if let dest = node.destination, !node.locked {
            NavigationLink(value: dest) { content }
                .buttonStyle(.plain)
        } else {
            content.opacity(node.locked ? 0.55 : 1)
        }
    }

    private func nodeFill(_ node: MapNode) -> Color {
        if node.locked { return palette.lockedBg }
        if node.current || node.done { return palette.accent }
        return palette.node
    }
}

private struct FinishFlagBadge: View {
    let palette: Palette

    var body: some View {
        let cell: CGFloat = 4
        Canvas { ctx, _ in
            for iy in 0..<2 {
                for ix in 0..<3 {
                    let dark = ((ix + iy) & 1) == 1
                    let rect = CGRect(x: CGFloat(ix) * cell, y: CGFloat(iy) * cell, width: cell, height: cell)
                    ctx.fill(
                        Path(rect),
                        with: .color(dark ? palette.finishLight : palette.finishStroke)
                    )
                }
            }
        }
        .frame(width: cell * 3, height: cell * 2)
    }
}
