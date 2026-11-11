package org.maturita.maturita.ui

import androidx.compose.animation.AnimatedVisibility
import androidx.compose.animation.core.animateFloatAsState
import androidx.compose.animation.core.tween
import androidx.compose.animation.expandVertically
import androidx.compose.animation.fadeIn
import androidx.compose.animation.fadeOut
import androidx.compose.animation.shrinkVertically
import androidx.compose.animation.animateColorAsState
import androidx.compose.foundation.BorderStroke
import androidx.compose.foundation.background
import androidx.compose.foundation.border
import androidx.compose.foundation.clickable
import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.BoxWithConstraints
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.ExperimentalLayoutApi
import androidx.compose.foundation.layout.FlowRow
import androidx.compose.foundation.layout.PaddingValues
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.RowScope
import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.WindowInsets
import androidx.compose.foundation.layout.fillMaxHeight
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.heightIn
import androidx.compose.foundation.layout.navigationBars
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.size
import androidx.compose.foundation.layout.width
import androidx.compose.foundation.layout.widthIn
import androidx.compose.foundation.layout.windowInsetsPadding
import androidx.compose.foundation.layout.wrapContentWidth
import androidx.compose.foundation.shape.CircleShape
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.ui.draw.clip
import androidx.compose.ui.draw.drawBehind
import androidx.compose.ui.geometry.Offset
import androidx.compose.ui.unit.Dp
import androidx.compose.foundation.lazy.LazyColumn
import androidx.compose.foundation.lazy.LazyListScope
import androidx.compose.foundation.text.KeyboardOptions
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.automirrored.filled.ArrowBack
import androidx.compose.material.icons.filled.Cancel
import androidx.compose.material.icons.filled.Check
import androidx.compose.material.icons.filled.CheckCircle
import androidx.compose.material.icons.filled.RadioButtonChecked
import androidx.compose.material.icons.automirrored.filled.KeyboardArrowRight
import androidx.compose.material.icons.filled.Lock
import androidx.compose.material.icons.filled.RadioButtonUnchecked
import androidx.compose.material.icons.filled.Search
import androidx.compose.material.icons.filled.Warning
import androidx.compose.material3.Button
import androidx.compose.material3.ButtonDefaults
import androidx.compose.material3.OutlinedButton
import androidx.compose.material3.OutlinedTextFieldDefaults
import androidx.compose.material3.ExperimentalMaterial3Api
import androidx.compose.material3.Icon
import androidx.compose.material3.IconButton
import androidx.compose.material3.LargeTopAppBar
import androidx.compose.material3.ListItem
import androidx.compose.material3.ListItemDefaults
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.ModalBottomSheet
import androidx.compose.material3.NavigationBar
import androidx.compose.material3.NavigationBarItem
import androidx.compose.material3.OutlinedTextField
import androidx.compose.material3.Scaffold
import androidx.compose.material3.Surface
import androidx.compose.material3.Text
import androidx.compose.material3.TopAppBar
import androidx.compose.material3.TopAppBarDefaults
import androidx.compose.material3.Typography
import androidx.compose.material3.rememberTopAppBarState
import androidx.compose.runtime.Composable
import androidx.compose.runtime.staticCompositionLocalOf
import androidx.compose.runtime.getValue
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.graphics.vector.ImageVector
import androidx.compose.ui.input.nestedscroll.nestedScroll
import androidx.compose.ui.text.TextStyle
import androidx.compose.ui.text.font.FontFamily
import androidx.compose.ui.text.font.FontSynthesis
import androidx.compose.ui.text.font.FontStyle
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.text.input.KeyboardCapitalization
import androidx.compose.ui.text.input.KeyboardType
import androidx.compose.ui.text.style.TextAlign
import androidx.compose.ui.text.style.TextOverflow
import androidx.compose.ui.unit.TextUnit
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import org.maturita.maturita.AppViewModel
import org.maturita.maturita.Route
import org.maturita.maturita.data.Accent
import org.maturita.maturita.data.ColorMode
import org.maturita.maturita.data.themePalette

