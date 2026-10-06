package org.maturita.maturita.data

import android.content.Context
import org.json.JSONObject

class Content(root: JSONObject) {
    val raw = J(root)
    val i18n: Map<String, Pair<String?, String?>> = run {
        val obj = root.optJSONObject("i18n") ?: JSONObject()
        obj.keys().asSequence().associateWith { key ->
            val e = obj.optJSONObject(key)
            val cs = e?.optString("cs")?.takeIf { it.isNotEmpty() && it != "null" }
            val en = e?.optString("en")?.takeIf { it.isNotEmpty() && it != "null" }
            cs to en
        }
    }
    val subjects = raw.arr("subjects")
    val german = raw.arr("german")
    val net = raw.obj("net") ?: J(JSONObject())
    val netLessons = net.arr("lessons")
    val netTasks = net.arr("tasks")
    val netAnswers: List<List<J>> = run {
        val a = net.o.optJSONArray("answers") ?: return@run emptyList()
        (0 until a.length()).map { i ->
            val block = a.optJSONArray(i) ?: org.json.JSONArray()
            (0 until block.length()).mapNotNull { j ->
                block.optJSONObject(j)?.let { J(it) }
            }
        }
    }
    val hw = raw.arr("hw")
    val on = raw.arr("on")
    val on2 = raw.arr("on2")
    val on3 = raw.arr("on3")
    val on4 = raw.arr("on4")
    val lit = raw.arr("lit")
    val lit2 = raw.arr("lit2")
    val lit3 = raw.arr("lit3")
    val lit4 = raw.arr("lit4")
    val chem = raw.arr("chem")
    val bio = raw.arr("bio")
    val fyz = raw.arr("fyz")
    val fyz2 = raw.arr("fyz2")
    val fyz3 = raw.arr("fyz3")
    val fyz4 = raw.arr("fyz4")
    val mluvnice = raw.arr("mluvnice")
    val books = raw.arr("books")
    val changelog = raw.arr("changelog")

    fun germanUnit(id: Int) = german.firstOrNull { it.int("id") == id }
    fun netLesson(id: Int) = netLessons.firstOrNull { it.int("id") == id }
    fun hwLesson(id: Int) = hw.firstOrNull { it.int("id") == id }
    fun onLesson(id: Int) = on.firstOrNull { it.int("id") == id }
    fun on2Lesson(id: Int) = on2.firstOrNull { it.int("id") == id }
    fun on3Lesson(id: Int) = on3.firstOrNull { it.int("id") == id }
    fun on4Lesson(id: Int) = on4.firstOrNull { it.int("id") == id }
    fun litLesson(id: Int) = lit.firstOrNull { it.int("id") == id }
    fun lit2Lesson(id: Int) = lit2.firstOrNull { it.int("id") == id }
    fun lit3Lesson(id: Int) = lit3.firstOrNull { it.int("id") == id }
    fun lit4Lesson(id: Int) = lit4.firstOrNull { it.int("id") == id }
    fun chemLesson(id: Int) = chem.firstOrNull { it.int("id") == id }
    fun bioLesson(id: Int) = bio.firstOrNull { it.int("id") == id }
    fun fyzLesson(id: Int) = fyz.firstOrNull { it.int("id") == id }
    fun fyz2Lesson(id: Int) = fyz2.firstOrNull { it.int("id") == id }
    fun fyz3Lesson(id: Int) = fyz3.firstOrNull { it.int("id") == id }
    fun fyz4Lesson(id: Int) = fyz4.firstOrNull { it.int("id") == id }
    fun mluv(n: Int) = mluvnice.firstOrNull { it.int("id") == n }
    fun book(id: String) = books.firstOrNull { it.str("id") == id }

    fun tr(key: String?, lang: UiLang): String {
        if (key.isNullOrEmpty()) return ""
        val hit = i18n[key]
        return if (lang == UiLang.En) {
            hit?.second ?: key
        } else {
            hit?.first ?: key
        }
    }

    fun fmt(key: String, lang: UiLang, vararg args: Any): String {
        val template = tr(key, lang)
        return try {
            String.format(template, *args)
        } catch (_: Exception) {
            template
        }
    }

    companion object {
        fun load(context: Context): Content {
            return try {
                val text = context.assets.open("content.json").bufferedReader().use { it.readText() }
                Content(JSONObject(text))
            } catch (_: Exception) {
                Content(JSONObject())
            }
        }
    }
}
