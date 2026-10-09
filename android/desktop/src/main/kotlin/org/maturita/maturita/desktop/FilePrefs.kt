package org.maturita.maturita.desktop

import org.maturita.maturita.platform.Prefs
import java.io.File
import java.nio.file.Files
import java.nio.file.StandardCopyOption
import java.util.Properties

/** Settings and progress in one properties file, rewritten atomically on every change. */
class FilePrefs(private val file: File) : Prefs {
    private val values = Properties()

    init {
        runCatching { file.inputStream().use { values.load(it.reader(Charsets.UTF_8)) } }
    }

    override fun getBoolean(key: String, fallback: Boolean) =
        values.getProperty(key)?.toBooleanStrictOrNull() ?: fallback

    override fun getInt(key: String, fallback: Int) =
        values.getProperty(key)?.toIntOrNull() ?: fallback

    override fun getString(key: String, fallback: String?): String? =
        values.getProperty(key) ?: fallback

    override fun edit(): Prefs.Editor = Editor()

    @Synchronized
    private fun save(changes: Map<String, String>) {
        values.putAll(changes)
        runCatching {
            file.parentFile?.mkdirs()
            val tmp = File(file.path + ".tmp")
            tmp.writer(Charsets.UTF_8).use { values.store(it, null) }
            Files.move(tmp.toPath(), file.toPath(), StandardCopyOption.REPLACE_EXISTING, StandardCopyOption.ATOMIC_MOVE)
        }
    }

    private inner class Editor : Prefs.Editor {
        private val changes = LinkedHashMap<String, String>()
        override fun putBoolean(key: String, value: Boolean) = apply { changes[key] = value.toString() }
        override fun putInt(key: String, value: Int) = apply { changes[key] = value.toString() }
        override fun putString(key: String, value: String) = apply { changes[key] = value }
        override fun apply() = save(changes)
    }
}
