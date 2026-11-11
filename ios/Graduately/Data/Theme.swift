import CoreText
import SwiftUI
#if os(iOS)
import UIKit
#endif

enum ColorMode {
    case dark, light
}

enum UiLang {
    case cs, en
}

/// Fill, its label colour, a tint that stays readable as text on paper, and a
/// pale wash of the same hue. The brand has one, and so does every subject.
struct Accent {
    let solid: Color
    let onSolid: Color
    let text: Color
    let soft: Color
}

/// One design in two modes: warm paper and ink, with the orange of the app icon.
struct Palette {
    let paper, surface, sunken, line: Color
    let ink, ink2, ink3: Color
    let accent: Accent
    let success, successSoft, error, errorSoft, warning, warningSoft: Color
    let glowInner, glowOuter, onGlow: Color
}

private struct Tone {
    let paper, surface, ink: UInt32
    let dark: Bool
}

private let lightTone = Tone(paper: 0xFAF5EC, surface: 0xFFFFFF, ink: 0x17130F, dark: false)
private let darkTone = Tone(paper: 0x131110, surface: 0x1E1B18, ink: 0xF5EEE3, dark: true)

private func hx(_ v: UInt32) -> Color {
    Color(
        red: Double((v >> 16) & 0xFF) / 255,
        green: Double((v >> 8) & 0xFF) / 255,
        blue: Double(v & 0xFF) / 255
    )
}

private func luminance(_ v: UInt32) -> Double {
    func channel(_ c: UInt32) -> Double {
        let s = Double(c) / 255
        return s <= 0.03928 ? s / 12.92 : pow((s + 0.055) / 1.055, 2.4)
    }
    return 0.2126 * channel((v >> 16) & 0xFF) + 0.7152 * channel((v >> 8) & 0xFF) + 0.0722 * channel(v & 0xFF)
}

private func contrast(_ a: UInt32, _ b: UInt32) -> Double {
    let la = luminance(a), lb = luminance(b)
    return (max(la, lb) + 0.05) / (min(la, lb) + 0.05)
}

private func mix(_ a: UInt32, _ b: UInt32, _ t: Double) -> UInt32 {
    func channel(_ shift: UInt32) -> UInt32 {
        let x = Double((a >> shift) & 0xFF), y = Double((b >> shift) & 0xFF)
        return UInt32((x + (y - x) * t).rounded())
    }
    return channel(16) << 16 | channel(8) << 8 | channel(0)
}

/// Amber and olive are fine as fills but unreadable as text on paper, so the
/// text tint is the same hue pushed towards the ink until it reaches 4.5 : 1.
private func readable(_ hex: UInt32, on tone: Tone) -> UInt32 {
    var t = 0.0
    while t < 1, contrast(mix(hex, tone.ink, t), tone.paper) < 4.5 { t += 0.05 }
    return mix(hex, tone.ink, t)
}

private func makeAccent(_ hex: UInt32, _ tone: Tone) -> Accent {
    Accent(
        solid: hx(hex),
        onSolid: contrast(hex, 0xFFFFFF) >= 3 ? .white : hx(0x1F1503),
        text: hx(readable(hex, on: tone)),
        soft: hx(mix(hex, tone.surface, tone.dark ? 0.78 : 0.88))
    )
}

func themePalette(_ mode: ColorMode) -> Palette {
    mode == .light
        ? Palette(
            paper: hx(lightTone.paper), surface: hx(lightTone.surface), sunken: hx(0xF1E9DC), line: hx(0xE4DACB),
            ink: hx(lightTone.ink), ink2: hx(0x5E564C), ink3: hx(0x8F8577),
            accent: makeAccent(0xD9612A, lightTone),
            success: hx(0x1E8A4C), successSoft: hx(0xDDF1E3),
            error: hx(0xC8352B), errorSoft: hx(0xFBE0DC),
            warning: hx(0x9A6700), warningSoft: hx(0xFBEBC8),
            glowInner: hx(0xFFF6E8), glowOuter: hx(0xE8935A), onGlow: hx(0x17130F)
        )
        : Palette(
            paper: hx(darkTone.paper), surface: hx(darkTone.surface), sunken: hx(0x2A2622), line: hx(0x37322D),
            ink: hx(darkTone.ink), ink2: hx(0xB5AA9B), ink3: hx(0x80776B),
            accent: makeAccent(0xF0854A, darkTone),
            success: hx(0x4CC27E), successSoft: hx(0x173524),
            error: hx(0xFF6B5E), errorSoft: hx(0x3D1A17),
            warning: hx(0xF2B544), warningSoft: hx(0x3A2C10),
            glowInner: hx(0x6A3A1E), glowOuter: hx(0x1E1B18), onGlow: hx(0xF5EEE3)
        )
}

