'use client';

import { useState } from 'react';

import type { HangmanExercise } from '@/content';
import { useApp } from '@/app/providers';
import type { TaskFrame } from './TaskShell';

const ALPHABET = 'ABCDEFGHIJKLMNOPQRSTUVWXYZÄÖÜ'.split('');
const MAX_MISSES = 7;

/** Guess the profession letter by letter; a Czech hint is always visible. */
export function HangmanTask({
  items,
  subtitle,
  onSolved,
}: TaskFrame & {
  items: HangmanExercise[];
  onSolved: () => void;
}) {
  const { t } = useApp();
  const [index, setIndex] = useState(0);
  const [guessed, setGuessed] = useState<string[]>([]);

  const current = items[index];
  const letters = current.word.toUpperCase().split('');
  const misses = guessed.filter((g) => !letters.includes(g)).length;

  const solved = letters.every((l) => !/\p{L}/u.test(l) || guessed.includes(l));
  const lost = misses >= MAX_MISSES;
  const last = index === items.length - 1;

  const guess = (letter: string) => {
    if (solved || lost || guessed.includes(letter)) return;
    setGuessed((prev) => [...prev, letter]);
  };

  const next = () => {
    if (last) {
      onSolved();
      return;
    }
    setIndex((i) => i + 1);
    setGuessed([]);
  };

  return (
    <div className="stack">
      {subtitle && <p className="page__sub">{subtitle}</p>}

      <div className="ex__item">
        <p className="ex__prompt">
          <span className="ex__num">{index + 1}</span>
          {t('hm_hint')}: {current.hint}
        </p>

        <p
          className="sentence"
          style={{ fontSize: '1.7rem', fontWeight: 700, letterSpacing: '0.16em' }}
        >
          {letters.map((letter, i) => (
            <span key={i}>
              {!/\p{L}/u.test(letter) ? letter : guessed.includes(letter) ? letter : '_'}
            </span>
          ))}
        </p>

        <p className="muted tabular">
          {misses} / {MAX_MISSES}
        </p>

        <div className="bank">
          {ALPHABET.map((letter) => {
            const used = guessed.includes(letter);
            const hit = used && letters.includes(letter);
            return (
              <button
                className={`chip${used ? (hit ? ' gap--ok' : ' gap--wrong') : ''}`}
                key={letter}
                disabled={used || solved || lost}
                onClick={() => guess(letter)}
              >
                {letter}
              </button>
            );
          })}
        </div>

        {(solved || lost) && (
          <>
            <p className={`feedback feedback--${solved ? 'ok' : 'err'}`} role="status">
              {solved ? t('feedback_ok') : current.word}
            </p>
            <div className="actions">
              <button className="btn" onClick={next}>
                {last ? t('check') : t('net_slide_next')}
              </button>
            </div>
          </>
        )}
      </div>
    </div>
  );
}
