package org.maturita.maturita.data

import org.json.JSONArray
import org.json.JSONObject

class J(val o: JSONObject) {
    fun has(key: String) = o.has(key) && !o.isNull(key)
    fun str(key: String, fallback: String = "") =
        if (!has(key)) fallback else o.optString(key, fallback)
    fun strOrNull(key: String): String? =
        if (!has(key)) null else o.optString(key).takeIf { it.isNotEmpty() && it != "null" }
    fun bool(key: String, fallback: Boolean = false) = o.optBoolean(key, fallback)
    fun int(key: String, fallback: Int = 0) = o.optInt(key, fallback)
    fun obj(key: String): J? = o.optJSONObject(key)?.let { J(it) }
    fun exercise(n: Int): J? = o.optJSONObject("exercises")?.optJSONObject(n.toString())?.let { J(it) }
    fun arr(key: String): List<J> {
        val a = o.optJSONArray(key) ?: return emptyList()
        return (0 until a.length()).mapNotNull { i -> a.optJSONObject(i)?.let { J(it) } }
    }
    fun strs(key: String): List<String> {
        val a = o.optJSONArray(key) ?: return emptyList()
        return (0 until a.length()).mapNotNull { i ->
            if (a.isNull(i)) null else a.optString(i).takeIf { it.isNotEmpty() }
        }
    }
    fun strsOrEmpty(key: String): List<String?> {
        val a = o.optJSONArray(key) ?: return emptyList()
        return (0 until a.length()).map { i ->
            if (a.isNull(i)) null else a.optString(i)
        }
    }
    fun intRows(key: String): List<List<String>> {
        val a = o.optJSONArray(key) ?: return emptyList()
        return (0 until a.length()).map { i ->
            val row = a.optJSONArray(i) ?: JSONArray()
            (0 until row.length()).map { j -> row.optString(j) }
        }
    }

    fun answerList(): List<String> {
        val a = o.optJSONArray("answers")
        if (a != null) {
            return (0 until a.length()).mapNotNull { i ->
                if (a.isNull(i)) null else a.optString(i).takeIf { it.isNotEmpty() }
            }
        }
        return listOfNotNull(strOrNull("answers"))
    }
}

fun JSONArray.objects(): List<J> =
    (0 until length()).mapNotNull { i -> optJSONObject(i)?.let { J(it) } }
