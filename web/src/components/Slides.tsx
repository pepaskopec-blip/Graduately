'use client';

import { useState } from 'react';

import type { Slide } from '@/content';
import { useApp } from '@/app/providers';

/** Theory note-cards with prev/next navigation, as in `build_net_unit_page`. */
export function Slides({ slides, onFinish }: { slides: Slide[]; onFinish: () => void }) {
  const { t } = useApp();
  const [index, setIndex] = useState(0);

  if (slides.length === 0) return null;

  const slide = slides[index];
  const last = index === slides.length - 1;

  return (
    <div className="stack">
      <article className="slide">
        <span className="slide__kicker">{slide.kicker}</span>
        <h2 className="slide__title">{slide.title}</h2>
        {slide.tip && <p className="slide__tip">{slide.tip}</p>}

        <ul className="slide__lines">
          {slide.lines.map((line, i) => (
            <li key={i}>{line}</li>
          ))}
        </ul>
      </article>

      <div className="slides__nav">
        <button
          className="btn btn--ghost"
          disabled={index === 0}
          onClick={() => setIndex((i) => i - 1)}
        >
          {t('net_slide_prev')}
        </button>

        <div className="dots" role="tablist" aria-label={slide.title}>
          {slides.map((s, i) => (
            <button
              key={i}
              role="tab"
              aria-selected={i === index}
              // Only the active dot carries aria-current; `false` still reads as current.
              aria-current={i === index ? 'true' : undefined}
              aria-label={s.title}
              onClick={() => setIndex(i)}
            />
          ))}
        </div>

        <button className="btn" onClick={() => (last ? onFinish() : setIndex((i) => i + 1))}>
          {last ? t('net_slide_start') : t('net_slide_next')}
        </button>
      </div>
    </div>
  );
}
