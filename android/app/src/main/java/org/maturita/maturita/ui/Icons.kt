package org.maturita.maturita.ui

import androidx.compose.foundation.Canvas
import androidx.compose.foundation.layout.size
import androidx.compose.runtime.Composable
import androidx.compose.ui.Modifier
import androidx.compose.ui.geometry.Offset
import androidx.compose.ui.geometry.Size
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.graphics.Path
import androidx.compose.ui.graphics.StrokeCap
import androidx.compose.ui.graphics.drawscope.Stroke
import androidx.compose.ui.graphics.drawscope.clipPath
import androidx.compose.ui.unit.Dp
import androidx.compose.ui.unit.dp
import org.maturita.maturita.data.Palette
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
fun SettingsIcon(color: Color, iconSize: Dp = 18.dp) {
    DrawnIcon(iconSize) {
        val c = Offset(size.width / 2, size.height / 2)
        drawCircle(color, size.minDimension * 0.16f, c, style = Stroke(size.minDimension * 0.1f))
        for (i in 0 until 6) {
            val a = i * Math.PI / 3.0
            val x = (c.x + kotlin.math.cos(a) * size.minDimension * 0.34).toFloat()
            val y = (c.y + kotlin.math.sin(a) * size.minDimension * 0.34).toFloat()
            drawCircle(color, size.minDimension * 0.07f, Offset(x, y))
        }
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
fun WifiIcon(color: Color, iconSize: Dp = 28.dp) {
    DrawnIcon(iconSize) {
        val c = Offset(size.width / 2, size.height * 0.72f)
        drawCircle(color, size.minDimension * 0.06f, c)
        for (i in 1..3) {
            val r = size.minDimension * (0.16f + i * 0.16f)
            drawArc(color, 210f, 120f, false, Offset(c.x - r, c.y - r), Size(r * 2, r * 2), style = Stroke(size.minDimension * 0.08f, cap = StrokeCap.Round))
        }
    }
}

@Composable
fun ChipIcon(color: Color, iconSize: Dp = 28.dp) {
    DrawnIcon(iconSize) {
        val m = size.minDimension * 0.22f
        drawRoundRect(color, Offset(m, m), Size(size.width - 2 * m, size.height - 2 * m), androidx.compose.ui.geometry.CornerRadius(6f), style = Stroke(size.minDimension * 0.08f))
        val pin = size.minDimension * 0.08f
        for (i in 0 until 3) {
            val t = m + (size.height - 2 * m) * (i + 1) / 4f
            drawLine(color, Offset(m - pin, t), Offset(m, t), size.minDimension * 0.06f)
            drawLine(color, Offset(size.width - m, t), Offset(size.width - m + pin, t), size.minDimension * 0.06f)
        }
    }
}

@Composable
fun GermanFlag(iconSize: Dp = 48.dp) {
    DrawnIcon(iconSize) {
        val w = this.size.width
        val h = this.size.height
        val clip = Path().apply { addOval(androidx.compose.ui.geometry.Rect(0f, 0f, w, h)) }
        clipPath(clip) {
            val band = h / 3f
            drawRect(Color(0xFF2E2E30), size = Size(w, band))
            drawRect(Color(0xFFB8453D), topLeft = Offset(0f, band), size = Size(w, band))
            drawRect(Color(0xFFD6B352), topLeft = Offset(0f, 2 * band), size = Size(w, band))
        }
    }
}

@Composable
fun CzechFlag(iconSize: Dp = 48.dp) {
    DrawnIcon(iconSize) {
        val w = this.size.width
        val h = this.size.height
        val clip = Path().apply { addOval(androidx.compose.ui.geometry.Rect(0f, 0f, w, h)) }
        clipPath(clip) {
            drawRect(Color(0xFFFAFAFA), size = Size(w, h))
            drawRect(Color(0xFFD4292E), topLeft = Offset(0f, h / 2), size = Size(w, h / 2))
            val tri = Path().apply {
                moveTo(0f, 0f)
                lineTo(w * 0.55f, h / 2)
                lineTo(0f, h)
                close()
            }
            drawPath(tri, Color(0xFF123D8C))
        }
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

@Composable
fun FinishFlag(p: Palette, number: String, iconSize: Dp) {
    DrawnIcon(iconSize) {
        val r = size.minDimension / 2f - 2f
        drawCircle(p.finishDark, r)
        drawCircle(p.finishStroke, r, style = Stroke(2f))
        val cell = 6f
        val fx = center.x - 1.5f * cell
        val fy = center.y + 8f
        for (iy in 0 until 2) for (ix in 0 until 3) {
            val c = if ((ix + iy) and 1 == 1) p.finishLight else p.finishStroke
            drawRect(c, Offset(fx + ix * cell, fy + iy * cell), Size(cell, cell))
        }
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
