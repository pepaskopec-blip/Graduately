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