/** Bold is capped at Medium: synthetic bold doubled glyphs on some devices. */
fun appTextStyle(
    fontSize: TextUnit,
    fontWeight: FontWeight = FontWeight.Normal,
    textAlign: TextAlign = TextAlign.Unspecified,
    color: Color = Color.Unspecified,
): TextStyle {
    val weight = if (fontWeight >= FontWeight.Bold) FontWeight.Medium else fontWeight
    return TextStyle(
        color = color,
        fontSize = fontSize,
        fontWeight = weight,
        fontFamily = FontFamily.SansSerif,
        fontSynthesis = FontSynthesis.None,
        lineHeight = fontSize * 1.35f,
        letterSpacing = 0.sp,
        textAlign = textAlign,
    )
}

/** Titles and numerals in the display serif, with lining figures; body text in the system sans. */
fun displayStyle(family: FontFamily, fontSize: TextUnit, italic: Boolean = false) = TextStyle(
    fontSize = fontSize,
    fontFamily = family,
    fontWeight = if (italic) FontWeight.Black else FontWeight.Bold,
    fontStyle = if (italic) FontStyle.Italic else FontStyle.Normal,
    fontSynthesis = FontSynthesis.None,
    fontFeatureSettings = "lnum",
    lineHeight = fontSize * 1.2f,
    letterSpacing = 0.sp,
)

fun appTypography(display: FontFamily): Typography = Typography(
    displaySmall = displayStyle(display, 34.sp),
    headlineLarge = displayStyle(display, 30.sp),
    headlineMedium = displayStyle(display, 26.sp),
    headlineSmall = displayStyle(display, 22.sp),
    titleLarge = displayStyle(display, 20.sp),
    titleMedium = appTextStyle(16.sp, FontWeight.Medium),
    titleSmall = appTextStyle(14.sp, FontWeight.Medium),
    bodyLarge = appTextStyle(16.sp),
    bodyMedium = appTextStyle(14.sp),
    bodySmall = appTextStyle(12.sp),
    labelLarge = appTextStyle(14.sp, FontWeight.Medium),
    labelMedium = appTextStyle(12.sp, FontWeight.Medium),
    labelSmall = appTextStyle(11.sp, FontWeight.Medium),
)

/** Paper, ink and the semantic colours; Material's scheme is derived from it in App.kt. */
val LocalPalette = staticCompositionLocalOf { themePalette(ColorMode.Light) }

/** The colour of the subject the current screen belongs to, or the brand's outside any subject. */
val LocalAccent = staticCompositionLocalOf<Accent> { themePalette(ColorMode.Light).accent }

val LocalDisplayFont = staticCompositionLocalOf<FontFamily> { FontFamily.Serif }

/** Small tracked capitals above a title. */
@Composable
fun Kicker(text: String, color: Color = LocalPalette.current.ink3, modifier: Modifier = Modifier) {
    Text(
        text.uppercase(),
        style = MaterialTheme.typography.labelMedium.copy(letterSpacing = 1.2.sp),
        color = color,
        modifier = modifier,
    )
}

/** White card on the paper, outlined by a hairline instead of a shadow. */
@Composable
fun AppCard(
    modifier: Modifier = Modifier,
    color: Color = LocalPalette.current.surface,
    border: Color = LocalPalette.current.line,
    radius: Dp = 20.dp,
    onClick: (() -> Unit)? = null,
    content: @Composable () -> Unit,
) {
    val shape = RoundedCornerShape(radius)
    if (onClick != null) {
        Surface(onClick = onClick, modifier = modifier, color = color, shape = shape, border = BorderStroke(1.dp, border), content = content)
    } else {
        Surface(modifier = modifier, color = color, shape = shape, border = BorderStroke(1.dp, border), content = content)
    }
}

// ---------------------------------------------------------------------------
// Chrome

