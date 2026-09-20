#if os(macOS)
import AppKit
import SwiftUI

extension NSColor {
    static var tertiarySystemFill: NSColor {
        NSColor.quaternaryLabelColor.withAlphaComponent(0.16)
    }

    static var secondarySystemGroupedBackground: NSColor {
        .controlBackgroundColor
    }

    static var separator: NSColor { .separatorColor }
}

enum TextInputAutocapitalization {
    case never
}

enum UIKeyboardType {
    case decimalPad
}

enum MacNavBarTitleMode {
    case large
    case inline
}

extension View {
    func textInputAutocapitalization(_ style: TextInputAutocapitalization) -> some View { self }

    func keyboardType(_ type: UIKeyboardType) -> some View { self }

    func navigationBarTitleDisplayMode(_ mode: MacNavBarTitleMode) -> some View { self }
}

#endif
