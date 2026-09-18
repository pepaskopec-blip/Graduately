package org.maturita.maturita.ui

import androidx.compose.foundation.clickable
import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.ExperimentalLayoutApi
import androidx.compose.foundation.layout.FlowRow
import androidx.compose.foundation.layout.PaddingValues
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.WindowInsets
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.heightIn
import androidx.compose.foundation.layout.navigationBars
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.size
import androidx.compose.foundation.layout.width
import androidx.compose.foundation.layout.windowInsetsPadding
import androidx.compose.foundation.lazy.LazyColumn
import androidx.compose.foundation.lazy.LazyListScope
import androidx.compose.foundation.text.KeyboardOptions
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.automirrored.filled.ArrowBack
import androidx.compose.material.icons.filled.Cancel
import androidx.compose.material.icons.filled.CheckCircle
import androidx.compose.material.icons.automirrored.filled.KeyboardArrowRight
import androidx.compose.material.icons.filled.Lock
import androidx.compose.material.icons.filled.RadioButtonUnchecked
import androidx.compose.material.icons.filled.Search
import androidx.compose.material.icons.filled.Warning
import androidx.compose.material3.Button
import androidx.compose.material3.ExperimentalMaterial3Api
import androidx.compose.material3.FilledTonalButton
import androidx.compose.material3.FilterChip
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
import androidx.compose.material3.RadioButton
import androidx.compose.material3.Scaffold
import androidx.compose.material3.SuggestionChip
import androidx.compose.material3.Surface
import androidx.compose.material3.Text
import androidx.compose.material3.TopAppBar
import androidx.compose.material3.TopAppBarDefaults
import androidx.compose.material3.Typography
import androidx.compose.material3.rememberTopAppBarState
import androidx.compose.runtime.Composable
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.input.nestedscroll.nestedScroll
import androidx.compose.ui.text.TextStyle
import androidx.compose.ui.text.font.FontFamily
import androidx.compose.ui.text.font.FontSynthesis
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

fun appTypography(): Typography = Typography(
    displaySmall = appTextStyle(34.sp, FontWeight.Medium),
    headlineLarge = appTextStyle(30.sp, FontWeight.Medium),
    headlineMedium = appTextStyle(26.sp, FontWeight.Medium),
    headlineSmall = appTextStyle(22.sp, FontWeight.Medium),
    titleLarge = appTextStyle(20.sp, FontWeight.Medium),
    titleMedium = appTextStyle(16.sp, FontWeight.Medium),
    titleSmall = appTextStyle(14.sp, FontWeight.Medium),
    bodyLarge = appTextStyle(16.sp),
    bodyMedium = appTextStyle(14.sp),
    bodySmall = appTextStyle(12.sp),
    labelLarge = appTextStyle(14.sp, FontWeight.Medium),
    labelMedium = appTextStyle(12.sp, FontWeight.Medium),
    labelSmall = appTextStyle(11.sp, FontWeight.Medium),
)

// ---------------------------------------------------------------------------
// Chrome

