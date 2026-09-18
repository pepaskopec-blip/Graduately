package org.maturita.maturita.ui

import androidx.compose.foundation.Canvas
import androidx.compose.foundation.background
import androidx.compose.foundation.border
import androidx.compose.foundation.clickable
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.BoxWithConstraints
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.offset
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.size
import androidx.compose.foundation.layout.width
import androidx.compose.foundation.rememberScrollState
import androidx.compose.foundation.shape.CircleShape
import androidx.compose.foundation.horizontalScroll
import androidx.compose.foundation.verticalScroll
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.draw.clip
import androidx.compose.ui.geometry.Offset
import androidx.compose.ui.graphics.Path
import androidx.compose.ui.graphics.StrokeCap
import androidx.compose.ui.graphics.StrokeJoin
import androidx.compose.ui.graphics.drawscope.Stroke
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.text.style.TextAlign
import androidx.compose.ui.platform.LocalDensity
import androidx.compose.ui.unit.IntOffset
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import org.maturita.maturita.data.Palette
import kotlin.math.PI
import kotlin.math.min
import kotlin.math.sin

data class MapNode(
    val id: String,
    val label: String,
    val locked: Boolean,
    val done: Boolean,
    val current: Boolean,
    val finish: Boolean = false,
    val onClick: (() -> Unit)? = null,
)

data class NodePos(val x: Float, val y: Float)

data class Serpentine(
    val points: List<NodePos>,
    val width: Float,
    val height: Float,
    val litUntil: Float,
)

fun serpentine(
    n: Int,
    avail: Float,
    mx: Float,
    my: Float,
    spac: Float,
    gap: Float,
    wave: Float,
    lift: Float = 0f,
    minScale: Float = 0.55f,
): Serpentine {
    if (n < 1) return Serpentine(emptyList(), avail, my * 2, 0f)
    val scale = min(1f, maxOf(minScale, avail / (2 * mx + maxOf(n - 1, 0) * spac)))
    val smx = mx * scale
    val smy = my * scale
    val sspac = spac * scale
    val sgap = gap * scale
    val swave = wave * scale
    val slift = lift * scale
    var rows = 1
    while (rows <= n) {
        val cols = (n + rows - 1) / rows
        if (2 * smx + (cols - 1) * sspac <= avail + 1) break
        rows++
    }
    rows = min(rows, n)
    val cols = maxOf(1, (n + rows - 1) / rows)
    val cw = 2 * smx + (cols - 1) * sspac
    val ch = 2 * smy + (rows - 1) * sgap + slift
    val phase = (2.0 * PI * 1.7 / maxOf(n - 1, 1)).toFloat()
    val pts = MutableList(n) { NodePos(0f, 0f) }
    for (r in 0 until rows) {
        val base = r * cols
        val len = min(cols, n - base)
        val fwd = r % 2 == 0
        for (c in 0 until len) {
            val i = base + c
            val cc = if (fwd) c else cols - 1 - c
            pts[i] = NodePos(
                smx + cc * sspac,
                smy + slift + r * sgap + swave * sin(i * phase),
            )
        }
    }
    return Serpentine(pts, cw, ch, 0f)
}

private fun catmull(pts: List<NodePos>, t: Float): NodePos {
    val n = pts.size
    if (n == 1) return pts[0]
    var k = t.toInt()
    var u = t - k
    if (k < 0) { k = 0; u = 0f }
    if (k >= n - 1) { k = n - 2; u = 1f }
    val p1 = pts[k]
    val p2 = pts[k + 1]
    val p0 = if (k - 1 >= 0) pts[k - 1] else NodePos(p1.x - (p2.x - p1.x), p1.y - (p2.y - p1.y))
    val p3 = if (k + 2 < n) pts[k + 2] else NodePos(p2.x + (p2.x - p1.x), p2.y + (p2.y - p1.y))
    val u2 = u * u
    val u3 = u2 * u
    val x = 0.5f * (2 * p1.x + (-p0.x + p2.x) * u + (2 * p0.x - 5 * p1.x + 4 * p2.x - p3.x) * u2 + (-p0.x + 3 * p1.x - 3 * p2.x + p3.x) * u3)
    val y = 0.5f * (2 * p1.y + (-p0.y + p2.y) * u + (2 * p0.y - 5 * p1.y + 4 * p2.y - p3.y) * u2 + (-p0.y + 3 * p1.y - 3 * p2.y + p3.y) * u3)
    return NodePos(x, y)
}

