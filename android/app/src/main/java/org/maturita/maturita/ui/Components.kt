package org.maturita.maturita.ui

import androidx.compose.foundation.background
import androidx.compose.foundation.border
import androidx.compose.foundation.clickable
import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.RowScope
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.size
import androidx.compose.foundation.shape.CircleShape
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.foundation.text.BasicTextField
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.draw.clip
import androidx.compose.material3.Typography
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.graphics.SolidColor
import androidx.compose.ui.text.TextStyle
import androidx.compose.ui.text.font.FontFamily
import androidx.compose.ui.text.font.FontSynthesis
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.text.style.TextAlign
import androidx.compose.ui.unit.TextUnit
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import org.maturita.maturita.AppViewModel
import org.maturita.maturita.data.Palette

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
        lineHeight = fontSize * 1.4f,
        letterSpacing = 0.sp,
        textAlign = textAlign,
    )
}

fun appTypography(): Typography {
    val base = appTextStyle(16.sp)
    return Typography(
        bodyLarge = base,
        bodyMedium = appTextStyle(14.sp),
        bodySmall = appTextStyle(12.sp),
        titleLarge = appTextStyle(22.sp, FontWeight.Medium),
        titleMedium = appTextStyle(18.sp, FontWeight.Medium),
        titleSmall = appTextStyle(16.sp, FontWeight.Medium),
        labelLarge = appTextStyle(14.sp, FontWeight.Medium),
        labelMedium = appTextStyle(12.sp, FontWeight.Medium),
        headlineMedium = appTextStyle(24.sp, FontWeight.Medium),
    )
}

private val Pill = RoundedCornerShape(999.dp)
private val Card = RoundedCornerShape(20.dp)

@Composable
fun PrimaryButton(label: String, palette: Palette, modifier: Modifier = Modifier, onClick: () -> Unit) {
    Box(
        modifier
            .clip(Pill)
            .background(palette.accent)
            .clickable(onClick = onClick)
            .padding(horizontal = 22.dp, vertical = 12.dp),
        contentAlignment = Alignment.Center,
    ) {
        Text(label, style = appTextStyle(15.sp, FontWeight.Medium, color = palette.onAccent))
    }
}

@Composable
fun PillButton(label: String, palette: Palette, modifier: Modifier = Modifier, onClick: () -> Unit) {
    Box(
        modifier
            .clip(Pill)
            .background(palette.text.copy(alpha = 0.06f))
            .clickable(onClick = onClick)
            .padding(horizontal = 16.dp, vertical = 8.dp),
    ) {
        Text(label, style = appTextStyle(13.sp, FontWeight.Medium, color = palette.text))
    }
}

@Composable
fun IconHit(onClick: () -> Unit, content: @Composable () -> Unit) {
    Box(
        Modifier
            .size(40.dp)
            .clip(CircleShape)
            .clickable(onClick = onClick),
        contentAlignment = Alignment.Center,
    ) { content() }
}

@Composable
fun PageTop(vm: AppViewModel, back: () -> Unit, title: String, subtitle: String? = null) {
    val p = vm.palette
    Column(Modifier.fillMaxWidth().padding(top = 4.dp, bottom = 14.dp)) {
        Box(Modifier.fillMaxWidth(), contentAlignment = Alignment.Center) {
            Box(Modifier.align(Alignment.CenterStart)) {
                IconHit(back) { BackIcon(p.text) }
            }
            Text(
                title,
                modifier = Modifier.padding(horizontal = 48.dp),
                style = appTextStyle(22.sp, FontWeight.Medium, TextAlign.Center, p.text),
            )
        }
        if (!subtitle.isNullOrBlank()) {
            Text(
                subtitle,
                modifier = Modifier.fillMaxWidth().padding(top = 6.dp),
                style = appTextStyle(14.sp, textAlign = TextAlign.Center, color = p.subtext),
            )
        }
    }
}

@Composable
fun CardBox(palette: Palette, modifier: Modifier = Modifier, content: @Composable () -> Unit) {
    Box(
        modifier
            .clip(Card)
            .background(palette.mantle)
            .border(1.dp, palette.text.copy(alpha = 0.07f), Card)
            .padding(20.dp),
    ) { content() }
}

@Composable
fun FeedbackLine(text: String, kind: String, palette: Palette) {
    if (text.isEmpty()) return
    val color = when (kind) {
        "ok" -> palette.success
        "warn" -> palette.warning
        else -> palette.error
    }
    Text(text, color = color, fontWeight = FontWeight.SemiBold, fontSize = 15.sp, modifier = Modifier.padding(top = 8.dp))
}

@Composable
fun Meaning(text: String, palette: Palette, visible: Boolean) {
    if (!visible || text.isEmpty()) return
    Text(text, color = palette.subtext, fontSize = 13.sp, modifier = Modifier.padding(bottom = 8.dp))
}

@Composable
fun WordField(
    value: String,
    onValue: (String) -> Unit,
    palette: Palette,
    placeholder: String,
    mark: Boolean? = null,
    modifier: Modifier = Modifier,
) {
    val border = when (mark) {
        true -> palette.success
        false -> palette.error
        null -> palette.text.copy(alpha = 0.12f)
    }
    Box(
        modifier
            .clip(RoundedCornerShape(14.dp))
            .background(palette.surface0)
            .border(1.dp, border, RoundedCornerShape(14.dp))
            .padding(horizontal = 14.dp, vertical = 12.dp)
            .fillMaxWidth(),
    ) {
        if (value.isEmpty()) Text(placeholder, color = palette.overlay, fontSize = 15.sp)
        BasicTextField(
            value = value,
            onValueChange = onValue,
            textStyle = TextStyle(color = palette.text, fontSize = 15.sp),
            cursorBrush = SolidColor(palette.accent),
            modifier = Modifier.fillMaxWidth(),
        )
    }
}

@Composable
fun Segmented(palette: Palette, content: @Composable RowScope.() -> Unit) {
    Row(
        Modifier
            .fillMaxWidth()
            .clip(Pill)
            .background(palette.surface1)
            .padding(3.dp),
        horizontalArrangement = Arrangement.spacedBy(4.dp),
        content = content,
    )
}

@Composable
fun SegChip(label: String, selected: Boolean, palette: Palette, modifier: Modifier = Modifier, onClick: () -> Unit) {
    Box(
        modifier
            .clip(Pill)
            .background(if (selected) palette.mantle else Color.Transparent)
            .clickable(onClick = onClick)
            .padding(vertical = 8.dp),
        contentAlignment = Alignment.Center,
    ) {
        Text(label, color = palette.text, fontWeight = FontWeight.SemiBold, fontSize = 14.sp)
    }
}

@Composable
fun Hint(text: String, palette: Palette) {
    Text(text, color = palette.subtext, fontSize = 13.sp, modifier = Modifier.padding(bottom = 8.dp))
}

@Composable
fun Prompt(text: String, palette: Palette) {
    Text(text, color = palette.text, fontSize = 16.sp, fontWeight = FontWeight.Medium)
}