/// A subject keeps one colour everywhere: its cover on the home screen, the
/// path through its lessons and the buttons in its exercises.
struct Subject {
    /// The `Ikona` column of content/aplikace/predmety.md.
    let key: String
    /// Timetable abbreviation, shown as the cover's watermark.
    let cs, en: String
    let symbol: String
    let hex: UInt32

    func accent(_ mode: ColorMode) -> Accent {
        makeAccent(hex, mode == .light ? lightTone : darkTone)
    }

    func mark(_ lang: UiLang) -> String { lang == .en ? en : cs }
}

let subjects: [Subject] = [
    Subject(key: "de", cs: "NJ", en: "DE", symbol: "character.book.closed.fill", hex: 0xE5A50A),
    Subject(key: "wifi", cs: "PS", en: "NET", symbol: "wifi", hex: 0x2E74DE),
    Subject(key: "chip", cs: "HW", en: "HW", symbol: "cpu", hex: 0x56616F),
    Subject(key: "cz", cs: "ČJ", en: "CZ", symbol: "text.book.closed.fill", hex: 0x3848B8),
    Subject(key: "people", cs: "ON", en: "CIV", symbol: "person.2.fill", hex: 0x9A3FA8),
    Subject(key: "uk", cs: "AJ", en: "EN", symbol: "quote.bubble.fill", hex: 0xC4314B),
    Subject(key: "function", cs: "MAT", en: "MAT", symbol: "function", hex: 0x7345D8),
    Subject(key: "bolt", cs: "FYZ", en: "PHY", symbol: "bolt.fill", hex: 0xE2681A),
    Subject(key: "flask", cs: "ZPV", en: "SCI", symbol: "flask.fill", hex: 0x12908A),
    Subject(key: "ruler", cs: "TG", en: "TG", symbol: "ruler.fill", hex: 0x1787B5),
    Subject(key: "brush", cs: "PG", en: "PG", symbol: "paintbrush.fill", hex: 0xD03A80),
    Subject(key: "code", cs: "PRG", en: "PRG", symbol: "chevron.left.forwardslash.chevron.right", hex: 0x1E8E5A),
    Subject(key: "apps", cs: "PV", en: "SW", symbol: "square.grid.2x2.fill", hex: 0x7A9A1E),
]

func subjectStyle(_ key: String) -> Subject? {
    subjects.first { $0.key == key }
}

extension Route {
    /// The subject a screen belongs to, read from the root of its navigation stack.
    var subject: Subject? {
        switch stack.first {
        case .deHome: return subjectStyle("de")
        case .netYears: return subjectStyle("wifi")
        case .hwYears: return subjectStyle("chip")
        case .czechMap: return subjectStyle("cz")
        case .onYears: return subjectStyle("people")
        case .enYears: return subjectStyle("uk")
        case .matYears: return subjectStyle("function")
        case .fyzYears: return subjectStyle("bolt")
        case .sciMap: return subjectStyle("flask")
        case .tgYears: return subjectStyle("ruler")
        case .pgYears: return subjectStyle("brush")
        case .csYears: return subjectStyle("code")
        case .swYears: return subjectStyle("apps")
        default: return nil
        }
    }
}

