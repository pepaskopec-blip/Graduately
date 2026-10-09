package org.maturita.maturita.data

import org.maturita.maturita.platform.Prefs

class ProgressStore(private val prefs: Prefs, content: Content) {
    private val ids = content.progressKeys

    init {
        migrate(content.progressLegacy)
    }

    /** Progress is stored under each lesson's id from content/, so lessons can be
     *  reordered or renamed without losing it. `address` is the in-app position. */
    private fun key(address: String) = "done." + (ids[address] ?: address)
    private fun done(address: String) = prefs.getBoolean(key(address), false)
    private fun mark(address: String) { prefs.edit().putBoolean(key(address), true).apply() }

    private fun migrate(legacy: Map<String, String>) {
        if (prefs.getBoolean("progress_ids_v2", false)) return
        val edit = prefs.edit()
        for ((old, id) in legacy) {
            if (prefs.getBoolean(old, false)) edit.putBoolean("done.$id", true)
        }
        edit.putBoolean("progress_ids_v2", true).apply()
    }

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

    fun germanDone(unit: Int, ex: Int) = done("g.$unit.$ex")
    fun markGerman(unit: Int, ex: Int) { mark("g.$unit.$ex") }
    fun vocabDone(unit: Int) = done("g.$unit.vocab")
    fun markVocab(unit: Int) { mark("g.$unit.vocab") }

    fun netDone(id: Int) = done("net.$id")
    fun markNet(id: Int) { mark("net.$id") }

    fun hwDone(id: Int) = done("hw.$id")
    fun markHw(id: Int) { mark("hw.$id") }

    fun onDone(id: Int) = done("on.$id")
    fun markOn(id: Int) { mark("on.$id") }

    fun on2Done(id: Int) = done("on2.$id")
    fun markOn2(id: Int) { mark("on2.$id") }

    fun on3Done(id: Int) = done("on3.$id")
    fun markOn3(id: Int) { mark("on3.$id") }

    fun on4Done(id: Int) = done("on4.$id")
    fun markOn4(id: Int) { mark("on4.$id") }

    fun enDone(year: Int, id: Int) = done("en.$year.$id")
    fun markEn(year: Int, id: Int) { mark("en.$year.$id") }
    fun deDone(year: Int, id: Int) = done("de.$year.$id")
    fun markDe(year: Int, id: Int) { mark("de.$year.$id") }
    fun courseDone(course: String, id: Int) = done("$course.$id")
    fun markCourse(course: String, id: Int) { mark("$course.$id") }

    fun litDone(id: Int) = done("lit.$id")
    fun markLit(id: Int) { mark("lit.$id") }
    fun lit2Done(id: Int) = done("lit2.$id")
    fun markLit2(id: Int) { mark("lit2.$id") }
    fun lit3Done(id: Int) = done("lit3.$id")
    fun markLit3(id: Int) { mark("lit3.$id") }
    fun lit4Done(id: Int) = done("lit4.$id")
    fun markLit4(id: Int) { mark("lit4.$id") }

    fun chemDone(id: Int) = done("chem.$id")
    fun markChem(id: Int) { mark("chem.$id") }
    fun bioDone(id: Int) = done("bio.$id")
    fun markBio(id: Int) { mark("bio.$id") }
    fun fyzDone(id: Int) = done("fyz.$id")
    fun markFyz(id: Int) { mark("fyz.$id") }
    fun fyz2Done(id: Int) = done("fyz2.$id")
    fun markFyz2(id: Int) { mark("fyz2.$id") }
    fun fyz3Done(id: Int) = done("fyz3.$id")
    fun markFyz3(id: Int) { mark("fyz3.$id") }
    fun fyz4Done(id: Int) = done("fyz4.$id")
    fun markFyz4(id: Int) { mark("fyz4.$id") }
    fun mat0Done(id: Int) = done("mat0.$id")
    fun markMat0(id: Int) { mark("mat0.$id") }
    fun matDone(id: Int) = done("mat.$id")
    fun markMat(id: Int) { mark("mat.$id") }
    fun mat2Done(id: Int) = done("mat2.$id")
    fun markMat2(id: Int) { mark("mat2.$id") }
    fun mat3Done(id: Int) = done("mat3.$id")
    fun markMat3(id: Int) { mark("mat3.$id") }
    fun mat4Done(id: Int) = done("mat4.$id")
    fun markMat4(id: Int) { mark("mat4.$id") }

    fun mluvDone(n: Int) = done("mluv.$n")
    fun markMluv(n: Int) { mark("mluv.$n") }

    fun bookQuiz(id: String) = done("book.$id.quiz")
    fun markBookQuiz(id: String) { mark("book.$id.quiz") }
    fun bookPlot(id: String) = done("book.$id.plot")
    fun markBookPlot(id: String) { mark("book.$id.plot") }
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
    listOf("net2", "net3", "net4", "hw2", "hw3", "hw4").forEach { course ->
        content.itLessons(course).forEach { l ->
            sum.totalEx += 1
            sum.openUnits += 1
            if (p.courseDone(course, l.int("id"))) {
                sum.doneEx += 1
                sum.doneUnits += 1
            }
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
    for (year in 1..4) {
        content.enYear(year).forEach { l ->
            sum.totalEx += 1
            sum.openUnits += 1
            if (p.enDone(year, l.int("id"))) {
                sum.doneEx += 1
                sum.doneUnits += 1
            }
        }
        content.deYear(year).forEach { l ->
            sum.totalEx += 1
            sum.openUnits += 1
            if (p.deDone(year, l.int("id"))) {
                sum.doneEx += 1
                sum.doneUnits += 1
            }
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
