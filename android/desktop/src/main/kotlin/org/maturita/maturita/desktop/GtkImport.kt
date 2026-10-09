package org.maturita.maturita.desktop

import org.maturita.maturita.data.ThemeId
import org.maturita.maturita.data.themeNames
import org.maturita.maturita.platform.Prefs
import java.io.File

/**
 * The GTK builds kept progress in `progress/<course>.conf` key files. Copy it
 * once into the shared progress keys (`net.3`, `en.2.5`, `g.0.vocab` …); the
 * shared ProgressStore then moves those onto lesson ids like on every
 * platform. The .conf files stay, so nothing is lost if the import misreads.
 */
fun importGtkProgress(dir: File, prefs: Prefs) {
    if (prefs.getBoolean("gtk_imported", false)) return
    val edit = prefs.edit()
    for (file in dir.listFiles { f -> f.name.endsWith(".conf") }.orEmpty()) {
        val course = file.name.removeSuffix(".conf")
        val groups = readKeyFile(file)
        if (course == "settings") {
            groups["ui"]?.let { importSettings(it, edit) }
            continue
        }
        for ((key, value) in groups["done"].orEmpty()) {
            if (value != "true") continue
            gtkAddress(course, key)?.let { edit.putBoolean(it, true) }
        }
    }
    edit.putBoolean("gtk_imported", true).apply()
}

/** GTK file and key to the address the apps use, or null for what has no equivalent. */
internal fun gtkAddress(course: String, key: String): String? {
    val unit = Regex("""unit(\d)""").matchEntire(course)?.groupValues?.get(1)?.toInt()
    return when {
        // The vocabulary branch used the last slot of the unit.
        unit != null -> key.toIntOrNull()?.let { if (it == 20) "g.${unit - 1}.vocab" else "g.${unit - 1}.$it" }
        course == "english" -> yearKey("en", key)
        course == "german-course" -> yearKey("de", key)
        course == "mluvnice" -> key.toIntOrNull()?.let { "mluv.$it" }
        // Book progress was one shared slot, not per book.
        course == "cetba" -> null
        else -> key.toIntOrNull()?.let { "$course.$it" }
    }
}

private fun yearKey(prefix: String, key: String): String? {
    val (year, lesson) = key.split("-").takeIf { it.size == 2 } ?: return null
    if (year.toIntOrNull() == null || lesson.toIntOrNull() == null) return null
    return "$prefix.$year.$lesson"
}

private fun importSettings(ui: Map<String, String>, edit: Prefs.Editor) {
    ui["theme"]?.let { name ->
        val i = themeNames.indexOfFirst { it.equals(name, ignoreCase = true) }
        if (i >= 0) edit.putInt("theme", ThemeId.entries[i].ordinal)
    }
    ui["mode"]?.let { edit.putString("mode", if (it.lowercase() in setOf("light", "white")) "light" else "dark") }
    ui["lang"]?.let { edit.putString("lang", if (it.lowercase() in setOf("en", "english")) "en" else "cs") }
    ui["seen_commit"]?.takeIf { it.isNotBlank() }?.let { edit.putString("seen_commit", it) }
}

internal fun readKeyFile(file: File): Map<String, Map<String, String>> {
    val groups = LinkedHashMap<String, MutableMap<String, String>>()
    var current: MutableMap<String, String>? = null
    runCatching { file.readLines() }.getOrDefault(emptyList()).forEach { raw ->
        val line = raw.trim()
        when {
            line.isEmpty() || line.startsWith("#") -> {}
            line.startsWith("[") && line.endsWith("]") ->
                current = groups.getOrPut(line.substring(1, line.length - 1)) { LinkedHashMap() }
            '=' in line -> current?.put(line.substringBefore('=').trim(), line.substringAfter('=').trim())
        }
    }
    return groups
}

/**
 * The GTK updater copied new packages over the install without deleting
 * anything, so a Windows folder updated from GTK still carries its DLLs and
 * data. style.css marks such a folder; jpackage never writes one.
 */
fun removeGtkLeftovers(install: Install) {
    if (install.kind != Install.Kind.WindowsDir) return
    val dir = install.target ?: return
    val marker = File(dir, "style.css")
    if (!marker.isFile) return
    dir.listFiles().orEmpty().forEach { f ->
        val name = f.name.lowercase()
        val gtkDll = name.endsWith(".dll") && (name.startsWith("lib") || name == "zlib1.dll")
        val stale = (f.isFile && (gtkDll || name == "changelog.txt")) ||
            (f.isDirectory && name in setOf("lib", "share", "icons"))
        if (stale) f.deleteRecursively()
    }
    marker.delete()
}
