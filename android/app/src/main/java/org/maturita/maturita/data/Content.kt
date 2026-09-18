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
    val mluvnice = raw.arr("mluvnice")
    val books = raw.arr("books")
    val changelog = raw.arr("changelog")

    fun germanUnit(id: Int) = german.firstOrNull { it.int("id") == id }
    fun netLesson(id: Int) = netLessons.firstOrNull { it.int("id") == id }
    fun hwLesson(id: Int) = hw.firstOrNull { it.int("id") == id }
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
