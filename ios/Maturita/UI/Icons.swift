import SwiftUI

struct BackIcon: View {
    var color: Color
    var size: CGFloat = 16
    var body: some View {
        Canvas { ctx, sz in
            var p = Path()
            p.move(to: CGPoint(x: sz.width * 0.68, y: sz.height * 0.18))
            p.addLine(to: CGPoint(x: sz.width * 0.28, y: sz.height * 0.5))
            p.addLine(to: CGPoint(x: sz.width * 0.68, y: sz.height * 0.82))
            ctx.stroke(p, with: .color(color), style: StrokeStyle(lineWidth: min(sz.width, sz.height) * 0.16, lineCap: .round, lineJoin: .round))
        }
        .frame(width: size, height: size)
    }
}

struct SearchIcon: View {
    var color: Color
    var size: CGFloat = 18
    var body: some View {
        Canvas { ctx, sz in
            let r = min(sz.width, sz.height) * 0.28
            let c = CGPoint(x: sz.width * 0.42, y: sz.height * 0.42)
            ctx.stroke(Path(ellipseIn: CGRect(x: c.x - r, y: c.y - r, width: r * 2, height: r * 2)), with: .color(color), lineWidth: min(sz.width, sz.height) * 0.12)
            var handle = Path()
            handle.move(to: CGPoint(x: sz.width * 0.62, y: sz.height * 0.62))
            handle.addLine(to: CGPoint(x: sz.width * 0.82, y: sz.height * 0.82))
            ctx.stroke(handle, with: .color(color), style: StrokeStyle(lineWidth: min(sz.width, sz.height) * 0.12, lineCap: .round))
        }
        .frame(width: size, height: size)
    }
}

struct StatsIcon: View {
    var color: Color
    var size: CGFloat = 18
    var body: some View {
        Canvas { ctx, sz in
            let w = min(sz.width, sz.height) * 0.16
            let bars: [(CGFloat, CGFloat, CGFloat)] = [
                (0.18, 0.48, 0.36), (0.42, 0.22, 0.62), (0.66, 0.36, 0.48),
            ]
            for b in bars {
                let rect = CGRect(x: sz.width * b.0, y: sz.height * b.1, width: w, height: sz.height * b.2)
                ctx.fill(Path(roundedRect: rect, cornerRadius: w), with: .color(color))
            }
        }
        .frame(width: size, height: size)
    }
}

struct SettingsIcon: View {
    var color: Color
    var size: CGFloat = 18
    var body: some View {
        Canvas { ctx, sz in
            let cx = sz.width / 2
            let cy = sz.height / 2
            let s = min(sz.width, sz.height)
            let rOuter = s * 0.48
            let rInner = s * 0.34
            let rHole = s * 0.16
            let teeth = 8
            let toothHalf = CGFloat.pi / CGFloat(teeth)
            var gear = Path()
            for i in 0..<teeth {
                let a = -CGFloat.pi / 2 + 2 * CGFloat.pi * CGFloat(i) / CGFloat(teeth)
                let a0 = a - toothHalf * 0.55
                let a1 = a - toothHalf * 0.28
                let a2 = a + toothHalf * 0.28
                let a3 = a + toothHalf * 0.55
                let p0 = CGPoint(x: cx + cos(a0) * rInner, y: cy + sin(a0) * rInner)
                if i == 0 { gear.move(to: p0) } else { gear.addLine(to: p0) }
                gear.addLine(to: CGPoint(x: cx + cos(a1) * rOuter, y: cy + sin(a1) * rOuter))
                gear.addLine(to: CGPoint(x: cx + cos(a2) * rOuter, y: cy + sin(a2) * rOuter))
                gear.addLine(to: CGPoint(x: cx + cos(a3) * rInner, y: cy + sin(a3) * rInner))
            }
            gear.closeSubpath()
            gear.addEllipse(in: CGRect(x: cx - rHole, y: cy - rHole, width: rHole * 2, height: rHole * 2))
            ctx.fill(gear, with: .color(color), style: FillStyle(eoFill: true))
        }
        .frame(width: size, height: size)
    }
}

struct LockIcon: View {
    var color: Color
    var size: CGFloat = 18
    var body: some View {
        Canvas { ctx, sz in
            let body = CGRect(x: sz.width * 0.28, y: sz.height * 0.46, width: sz.width * 0.44, height: sz.height * 0.36)
            ctx.fill(Path(roundedRect: body, cornerRadius: min(sz.width, sz.height) * 0.08), with: .color(color))
            var arc = Path()
            arc.addArc(
                center: CGPoint(x: sz.width * 0.5, y: sz.height * 0.38),
                radius: sz.width * 0.18,
                startAngle: .degrees(200),
                endAngle: .degrees(340),
                clockwise: false
            )
            ctx.stroke(arc, with: .color(color), style: StrokeStyle(lineWidth: min(sz.width, sz.height) * 0.1, lineCap: .round))
        }
        .frame(width: size, height: size)
    }
}

