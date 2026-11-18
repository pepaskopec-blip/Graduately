package org.maturita.maturita.data

import org.json.JSONArray
import org.json.JSONObject
import org.maturita.maturita.platform.Prefs
import java.net.HttpURLConnection
import java.net.URI
import java.net.URLEncoder
import java.util.concurrent.Executors

class ProgressStore(private val prefs: Prefs, content: Content) {
    private val ids = content.progressKeys
    private val knownIds = content.progressKeys.values.filterTo(HashSet()) { it.isNotEmpty() }
    var onMark: (() -> Unit)? = null

    init {
        migrate(content.progressLegacy)
    }

    /** Progress is stored under each lesson's id from content/, so lessons can be
     *  reordered or renamed without losing it. `address` is the in-app position. */
    private fun key(address: String) = "done." + (ids[address] ?: address)
    private fun done(address: String) = prefs.getBoolean(key(address), false)
    private fun mark(address: String) {
        prefs.edit().putBoolean(key(address), true).apply()
        onMark?.invoke()
    }

    fun completedIds(): Set<String> {
        val out = HashSet<String>()
        for (id in knownIds) {
            if (prefs.getBoolean("done.$id", false)) out.add(id)
        }
        val extra = prefs.getString("sync_extra_done", null).orEmpty()
        for (raw in extra.split('\n')) {
            val id = raw.trim()
            if (id.isNotEmpty()) out.add(id)
        }
        return out
    }

    fun mergeCompletedIds(remote: Collection<String>): Boolean {
        var changed = false
        val extra = HashSet<String>()
        val existing = prefs.getString("sync_extra_done", null).orEmpty()
        for (raw in existing.split('\n')) {
            val id = raw.trim()
            if (id.isNotEmpty() && id !in knownIds) extra.add(id)
        }
        val edit = prefs.edit()
        for (raw in remote) {
            val id = raw.trim()
            if (id.isEmpty()) continue
            if (!prefs.getBoolean("done.$id", false)) {
                edit.putBoolean("done.$id", true)
                changed = true
            }
            if (id !in knownIds) extra.add(id)
        }
        edit.putString("sync_extra_done", extra.sorted().joinToString("\n"))
        edit.apply()
        return changed
    }

