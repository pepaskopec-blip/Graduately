package org.maturita.maturita

import org.json.JSONArray
import org.json.JSONObject
import org.maturita.maturita.data.answerAccepts
import org.maturita.maturita.data.answerIncomplete
import org.maturita.maturita.data.hasUmlaut
import org.maturita.maturita.data.mathAnswerOk
import org.maturita.maturita.data.mathDrawGiven
import org.maturita.maturita.data.mathDrawNeed
import org.maturita.maturita.data.mathDrawOk
import org.maturita.maturita.data.mathIsDraw
import org.maturita.maturita.data.netTxtEq
import org.maturita.maturita.data.normalizeAnswer
import java.io.File
import kotlin.test.Test
import kotlin.test.assertEquals
import kotlin.test.assertTrue

/** Runs tests/answers.json, the cases the Swift apps check too (tests/swift). */
class AnswersTest {
    private val cases = JSONObject(File(System.getProperty("graduately.repo"), "tests/answers.json").readText())

    private fun rows(name: String): List<JSONArray> =
        cases.getJSONArray(name).let { a -> (0 until a.length()).map { a.getJSONArray(it) } }

    private fun JSONArray.str(i: Int): String? = if (isNull(i)) null else getString(i)

    private fun JSONArray.points(i: Int): List<Pair<Int, Int>> =
        getJSONArray(i).let { a -> (0 until a.length()).map { a.getJSONArray(it).let { p -> p.getInt(0) to p.getInt(1) } } }

    @Test
    fun normalize() = rows("normalize").forEach { assertEquals(it.str(1), normalizeAnswer(it.str(0)), "normalize ${it.str(0)}") }

    @Test
    fun accepts() = rows("accepts").forEach {
        assertEquals(it.getBoolean(2), answerAccepts(normalizeAnswer(it.str(0)), it.str(1)), "accepts $it")
    }

    @Test
    fun incomplete() = rows("incomplete").forEach {
        assertEquals(it.getBoolean(2), answerIncomplete(normalizeAnswer(it.str(0)), it.str(1)), "incomplete $it")
    }

    @Test
    fun netText() = rows("netTxtEq").forEach { assertEquals(it.getBoolean(2), netTxtEq(it.str(0), it.str(1)), "netTxtEq $it") }

    @Test
    fun umlaut() = rows("hasUmlaut").forEach { assertEquals(it.getBoolean(1), hasUmlaut(it.str(0)), "hasUmlaut $it") }

    @Test
    fun math() = rows("math").forEach { assertEquals(it.getBoolean(2), mathAnswerOk(it.str(0), it.str(1)), "math $it") }

    @Test
    fun draw() {
        rows("drawNeed").forEach { assertEquals(it.getInt(1), mathDrawNeed(it.str(0)), "drawNeed $it") }
        rows("drawGiven").forEach { assertEquals(it.points(1), mathDrawGiven(it.str(0)), "drawGiven $it") }
        rows("draw").forEach { assertEquals(it.getBoolean(2), mathDrawOk(it.str(0), it.points(1)), "draw $it") }
    }

    /** Every worked answer in content/ must pass its own check, or students could never solve it. */
    @Test
    fun contentAnswersSolveThemselves() {
        val root = JSONObject(javaClass.classLoader.getResource("content.json")!!.readText())
        var checked = 0
        for (section in listOf("mat0", "mat", "mat2", "mat3", "mat4")) {
            val lessons = root.getJSONArray(section)
            for (l in 0 until lessons.length()) {
                val lesson = lessons.getJSONObject(l)
                val problems = lesson.getJSONArray("problems")
                for (p in 0 until problems.length()) {
                    val spec = problems.getJSONObject(p).getString("answer")
                    val where = "$section ${lesson.getString("key")}: $spec"
                    if (mathIsDraw(spec)) {
                        assertTrue(mathDrawNeed(spec) > 0, where)
                    } else {
                        val body = spec.substringAfter(':').substringBefore('|')
                        assertTrue(mathAnswerOk(spec, body), where)
                    }
                    checked++
                }
            }
        }
        assertTrue(checked > 100, "only $checked problems found")
    }
}