@Composable
fun AppNavBar(vm: AppViewModel) {
    val root = vm.root
    NavigationBar {
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

/** Tab root: large collapsing title, search action, bottom navigation. */
@OptIn(ExperimentalMaterial3Api::class)
@Composable
fun RootScaffold(
    vm: AppViewModel,
    title: String,
    showSearch: Boolean = true,
    content: LazyListScope.() -> Unit,
) {
    val scroll = TopAppBarDefaults.exitUntilCollapsedScrollBehavior(rememberTopAppBarState())
    Scaffold(
        modifier = Modifier.nestedScroll(scroll.nestedScrollConnection),
        topBar = {
            LargeTopAppBar(
                title = { Text(title, maxLines = 1, overflow = TextOverflow.Ellipsis) },
                actions = {
                    if (showSearch) {
                        IconButton(onClick = { vm.searchOpen = true }) {
                            Icon(Icons.Filled.Search, contentDescription = vm.tr("search_placeholder"))
                        }
                    }
                },
                scrollBehavior = scroll,
            )
        },
        bottomBar = { AppNavBar(vm) },
    ) { inner ->
        LazyColumn(
            contentPadding = PaddingValues(
                top = inner.calculateTopPadding(),
                bottom = inner.calculateBottomPadding() + 16.dp,
            ),
        ) { content() }
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
    content: @Composable (PaddingValues) -> Unit,
) {
    val scroll = TopAppBarDefaults.pinnedScrollBehavior(rememberTopAppBarState())
    Scaffold(
        modifier = Modifier.nestedScroll(scroll.nestedScrollConnection),
        topBar = {
            TopAppBar(
                title = {
                    Column {
                        Text(title, maxLines = 1, overflow = TextOverflow.Ellipsis, style = MaterialTheme.typography.titleLarge)
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
                scrollBehavior = scroll,
            )
        },
        bottomBar = { bottomBar?.invoke() },
        content = content,
    )
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
    Surface(color = MaterialTheme.colorScheme.surfaceContainer, tonalElevation = 2.dp) {
        Column(
            Modifier
                .fillMaxWidth()
                .windowInsetsPadding(WindowInsets.navigationBars)
                .padding(horizontal = 16.dp, vertical = 12.dp),
        ) {
            if (feedback.isNotEmpty()) {
                FeedbackLine(feedback, kind)
                Spacer(Modifier.height(10.dp))
            }
            Row(horizontalArrangement = Arrangement.spacedBy(10.dp)) {
                leading?.invoke()
                Button(onClick = onClick, enabled = enabled, modifier = Modifier.weight(1f).height(50.dp)) {
                    Text(label, style = MaterialTheme.typography.titleMedium)
                }
            }
        }
    }
}

// ---------------------------------------------------------------------------
// Lists

@Composable
fun SectionHeader(text: String) {
    Text(
        text,
        style = MaterialTheme.typography.titleSmall,
        color = MaterialTheme.colorScheme.primary,
        modifier = Modifier.padding(start = 16.dp, end = 16.dp, top = 20.dp, bottom = 6.dp),
    )
}

/** One navigable row: done/pending/locked state on the left, chevron on the right. */
@Composable
fun NavRow(
    title: String,
    subtitle: String? = null,
    done: Boolean = false,
    locked: Boolean = false,
    leading: (@Composable () -> Unit)? = null,
    onClick: (() -> Unit)?,
) {
    val enabled = !locked && onClick != null
    val textColor = if (enabled) MaterialTheme.colorScheme.onSurface else MaterialTheme.colorScheme.onSurfaceVariant
    val rowModifier = if (enabled) Modifier.clickable(onClick = onClick!!) else Modifier
    ListItem(
        headlineContent = { Text(title, color = textColor) },
        supportingContent = subtitle?.takeIf { it.isNotBlank() }?.let {
            { Text(it, color = MaterialTheme.colorScheme.onSurfaceVariant) }
        },
        leadingContent = {
            when {
                leading != null -> leading()
                locked -> Icon(Icons.Filled.Lock, null, tint = MaterialTheme.colorScheme.outline)
                done -> Icon(Icons.Filled.CheckCircle, null, tint = MaterialTheme.colorScheme.primary)
                else -> Icon(Icons.Filled.RadioButtonUnchecked, null, tint = MaterialTheme.colorScheme.outline)
            }
        },
        trailingContent = if (enabled) {
            { Icon(Icons.AutoMirrored.Filled.KeyboardArrowRight, null, tint = MaterialTheme.colorScheme.outline) }
        } else null,
        colors = ListItemDefaults.colors(containerColor = Color.Transparent),
        modifier = rowModifier,
    )
}

// ---------------------------------------------------------------------------
// Exercise widgets

@Composable
fun PrimaryButton(label: String, modifier: Modifier = Modifier, onClick: () -> Unit) {
    Button(onClick = onClick, modifier = modifier) { Text(label) }
}

@Composable
fun PillButton(label: String, modifier: Modifier = Modifier, onClick: () -> Unit) {
    FilledTonalButton(onClick = onClick, modifier = modifier) { Text(label) }
}

@Composable
fun FeedbackLine(text: String, kind: String) {
    if (text.isEmpty()) return
    val color = when (kind) {
        "ok" -> MaterialTheme.colorScheme.primary
        "warn" -> MaterialTheme.colorScheme.tertiary
        else -> MaterialTheme.colorScheme.error
    }
    val icon = when (kind) {
        "ok" -> Icons.Filled.CheckCircle
        "warn" -> Icons.Filled.Warning
        else -> Icons.Filled.Cancel
    }
    Row(verticalAlignment = Alignment.CenterVertically) {
        Icon(icon, null, tint = color, modifier = Modifier.size(20.dp))
        Spacer(Modifier.width(8.dp))
        Text(text, color = color, style = MaterialTheme.typography.titleSmall)
    }
}

@Composable
fun Meaning(text: String, visible: Boolean) {
    if (!visible || text.isEmpty()) return
    Text(
        text,
        color = MaterialTheme.colorScheme.onSurfaceVariant,
        style = MaterialTheme.typography.bodySmall,
        modifier = Modifier.padding(bottom = 8.dp),
    )
}

@Composable
fun Hint(text: String) {
    Text(
        text,
        color = MaterialTheme.colorScheme.onSurfaceVariant,
        style = MaterialTheme.typography.bodyMedium,
        modifier = Modifier.padding(bottom = 8.dp),
    )
}

@Composable
fun Prompt(text: String) {
    Text(text, style = MaterialTheme.typography.titleMedium, modifier = Modifier.padding(top = 4.dp, bottom = 4.dp))
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
    OutlinedTextField(
        value = value,
        onValueChange = onValue,
        placeholder = if (placeholder.isNotEmpty()) ({ Text(placeholder) }) else null,
        singleLine = true,
        isError = mark == false,
        trailingIcon = when (mark) {
            true -> ({ Icon(Icons.Filled.CheckCircle, null, tint = MaterialTheme.colorScheme.primary) })
            false -> ({ Icon(Icons.Filled.Cancel, null, tint = MaterialTheme.colorScheme.error) })
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
fun Chip(text: String, onClick: () -> Unit) {
    SuggestionChip(onClick = onClick, label = { Text(text) })
}

@Composable
fun SegChip(label: String, selected: Boolean, onClick: () -> Unit) {
    FilterChip(selected = selected, onClick = onClick, label = { Text(label) })
}

/** Multiple-choice answer with radio state and, after checking, a correct/wrong mark. */
@Composable
fun OptionRow(text: String, selected: Boolean, mark: Boolean?, enabled: Boolean = true, onClick: () -> Unit) {
    val container = when (mark) {
        true -> MaterialTheme.colorScheme.primaryContainer
        false -> MaterialTheme.colorScheme.errorContainer
        null -> if (selected) MaterialTheme.colorScheme.secondaryContainer else MaterialTheme.colorScheme.surfaceContainerHigh
    }
    Surface(
        onClick = onClick,
        enabled = enabled,
        color = container,
        shape = MaterialTheme.shapes.large,
        modifier = Modifier.fillMaxWidth().padding(bottom = 8.dp),
    ) {
        Row(Modifier.padding(horizontal = 12.dp, vertical = 10.dp), verticalAlignment = Alignment.CenterVertically) {
            when (mark) {
                true -> Icon(Icons.Filled.CheckCircle, null, tint = MaterialTheme.colorScheme.primary)
                false -> Icon(Icons.Filled.Cancel, null, tint = MaterialTheme.colorScheme.error)
                null -> RadioButton(selected = selected, onClick = null)
            }
            Spacer(Modifier.width(12.dp))
            Text(text, style = MaterialTheme.typography.bodyLarge, modifier = Modifier.weight(1f))
        }
    }
}

/** Drop zone for word ordering; visible even without words. */
@OptIn(ExperimentalLayoutApi::class)
@Composable
fun WordSlot(empty: Boolean, content: @Composable () -> Unit) {
    Surface(
        color = MaterialTheme.colorScheme.surfaceContainerHigh,
        shape = MaterialTheme.shapes.large,
        modifier = Modifier.fillMaxWidth().padding(bottom = 8.dp),
    ) {
        FlowRow(
            Modifier.fillMaxWidth().heightIn(min = 52.dp).padding(8.dp),
            horizontalArrangement = Arrangement.spacedBy(6.dp),
            verticalArrangement = Arrangement.spacedBy(4.dp),
        ) {
            if (empty) Text("…", color = MaterialTheme.colorScheme.outline, modifier = Modifier.padding(8.dp))
            content()
        }
    }
}

/** Bottom sheet with the word bank for combo/dialogue blanks. */
@OptIn(ExperimentalMaterial3Api::class)
@Composable
fun WordPicker(title: String, pool: List<String>, onPick: (String?) -> Unit) {
    ModalBottomSheet(onDismissRequest = { onPick(null) }) {
        Text(
            title,
            style = MaterialTheme.typography.titleMedium,
            modifier = Modifier.padding(horizontal = 16.dp, vertical = 8.dp),
        )
        LazyColumn(contentPadding = PaddingValues(bottom = 24.dp)) {
            items(pool.size) { i ->
                ListItem(
                    headlineContent = { Text(pool[i]) },
                    modifier = Modifier.clickable { onPick(pool[i]) },
                )
            }
        }
    }
}