struct CheckIcon: View {
    var color: Color
    var size: CGFloat = 16
    var body: some View {
        Canvas { ctx, sz in
            var p = Path()
            p.move(to: CGPoint(x: sz.width * 0.18, y: sz.height * 0.52))
            p.addLine(to: CGPoint(x: sz.width * 0.4, y: sz.height * 0.74))
            p.addLine(to: CGPoint(x: sz.width * 0.84, y: sz.height * 0.26))
            ctx.stroke(p, with: .color(color), style: StrokeStyle(lineWidth: min(sz.width, sz.height) * 0.16, lineCap: .round, lineJoin: .round))
        }
        .frame(width: size, height: size)
    }
}

struct WifiIcon: View {
    var color: Color
    var size: CGFloat = 28
    var body: some View {
        Canvas { ctx, sz in
            let c = CGPoint(x: sz.width / 2, y: sz.height * 0.72)
            ctx.fill(Path(ellipseIn: CGRect(x: c.x - sz.width * 0.06, y: c.y - sz.width * 0.06, width: sz.width * 0.12, height: sz.width * 0.12)), with: .color(color))
            for i in 1...3 {
                let r = min(sz.width, sz.height) * (0.16 + CGFloat(i) * 0.16)
                var p = Path()
                p.addArc(center: c, radius: r, startAngle: .degrees(210), endAngle: .degrees(330), clockwise: false)
                ctx.stroke(p, with: .color(color), style: StrokeStyle(lineWidth: min(sz.width, sz.height) * 0.08, lineCap: .round))
            }
        }
        .frame(width: size, height: size)
    }
}

struct ChipIcon: View {
    var color: Color
    var size: CGFloat = 28
    var body: some View {
        Canvas { ctx, sz in
            let m = min(sz.width, sz.height) * 0.22
            ctx.stroke(
                Path(roundedRect: CGRect(x: m, y: m, width: sz.width - 2 * m, height: sz.height - 2 * m), cornerRadius: 6),
                with: .color(color),
                lineWidth: min(sz.width, sz.height) * 0.08
            )
            let pin = min(sz.width, sz.height) * 0.08
            for i in 0..<3 {
                let t = m + (sz.height - 2 * m) * CGFloat(i + 1) / 4
                var l = Path(); l.move(to: CGPoint(x: m - pin, y: t)); l.addLine(to: CGPoint(x: m, y: t))
                var r = Path(); r.move(to: CGPoint(x: sz.width - m, y: t)); r.addLine(to: CGPoint(x: sz.width - m + pin, y: t))
                ctx.stroke(l, with: .color(color), lineWidth: min(sz.width, sz.height) * 0.06)
                ctx.stroke(r, with: .color(color), lineWidth: min(sz.width, sz.height) * 0.06)
            }
        }
        .frame(width: size, height: size)
    }
}

struct GermanFlag: View {
    var size: CGFloat = 48
    var body: some View {
        Canvas { ctx, sz in
            let band = sz.height / 3
            ctx.clip(to: Path(ellipseIn: CGRect(origin: .zero, size: sz)))
            ctx.fill(Path(CGRect(x: 0, y: 0, width: sz.width, height: band)), with: .color(Color(red: 0.18, green: 0.18, blue: 0.19)))
            ctx.fill(Path(CGRect(x: 0, y: band, width: sz.width, height: band)), with: .color(Color(red: 0.72, green: 0.27, blue: 0.24)))
            ctx.fill(Path(CGRect(x: 0, y: 2 * band, width: sz.width, height: band)), with: .color(Color(red: 0.84, green: 0.70, blue: 0.32)))
        }
        .frame(width: size, height: size)
    }
}

struct CzechFlag: View {
    var size: CGFloat = 48
    var body: some View {
        Canvas { ctx, sz in
            ctx.clip(to: Path(ellipseIn: CGRect(origin: .zero, size: sz)))
            ctx.fill(Path(CGRect(origin: .zero, size: sz)), with: .color(Color(white: 0.98)))
            ctx.fill(Path(CGRect(x: 0, y: sz.height / 2, width: sz.width, height: sz.height / 2)), with: .color(Color(red: 0.83, green: 0.16, blue: 0.18)))
            var tri = Path()
            tri.move(to: .zero)
            tri.addLine(to: CGPoint(x: sz.width * 0.55, y: sz.height / 2))
            tri.addLine(to: CGPoint(x: 0, y: sz.height))
            tri.closeSubpath()
            ctx.fill(tri, with: .color(Color(red: 0.07, green: 0.24, blue: 0.55)))
        }
        .frame(width: size, height: size)
    }
}

struct BookIcon: View {
    var color: Color
    var size: CGFloat = 22
    var body: some View {
        Canvas { ctx, sz in
            ctx.stroke(
                Path(roundedRect: CGRect(x: sz.width * 0.22, y: sz.height * 0.18, width: sz.width * 0.56, height: sz.height * 0.64), cornerRadius: 4),
                with: .color(color),
                lineWidth: min(sz.width, sz.height) * 0.08
            )
            var spine = Path()
            spine.move(to: CGPoint(x: sz.width * 0.38, y: sz.height * 0.18))
            spine.addLine(to: CGPoint(x: sz.width * 0.38, y: sz.height * 0.82))
            ctx.stroke(spine, with: .color(color), lineWidth: min(sz.width, sz.height) * 0.08)
        }
        .frame(width: size, height: size)
    }
}

