package org.maturita.maturita.ui

import androidx.compose.foundation.Canvas
import androidx.compose.foundation.layout.size
import androidx.compose.runtime.Composable
import androidx.compose.ui.Modifier
import androidx.compose.ui.geometry.Offset
import androidx.compose.ui.geometry.Size
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.graphics.Path
import androidx.compose.ui.graphics.PathFillType
import androidx.compose.ui.graphics.StrokeCap
import androidx.compose.ui.graphics.drawscope.Stroke
import androidx.compose.ui.unit.Dp
import androidx.compose.ui.unit.dp
@Composable
fun DrawnIcon(iconSize: Dp, modifier: Modifier = Modifier, onDraw: androidx.compose.ui.graphics.drawscope.DrawScope.() -> Unit) {
    Canvas(modifier.size(iconSize)) { onDraw() }
}

@Composable
fun BackIcon(color: Color, iconSize: Dp = 16.dp) {
    DrawnIcon(iconSize) {
        val p = Path().apply {
            moveTo(size.width * 0.68f, size.height * 0.18f)
            lineTo(size.width * 0.28f, size.height * 0.5f)
            lineTo(size.width * 0.68f, size.height * 0.82f)
        }
        drawPath(p, color, style = Stroke(width = size.minDimension * 0.16f, cap = StrokeCap.Round))
    }
}

@Composable
fun SearchIcon(color: Color, iconSize: Dp = 18.dp) {
    DrawnIcon(iconSize) {
        val r = size.minDimension * 0.28f
        drawCircle(color, r, Offset(size.width * 0.42f, size.height * 0.42f), style = Stroke(size.minDimension * 0.12f))
        drawLine(color, Offset(size.width * 0.62f, size.height * 0.62f), Offset(size.width * 0.82f, size.height * 0.82f), size.minDimension * 0.12f, StrokeCap.Round)
    }
}

@Composable
fun StatsIcon(color: Color, iconSize: Dp = 18.dp) {
    DrawnIcon(iconSize) {
        val w = size.minDimension * 0.16f
        drawRoundRect(color, Offset(size.width * 0.18f, size.height * 0.48f), Size(w, size.height * 0.36f), androidx.compose.ui.geometry.CornerRadius(w))
        drawRoundRect(color, Offset(size.width * 0.42f, size.height * 0.22f), Size(w, size.height * 0.62f), androidx.compose.ui.geometry.CornerRadius(w))
        drawRoundRect(color, Offset(size.width * 0.66f, size.height * 0.36f), Size(w, size.height * 0.48f), androidx.compose.ui.geometry.CornerRadius(w))
    }
}

@Composable
fun SubjectsIcon(color: Color, iconSize: Dp = 22.dp) {
    DrawnIcon(iconSize) {
        val gap = size.minDimension * 0.08f
        val cell = (size.minDimension - gap) / 2f
        val r = androidx.compose.ui.geometry.CornerRadius(cell * 0.28f)
        for (iy in 0 until 2) for (ix in 0 until 2) {
            drawRoundRect(color, Offset(ix * (cell + gap), iy * (cell + gap)), Size(cell, cell), r)
        }
    }
}

@Composable
fun SettingsIcon(color: Color, iconSize: Dp = 18.dp) {
    DrawnIcon(iconSize) {
        val cx = size.width / 2f
        val cy = size.height / 2f
        val s = size.minDimension
        val rOuter = s * 0.48f
        val rInner = s * 0.34f
        val rHole = s * 0.16f
        val teeth = 8
        val toothHalf = (Math.PI / teeth).toFloat()
        val gear = Path().apply {
            fillType = PathFillType.EvenOdd
            for (i in 0 until teeth) {
                val a = (-Math.PI / 2.0 + 2.0 * Math.PI * i / teeth).toFloat()
                val a0 = a - toothHalf * 0.55f
                val a1 = a - toothHalf * 0.28f
                val a2 = a + toothHalf * 0.28f
                val a3 = a + toothHalf * 0.55f
                val p0 = Offset(cx + kotlin.math.cos(a0) * rInner, cy + kotlin.math.sin(a0) * rInner)
                if (i == 0) moveTo(p0.x, p0.y) else lineTo(p0.x, p0.y)
                lineTo(cx + kotlin.math.cos(a1) * rOuter, cy + kotlin.math.sin(a1) * rOuter)
                lineTo(cx + kotlin.math.cos(a2) * rOuter, cy + kotlin.math.sin(a2) * rOuter)
                lineTo(cx + kotlin.math.cos(a3) * rInner, cy + kotlin.math.sin(a3) * rInner)
            }
            close()
            addOval(
                androidx.compose.ui.geometry.Rect(
                    cx - rHole, cy - rHole, cx + rHole, cy + rHole,
                ),
            )
        }
        drawPath(gear, color)
    }
}

