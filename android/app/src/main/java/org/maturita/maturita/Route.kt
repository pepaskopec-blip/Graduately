package org.maturita.maturita

sealed class Route(val page: String) {
    data object Welcome : Route("welcome")
    data object Subjects : Route("subjects")
    data object Stats : Route("stats")
    data object Settings : Route("settings")
    data object Roadmap : Route("roadmap")
    data object DeHome : Route("dehome")
    data class DeMap(val year: Int) : Route("de${year}map")
    data class DeLesson(val year: Int, val id: Int) : Route("de${year}unit$id")
    data class DeEx(val year: Int, val id: Int) : Route("de${year}ex$id")
    data class UnitMap(val unitId: Int) : Route("unit${unitId + 1}")
    data class GermanEx(val unitId: Int, val ex: Int) : Route("u${unitId + 1}e$ex")
    data class Vocab(val unitId: Int) : Route("u${unitId + 1}vocab")
    data object NetYears : Route("netyears")
    data object NetMap : Route("netmap")
    data class NetLesson(val id: Int) : Route("netunit$id")
    data class NetEx(val id: Int) : Route("netex$id")
    data object HwYears : Route("hwyears")
    data object HwMap : Route("hwmap")
    data class HwLesson(val id: Int) : Route("hwunit$id")
    data class HwEx(val id: Int) : Route("hwex$id")
    data object OnYears : Route("onyears")
    data object OnMap : Route("onmap")
    data class OnLesson(val id: Int) : Route("onunit$id")
    data class OnEx(val id: Int) : Route("onex$id")
    data object On2Map : Route("on2map")
    data class On2Lesson(val id: Int) : Route("on2unit$id")
    data class On2Ex(val id: Int) : Route("on2ex$id")
    data object On3Map : Route("on3map")
    data class On3Lesson(val id: Int) : Route("on3unit$id")
    data class On3Ex(val id: Int) : Route("on3ex$id")
    data object On4Map : Route("on4map")
    data class On4Lesson(val id: Int) : Route("on4unit$id")
    data class On4Ex(val id: Int) : Route("on4ex$id")
    data object EnYears : Route("enyears")
    data class EnMap(val year: Int) : Route("en${year}map")
    data class EnLesson(val year: Int, val id: Int) : Route("en${year}unit$id")
    data class EnEx(val year: Int, val id: Int) : Route("en${year}ex$id")
    data object LitYears : Route("lityears")
    data object LitMap : Route("litmap")
    data class LitLesson(val id: Int) : Route("litunit$id")
    data class LitEx(val id: Int) : Route("litex$id")
    data object Lit2Map : Route("lit2map")
    data class Lit2Lesson(val id: Int) : Route("lit2unit$id")
    data class Lit2Ex(val id: Int) : Route("lit2ex$id")
    data object Lit3Map : Route("lit3map")
    data class Lit3Lesson(val id: Int) : Route("lit3unit$id")
    data class Lit3Ex(val id: Int) : Route("lit3ex$id")
    data object Lit4Map : Route("lit4map")
    data class Lit4Lesson(val id: Int) : Route("lit4unit$id")
    data class Lit4Ex(val id: Int) : Route("lit4ex$id")
    data object SciMap : Route("scimap")
    data object ChemMap : Route("chemmap")
    data class ChemLesson(val id: Int) : Route("chemunit$id")
    data class ChemEx(val id: Int) : Route("chemex$id")
    data object BioMap : Route("biomap")
    data class BioLesson(val id: Int) : Route("biounit$id")
    data class BioEx(val id: Int) : Route("bioex$id")
    data object MatYears : Route("matyears")
    data object Mat0Map : Route("mat0map")
    data class Mat0Lesson(val id: Int) : Route("mat0unit$id")
    data class Mat0Ex(val id: Int) : Route("mat0ex$id")
    data object MatMap : Route("matmap")
    data class MatLesson(val id: Int) : Route("matunit$id")
    data class MatEx(val id: Int) : Route("matex$id")
    data object Mat2Map : Route("mat2map")
    data class Mat2Lesson(val id: Int) : Route("mat2unit$id")
    data class Mat2Ex(val id: Int) : Route("mat2ex$id")
    data object Mat3Map : Route("mat3map")
    data class Mat3Lesson(val id: Int) : Route("mat3unit$id")
    data class Mat3Ex(val id: Int) : Route("mat3ex$id")
    data object Mat4Map : Route("mat4map")
    data class Mat4Lesson(val id: Int) : Route("mat4unit$id")
    data class Mat4Ex(val id: Int) : Route("mat4ex$id")
    data object FyzYears : Route("fyzyears")
    data object FyzMap : Route("fyzmap")
    data class FyzLesson(val id: Int) : Route("fyzunit$id")
    data class FyzEx(val id: Int) : Route("fyzex$id")
    data object Fyz2Map : Route("fyz2map")
    data class Fyz2Lesson(val id: Int) : Route("fyz2unit$id")
    data class Fyz2Ex(val id: Int) : Route("fyz2ex$id")
    data object Fyz3Map : Route("fyz3map")
    data class Fyz3Lesson(val id: Int) : Route("fyz3unit$id")
    data class Fyz3Ex(val id: Int) : Route("fyz3ex$id")
    data object Fyz4Map : Route("fyz4map")
    data class Fyz4Lesson(val id: Int) : Route("fyz4unit$id")
    data class Fyz4Ex(val id: Int) : Route("fyz4ex$id")
    data object CzechMap : Route("czechmap")
    data object Mluvnice : Route("mluvnice")
    data class MluvEx(val n: Int) : Route("mluve$n")
    data object ReadingList : Route("readinglist")
    data class Book(val id: String) : Route(if (id == "1984") "cetba1984" else "cetbaFuks")
    data class BookQuiz(val id: String) : Route(if (id == "1984") "cetba1984quiz" else "cetbaFuksQuiz")
    data class BookPlot(val id: String) : Route(if (id == "1984") "cetba1984dej" else "cetbaFuksDej")

