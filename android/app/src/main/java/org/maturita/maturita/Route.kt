package org.maturita.maturita

sealed class Route(val page: String) {
    data object Welcome : Route("welcome")
    data object Subjects : Route("subjects")
    data object Stats : Route("stats")
    data object Settings : Route("settings")
    data object Roadmap : Route("roadmap")
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
            this is OnEx || this is On2Ex || this is On3Ex || this is On4Ex || this is MluvEx || this is BookQuiz || this is BookPlot ||
            this is NetLesson || this is HwLesson || this is OnLesson || this is On2Lesson || this is On3Lesson || this is On4Lesson

    /** Full stack from the practice root to this screen, so back always goes up one level. */
    fun stack(): List<Route> = when (this) {
        Welcome, Subjects -> listOf(Subjects)
        Stats -> listOf(Stats)
        Settings -> listOf(Settings)
        Roadmap -> listOf(Subjects, Roadmap)
        is UnitMap -> listOf(Subjects, Roadmap, this)
        is GermanEx -> listOf(Subjects, Roadmap, UnitMap(unitId), this)
        is Vocab -> listOf(Subjects, Roadmap, UnitMap(unitId), this)
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
    "netyears" -> Route.NetYears
    "netmap" -> Route.NetMap
    "hwyears" -> Route.HwYears
    "hwmap" -> Route.HwMap
    "onyears" -> Route.OnYears
    "onmap" -> Route.OnMap
    "on2map" -> Route.On2Map
    "on3map" -> Route.On3Map
    "on4map" -> Route.On4Map
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