@Composable
fun AppNavBar(vm: AppViewModel) {
    val root = vm.root
    NavigationBar(containerColor = LocalPalette.current.surface, tonalElevation = 0.dp) {
        NavigationBarItem(
            selected = root == Route.Subjects,
            onClick = { vm.go(Route.Subjects) },
            icon = { SubjectsIcon(navIconColor(root == Route.Subjects)) },
            label = { Text(vm.tr("subjects_title"), maxLines = 1, overflow = TextOverflow.Ellipsis) },
        )
        NavigationBarItem(
            selected = root == Route.Stats,
            onClick = { vm.go(Route.Stats) },
            icon = { StatsIcon(navIconColor(root == Route.Stats), 22.dp) },
            label = { Text(vm.tr("stats"), maxLines = 1, overflow = TextOverflow.Ellipsis) },
        )
        NavigationBarItem(
            selected = root == Route.Settings,
            onClick = { vm.go(Route.Settings) },
            icon = { SettingsIcon(navIconColor(root == Route.Settings), 22.dp) },
            label = { Text(vm.tr("settings"), maxLines = 1, overflow = TextOverflow.Ellipsis) },
        )
    }
}

@Composable
private fun navIconColor(selected: Boolean): Color =
    if (selected) MaterialTheme.colorScheme.onSecondaryContainer else MaterialTheme.colorScheme.onSurfaceVariant

/** Lists and exercises stop growing here so they keep their shape in wide desktop windows. */
val ContentMaxWidth = 880.dp
val ActionMaxWidth = 560.dp

/** Caps the width of [content] and centres it; phones are narrower than the cap anyway. */
@Composable
fun CenteredColumn(maxWidth: Dp = ContentMaxWidth, content: @Composable () -> Unit) {
    Box(Modifier.fillMaxSize(), contentAlignment = Alignment.TopCenter) {
        Box(Modifier.widthIn(max = maxWidth).fillMaxSize()) { content() }
    }
}

/** Tab root: large collapsing title on phones, a compact one in wide windows. */
@OptIn(ExperimentalMaterial3Api::class)
@Composable
fun RootScaffold(
    vm: AppViewModel,
    title: String,
    showSearch: Boolean = true,
    /** The home screen sets the app's name as on the icon, in black italic. */
    wordmark: Boolean = false,
    content: LazyListScope.() -> Unit,
) {
    val paper = LocalPalette.current.paper
    val barColors = TopAppBarDefaults.topAppBarColors(containerColor = paper, scrolledContainerColor = paper)
    val titleStyle = if (wordmark) displayStyle(LocalDisplayFont.current, 28.sp, italic = true) else MaterialTheme.typography.headlineMedium
    BoxWithConstraints {
        val wide = maxWidth >= 600.dp
        val scroll = if (wide) {
            TopAppBarDefaults.pinnedScrollBehavior(rememberTopAppBarState())
        } else {
            TopAppBarDefaults.exitUntilCollapsedScrollBehavior(rememberTopAppBarState())
        }
        val actions: @Composable RowScope.() -> Unit = {
            if (showSearch) {
                IconButton(onClick = { vm.searchOpen = true }) {
                    Icon(Icons.Filled.Search, contentDescription = vm.tr("search_placeholder"))
                }
            }
        }
        Scaffold(
            modifier = Modifier.nestedScroll(scroll.nestedScrollConnection),
            topBar = {
                if (wide) {
                    TopAppBar(
                        title = { Text(title, maxLines = 1, overflow = TextOverflow.Ellipsis, style = titleStyle) },
                        actions = actions,
                        colors = barColors,
                        scrollBehavior = scroll,
                    )
                } else {
                    LargeTopAppBar(
                        title = { Text(title, maxLines = 1, overflow = TextOverflow.Ellipsis, style = titleStyle) },
                        actions = actions,
                        colors = barColors,
                        scrollBehavior = scroll,
                    )
                }
            },
            bottomBar = { AppNavBar(vm) },
        ) { inner ->
            CenteredColumn {
                LazyColumn(
                    contentPadding = PaddingValues(
                        top = inner.calculateTopPadding(),
                        bottom = inner.calculateBottomPadding() + 16.dp,
                    ),
                ) { content() }
            }
        }
    }
}