    /** Bottom-navigation roots; everything else is pushed on top of [Subjects]. */
    val isRoot: Boolean get() = this is Welcome || this is Subjects || this is Stats || this is Settings

    /** Screens with their own bottom action bar; the navigation bar stays hidden there. */
    val isExercise: Boolean
        get() = this is GermanEx || this is Vocab || this is NetEx || this is HwEx ||
            this is OnEx || this is On2Ex || this is On3Ex || this is On4Ex || this is EnLesson || this is EnEx || this is DeLesson || this is DeEx || this is MluvEx || this is BookQuiz || this is BookPlot ||
            this is NetLesson || this is HwLesson || this is OnLesson || this is On2Lesson || this is On3Lesson || this is On4Lesson ||
            this is LitLesson || this is LitEx || this is Lit2Lesson || this is Lit2Ex ||
            this is Lit3Lesson || this is Lit3Ex || this is Lit4Lesson || this is Lit4Ex ||
            this is ChemLesson || this is ChemEx || this is BioLesson || this is BioEx ||
            this is FyzLesson || this is FyzEx || this is Fyz2Lesson || this is Fyz2Ex ||
            this is Fyz3Lesson || this is Fyz3Ex || this is Fyz4Lesson || this is Fyz4Ex ||
            this is Mat0Lesson || this is Mat0Ex || this is MatLesson || this is MatEx ||
            this is Mat2Lesson || this is Mat2Ex || this is Mat3Lesson || this is Mat3Ex ||
            this is Mat4Lesson || this is Mat4Ex