    private fun migrate(legacy: Map<String, String>) {
        if (prefs.getBoolean("progress_ids_v2", false)) return
        val edit = prefs.edit()
        for ((old, id) in legacy) {
            if (prefs.getBoolean(old, false)) edit.putBoolean("done.$id", true)
        }
        edit.putBoolean("progress_ids_v2", true).apply()
    }

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
    fun tgDone(id: Int) = done("tg.$id")
    fun markTg(id: Int) { mark("tg.$id") }
    fun tg2Done(id: Int) = done("tg2.$id")
    fun markTg2(id: Int) { mark("tg2.$id") }
    fun tg3Done(id: Int) = done("tg3.$id")
    fun markTg3(id: Int) { mark("tg3.$id") }
    fun tg4Done(id: Int) = done("tg4.$id")
    fun markTg4(id: Int) { mark("tg4.$id") }
    fun pgDone(id: Int) = done("pg.$id")
    fun markPg(id: Int) { mark("pg.$id") }
    fun pg2Done(id: Int) = done("pg2.$id")
    fun markPg2(id: Int) { mark("pg2.$id") }
    fun pg3Done(id: Int) = done("pg3.$id")
    fun markPg3(id: Int) { mark("pg3.$id") }
    fun pg4Done(id: Int) = done("pg4.$id")
    fun markPg4(id: Int) { mark("pg4.$id") }
    fun swDone(id: Int) = done("sw.$id")
    fun markSw(id: Int) { mark("sw.$id") }
    fun sw2Done(id: Int) = done("sw2.$id")
    fun markSw2(id: Int) { mark("sw2.$id") }
    fun sw3Done(id: Int) = done("sw3.$id")
    fun markSw3(id: Int) { mark("sw3.$id") }
    fun sw4Done(id: Int) = done("sw4.$id")
    fun markSw4(id: Int) { mark("sw4.$id") }
    fun csDone(id: Int) = done("cs.$id")
    fun markCs(id: Int) { mark("cs.$id") }
    fun cs2Done(id: Int) = done("cs2.$id")
    fun markCs2(id: Int) { mark("cs2.$id") }
    fun cs3Done(id: Int) = done("cs3.$id")
    fun markCs3(id: Int) { mark("cs3.$id") }
    fun cs4Done(id: Int) = done("cs4.$id")
    fun markCs4(id: Int) { mark("cs4.$id") }
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
    content.tg.forEach { l ->
        sum.totalEx += 1
        sum.openUnits += 1
        if (p.tgDone(l.int("id"))) {
            sum.doneEx += 1
            sum.doneUnits += 1
        }
    }
    content.tg2.forEach { l ->
        sum.totalEx += 1
        sum.openUnits += 1
        if (p.tg2Done(l.int("id"))) {
            sum.doneEx += 1
            sum.doneUnits += 1
        }
    }
    content.tg3.forEach { l ->
        sum.totalEx += 1
        sum.openUnits += 1
        if (p.tg3Done(l.int("id"))) {
            sum.doneEx += 1
            sum.doneUnits += 1
        }
    }
    content.tg4.forEach { l ->
        sum.totalEx += 1
        sum.openUnits += 1
        if (p.tg4Done(l.int("id"))) {
            sum.doneEx += 1
            sum.doneUnits += 1
        }
    }
    content.pg.forEach { l ->
        sum.totalEx += 1
        sum.openUnits += 1
        if (p.pgDone(l.int("id"))) {
            sum.doneEx += 1
            sum.doneUnits += 1
        }
    }
    content.pg2.forEach { l ->
        sum.totalEx += 1
        sum.openUnits += 1
        if (p.pg2Done(l.int("id"))) {
            sum.doneEx += 1
            sum.doneUnits += 1
        }
    }
    content.pg3.forEach { l ->
        sum.totalEx += 1
        sum.openUnits += 1
        if (p.pg3Done(l.int("id"))) {
            sum.doneEx += 1
            sum.doneUnits += 1
        }
    }
    content.pg4.forEach { l ->
        sum.totalEx += 1
        sum.openUnits += 1
        if (p.pg4Done(l.int("id"))) {
            sum.doneEx += 1
            sum.doneUnits += 1
        }
    }
    content.sw.forEach { l ->
        sum.totalEx += 1
        sum.openUnits += 1
        if (p.swDone(l.int("id"))) {
            sum.doneEx += 1
            sum.doneUnits += 1
        }
    }
    content.sw2.forEach { l ->
        sum.totalEx += 1
        sum.openUnits += 1
        if (p.sw2Done(l.int("id"))) {
            sum.doneEx += 1
            sum.doneUnits += 1
        }
    }
    content.sw3.forEach { l ->
        sum.totalEx += 1
        sum.openUnits += 1
        if (p.sw3Done(l.int("id"))) {
            sum.doneEx += 1
            sum.doneUnits += 1
        }
    }
    content.sw4.forEach { l ->
        sum.totalEx += 1
        sum.openUnits += 1
        if (p.sw4Done(l.int("id"))) {
            sum.doneEx += 1
            sum.doneUnits += 1
        }
    }
    content.cs.forEach { l ->
        sum.totalEx += 1
        sum.openUnits += 1
        if (p.csDone(l.int("id"))) {
            sum.doneEx += 1
            sum.doneUnits += 1
        }
    }
    content.cs2.forEach { l ->
        sum.totalEx += 1
        sum.openUnits += 1
        if (p.cs2Done(l.int("id"))) {
            sum.doneEx += 1
            sum.doneUnits += 1
        }
    }
    content.cs3.forEach { l ->
        sum.totalEx += 1
        sum.openUnits += 1
        if (p.cs3Done(l.int("id"))) {
            sum.doneEx += 1
            sum.doneUnits += 1
        }
    }
    content.cs4.forEach { l ->
        sum.totalEx += 1
        sum.openUnits += 1
        if (p.cs4Done(l.int("id"))) {
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

data class AccountState(
    val status: String = "idle",
    val userDisplay: String = "",
    val isSignedIn: Boolean = false,
    val hasBuiltInConfig: Boolean = false,
    val customUrl: String = "",
    val customAnonKey: String = "",
    val messageKey: String? = null,
    val messageCount: Int? = null,
)

fun normalizeAccountEmail(raw: String): String {
    val trimmed = raw.trim().lowercase()
    if (trimmed.isEmpty()) return ""
    if ('@' in trimmed) return trimmed
    val local = trimmed.filter { it in 'a'..'z' || it in '0'..'9' || it == '.' || it == '_' || it == '-' }
    return if (local.isEmpty()) "" else "$local@graduately.app"
}

class AccountSync(private val prefs: Prefs, private val content: Content) {
    private val io = Executors.newSingleThreadExecutor { r -> Thread(r, "account-sync").apply { isDaemon = true } }

    private data class HttpResult(val code: Int, val body: String)

    private sealed class SyncException : Exception() {
        object Config : SyncException()
        object Auth : SyncException()
        object ConfirmEmail : SyncException()
        object Unauthorized : SyncException()
        object Network : SyncException()
    }

    fun initialState(): AccountState = makeState("idle", null, null)

    fun signIn(
        user: String,
        pass: String,
        urlOverride: String?,
        keyOverride: String?,
        localDone: Set<String>,
        cb: (AccountState, Set<String>?) -> Unit,
    ) = authenticate(false, user, pass, urlOverride, keyOverride, localDone, cb)

    fun signUp(
        user: String,
        pass: String,
        urlOverride: String?,
        keyOverride: String?,
        localDone: Set<String>,
        cb: (AccountState, Set<String>?) -> Unit,
    ) = authenticate(true, user, pass, urlOverride, keyOverride, localDone, cb)

    fun signOut(cb: (AccountState) -> Unit) {
        io.execute {
            clearSession()
            cb(makeState("idle", null, null))
        }
    }

    fun sync(localDone: Set<String>, interactive: Boolean, cb: (AccountState, Set<String>?) -> Unit) {
        io.execute {
            val token = prefs.getString(KEY_ACCESS_TOKEN, "").orEmpty()
            val uid = prefs.getString(KEY_USER_ID, "").orEmpty()
            if (token.isEmpty() || uid.isEmpty()) {
                if (interactive) cb(makeState("error", "account_err_auth", null), null)
                return@execute
            }
            val (url, anonKey) = resolvedConfig()
            if (url.isEmpty() || anonKey.isEmpty()) {
                if (interactive) cb(makeState("error", "account_err_config", null), null)
                return@execute
            }
            if (interactive) cb(makeState("syncing", "account_syncing", null), null)
            try {
                val merged = syncWithRefresh(url, anonKey, localDone)
                cb(makeState("ok", "account_synced", merged.size), merged)
            } catch (_: SyncException.Unauthorized) {
                clearSession()
                cb(makeState("error", "account_err_auth", null), null)
            } catch (_: Exception) {
                if (interactive) cb(makeState("error", "account_err_network", null), null)
            }
        }
    }

    private fun authenticate(
        signup: Boolean,
        user: String,
        pass: String,
        urlOverride: String?,
        keyOverride: String?,
        localDone: Set<String>,
        cb: (AccountState, Set<String>?) -> Unit,
    ) {
        io.execute {
            saveOverrides(urlOverride, keyOverride)
            val (url, anonKey) = resolvedConfig()
            if (url.isEmpty() || anonKey.isEmpty()) {
                cb(makeState("error", "account_err_config", null), null)
                return@execute
            }
            val display = user.trim()
            val email = normalizeAccountEmail(display)
            if (display.isEmpty() || email.isEmpty() || pass.isEmpty()) {
                cb(makeState("error", "account_err_fill", null), null)
                return@execute
            }
            if (pass.length < 6) {
                cb(makeState("error", "account_err_short_pass", null), null)
                return@execute
            }
            cb(makeState("working", "account_working", null), null)
            try {
                performAuth(signup, url, anonKey, email, pass, display)
                val merged = syncWithRefresh(url, anonKey, localDone)
                cb(makeState("ok", "account_synced", merged.size), merged)
            } catch (_: SyncException.ConfirmEmail) {
                cb(makeState("idle", "account_confirm_email", null), null)
            } catch (_: SyncException.Auth) {
                cb(makeState("error", "account_err_auth", null), null)
            } catch (_: SyncException.Config) {
                cb(makeState("error", "account_err_config", null), null)
            } catch (_: Exception) {
                cb(makeState("error", "account_err_network", null), null)
            }
        }
    }

    private fun performAuth(
        signup: Boolean,
        url: String,
        anonKey: String,
        email: String,
        pass: String,
        display: String,
    ) {
        val path = if (signup) "/auth/v1/signup" else "/auth/v1/token?grant_type=password"
        val body = JSONObject().put("email", email).put("password", pass).toString()
        val res = request("POST", url + path, anonKey, null, body = body)
        if (res.code !in 200..299) {
            if (res.code in 400..422) throw SyncException.Auth
            throw SyncException.Network
        }
        val obj = runCatching { JSONObject(res.body) }.getOrNull() ?: throw SyncException.Network
        val session = obj.optJSONObject("session")
        val accessToken = optStr(obj, "access_token") ?: optStr(session, "access_token") ?: ""
        val refreshToken = optStr(obj, "refresh_token") ?: optStr(session, "refresh_token") ?: ""
        val userObj = obj.optJSONObject("user") ?: session?.optJSONObject("user") ?: obj
        val userId = optStr(userObj, "id") ?: ""

        if (accessToken.isEmpty() || userId.isEmpty()) {
            if (signup) {
                val identities = userObj.optJSONArray("identities")
                if (identities != null && identities.length() == 0) {
                    throw SyncException.Auth
                }
                throw SyncException.ConfirmEmail
            }
            throw SyncException.Auth
        }

        prefs.edit()
            .putString(KEY_ACCESS_TOKEN, accessToken)
            .putString(KEY_REFRESH_TOKEN, refreshToken)
            .putString(KEY_USER_ID, userId)
            .putString(KEY_USER_DISPLAY, display)
            .apply()
    }

    private fun syncWithRefresh(url: String, anonKey: String, localDone: Set<String>): Set<String> = try {
        syncOnce(url, anonKey, localDone)
    } catch (_: SyncException.Unauthorized) {
        refreshSession(url, anonKey)
        syncOnce(url, anonKey, localDone)
    }

    private fun syncOnce(url: String, anonKey: String, localDone: Set<String>): Set<String> {
        val token = prefs.getString(KEY_ACCESS_TOKEN, "").orEmpty()
        val uid = prefs.getString(KEY_USER_ID, "").orEmpty()
        if (token.isEmpty() || uid.isEmpty()) throw SyncException.Unauthorized

        val rpcBody = JSONObject().put("client_done", JSONArray(localDone.sorted())).toString()
        val rpc = request("POST", "$url/rest/v1/rpc/sync_progress", anonKey, token, body = rpcBody)
        if (rpc.code == 401) throw SyncException.Unauthorized
        if (rpc.code in 200..299) {
            val arr = runCatching { JSONArray(rpc.body) }.getOrNull() ?: return localDone
            val out = HashSet<String>()
            for (i in 0 until arr.length()) {
                val id = arr.optString(i).trim()
                if (id.isNotEmpty() && id != "null") out.add(id)
            }
            return out
        }
        if (rpc.code != 404) throw SyncException.Network

        val encodedUid = URLEncoder.encode(uid, "UTF-8")
        val getRes = request("GET", "$url/rest/v1/user_progress?user_id=eq.$encodedUid&select=done", anonKey, token)
        if (getRes.code == 401) throw SyncException.Unauthorized
        if (getRes.code !in 200..299) throw SyncException.Network

        val remote = HashSet<String>()
        val rows = runCatching { JSONArray(getRes.body) }.getOrNull()
        val arr = rows?.optJSONObject(0)?.optJSONArray("done")
        if (arr != null) {
            for (i in 0 until arr.length()) {
                val id = arr.optString(i).trim()
                if (id.isNotEmpty() && id != "null") remote.add(id)
            }
        }
        val merged = (localDone + remote).toSet()
        val upsertBody = JSONObject()
            .put("user_id", uid)
            .put("done", JSONArray(merged.sorted()))
            .toString()
        val upsertRes = request(
            method = "POST",
            urlString = "$url/rest/v1/user_progress?on_conflict=user_id",
            anonKey = anonKey,
            bearer = token,
            extraHeaders = mapOf("Prefer" to "resolution=merge-duplicates,return=minimal"),
            body = upsertBody,
        )
        if (upsertRes.code == 401) throw SyncException.Unauthorized
        if (upsertRes.code !in 200..299) throw SyncException.Network
        return merged
    }

    private fun refreshSession(url: String, anonKey: String) {
        val refreshToken = prefs.getString(KEY_REFRESH_TOKEN, "").orEmpty()
        if (refreshToken.isEmpty()) throw SyncException.Unauthorized
        val body = JSONObject().put("refresh_token", refreshToken).toString()
        val res = request("POST", "$url/auth/v1/token?grant_type=refresh_token", anonKey, null, body = body)
        if (res.code in 400..403) throw SyncException.Unauthorized
        if (res.code !in 200..299) throw SyncException.Network
        val obj = runCatching { JSONObject(res.body) }.getOrNull() ?: throw SyncException.Network
        val newAccess = optStr(obj, "access_token") ?: throw SyncException.Network
        val edit = prefs.edit().putString(KEY_ACCESS_TOKEN, newAccess)
        optStr(obj, "refresh_token")?.let { edit.putString(KEY_REFRESH_TOKEN, it) }
        optStr(obj.optJSONObject("user"), "id")?.let { edit.putString(KEY_USER_ID, it) }
        edit.apply()
    }

    private fun clearSession() {
        prefs.edit()
            .putString(KEY_ACCESS_TOKEN, "")
            .putString(KEY_REFRESH_TOKEN, "")
            .putString(KEY_USER_ID, "")
            .putString(KEY_USER_DISPLAY, "")
            .apply()
    }

    private fun env(name: String): String = runCatching { System.getenv(name)?.trim().orEmpty() }.getOrDefault("")

    private fun hasBuiltInConfig(): Boolean {
        if (env("SUPABASE_URL").isNotEmpty() && env("SUPABASE_ANON_KEY").isNotEmpty()) return true
        return content.supabaseUrl.isNotEmpty() && content.supabaseAnonKey.isNotEmpty()
    }

    private fun resolvedConfig(): Pair<String, String> {
        val savedUrl = cleanUrl(prefs.getString(KEY_CUSTOM_URL, "").orEmpty())
        val savedKey = prefs.getString(KEY_CUSTOM_ANON_KEY, "").orEmpty().trim()
        val envUrl = cleanUrl(env("SUPABASE_URL"))
        val envKey = env("SUPABASE_ANON_KEY")
        val builtUrl = cleanUrl(content.supabaseUrl)
        val builtKey = content.supabaseAnonKey.trim()

        val url = savedUrl.ifEmpty { envUrl.ifEmpty { builtUrl } }
        val key = savedKey.ifEmpty { envKey.ifEmpty { builtKey } }
        return url to key
    }

    private fun saveOverrides(urlOverride: String?, keyOverride: String?) {
        val edit = prefs.edit()
        var dirty = false
        if (urlOverride != null) {
            val cleaned = cleanUrl(urlOverride)
            if (cleaned.isNotEmpty() || !hasBuiltInConfig()) {
                edit.putString(KEY_CUSTOM_URL, cleaned)
                dirty = true
            }
        }
        if (keyOverride != null) {
            val cleaned = keyOverride.trim()
            if (cleaned.isNotEmpty() || !hasBuiltInConfig()) {
                edit.putString(KEY_CUSTOM_ANON_KEY, cleaned)
                dirty = true
            }
        }
        if (dirty) edit.apply()
    }

    private fun cleanUrl(raw: String): String {
        var s = raw.trim().trimEnd('/')
        if (s.isNotEmpty() && "://" !in s) s = "https://$s"
        return s
    }

    private fun makeState(status: String, messageKey: String?, messageCount: Int?): AccountState {
        val token = prefs.getString(KEY_ACCESS_TOKEN, "").orEmpty()
        val uid = prefs.getString(KEY_USER_ID, "").orEmpty()
        val user = prefs.getString(KEY_USER_DISPLAY, "").orEmpty()
        val (url, key) = resolvedConfig()
        return AccountState(
            status = status,
            userDisplay = user,
            isSignedIn = token.isNotEmpty() && uid.isNotEmpty(),
            hasBuiltInConfig = hasBuiltInConfig(),
            customUrl = prefs.getString(KEY_CUSTOM_URL, null) ?: url,
            customAnonKey = prefs.getString(KEY_CUSTOM_ANON_KEY, null) ?: key,
            messageKey = messageKey,
            messageCount = messageCount,
        )
    }

    private fun optStr(obj: JSONObject?, key: String): String? {
        if (obj == null || !obj.has(key) || obj.isNull(key)) return null
        return obj.optString(key).takeIf { it.isNotEmpty() && it != "null" }
    }

    private fun request(
        method: String,
        urlString: String,
        anonKey: String,
        bearer: String?,
        extraHeaders: Map<String, String> = emptyMap(),
        body: String? = null,
    ): HttpResult {
        val c = runCatching { URI(urlString).toURL().openConnection() as? HttpURLConnection }.getOrNull()
            ?: throw SyncException.Config
        c.connectTimeout = 15000
        c.readTimeout = 15000
        c.requestMethod = method
        c.setRequestProperty("apikey", anonKey)
        if (!bearer.isNullOrEmpty()) {
            c.setRequestProperty("Authorization", "Bearer $bearer")
        }
        c.setRequestProperty("Accept", "application/json")
        extraHeaders.forEach { (k, v) -> c.setRequestProperty(k, v) }
        return try {
            if (body != null) {
                c.doOutput = true
                c.setRequestProperty("Content-Type", "application/json")
                c.outputStream.use { it.write(body.toByteArray(Charsets.UTF_8)) }
            }
            val code = c.responseCode
            val stream = if (code >= 400) c.errorStream else c.inputStream
            val text = stream?.bufferedReader(Charsets.UTF_8)?.use { it.readText() }.orEmpty()
            HttpResult(code, text)
        } catch (_: Exception) {
            throw SyncException.Network
        } finally {
            c.disconnect()
        }
    }

    private companion object {
        const val KEY_ACCESS_TOKEN = "account_access_token"
        const val KEY_REFRESH_TOKEN = "account_refresh_token"
        const val KEY_USER_ID = "account_user_id"
        const val KEY_USER_DISPLAY = "account_user_display"
        const val KEY_CUSTOM_URL = "supabase_url"
        const val KEY_CUSTOM_ANON_KEY = "supabase_anon_key"
    }
}