/** Pushed screen: back arrow, title with optional subtitle, optional bottom action bar. */
@OptIn(ExperimentalMaterial3Api::class)
@Composable
fun DetailScaffold(
    vm: AppViewModel,
    title: String,
    subtitle: String? = null,
    bottomBar: (@Composable () -> Unit)? = null,
    maxContentWidth: Dp = ContentMaxWidth,
    content: @Composable (PaddingValues) -> Unit,
) {
    val scroll = TopAppBarDefaults.pinnedScrollBehavior(rememberTopAppBarState())
    Scaffold(
        modifier = Modifier.nestedScroll(scroll.nestedScrollConnection),
        topBar = {
            TopAppBar(
                title = {
                    Column {
                        Text(title, maxLines = 1, overflow = TextOverflow.Ellipsis, style = MaterialTheme.typography.headlineSmall)
                        if (!subtitle.isNullOrBlank()) {
                            Text(
                                subtitle,
                                maxLines = 1,
                                overflow = TextOverflow.Ellipsis,
                                style = MaterialTheme.typography.bodySmall,
                                color = MaterialTheme.colorScheme.onSurfaceVariant,
                            )
                        }
                    }
                },
                navigationIcon = {
                    IconButton(onClick = { vm.back() }) {
                        Icon(Icons.AutoMirrored.Filled.ArrowBack, contentDescription = vm.tr("back"))
                    }
                },
                colors = LocalPalette.current.paper.let { TopAppBarDefaults.topAppBarColors(containerColor = it, scrolledContainerColor = it) },
                scrollBehavior = scroll,
            )
        },
        bottomBar = { bottomBar?.invoke() },
    ) { inner ->
        CenteredColumn(maxContentWidth) { content(inner) }
    }
}

/** Feedback line plus the primary action, anchored at the bottom of an exercise. */
@Composable
fun ActionBar(
    label: String,
    feedback: String = "",
    kind: String = "",
    enabled: Boolean = true,
    leading: (@Composable () -> Unit)? = null,
    onClick: () -> Unit,
) {
    val palette = LocalPalette.current
    Surface(color = palette.paper) {
        Column(
            Modifier
                .fillMaxWidth()
                .drawBehind { drawLine(palette.line, Offset.Zero, Offset(size.width, 0f), 1.dp.toPx()) }
                .wrapContentWidth(Alignment.CenterHorizontally)
                .widthIn(max = ActionMaxWidth)
                .windowInsetsPadding(WindowInsets.navigationBars)
                .padding(horizontal = 16.dp, vertical = 12.dp),
        ) {
            AnimatedVisibility(
                visible = feedback.isNotEmpty(),
                enter = fadeIn(tween(180)) + expandVertically(),
                exit = fadeOut(tween(120)) + shrinkVertically(),
            ) {
                FeedbackLine(feedback, kind, animate = false)
                Spacer(Modifier.height(10.dp))
            }
            Row(horizontalArrangement = Arrangement.spacedBy(10.dp)) {
                leading?.invoke()
                Button(
                    onClick = onClick,
                    enabled = enabled,
                    shape = RoundedCornerShape(14.dp),
                    colors = solidButtonColors(),
                    modifier = Modifier.weight(1f).height(52.dp),
                ) {
                    Text(label, style = MaterialTheme.typography.titleMedium)
                }
            }
        }
    }
}

// ---------------------------------------------------------------------------
// Lists

/** Heading of a block inside a page, in the display serif. */
@Composable
fun SectionHeader(text: String) {
    Text(
        text,
        style = MaterialTheme.typography.headlineSmall,
        color = LocalPalette.current.ink,
        modifier = Modifier.padding(start = 16.dp, end = 16.dp, top = 24.dp, bottom = 10.dp),
    )
}

