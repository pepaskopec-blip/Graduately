package org.maturita.maturita.desktop

import org.maturita.maturita.platform.Platform
import org.maturita.maturita.platform.Prefs
import org.maturita.maturita.platform.Speaker

class DesktopPlatform(
    override val commit: String,
    override val prefs: Prefs,
    install: Install,
) : Platform {
    override val versionName: String = if (Install.isWindows) "windows" else "linux"
    override val welcomeKey = "welcome_body_desktop"
    override val updater = DesktopUpdater(commit, install)

    override fun speaker(): Speaker? = DesktopSpeaker.create()
}