    /** Full stack from the practice root to this screen, so back always goes up one level. */
    fun stack(): List<Route> = when (this) {
        Welcome, Subjects -> listOf(Subjects)
        Stats -> listOf(Stats)
        Settings -> listOf(Settings)
        DeHome -> listOf(Subjects, DeHome)
        is DeMap -> listOf(Subjects, DeHome, this)
        is DeLesson -> listOf(Subjects, DeHome, DeMap(year), this)
        is DeEx -> listOf(Subjects, DeHome, DeMap(year), DeLesson(year, id), this)
        Roadmap -> listOf(Subjects, DeHome, Roadmap)
        is UnitMap -> listOf(Subjects, DeHome, Roadmap, this)
        is GermanEx -> listOf(Subjects, DeHome, Roadmap, UnitMap(unitId), this)
        is Vocab -> listOf(Subjects, DeHome, Roadmap, UnitMap(unitId), this)
        NetYears -> listOf(Subjects, NetYears)
        NetMap -> listOf(Subjects, NetYears, NetMap)
        is NetLesson -> listOf(Subjects, NetYears, NetMap, this)
        is NetEx -> listOf(Subjects, NetYears, NetMap, NetLesson(id), this)
        HwYears -> listOf(Subjects, HwYears)
        HwMap -> listOf(Subjects, HwYears, HwMap)
        is HwLesson -> listOf(Subjects, HwYears, HwMap, this)
        is HwEx -> listOf(Subjects, HwYears, HwMap, HwLesson(id), this)
        OnYears -> listOf(Subjects, OnYears)
        OnMap -> listOf(Subjects, OnYears, OnMap)
        is OnLesson -> listOf(Subjects, OnYears, OnMap, this)
        is OnEx -> listOf(Subjects, OnYears, OnMap, OnLesson(id), this)
        On2Map -> listOf(Subjects, OnYears, On2Map)
        is On2Lesson -> listOf(Subjects, OnYears, On2Map, this)
        is On2Ex -> listOf(Subjects, OnYears, On2Map, On2Lesson(id), this)
        On3Map -> listOf(Subjects, OnYears, On3Map)
        is On3Lesson -> listOf(Subjects, OnYears, On3Map, this)
        is On3Ex -> listOf(Subjects, OnYears, On3Map, On3Lesson(id), this)
        On4Map -> listOf(Subjects, OnYears, On4Map)
        is On4Lesson -> listOf(Subjects, OnYears, On4Map, this)
        is On4Ex -> listOf(Subjects, OnYears, On4Map, On4Lesson(id), this)
        EnYears -> listOf(Subjects, EnYears)
        is EnMap -> listOf(Subjects, EnYears, this)
        is EnLesson -> listOf(Subjects, EnYears, EnMap(year), this)
        is EnEx -> listOf(Subjects, EnYears, EnMap(year), EnLesson(year, id), this)
        LitYears -> listOf(Subjects, CzechMap, LitYears)
        LitMap -> listOf(Subjects, CzechMap, LitYears, LitMap)
        is LitLesson -> listOf(Subjects, CzechMap, LitYears, LitMap, this)
        is LitEx -> listOf(Subjects, CzechMap, LitYears, LitMap, LitLesson(id), this)
        Lit2Map -> listOf(Subjects, CzechMap, LitYears, Lit2Map)
        is Lit2Lesson -> listOf(Subjects, CzechMap, LitYears, Lit2Map, this)
        is Lit2Ex -> listOf(Subjects, CzechMap, LitYears, Lit2Map, Lit2Lesson(id), this)
        Lit3Map -> listOf(Subjects, CzechMap, LitYears, Lit3Map)
        is Lit3Lesson -> listOf(Subjects, CzechMap, LitYears, Lit3Map, this)
        is Lit3Ex -> listOf(Subjects, CzechMap, LitYears, Lit3Map, Lit3Lesson(id), this)
        Lit4Map -> listOf(Subjects, CzechMap, LitYears, Lit4Map)
        is Lit4Lesson -> listOf(Subjects, CzechMap, LitYears, Lit4Map, this)
        is Lit4Ex -> listOf(Subjects, CzechMap, LitYears, Lit4Map, Lit4Lesson(id), this)
        SciMap -> listOf(Subjects, SciMap)
        ChemMap -> listOf(Subjects, SciMap, ChemMap)
        is ChemLesson -> listOf(Subjects, SciMap, ChemMap, this)
        is ChemEx -> listOf(Subjects, SciMap, ChemMap, ChemLesson(id), this)
        BioMap -> listOf(Subjects, SciMap, BioMap)
        is BioLesson -> listOf(Subjects, SciMap, BioMap, this)
        is BioEx -> listOf(Subjects, SciMap, BioMap, BioLesson(id), this)
        MatYears -> listOf(Subjects, MatYears)
        Mat0Map -> listOf(Subjects, MatYears, Mat0Map)
        is Mat0Lesson -> listOf(Subjects, MatYears, Mat0Map, this)
        is Mat0Ex -> listOf(Subjects, MatYears, Mat0Map, Mat0Lesson(id), this)
        MatMap -> listOf(Subjects, MatYears, MatMap)
        is MatLesson -> listOf(Subjects, MatYears, MatMap, this)
        is MatEx -> listOf(Subjects, MatYears, MatMap, MatLesson(id), this)
        Mat2Map -> listOf(Subjects, MatYears, Mat2Map)
        is Mat2Lesson -> listOf(Subjects, MatYears, Mat2Map, this)
        is Mat2Ex -> listOf(Subjects, MatYears, Mat2Map, Mat2Lesson(id), this)
        Mat3Map -> listOf(Subjects, MatYears, Mat3Map)
        is Mat3Lesson -> listOf(Subjects, MatYears, Mat3Map, this)
        is Mat3Ex -> listOf(Subjects, MatYears, Mat3Map, Mat3Lesson(id), this)
        Mat4Map -> listOf(Subjects, MatYears, Mat4Map)
        is Mat4Lesson -> listOf(Subjects, MatYears, Mat4Map, this)
        is Mat4Ex -> listOf(Subjects, MatYears, Mat4Map, Mat4Lesson(id), this)
        FyzYears -> listOf(Subjects, FyzYears)
        FyzMap -> listOf(Subjects, FyzYears, FyzMap)
        is FyzLesson -> listOf(Subjects, FyzYears, FyzMap, this)
        is FyzEx -> listOf(Subjects, FyzYears, FyzMap, FyzLesson(id), this)
        Fyz2Map -> listOf(Subjects, FyzYears, Fyz2Map)
        is Fyz2Lesson -> listOf(Subjects, FyzYears, Fyz2Map, this)
        is Fyz2Ex -> listOf(Subjects, FyzYears, Fyz2Map, Fyz2Lesson(id), this)
        Fyz3Map -> listOf(Subjects, FyzYears, Fyz3Map)
        is Fyz3Lesson -> listOf(Subjects, FyzYears, Fyz3Map, this)
        is Fyz3Ex -> listOf(Subjects, FyzYears, Fyz3Map, Fyz3Lesson(id), this)
        Fyz4Map -> listOf(Subjects, FyzYears, Fyz4Map)
        is Fyz4Lesson -> listOf(Subjects, FyzYears, Fyz4Map, this)
        is Fyz4Ex -> listOf(Subjects, FyzYears, Fyz4Map, Fyz4Lesson(id), this)
        CzechMap -> listOf(Subjects, CzechMap)
        Mluvnice -> listOf(Subjects, CzechMap, Mluvnice)
        is MluvEx -> listOf(Subjects, CzechMap, Mluvnice, this)
        ReadingList -> listOf(Subjects, CzechMap, ReadingList)
        is Book -> listOf(Subjects, CzechMap, ReadingList, this)
        is BookQuiz -> listOf(Subjects, CzechMap, ReadingList, Book(id), this)
        is BookPlot -> listOf(Subjects, CzechMap, ReadingList, Book(id), this)
    }
}

