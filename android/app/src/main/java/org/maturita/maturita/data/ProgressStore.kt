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

    fun onDone(id: Int) = prefs.getBoolean("on.$id", false)
    fun markOn(id: Int) { prefs.edit().putBoolean("on.$id", true).apply() }

    fun on2Done(id: Int) = prefs.getBoolean("on2.$id", false)
    fun markOn2(id: Int) { prefs.edit().putBoolean("on2.$id", true).apply() }

    fun on3Done(id: Int) = prefs.getBoolean("on3.$id", false)
    fun markOn3(id: Int) { prefs.edit().putBoolean("on3.$id", true).apply() }

    fun on4Done(id: Int) = prefs.getBoolean("on4.$id", false)
    fun markOn4(id: Int) { prefs.edit().putBoolean("on4.$id", true).apply() }

    fun litDone(id: Int) = prefs.getBoolean("lit.$id", false)
    fun markLit(id: Int) { prefs.edit().putBoolean("lit.$id", true).apply() }
    fun lit2Done(id: Int) = prefs.getBoolean("lit2.$id", false)
    fun markLit2(id: Int) { prefs.edit().putBoolean("lit2.$id", true).apply() }
    fun lit3Done(id: Int) = prefs.getBoolean("lit3.$id", false)
    fun markLit3(id: Int) { prefs.edit().putBoolean("lit3.$id", true).apply() }
    fun lit4Done(id: Int) = prefs.getBoolean("lit4.$id", false)
    fun markLit4(id: Int) { prefs.edit().putBoolean("lit4.$id", true).apply() }

    fun chemDone(id: Int) = prefs.getBoolean("chem.$id", false)
    fun markChem(id: Int) { prefs.edit().putBoolean("chem.$id", true).apply() }
    fun bioDone(id: Int) = prefs.getBoolean("bio.$id", false)
    fun markBio(id: Int) { prefs.edit().putBoolean("bio.$id", true).apply() }
    fun fyzDone(id: Int) = prefs.getBoolean("fyz.$id", false)
    fun markFyz(id: Int) { prefs.edit().putBoolean("fyz.$id", true).apply() }
    fun fyz2Done(id: Int) = prefs.getBoolean("fyz2.$id", false)
    fun markFyz2(id: Int) { prefs.edit().putBoolean("fyz2.$id", true).apply() }
    fun fyz3Done(id: Int) = prefs.getBoolean("fyz3.$id", false)
    fun markFyz3(id: Int) { prefs.edit().putBoolean("fyz3.$id", true).apply() }
    fun fyz4Done(id: Int) = prefs.getBoolean("fyz4.$id", false)
    fun markFyz4(id: Int) { prefs.edit().putBoolean("fyz4.$id", true).apply() }
    fun mat0Done(id: Int) = prefs.getBoolean("mat0.$id", false)
    fun markMat0(id: Int) { prefs.edit().putBoolean("mat0.$id", true).apply() }
    fun matDone(id: Int) = prefs.getBoolean("mat.$id", false)
    fun markMat(id: Int) { prefs.edit().putBoolean("mat.$id", true).apply() }
    fun mat2Done(id: Int) = prefs.getBoolean("mat2.$id", false)
    fun markMat2(id: Int) { prefs.edit().putBoolean("mat2.$id", true).apply() }
    fun mat3Done(id: Int) = prefs.getBoolean("mat3.$id", false)
    fun markMat3(id: Int) { prefs.edit().putBoolean("mat3.$id", true).apply() }
    fun mat4Done(id: Int) = prefs.getBoolean("mat4.$id", false)
    fun markMat4(id: Int) { prefs.edit().putBoolean("mat4.$id", true).apply() }

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
    content.on.forEach { l ->
        sum.totalEx += 1
        sum.openUnits += 1
        if (p.onDone(l.int("id"))) {
            sum.doneEx += 1
            sum.doneUnits += 1
        }
    }
    content.on2.forEach { l ->
        sum.totalEx += 1
        sum.openUnits += 1
        if (p.on2Done(l.int("id"))) {
            sum.doneEx += 1
            sum.doneUnits += 1
        }
    }
    content.on3.forEach { l ->
        sum.totalEx += 1
        sum.openUnits += 1
        if (p.on3Done(l.int("id"))) {
            sum.doneEx += 1
            sum.doneUnits += 1
        }
    }
    content.on4.forEach { l ->
        sum.totalEx += 1
        sum.openUnits += 1
        if (p.on4Done(l.int("id"))) {
            sum.doneEx += 1
            sum.doneUnits += 1
        }
    }
    content.lit.forEach { l ->
        sum.totalEx += 1
        sum.openUnits += 1
        if (p.litDone(l.int("id"))) {
            sum.doneEx += 1
            sum.doneUnits += 1
        }
    }
    content.lit2.forEach { l ->
        sum.totalEx += 1
        sum.openUnits += 1
        if (p.lit2Done(l.int("id"))) {
            sum.doneEx += 1
            sum.doneUnits += 1
        }
    }
    content.lit3.forEach { l ->
        sum.totalEx += 1
        sum.openUnits += 1
        if (p.lit3Done(l.int("id"))) {
            sum.doneEx += 1
            sum.doneUnits += 1
        }
    }
    content.lit4.forEach { l ->
        sum.totalEx += 1
        sum.openUnits += 1
        if (p.lit4Done(l.int("id"))) {
            sum.doneEx += 1
            sum.doneUnits += 1
        }
    }
    content.chem.forEach { l ->
        sum.totalEx += 1
        sum.openUnits += 1
        if (p.chemDone(l.int("id"))) {
            sum.doneEx += 1
            sum.doneUnits += 1
        }
    }
    content.bio.forEach { l ->
        sum.totalEx += 1
        sum.openUnits += 1
        if (p.bioDone(l.int("id"))) {
            sum.doneEx += 1
            sum.doneUnits += 1
        }
    }
    content.fyz.forEach { l ->
        sum.totalEx += 1
        sum.openUnits += 1
        if (p.fyzDone(l.int("id"))) {
            sum.doneEx += 1
            sum.doneUnits += 1
        }
    }
    content.fyz2.forEach { l ->
        sum.totalEx += 1
        sum.openUnits += 1
        if (p.fyz2Done(l.int("id"))) {
            sum.doneEx += 1
            sum.doneUnits += 1
        }
    }
    content.fyz3.forEach { l ->
        sum.totalEx += 1
        sum.openUnits += 1
        if (p.fyz3Done(l.int("id"))) {
            sum.doneEx += 1
            sum.doneUnits += 1
        }
    }
    content.fyz4.forEach { l ->
        sum.totalEx += 1
        sum.openUnits += 1
        if (p.fyz4Done(l.int("id"))) {
            sum.doneEx += 1
            sum.doneUnits += 1
        }
    }
    content.mat0.forEach { l ->
        sum.totalEx += 1
        sum.openUnits += 1
        if (p.mat0Done(l.int("id"))) {
            sum.doneEx += 1
            sum.doneUnits += 1
        }
    }
    content.mat.forEach { l ->
        sum.totalEx += 1
        sum.openUnits += 1
        if (p.matDone(l.int("id"))) {
            sum.doneEx += 1
            sum.doneUnits += 1
        }
    }
    content.mat2.forEach { l ->
        sum.totalEx += 1
        sum.openUnits += 1
        if (p.mat2Done(l.int("id"))) {
            sum.doneEx += 1
            sum.doneUnits += 1
        }
    }
    content.mat3.forEach { l ->
        sum.totalEx += 1
        sum.openUnits += 1
        if (p.mat3Done(l.int("id"))) {
            sum.doneEx += 1
            sum.doneUnits += 1
        }
    }
    content.mat4.forEach { l ->
        sum.totalEx += 1
        sum.openUnits += 1
        if (p.mat4Done(l.int("id"))) {
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
