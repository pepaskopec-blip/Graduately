package org.maturita.maturita

import android.app.Application
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableStateListOf
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.setValue
import androidx.lifecycle.AndroidViewModel
import org.maturita.maturita.data.ColorMode
import org.maturita.maturita.data.Content
import org.maturita.maturita.data.ProgressStore
import org.maturita.maturita.data.ThemeId
import org.maturita.maturita.data.UiLang
import org.maturita.maturita.data.themePalette
import org.maturita.maturita.update.UpdateState
import org.maturita.maturita.update.Updater

class AppViewModel(app: Application) : AndroidViewModel(app) {
    val content = Content.load(app)
    val progress = ProgressStore(app)
    val updater = Updater(app)

    var themeId by mutableStateOf(progress.themeId)
        private set
    var mode by mutableStateOf(progress.mode)
        private set
    var lang by mutableStateOf(progress.lang)
        private set
    var searchOpen by mutableStateOf(false)
    var searchQuery by mutableStateOf("")
    var tick by mutableStateOf(0)
        private set
    var update by mutableStateOf(UpdateState())
        private set

    private val stack = mutableStateListOf<Route>(Route.Subjects)
    val route: Route get() = stack.last()
    val root: Route get() = stack.first()
    val canGoBack: Boolean get() = stack.size > 1 || root != Route.Subjects

    val palette get() = themePalette(themeId, mode)

    fun tr(key: String?) = content.tr(key, lang)
    fun fmt(key: String, vararg args: Any) = content.fmt(key, lang, *args)

    /** Roots switch the bottom tab; everything else is pushed on the practice stack. */
    fun go(r: Route) {
        searchOpen = false
        if (stack.last() == r) return
        if (r.isRoot) {
            val target = if (r == Route.Welcome) Route.Subjects else r
            stack.clear()
            stack.add(target)
            return
        }
        if (root != Route.Subjects) {
            stack.clear()
            stack.addAll(r.stack())
            return
        }
        stack.add(r)
    }

    fun goPage(page: String?) {
        val r = page?.let { routeFromPage(it) } ?: return
        go(r)
    }

    /** Jumping from search rebuilds the whole path so back walks up level by level. */
    fun openPage(page: String) {
        val r = routeFromPage(page) ?: return
        searchOpen = false
        stack.clear()
        stack.addAll(r.stack())
    }

    /** Replace the current screen (lesson slides -> its exercise). */
    fun replace(r: Route) {
        if (stack.size > 1) stack.removeAt(stack.lastIndex)
        stack.add(r)
    }

    fun back() {
        when {
            stack.size > 1 -> stack.removeAt(stack.lastIndex)
            root != Route.Subjects -> go(Route.Subjects)
        }
    }

    fun setTheme(id: ThemeId) {
        themeId = id
        progress.themeId = id
    }

    fun applyMode(m: ColorMode) {
        mode = m
        progress.mode = m
    }

    fun applyLang(l: UiLang) {
        lang = l
        progress.lang = l
    }

    fun refresh() { tick++ }

    fun markGerman(unit: Int, ex: Int) {
        progress.markGerman(unit, ex)
        refresh()
    }

    fun markVocab(unit: Int) {
        progress.markVocab(unit)
        refresh()
    }

    fun markNet(id: Int) {
        progress.markNet(id)
        refresh()
    }

    fun markHw(id: Int) {
        progress.markHw(id)
        refresh()
    }

    fun markMluv(n: Int) {
        progress.markMluv(n)
        refresh()
    }

    fun markBookQuiz(id: String) {
        progress.markBookQuiz(id)
        refresh()
    }

    fun markBookPlot(id: String) {
        progress.markBookPlot(id)
        refresh()
    }

    fun checkUpdate(interactive: Boolean) {
        updater.check(interactive) { update = it }
    }

    fun installUpdate() {
        updater.install { update = it }
    }
}
