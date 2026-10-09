package org.maturita.maturita

import org.json.JSONObject
import org.maturita.maturita.data.ColorMode
import org.maturita.maturita.data.Content
import org.maturita.maturita.data.ProgressStore
import org.maturita.maturita.data.ThemeId
import org.maturita.maturita.desktop.FilePrefs
import org.maturita.maturita.desktop.gtkAddress
import org.maturita.maturita.desktop.importGtkProgress
import java.nio.file.Files
import kotlin.test.Test
import kotlin.test.assertEquals
import kotlin.test.assertFalse
import kotlin.test.assertTrue

class ProgressTest {
    private val content = Content.parse(javaClass.classLoader.getResource("content.json")!!.readText())

    @Test
    fun everyLegacyAddressHasALesson() {
        val root = JSONObject(javaClass.classLoader.getResource("content.json")!!.readText())
        val keys = root.getJSONObject("progress").getJSONObject("keys")
        assertTrue(content.progressLegacy.isNotEmpty())
        for ((old, id) in content.progressLegacy) {
            assertTrue(keys.keySet().any { keys.getString(it) == id }, "$old -> $id has no lesson")
        }
    }

    @Test
    fun oldProgressMovesToLessonIds() {
        val dir = Files.createTempDirectory("progress").toFile()
        val prefs = FilePrefs(dir.resolve("p.properties"))
        prefs.edit().putBoolean("fyz.1", true).putBoolean("g.0.vocab", true).putBoolean("en.2.3", true).apply()

        val store = ProgressStore(prefs, content)
        assertTrue(store.fyzDone(1))
        assertTrue(store.vocabDone(0))
        assertTrue(store.enDone(2, 3))
        assertFalse(store.fyzDone(2))
        assertTrue(prefs.getBoolean("done.fyzika-1-fyzikalni-veliciny", false))

        store.markMat(1)
        assertTrue(FilePrefs(dir.resolve("p.properties")).getBoolean("done.matematika-1-delitelnost-a-prvocisla", false))
    }

    @Test
    fun gtkProgressIsImported() {
        val dir = Files.createTempDirectory("gtk").toFile()
        dir.resolve("net.conf").writeText("[done]\n1=true\n2=false\n")
        dir.resolve("unit1.conf").writeText("[done]\n1=true\n20=true\n")
        dir.resolve("english.conf").writeText("[done]\n2-3=true\n")
        dir.resolve("settings.conf").writeText("[ui]\ntheme=Nord\nmode=dark\nlang=en\nseen_commit=abcdef1\n")
        val prefs = FilePrefs(dir.resolve("graduately.properties"))

        importGtkProgress(dir, prefs)
        val store = ProgressStore(prefs, content)

        assertTrue(store.netDone(1))
        assertFalse(store.netDone(2))
        assertTrue(store.germanDone(0, 1))
        assertTrue(store.vocabDone(0))
        assertTrue(store.enDone(2, 3))
        assertEquals(ThemeId.Nord, store.themeId)
        assertEquals(ColorMode.Dark, store.mode)
        assertEquals("abcdef1", store.seenCommit)
    }

    @Test
    fun gtkAddresses() {
        assertEquals("g.2.5", gtkAddress("unit3", "5"))
        assertEquals("de.4.6", gtkAddress("german-course", "4-6"))
        assertEquals("mluv.7", gtkAddress("mluvnice", "7"))
        assertEquals("hw3.2", gtkAddress("hw3", "2"))
        assertEquals(null, gtkAddress("cetba", "1"))
    }
}