/**
 * Number or glyph on the subject's colour: the year of a course, a lesson's
 * place in the list. Filled for what is done or current, pale for what is ahead.
 */
@Composable
fun Badge(text: String? = null, icon: ImageVector? = null, filled: Boolean = true, size: Dp = 40.dp, accent: Accent = LocalAccent.current) {
    val ink = if (filled) accent.onSolid else accent.text
    Box(
        Modifier.size(size).clip(RoundedCornerShape(size * 0.3f)).background(if (filled) accent.solid else accent.soft),
        contentAlignment = Alignment.Center,
    ) {
        when {
            icon != null -> Icon(icon, null, tint = ink, modifier = Modifier.size(size * 0.5f))
            text != null -> Text(text, style = displayStyle(LocalDisplayFont.current, (size.value * 0.46f).sp), color = ink, maxLines = 1)
        }
    }
}

@Composable
fun LeadingBadge(text: String) = Badge(text = text)

@Composable
fun LeadingIcon(icon: ImageVector) = Badge(icon = icon)

/**
 * One navigable card: a badge on the left for the step's number and state
 * (done, pending, locked), the title, and optionally its progress.
 */
@Composable
fun NavRow(
    title: String,
    subtitle: String? = null,
    done: Boolean = false,
    locked: Boolean = false,
    index: Int? = null,
    current: Boolean = false,
    leading: (@Composable () -> Unit)? = null,
    progress: Pair<Int, Int>? = null,
    onClick: (() -> Unit)?,
) {
    val palette = LocalPalette.current
    val enabled = !locked && onClick != null
    val shownSubtitle = subtitle?.takeIf { it.isNotBlank() }
    val shownProgress = progress?.takeIf { it.second > 0 }
    AppCard(
        modifier = Modifier.fillMaxWidth().padding(horizontal = 16.dp, vertical = 5.dp),
        radius = 18.dp,
        onClick = if (enabled) onClick else null,
    ) {
        Row(Modifier.padding(horizontal = 14.dp, vertical = 12.dp), verticalAlignment = Alignment.CenterVertically) {
            when {
                leading != null -> leading()
                locked -> Badge(icon = Icons.Filled.Lock, filled = false, size = 36.dp)
                done -> Badge(icon = Icons.Filled.Check, size = 36.dp)
                index != null -> Badge(text = "$index", filled = current, size = 36.dp)
                else -> Badge(icon = Icons.AutoMirrored.Filled.KeyboardArrowRight, filled = false, size = 36.dp)
            }
            Spacer(Modifier.width(14.dp))
            Column(Modifier.weight(1f)) {
                Text(
                    title,
                    style = if (leading != null) MaterialTheme.typography.titleLarge else MaterialTheme.typography.bodyLarge,
                    fontWeight = if (current) FontWeight.Medium else null,
                    color = if (enabled) palette.ink else palette.ink3,
                )
                shownSubtitle?.let { Text(it, style = MaterialTheme.typography.bodyMedium, color = palette.ink2) }
                shownProgress?.let { (doneCount, total) ->
                    Row(Modifier.padding(top = 10.dp), verticalAlignment = Alignment.CenterVertically) {
                        SmoothProgress(doneCount / total.toFloat(), Modifier.weight(1f).height(6.dp))
                        Spacer(Modifier.width(10.dp))
                        Text("$doneCount / $total", style = MaterialTheme.typography.labelMedium, color = palette.ink2)
                    }
                }
            }
            if (enabled && leading != null) {
                Spacer(Modifier.width(8.dp))
                Icon(Icons.AutoMirrored.Filled.KeyboardArrowRight, null, tint = palette.ink3)
            }
        }
    }
}

// ---------------------------------------------------------------------------
// Exercise widgets

/** Fill and label of the one solid button of a screen, in the subject's colour. */
@Composable
fun solidButtonColors() = LocalAccent.current.let {
    ButtonDefaults.buttonColors(containerColor = it.solid, contentColor = it.onSolid)
}