@Composable
fun LockIcon(color: Color, iconSize: Dp = 18.dp) {
    DrawnIcon(iconSize) {
        val body = Path().apply {
            addRoundRect(
                androidx.compose.ui.geometry.RoundRect(
                    size.width * 0.28f, size.height * 0.46f,
                    size.width * 0.72f, size.height * 0.82f,
                    androidx.compose.ui.geometry.CornerRadius(size.minDimension * 0.08f),
                ),
            )
        }
        drawPath(body, color)
        drawArc(
            color,
            200f, 140f, false,
            Offset(size.width * 0.32f, size.height * 0.18f),
            Size(size.width * 0.36f, size.height * 0.4f),
            style = Stroke(size.minDimension * 0.1f, cap = StrokeCap.Round),
        )
    }
}

@Composable
fun CheckIcon(color: Color, iconSize: Dp = 16.dp) {
    DrawnIcon(iconSize) {
        val p = Path().apply {
            moveTo(size.width * 0.18f, size.height * 0.52f)
            lineTo(size.width * 0.4f, size.height * 0.74f)
            lineTo(size.width * 0.84f, size.height * 0.26f)
        }
        drawPath(p, color, style = Stroke(size.minDimension * 0.16f, cap = StrokeCap.Round))
    }
}

@Composable
fun BookIcon(color: Color, iconSize: Dp = 22.dp) {
    DrawnIcon(iconSize) {
        drawRoundRect(color, Offset(size.width * 0.22f, size.height * 0.18f), Size(size.width * 0.56f, size.height * 0.64f), androidx.compose.ui.geometry.CornerRadius(4f), style = Stroke(size.minDimension * 0.08f))
        drawLine(color, Offset(size.width * 0.38f, size.height * 0.18f), Offset(size.width * 0.38f, size.height * 0.82f), size.minDimension * 0.08f)
    }
}

@Composable
fun QuizIcon(color: Color, iconSize: Dp = 22.dp) {
    DrawnIcon(iconSize) {
        drawCircle(color, size.minDimension * 0.36f, Offset(size.width / 2, size.height / 2), style = Stroke(size.minDimension * 0.08f))
        drawCircle(color, size.minDimension * 0.06f, Offset(size.width / 2, size.height * 0.68f))
        drawLine(color, Offset(size.width / 2, size.height * 0.32f), Offset(size.width / 2, size.height * 0.52f), size.minDimension * 0.08f, StrokeCap.Round)
    }
}

@Composable
fun OrderIcon(color: Color, iconSize: Dp = 22.dp) {
    DrawnIcon(iconSize) {
        for (i in 0 until 3) {
            val y = size.height * (0.28f + i * 0.22f)
            drawLine(color, Offset(size.width * 0.22f, y), Offset(size.width * 0.78f, y), size.minDimension * 0.08f, StrokeCap.Round)
        }
    }
}

@Composable
fun GlobeIcon(color: Color, iconSize: Dp = 22.dp) {
    DrawnIcon(iconSize) {
        drawCircle(color, size.minDimension * 0.34f, center, style = Stroke(size.minDimension * 0.08f))
        drawOval(color, Offset(size.width * 0.32f, size.height * 0.18f), Size(size.width * 0.36f, size.height * 0.64f), style = Stroke(size.minDimension * 0.06f))
    }
}

@Composable
fun PeopleIcon(color: Color, iconSize: Dp = 22.dp) {
    DrawnIcon(iconSize) {
        drawCircle(color, size.minDimension * 0.12f, Offset(size.width * 0.35f, size.height * 0.32f))
        drawCircle(color, size.minDimension * 0.12f, Offset(size.width * 0.65f, size.height * 0.32f))
        drawArc(color, 200f, 140f, false, Offset(size.width * 0.18f, size.height * 0.42f), Size(size.width * 0.34f, size.height * 0.4f), style = Stroke(size.minDimension * 0.08f, cap = StrokeCap.Round))
        drawArc(color, 200f, 140f, false, Offset(size.width * 0.48f, size.height * 0.42f), Size(size.width * 0.34f, size.height * 0.4f), style = Stroke(size.minDimension * 0.08f, cap = StrokeCap.Round))
    }
}

@Composable
fun BulbIcon(color: Color, iconSize: Dp = 22.dp) {
    DrawnIcon(iconSize) {
        drawCircle(color, size.minDimension * 0.22f, Offset(size.width / 2, size.height * 0.4f), style = Stroke(size.minDimension * 0.08f))
        drawRoundRect(color, Offset(size.width * 0.4f, size.height * 0.58f), Size(size.width * 0.2f, size.height * 0.2f), androidx.compose.ui.geometry.CornerRadius(3f))
    }
}

fun noteIcon(name: String, color: Color): @Composable () -> Unit = {
    when (name) {
        "globe" -> GlobeIcon(color)
        "people" -> PeopleIcon(color)
        "bulb" -> BulbIcon(color)
        "quiz" -> QuizIcon(color)
        "order" -> OrderIcon(color)
        else -> BookIcon(color)
    }
}
