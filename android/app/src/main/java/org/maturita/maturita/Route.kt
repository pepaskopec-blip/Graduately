package org.maturita.maturita

sealed class Route(val page: String) {
    data object Welcome : Route("welcome")
    data object Subjects : Route("subjects")
    data object Stats : Route("stats")
    data object Roadmap : Route("roadmap")
    data class UnitMap(val unitId: Int) : Route("unit${unitId + 1}")
    data class GermanEx(val unitId: Int, val ex: Int) : Route("u${unitId + 1}e$ex")
    data class Vocab(val unitId: Int) : Route("u${unitId + 1}vocab")
    data object NetYears : Route("netyears")
    data object NetMap : Route("netmap")
    data class NetLesson(val id: Int) : Route("netunit$id")
    data class NetEx(val id: Int) : Route("netex$id")
    data object HwMap : Route("hwmap")
    data class HwLesson(val id: Int) : Route("hwunit$id")
    data class HwEx(val id: Int) : Route("hwex$id")
    data object CzechMap : Route("czechmap")
    data object Mluvnice : Route("mluvnice")
    data class MluvEx(val n: Int) : Route("mluve$n")
    data object ReadingList : Route("readinglist")
    data class Book(val id: String) : Route(if (id == "1984") "cetba1984" else "cetbaFuks")
    data class BookQuiz(val id: String) : Route(if (id == "1984") "cetba1984quiz" else "cetbaFuksQuiz")
    data class BookPlot(val id: String) : Route(if (id == "1984") "cetba1984dej" else "cetbaFuksDej")
}

fun routeFromPage(page: String): Route? = when (page) {
    "welcome" -> Route.Welcome
    "subjects" -> Route.Subjects
    "stats" -> Route.Stats
    "roadmap" -> Route.Roadmap
    "netyears" -> Route.NetYears
    "netmap" -> Route.NetMap
    "hwmap" -> Route.HwMap
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
        Regex("""mluve(\d+)""").matchEntire(page)?.let {
            return Route.MluvEx(it.groupValues[1].toInt())
        }
        null
    }
}