@Composable
fun PrimaryButton(label: String, modifier: Modifier = Modifier, onClick: () -> Unit) {
    Button(onClick = onClick, modifier = modifier, shape = RoundedCornerShape(14.dp), colors = solidButtonColors()) { Text(label) }
}

/** Outlined companion of the solid button: "show the answer", "check for updates". */
@Composable
fun PillButton(label: String, modifier: Modifier = Modifier, onClick: () -> Unit) {
    val palette = LocalPalette.current
    OutlinedButton(
        onClick = onClick,
        modifier = modifier,
        shape = RoundedCornerShape(11.dp),
        border = BorderStroke(1.dp, palette.line),
        colors = ButtonDefaults.outlinedButtonColors(containerColor = palette.surface, contentColor = palette.ink),
    ) { Text(label) }
}

@Composable
fun SmoothProgress(
    progress: Float,
    modifier: Modifier = Modifier,
    color: Color = LocalAccent.current.solid,
    trackColor: Color = LocalPalette.current.sunken,
) {
    val animated by animateFloatAsState(
        targetValue = progress.coerceIn(0f, 1f),
        animationSpec = tween(durationMillis = 380),
        label = "progress",
    )
    Box(modifier.heightIn(min = 4.dp).clip(CircleShape).background(trackColor)) {
        if (animated > 0f) {
            Box(Modifier.fillMaxHeight().fillMaxWidth(animated).clip(CircleShape).background(color))
        }
    }
}

/**
 * All subjects on one line, each as wide as its share of the exercises and
 * filled as far as it is done.
 */
@Composable
fun SpectrumBar(parts: List<Triple<Accent, Int, Int>>, track: Color, modifier: Modifier = Modifier) {
    Row(modifier.height(10.dp), horizontalArrangement = Arrangement.spacedBy(3.dp)) {
        parts.filter { it.third > 0 }.forEach { (accent, done, total) ->
            SmoothProgress(done / total.toFloat(), Modifier.weight(total.toFloat()).fillMaxHeight(), accent.solid, track)
        }
    }
}

@Composable
fun FeedbackLine(text: String, kind: String, animate: Boolean = true) {
    val palette = LocalPalette.current
    val (color, soft) = when (kind) {
        "ok" -> palette.success to palette.successSoft
        "warn" -> palette.warning to palette.warningSoft
        else -> palette.error to palette.errorSoft
    }
    val icon = when (kind) {
        "ok" -> Icons.Filled.CheckCircle
        "warn" -> Icons.Filled.Warning
        else -> Icons.Filled.Cancel
    }
    if (!animate) {
        if (text.isEmpty()) return
        FeedbackRow(text, color, soft, icon)
        return
    }
    AnimatedVisibility(
        visible = text.isNotEmpty(),
        enter = fadeIn(tween(180)) + expandVertically(),
        exit = fadeOut(tween(120)) + shrinkVertically(),
    ) {
        FeedbackRow(text, color, soft, icon)
    }
}

@Composable
private fun FeedbackRow(text: String, color: Color, soft: Color, icon: ImageVector) {
    Row(
        Modifier.fillMaxWidth().clip(RoundedCornerShape(12.dp)).background(soft).padding(horizontal = 14.dp, vertical = 10.dp),
        verticalAlignment = Alignment.CenterVertically,
    ) {
        Icon(icon, null, tint = color, modifier = Modifier.size(20.dp))
        Spacer(Modifier.width(8.dp))
        Text(text, color = color, style = MaterialTheme.typography.titleSmall)
    }
}

/** Translation or explanation shown under an answer: a quiet note with a rule on its left. */
@Composable
fun Meaning(text: String, visible: Boolean) {
    val palette = LocalPalette.current
    AnimatedVisibility(
        visible = visible && text.isNotEmpty(),
        enter = fadeIn(tween(180)),
        exit = fadeOut(tween(120)),
    ) {
        Text(
            text,
            color = palette.ink2,
            style = MaterialTheme.typography.bodyMedium,
            modifier = Modifier
                .padding(top = 6.dp, bottom = 6.dp)
                .drawBehind { drawLine(palette.line, Offset(1.5.dp.toPx(), 0f), Offset(1.5.dp.toPx(), size.height), 3.dp.toPx()) }
                .padding(start = 12.dp),
        )
    }
}