/// Playfair Display, the serif of the app icon, for titles and numerals.
/// Body text stays in the system font.
enum Display {
    private static let registered: Bool = {
        for name in ["PlayfairDisplay", "PlayfairDisplay-Italic"] {
            guard let url = Bundle.main.url(forResource: name, withExtension: "ttf") else { return false }
            CTFontManagerRegisterFontsForURL(url as CFURL, .process, nil)
        }
        return true
    }()

    private static var cache: [String: Font] = [:]

    /// Bold roman, or black italic as on the icon. Playfair's default figures
    /// are old-style, where a 1 reads as a small I, so lining figures are on.
    static func font(_ size: CGFloat, italic: Bool = false) -> Font {
        guard registered else {
            let system = Font.system(size: size, weight: italic ? .black : .bold, design: .serif)
            return italic ? system.italic() : system
        }
        let name = italic ? "PlayfairDisplayItalic-Black" : "PlayfairDisplayRoman-Bold"
        let key = "\(name)/\(size)"
        if let font = cache[key] { return font }
        let font = Font(coreText(name, size))
        cache[key] = font
        return font
    }

    private static func coreText(_ name: String, _ size: CGFloat) -> CTFont {
        let lining: [CFString: Any] = [kCTFontOpenTypeFeatureTag: "lnum", kCTFontOpenTypeFeatureValue: 1]
        let descriptor = CTFontDescriptorCreateWithAttributes([
            kCTFontNameAttribute: name,
            kCTFontFeatureSettingsAttribute: [lining],
        ] as CFDictionary)
        return CTFontCreateWithFontDescriptor(descriptor, size, nil)
    }

    #if os(iOS)
    /// Navigation bar titles on the iPhone are set in the display serif too.
    static func styleNavigationBars() {
        guard registered else { return }
        func scaled(_ size: CGFloat, _ style: UIFont.TextStyle) -> UIFont {
            UIFontMetrics(forTextStyle: style).scaledFont(for: coreText("PlayfairDisplayRoman-Bold", size) as UIFont)
        }
        UINavigationBar.appearance().largeTitleTextAttributes = [.font: scaled(32, .largeTitle)]
        UINavigationBar.appearance().titleTextAttributes = [.font: scaled(18, .headline)]
    }
    #endif
}

/// Applies the display serif at a size that follows Dynamic Type.
private struct DisplayType<Inner: View>: View {
    @ScaledMetric private var size: CGFloat
    private let italic: Bool
    private let inner: Inner

    init(size: CGFloat, relativeTo style: Font.TextStyle, italic: Bool, inner: Inner) {
        _size = ScaledMetric(wrappedValue: size, relativeTo: style)
        self.italic = italic
        self.inner = inner
    }

    var body: some View {
        inner.font(Display.font(size, italic: italic))
    }
}

extension View {
    func display(_ size: CGFloat, relativeTo style: Font.TextStyle = .title) -> some View {
        DisplayType(size: size, relativeTo: style, italic: false, inner: self)
    }

    func displayItalic(_ size: CGFloat, relativeTo style: Font.TextStyle = .largeTitle) -> some View {
        DisplayType(size: size, relativeTo: style, italic: true, inner: self)
    }
}

private struct PaletteKey: EnvironmentKey {
    static let defaultValue = themePalette(.light)
}

private struct AccentKey: EnvironmentKey {
    static let defaultValue = themePalette(.light).accent
}

private struct PageKickerKey: EnvironmentKey {
    static let defaultValue: String? = nil
}

extension EnvironmentValues {
    var palette: Palette {
        get { self[PaletteKey.self] }
        set { self[PaletteKey.self] = newValue }
    }

    var accent: Accent {
        get { self[AccentKey.self] }
        set { self[AccentKey.self] = newValue }
    }

    /// Name of the subject a pushed screen belongs to, shown above its title.
    var pageKicker: String? {
        get { self[PageKickerKey.self] }
        set { self[PageKickerKey.self] = newValue }
    }
}

extension View {
    /// Colours everything below: system controls through the tint, custom ones through the environment.
    func accented(_ accent: Accent) -> some View {
        environment(\.accent, accent).tint(accent.text)
    }
}