fun routeFromPage(page: String): Route? = when (page) {
    "welcome" -> Route.Welcome
    "subjects" -> Route.Subjects
    "stats" -> Route.Stats
    "settings" -> Route.Settings
    "roadmap" -> Route.Roadmap
    "dehome" -> Route.DeHome
    "netyears" -> Route.NetYears
    "netmap" -> Route.NetMap
    "hwyears" -> Route.HwYears
    "hwmap" -> Route.HwMap
    "onyears" -> Route.OnYears
    "onmap" -> Route.OnMap
    "on2map" -> Route.On2Map
    "on3map" -> Route.On3Map
    "on4map" -> Route.On4Map
    "enyears" -> Route.EnYears
    "lityears" -> Route.LitYears
    "litmap" -> Route.LitMap
    "lit2map" -> Route.Lit2Map
    "lit3map" -> Route.Lit3Map
    "lit4map" -> Route.Lit4Map
    "scimap" -> Route.SciMap
    "chemmap" -> Route.ChemMap
    "biomap" -> Route.BioMap
    "matyears" -> Route.MatYears
    "mat0map" -> Route.Mat0Map
    "matmap" -> Route.MatMap
    "mat2map" -> Route.Mat2Map
    "mat3map" -> Route.Mat3Map
    "mat4map" -> Route.Mat4Map
    "fyzyears" -> Route.FyzYears
    "fyzmap" -> Route.FyzMap
    "fyz2map" -> Route.Fyz2Map
    "fyz3map" -> Route.Fyz3Map
    "fyz4map" -> Route.Fyz4Map
    "czechmap" -> Route.CzechMap
    "mluvnice" -> Route.Mluvnice
    "readinglist" -> Route.ReadingList
    "cetba1984" -> Route.Book("1984")
    "cetba1984quiz" -> Route.BookQuiz("1984")
    "cetba1984dej" -> Route.BookPlot("1984")
    "cetbaFuks" -> Route.Book("fuks")
    "cetbaFuksQuiz" -> Route.BookQuiz("fuks")
    "cetbaFuksDej" -> Route.BookPlot("fuks")
    else -> {
        Regex("""unit(\d+)""").matchEntire(page)?.let {
            return Route.UnitMap(it.groupValues[1].toInt() - 1)
        }
        Regex("""u(\d+)e(\d+)""").matchEntire(page)?.let {
            return Route.GermanEx(it.groupValues[1].toInt() - 1, it.groupValues[2].toInt())
        }
        Regex("""u(\d+)vocab""").matchEntire(page)?.let {
            return Route.Vocab(it.groupValues[1].toInt() - 1)
        }
        Regex("""netunit(\d+)""").matchEntire(page)?.let {
            return Route.NetLesson(it.groupValues[1].toInt())
        }
        Regex("""netex(\d+)""").matchEntire(page)?.let {
            return Route.NetEx(it.groupValues[1].toInt())
        }
        Regex("""hwunit(\d+)""").matchEntire(page)?.let {
            return Route.HwLesson(it.groupValues[1].toInt())
        }
        Regex("""hwex(\d+)""").matchEntire(page)?.let {
            return Route.HwEx(it.groupValues[1].toInt())
        }
        Regex("""chemunit(\d+)""").matchEntire(page)?.let {
            return Route.ChemLesson(it.groupValues[1].toInt())
        }
        Regex("""chemex(\d+)""").matchEntire(page)?.let {
            return Route.ChemEx(it.groupValues[1].toInt())
        }
        Regex("""biounit(\d+)""").matchEntire(page)?.let {
            return Route.BioLesson(it.groupValues[1].toInt())
        }
        Regex("""bioex(\d+)""").matchEntire(page)?.let {
            return Route.BioEx(it.groupValues[1].toInt())
        }
        Regex("""mat0unit(\d+)""").matchEntire(page)?.let {
            return Route.Mat0Lesson(it.groupValues[1].toInt())
        }
        Regex("""mat0ex(\d+)""").matchEntire(page)?.let {
            return Route.Mat0Ex(it.groupValues[1].toInt())
        }
        Regex("""mat4unit(\d+)""").matchEntire(page)?.let {
            return Route.Mat4Lesson(it.groupValues[1].toInt())
        }
        Regex("""mat4ex(\d+)""").matchEntire(page)?.let {
            return Route.Mat4Ex(it.groupValues[1].toInt())
        }
        Regex("""mat3unit(\d+)""").matchEntire(page)?.let {
            return Route.Mat3Lesson(it.groupValues[1].toInt())
        }
        Regex("""mat3ex(\d+)""").matchEntire(page)?.let {
            return Route.Mat3Ex(it.groupValues[1].toInt())
        }
        Regex("""mat2unit(\d+)""").matchEntire(page)?.let {
            return Route.Mat2Lesson(it.groupValues[1].toInt())
        }
        Regex("""mat2ex(\d+)""").matchEntire(page)?.let {
            return Route.Mat2Ex(it.groupValues[1].toInt())
        }
        Regex("""matunit(\d+)""").matchEntire(page)?.let {
            return Route.MatLesson(it.groupValues[1].toInt())
        }
        Regex("""matex(\d+)""").matchEntire(page)?.let {
            return Route.MatEx(it.groupValues[1].toInt())
        }
        Regex("""fyz4unit(\d+)""").matchEntire(page)?.let {
            return Route.Fyz4Lesson(it.groupValues[1].toInt())
        }
        Regex("""fyz4ex(\d+)""").matchEntire(page)?.let {
            return Route.Fyz4Ex(it.groupValues[1].toInt())
        }
        Regex("""fyz3unit(\d+)""").matchEntire(page)?.let {
            return Route.Fyz3Lesson(it.groupValues[1].toInt())
        }
        Regex("""fyz3ex(\d+)""").matchEntire(page)?.let {
            return Route.Fyz3Ex(it.groupValues[1].toInt())
        }
        Regex("""fyz2unit(\d+)""").matchEntire(page)?.let {
            return Route.Fyz2Lesson(it.groupValues[1].toInt())
        }
        Regex("""fyz2ex(\d+)""").matchEntire(page)?.let {
            return Route.Fyz2Ex(it.groupValues[1].toInt())
        }
        Regex("""fyzunit(\d+)""").matchEntire(page)?.let {
            return Route.FyzLesson(it.groupValues[1].toInt())
        }
        Regex("""fyzex(\d+)""").matchEntire(page)?.let {
            return Route.FyzEx(it.groupValues[1].toInt())
        }
        Regex("""lit4unit(\d+)""").matchEntire(page)?.let {
            return Route.Lit4Lesson(it.groupValues[1].toInt())
        }
        Regex("""lit4ex(\d+)""").matchEntire(page)?.let {
            return Route.Lit4Ex(it.groupValues[1].toInt())
        }
        Regex("""lit3unit(\d+)""").matchEntire(page)?.let {
            return Route.Lit3Lesson(it.groupValues[1].toInt())
        }
        Regex("""lit3ex(\d+)""").matchEntire(page)?.let {
            return Route.Lit3Ex(it.groupValues[1].toInt())
        }
        Regex("""lit2unit(\d+)""").matchEntire(page)?.let {
            return Route.Lit2Lesson(it.groupValues[1].toInt())
        }
        Regex("""lit2ex(\d+)""").matchEntire(page)?.let {
            return Route.Lit2Ex(it.groupValues[1].toInt())
        }
        Regex("""litunit(\d+)""").matchEntire(page)?.let {
            return Route.LitLesson(it.groupValues[1].toInt())
        }
        Regex("""litex(\d+)""").matchEntire(page)?.let {
            return Route.LitEx(it.groupValues[1].toInt())
        }
        Regex("""de(\d)map""").matchEntire(page)?.let {
            return Route.DeMap(it.groupValues[1].toInt())
        }
        Regex("""de(\d)unit(\d+)""").matchEntire(page)?.let {
            return Route.DeLesson(it.groupValues[1].toInt(), it.groupValues[2].toInt())
        }
        Regex("""de(\d)ex(\d+)""").matchEntire(page)?.let {
            return Route.DeEx(it.groupValues[1].toInt(), it.groupValues[2].toInt())
        }
        Regex("""en(\d)map""").matchEntire(page)?.let {
            return Route.EnMap(it.groupValues[1].toInt())
        }
        Regex("""en(\d)unit(\d+)""").matchEntire(page)?.let {
            return Route.EnLesson(it.groupValues[1].toInt(), it.groupValues[2].toInt())
        }
        Regex("""en(\d)ex(\d+)""").matchEntire(page)?.let {
            return Route.EnEx(it.groupValues[1].toInt(), it.groupValues[2].toInt())
        }
        Regex("""on4unit(\d+)""").matchEntire(page)?.let {
            return Route.On4Lesson(it.groupValues[1].toInt())
        }
        Regex("""on4ex(\d+)""").matchEntire(page)?.let {
            return Route.On4Ex(it.groupValues[1].toInt())
        }
        Regex("""on3unit(\d+)""").matchEntire(page)?.let {
            return Route.On3Lesson(it.groupValues[1].toInt())
        }
        Regex("""on3ex(\d+)""").matchEntire(page)?.let {
            return Route.On3Ex(it.groupValues[1].toInt())
        }
        Regex("""on2unit(\d+)""").matchEntire(page)?.let {
            return Route.On2Lesson(it.groupValues[1].toInt())
        }
        Regex("""on2ex(\d+)""").matchEntire(page)?.let {
            return Route.On2Ex(it.groupValues[1].toInt())
        }
        Regex("""onunit(\d+)""").matchEntire(page)?.let {
            return Route.OnLesson(it.groupValues[1].toInt())
        }
        Regex("""onex(\d+)""").matchEntire(page)?.let {
            return Route.OnEx(it.groupValues[1].toInt())
        }
        Regex("""mluve(\d+)""").matchEntire(page)?.let {
            return Route.MluvEx(it.groupValues[1].toInt())
        }
        null
    }
}