@Composable
fun Hint(text: String) {
    Text(
        text,
        color = LocalPalette.current.ink2,
        style = MaterialTheme.typography.bodyMedium,
        modifier = Modifier.padding(bottom = 8.dp),
    )
}

/** One task of an exercise on its own card: its number, the prompt, then the answer controls. */
@Composable
fun QuestionCard(number: Int? = null, prompt: String? = null, mark: Boolean? = null, content: @Composable () -> Unit) {
    val palette = LocalPalette.current
    AppCard(
        modifier = Modifier.fillMaxWidth().padding(bottom = 12.dp),
        radius = 18.dp,
        border = when (mark) {
            true -> palette.success
            false -> palette.error
            null -> palette.line
        },
    ) {
        Column(Modifier.padding(16.dp)) {
            if (number != null || !prompt.isNullOrEmpty()) {
                Row(Modifier.padding(bottom = 10.dp)) {
                    if (number != null) {
                        Text(
                            "$number",
                            style = displayStyle(LocalDisplayFont.current, 19.sp),
                            color = LocalAccent.current.text,
                            modifier = Modifier.alignByBaseline().widthIn(min = 28.dp),
                        )
                    }
                    if (!prompt.isNullOrEmpty()) {
                        Text(prompt, style = MaterialTheme.typography.titleMedium, color = palette.ink, modifier = Modifier.alignByBaseline())
                    }
                }
            }
            content()
        }
    }
}

@Composable
fun WordField(
    value: String,
    onValue: (String) -> Unit,
    placeholder: String,
    mark: Boolean? = null,
    modifier: Modifier = Modifier,
    numeric: Boolean = false,
) {
    val palette = LocalPalette.current
    val edge = when (mark) {
        true -> palette.success
        false -> palette.error
        null -> palette.line
    }
    // Fields are set into the white task card, so they take the page's tone.
    val fill = when (mark) {
        true -> palette.successSoft
        false -> palette.errorSoft
        null -> palette.paper
    }
    OutlinedTextField(
        value = value,
        onValueChange = onValue,
        placeholder = if (placeholder.isNotEmpty()) ({ Text(placeholder, color = palette.ink3) }) else null,
        singleLine = true,
        isError = mark == false,
        shape = RoundedCornerShape(12.dp),
        colors = OutlinedTextFieldDefaults.colors(
            focusedContainerColor = fill,
            unfocusedContainerColor = fill,
            errorContainerColor = fill,
            focusedBorderColor = if (mark == null) LocalAccent.current.solid else edge,
            unfocusedBorderColor = edge,
            errorBorderColor = edge,
        ),
        trailingIcon = when (mark) {
            true -> ({ Icon(Icons.Filled.CheckCircle, null, tint = palette.success) })
            false -> ({ Icon(Icons.Filled.Cancel, null, tint = palette.error) })
            null -> null
        },
        keyboardOptions = KeyboardOptions(
            capitalization = KeyboardCapitalization.None,
            autoCorrectEnabled = false,
            keyboardType = if (numeric) KeyboardType.Decimal else KeyboardType.Text,
        ),
        modifier = modifier.fillMaxWidth().padding(vertical = 4.dp),
    )
}

/** Tappable word in a bank or a built sentence. */
@Composable
fun Chip(text: String, onClick: () -> Unit) = SegChip(text, selected = false, onClick = onClick)