@Composable
fun PathMap(
    nodes: List<MapNode>,
    palette: Palette,
    litUntil: Float,
    nodeSize: Float = 72f,
    mx: Float = 90f,
    my: Float = 110f,
    spac: Float = 170f,
    gap: Float = 170f,
    wave: Float = 28f,
    lift: Float = 0f,
    extra: List<Pair<NodePos, @Composable () -> Unit>> = emptyList(),
    extraPoints: List<NodePos> = emptyList(),
) {
    if (nodes.isEmpty()) return
    val density = LocalDensity.current
    fun Float.toDp(): androidx.compose.ui.unit.Dp = with(density) { this@toDp.toDp() }
    BoxWithConstraints(Modifier.fillMaxSize()) {
        val avail = constraints.maxWidth.toFloat()
        val geo = serpentine(nodes.size, avail, mx, my, spac, gap, wave, lift)
        Box(
            Modifier
                .horizontalScroll(rememberScrollState())
                .verticalScroll(rememberScrollState()),
        ) {
            Box(Modifier.width(geo.width.toDp()).height((geo.height + 80f).toDp())) {
                Canvas(Modifier.fillMaxSize()) {
                    if (geo.points.size < 2) return@Canvas
                    fun pathBetween(t0: Float, t1: Float): Path {
                        val p = Path()
                        val steps = maxOf(1, ((t1 - t0) * 48).toInt())
                        val start = catmull(geo.points, t0)
                        p.moveTo(start.x, start.y)
                        for (s in 1..steps) {
                            val t = t0 + (t1 - t0) * s / steps
                            val pt = catmull(geo.points, t)
                            p.lineTo(pt.x, pt.y)
                        }
                        return p
                    }
                    val end = (nodes.size - 1).toFloat()
                    drawPath(pathBetween(0f, end), palette.rail, style = Stroke(12f, cap = StrokeCap.Round, join = StrokeJoin.Round))
                    if (litUntil > 0f) {
                        drawPath(pathBetween(0f, litUntil), palette.accent, style = Stroke(12f, cap = StrokeCap.Round, join = StrokeJoin.Round))
                    }
                    extraPoints.forEach { }
                }
                nodes.forEachIndexed { i, node ->
                    val pt = geo.points[i]
                    Box(
                        Modifier
                            .offset { IntOffset((pt.x - nodeSize / 2).toInt(), (pt.y - nodeSize / 2).toInt()) }
                            .size(nodeSize.toDp()),
                        contentAlignment = Alignment.Center,
                    ) {
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
                                .fillMaxSize()
                                .clip(CircleShape)
                                .background(bg)
                                .then(
                                    if (node.locked) Modifier.border(1.dp, palette.lockedBorder, CircleShape)
                                    else Modifier
                                )
                                .then(if (!node.locked && node.onClick != null) Modifier.clickable { node.onClick.invoke() } else Modifier),
                            contentAlignment = Alignment.Center,
                        ) {
                            if (node.finish) {
                                FinishFlag(palette, "${i + 1}", nodeSize.toDp())
                            } else {
                                androidx.compose.foundation.layout.Column(horizontalAlignment = Alignment.CenterHorizontally) {
                                    Text("${i + 1}", color = fg, fontWeight = FontWeight.Black, fontSize = 18.sp)
                                    if (node.locked) LockIcon(palette.overlay, 14.dp)
                                    else if (node.done) CheckIcon(palette.onAccent, 14.dp)
                                }
                            }
                        }
                    }
                    Text(
                        node.label,
                        color = if (node.locked) palette.overlay else palette.text,
                        fontSize = 12.sp,
                        fontWeight = FontWeight.SemiBold,
                        textAlign = TextAlign.Center,
                        modifier = Modifier
                            .offset { IntOffset((pt.x - 70).toInt(), (pt.y + nodeSize / 2 + 8).toInt()) }
                            .width(140.dp),
                    )
                }
                extra.forEach { (pt, content) ->
                    Box(Modifier.offset { IntOffset(pt.x.toInt(), pt.y.toInt()) }) { content() }
                }
            }
        }
    }
}

fun branchPoint(points: List<NodePos>, index: Int = 1, dx: Float = 40f, lift: Float = 110f): NodePos? {
    if (index !in points.indices) return null
    val p = points[index]
    return NodePos(p.x + dx, p.y - lift)
}
