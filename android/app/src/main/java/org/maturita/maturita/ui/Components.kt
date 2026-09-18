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
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.graphics.SolidColor
import androidx.compose.ui.text.TextStyle
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.text.style.TextAlign
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import org.maturita.maturita.AppViewModel
import org.maturita.maturita.data.Palette

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
        Text(label, color = palette.onAccent, fontWeight = FontWeight.SemiBold, fontSize = 15.sp)
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
        Text(label, color = palette.text, fontWeight = FontWeight.SemiBold, fontSize = 13.sp)
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
    Box(Modifier.fillMaxWidth().padding(top = 4.dp, bottom = 14.dp)) {
        Box(Modifier.align(Alignment.CenterStart)) {
            IconHit(back) { BackIcon(p.text) }
        }
        Column(Modifier.align(Alignment.Center).padding(horizontal = 48.dp), horizontalAlignment = Alignment.CenterHorizontally) {
            Text(title, color = p.text, fontSize = 24.sp, fontWeight = FontWeight.Bold, textAlign = TextAlign.Center)
            if (!subtitle.isNullOrBlank()) {
                Text(subtitle, color = p.subtext, fontSize = 14.sp, textAlign = TextAlign.Center, modifier = Modifier.padding(top = 4.dp))
            }
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
