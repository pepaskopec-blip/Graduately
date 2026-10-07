import SwiftUI

// Circular flags used as subject glyphs; everything else is an SF Symbol.

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

struct UkFlag: View {
    var size: CGFloat = 48
    var body: some View {
        Canvas { ctx, sz in
            let w = sz.width
            let h = sz.height
            ctx.clip(to: Path(ellipseIn: CGRect(origin: .zero, size: sz)))
            ctx.fill(Path(CGRect(origin: .zero, size: sz)), with: .color(Color(red: 0.01, green: 0.14, blue: 0.40)))
            var saltire = Path()
            saltire.move(to: .zero)
            saltire.addLine(to: CGPoint(x: w, y: h))
            saltire.move(to: CGPoint(x: w, y: 0))
            saltire.addLine(to: CGPoint(x: 0, y: h))
            ctx.stroke(saltire, with: .color(.white), lineWidth: w * 0.16)
            ctx.stroke(saltire, with: .color(Color(red: 0.80, green: 0.05, blue: 0.15)), lineWidth: w * 0.055)
            ctx.fill(Path(CGRect(x: 0, y: h * 0.38, width: w, height: h * 0.24)), with: .color(.white))
            ctx.fill(Path(CGRect(x: w * 0.38, y: 0, width: w * 0.24, height: h)), with: .color(.white))
            ctx.fill(Path(CGRect(x: 0, y: h * 0.43, width: w, height: h * 0.14)), with: .color(Color(red: 0.80, green: 0.05, blue: 0.15)))
            ctx.fill(Path(CGRect(x: w * 0.43, y: 0, width: w * 0.14, height: h)), with: .color(Color(red: 0.80, green: 0.05, blue: 0.15)))
        }
        .frame(width: size, height: size)
    }
}
