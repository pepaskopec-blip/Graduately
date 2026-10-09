package org.maturita.maturita.desktop

import java.io.File

/**
 * How this copy runs, which decides where progress lives and what an update
 * replaces. Progress sits next to the app, as it did in the GTK builds, so
 * existing students keep it: beside the AppImage on Linux, inside the install
 * directory on Windows.
 */
class Install(
    val kind: Kind,
    /** The AppImage file, or the Windows directory holding graduately.exe. */
    val target: File?,
    val progressDir: File,
) {
    enum class Kind { AppImage, WindowsDir, Unsupported }

    companion object {
        val isWindows = System.getProperty("os.name").orEmpty().startsWith("Windows")

        fun detect(): Install {
            val appImage = System.getenv("APPIMAGE")?.takeIf { it.isNotBlank() }?.let(::File)
            val launcher = System.getProperty("jpackage.app-path")?.let(::File)
            val windowsDir = launcher?.parentFile?.takeIf {
                isWindows && launcher.name.equals("graduately.exe", ignoreCase = true) &&
                    File(it, "app").isDirectory && File(it, "runtime").isDirectory
            }
            val (kind, target) = when {
                appImage != null && !isWindows -> Kind.AppImage to appImage
                windowsDir != null -> Kind.WindowsDir to windowsDir
                else -> Kind.Unsupported to null
            }
            val beside = target?.let { if (kind == Kind.AppImage) it.parentFile else it }
            val home = File(System.getProperty("user.home"), ".graduately")
            val root = beside?.takeIf { it.canWrite() } ?: home
            return Install(kind, target, File(root, "progress"))
        }
    }
}