/** A chip that can be the chosen one of a set: takes the subject's wash and outline. */
@Composable
fun SegChip(label: String, selected: Boolean, onClick: () -> Unit) {
    val palette = LocalPalette.current
    val accent = LocalAccent.current
    Surface(
        onClick = onClick,
        color = if (selected) accent.soft else palette.surface,
        shape = RoundedCornerShape(11.dp),
        border = BorderStroke(if (selected) 1.5.dp else 1.dp, if (selected) accent.solid else palette.line),
        modifier = Modifier.padding(vertical = 3.dp),
    ) {
        Text(
            label,
            style = MaterialTheme.typography.labelLarge,
            color = if (selected) accent.text else palette.ink,
            modifier = Modifier.padding(horizontal = 14.dp, vertical = 9.dp),
        )
    }
}

/**
 * One answer of a multiple choice: takes the subject's colour when picked,
 * then turns green or red once the answer is checked.
 */
@Composable
fun OptionRow(text: String, selected: Boolean, mark: Boolean?, enabled: Boolean = true, onClick: () -> Unit) {
    val palette = LocalPalette.current
    val accent = LocalAccent.current
    val fill = when (mark) {
        true -> palette.successSoft
        false -> palette.errorSoft
        null -> if (selected) accent.soft else palette.paper
    }
    val edge = when (mark) {
        true -> palette.success
        false -> palette.error
        null -> if (selected) accent.solid else palette.line
    }
    val container by animateColorAsState(fill, tween(180), label = "option")
    Surface(
        onClick = onClick,
        enabled = enabled,
        color = container,
        shape = RoundedCornerShape(14.dp),
        border = BorderStroke(if (selected || mark != null) 1.5.dp else 1.dp, edge),
        modifier = Modifier.fillMaxWidth().padding(bottom = 8.dp),
    ) {
        Row(Modifier.padding(horizontal = 14.dp, vertical = 12.dp), verticalAlignment = Alignment.CenterVertically) {
            Icon(
                when (mark) {
                    true -> Icons.Filled.CheckCircle
                    false -> Icons.Filled.Cancel
                    null -> if (selected) Icons.Filled.RadioButtonChecked else Icons.Filled.RadioButtonUnchecked
                },
                null,
                tint = if (mark == null && !selected) palette.ink3 else edge,
            )
            Spacer(Modifier.width(12.dp))
            Text(text, style = MaterialTheme.typography.bodyLarge, color = palette.ink, modifier = Modifier.weight(1f))
        }
    }
}

/** Drop zone for word ordering; visible even without words. */
@OptIn(ExperimentalLayoutApi::class)
@Composable
fun WordSlot(empty: Boolean, content: @Composable () -> Unit) {
    val palette = LocalPalette.current
    Surface(
        color = palette.sunken,
        shape = RoundedCornerShape(14.dp),
        modifier = Modifier.fillMaxWidth().padding(bottom = 8.dp),
    ) {
        FlowRow(
            Modifier.fillMaxWidth().heightIn(min = 52.dp).padding(8.dp),
            horizontalArrangement = Arrangement.spacedBy(6.dp),
            verticalArrangement = Arrangement.spacedBy(2.dp),
        ) {
            if (empty) Text("…", color = palette.ink3, modifier = Modifier.padding(8.dp))
            content()
        }
    }
}

/** Bottom sheet with the word bank for combo/dialogue blanks. */
@OptIn(ExperimentalMaterial3Api::class)
@Composable
fun WordPicker(title: String, pool: List<String>, onPick: (String?) -> Unit) {
    ModalBottomSheet(onDismissRequest = { onPick(null) }, containerColor = LocalPalette.current.surface) {
        Text(
            title,
            style = MaterialTheme.typography.titleLarge,
            modifier = Modifier.padding(horizontal = 16.dp, vertical = 8.dp),
        )
        LazyColumn(contentPadding = PaddingValues(bottom = 24.dp)) {
            items(pool.size) { i ->
                ListItem(
                    headlineContent = { Text(pool[i]) },
                    colors = ListItemDefaults.colors(containerColor = Color.Transparent),
                    modifier = Modifier.clickable { onPick(pool[i]) },
                )
            }
        }
    }
}
