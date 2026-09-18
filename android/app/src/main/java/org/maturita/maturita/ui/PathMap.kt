package org.maturita.maturita.ui

import androidx.compose.foundation.background
import androidx.compose.foundation.clickable
import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.size
import androidx.compose.foundation.layout.width
import androidx.compose.foundation.rememberScrollState
import androidx.compose.foundation.shape.CircleShape
import androidx.compose.foundation.verticalScroll
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.draw.alpha
import androidx.compose.ui.draw.clip
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import org.maturita.maturita.data.Palette

data class MapNode(
    val id: String,
    val label: String,
    val locked: Boolean,
    val done: Boolean,
    val current: Boolean,
    val finish: Boolean = false,
    val onClick: (() -> Unit)? = null,
)

@Composable
fun PathMap(
    nodes: List<MapNode>,
    palette: Palette,
    @Suppress("UNUSED_PARAMETER") litUntil: Float,
    modifier: Modifier = Modifier,
    @Suppress("UNUSED_PARAMETER") nodeSize: Float = 72f,
    @Suppress("UNUSED_PARAMETER") mx: Float = 90f,
    @Suppress("UNUSED_PARAMETER") my: Float = 110f,
    @Suppress("UNUSED_PARAMETER") spac: Float = 170f,
    @Suppress("UNUSED_PARAMETER") gap: Float = 170f,
    @Suppress("UNUSED_PARAMETER") wave: Float = 28f,
    @Suppress("UNUSED_PARAMETER") lift: Float = 0f,
) {
    Column(
        modifier
            .fillMaxSize()
            .verticalScroll(rememberScrollState())
            .padding(bottom = 24.dp),
        verticalArrangement = Arrangement.spacedBy(10.dp),
    ) {
        nodes.forEachIndexed { i, node ->
            val click = node.onClick
            CardBox(
                palette,
                Modifier
                    .fillMaxWidth()
                    .alpha(if (node.locked) 0.48f else 1f)
                    .then(
                        if (!node.locked && click != null) Modifier.clickable { click() }
                        else Modifier,
                    ),
            ) {
                Row(verticalAlignment = Alignment.CenterVertically) {
                    val bg = when {
                        node.locked -> palette.lockedBg
                        node.current || node.done -> palette.accent
                        else -> palette.node
                    }
                    val fg = when {
                        node.locked -> palette.overlay
                        node.current || node.done -> palette.onAccent
                        else -> palette.text
                    }
                    Box(
                        Modifier
                            .size(44.dp)
                            .clip(CircleShape)
                            .background(bg),
                        contentAlignment = Alignment.Center,
                    ) {
                        when {
                            node.locked -> LockIcon(palette.overlay, 16.dp)
                            node.done -> CheckIcon(palette.onAccent, 16.dp)
                            else -> Text("${i + 1}", color = fg, fontWeight = FontWeight.Black, fontSize = 16.sp)
                        }
                    }
                    Spacer(Modifier.width(12.dp))
                    Text(
                        node.label,
                        color = if (node.locked) palette.overlay else palette.text,
                        fontWeight = if (node.current) FontWeight.Bold else FontWeight.SemiBold,
                        fontSize = 16.sp,
                        modifier = Modifier.weight(1f),
                    )
                }
            }
        }
    }
}