struct QuizIcon: View {
    var color: Color
    var size: CGFloat = 22
    var body: some View {
        Canvas { ctx, sz in
            let r = min(sz.width, sz.height) * 0.36
            ctx.stroke(Path(ellipseIn: CGRect(x: sz.width / 2 - r, y: sz.height / 2 - r, width: r * 2, height: r * 2)), with: .color(color), lineWidth: min(sz.width, sz.height) * 0.08)
            let d = min(sz.width, sz.height) * 0.12
            ctx.fill(Path(ellipseIn: CGRect(x: sz.width / 2 - d / 2, y: sz.height * 0.68 - d / 2, width: d, height: d)), with: .color(color))
            var stem = Path()
            stem.move(to: CGPoint(x: sz.width / 2, y: sz.height * 0.32))
            stem.addLine(to: CGPoint(x: sz.width / 2, y: sz.height * 0.52))
            ctx.stroke(stem, with: .color(color), style: StrokeStyle(lineWidth: min(sz.width, sz.height) * 0.08, lineCap: .round))
        }
        .frame(width: size, height: size)
    }
}

struct OrderIcon: View {
    var color: Color
    var size: CGFloat = 22
    var body: some View {
        Canvas { ctx, sz in
            for i in 0..<3 {
                let y = sz.height * (0.28 + CGFloat(i) * 0.22)
                var p = Path()
                p.move(to: CGPoint(x: sz.width * 0.22, y: y))
                p.addLine(to: CGPoint(x: sz.width * 0.78, y: y))
                ctx.stroke(p, with: .color(color), style: StrokeStyle(lineWidth: min(sz.width, sz.height) * 0.08, lineCap: .round))
            }
        }
        .frame(width: size, height: size)
    }
}

struct GlobeIcon: View {
    var color: Color
    var size: CGFloat = 22
    var body: some View {
        Canvas { ctx, sz in
            let r = min(sz.width, sz.height) * 0.34
            ctx.stroke(Path(ellipseIn: CGRect(x: sz.width / 2 - r, y: sz.height / 2 - r, width: r * 2, height: r * 2)), with: .color(color), lineWidth: min(sz.width, sz.height) * 0.08)
            ctx.stroke(Path(ellipseIn: CGRect(x: sz.width * 0.32, y: sz.height * 0.18, width: sz.width * 0.36, height: sz.height * 0.64)), with: .color(color), lineWidth: min(sz.width, sz.height) * 0.06)
        }
        .frame(width: size, height: size)
    }
}

struct PeopleIcon: View {
    var color: Color
    var size: CGFloat = 22
    var body: some View {
        Canvas { ctx, sz in
            let r = min(sz.width, sz.height) * 0.12
            ctx.fill(Path(ellipseIn: CGRect(x: sz.width * 0.35 - r, y: sz.height * 0.32 - r, width: r * 2, height: r * 2)), with: .color(color))
            ctx.fill(Path(ellipseIn: CGRect(x: sz.width * 0.65 - r, y: sz.height * 0.32 - r, width: r * 2, height: r * 2)), with: .color(color))
            var a = Path(); a.addArc(center: CGPoint(x: sz.width * 0.35, y: sz.height * 0.62), radius: sz.width * 0.17, startAngle: .degrees(200), endAngle: .degrees(340), clockwise: false)
            var b = Path(); b.addArc(center: CGPoint(x: sz.width * 0.65, y: sz.height * 0.62), radius: sz.width * 0.17, startAngle: .degrees(200), endAngle: .degrees(340), clockwise: false)
            ctx.stroke(a, with: .color(color), style: StrokeStyle(lineWidth: min(sz.width, sz.height) * 0.08, lineCap: .round))
            ctx.stroke(b, with: .color(color), style: StrokeStyle(lineWidth: min(sz.width, sz.height) * 0.08, lineCap: .round))
        }
        .frame(width: size, height: size)
    }
}

struct BulbIcon: View {
    var color: Color
    var size: CGFloat = 22
    var body: some View {
        Canvas { ctx, sz in
            let r = min(sz.width, sz.height) * 0.22
            ctx.stroke(Path(ellipseIn: CGRect(x: sz.width / 2 - r, y: sz.height * 0.4 - r, width: r * 2, height: r * 2)), with: .color(color), lineWidth: min(sz.width, sz.height) * 0.08)
            ctx.fill(Path(roundedRect: CGRect(x: sz.width * 0.4, y: sz.height * 0.58, width: sz.width * 0.2, height: sz.height * 0.2), cornerRadius: 3), with: .color(color))
        }
        .frame(width: size, height: size)
    }
}

@ViewBuilder
func noteIcon(_ name: String, _ color: Color) -> some View {
    switch name {
    case "globe": GlobeIcon(color: color)
    case "people": PeopleIcon(color: color)
    case "bulb": BulbIcon(color: color)
    case "quiz": QuizIcon(color: color)
    case "order": OrderIcon(color: color)
    default: BookIcon(color: color)
    }
}
