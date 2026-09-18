package org.maturita.maturita.data

import android.content.Context

class ProgressStore(context: Context) {
    private val prefs = context.getSharedPreferences("maturita", Context.MODE_PRIVATE)

    var themeId: ThemeId
        get() = ThemeId.entries.getOrElse(prefs.getInt("theme", 0)) { ThemeId.Catppuccin }
        set(v) { prefs.edit().putInt("theme", v.ordinal).apply() }

    var mode: ColorMode
        get() = if (prefs.getString("mode", "light") == "dark") ColorMode.Dark else ColorMode.Light
        set(v) { prefs.edit().putString("mode", if (v == ColorMode.Light) "light" else "dark").apply() }

    var lang: UiLang
        get() = if (prefs.getString("lang", "cs") == "en") UiLang.En else UiLang.Cs
        set(v) { prefs.edit().putString("lang", if (v == UiLang.En) "en" else "cs").apply() }

    var seenCommit: String
        get() = prefs.getString("seen_commit", "") ?: ""
        set(v) { prefs.edit().putString("seen_commit", v).apply() }

    fun germanDone(unit: Int, ex: Int) = prefs.getBoolean("g.$unit.$ex", false)
    fun markGerman(unit: Int, ex: Int) { prefs.edit().putBoolean("g.$unit.$ex", true).apply() }
    fun vocabDone(unit: Int) = prefs.getBoolean("g.$unit.vocab", false)
    fun markVocab(unit: Int) { prefs.edit().putBoolean("g.$unit.vocab", true).apply() }

    fun netDone(id: Int) = prefs.getBoolean("net.$id", false)
    fun markNet(id: Int) { prefs.edit().putBoolean("net.$id", true).apply() }

    fun hwDone(id: Int) = prefs.getBoolean("hw.$id", false)
    fun markHw(id: Int) { prefs.edit().putBoolean("hw.$id", true).apply() }

    fun mluvDone(n: Int) = prefs.getBoolean("mluv.$n", false)
    fun markMluv(n: Int) { prefs.edit().putBoolean("mluv.$n", true).apply() }

    fun bookQuiz(id: String) = prefs.getBoolean("book.$id.quiz", false)
    fun markBookQuiz(id: String) { prefs.edit().putBoolean("book.$id.quiz", true).apply() }
    fun bookPlot(id: String) = prefs.getBoolean("book.$id.plot", false)
    fun markBookPlot(id: String) { prefs.edit().putBoolean("book.$id.plot", true).apply() }
}

data class ProgressSum(var doneEx: Int = 0, var totalEx: Int = 0, var doneUnits: Int = 0, var openUnits: Int = 0)

fun summarize(content: Content, p: ProgressStore): ProgressSum {
    val sum = ProgressSum()
    content.german.filter { it.bool("unlocked") }.forEach { u ->
        val id = u.int("id")
        val names = u.strs("names")
        sum.totalEx += names.size
        sum.openUnits += 1
        val done = names.indices.count { p.germanDone(id, it + 1) }
        sum.doneEx += done
        if (names.isNotEmpty() && done == names.size) sum.doneUnits += 1
    }
    content.netLessons.forEach { l ->
        sum.totalEx += 1
        sum.openUnits += 1
        if (p.netDone(l.int("id"))) {
            sum.doneEx += 1
            sum.doneUnits += 1
        }
    }
    content.hw.forEach { l ->
        sum.totalEx += 1
        sum.openUnits += 1
        if (p.hwDone(l.int("id"))) {
            sum.doneEx += 1
            sum.doneUnits += 1
        }
    }
    sum.totalEx += content.mluvnice.size
    sum.openUnits += 1
    val md = content.mluvnice.count { p.mluvDone(it.int("id")) }
    sum.doneEx += md
    if (md == content.mluvnice.size && content.mluvnice.isNotEmpty()) sum.doneUnits += 1
    return sum
}
