'use client';

import { useMemo, useState } from 'react';

import type { AssemblyExercise } from '@/content';
import { TaskShell, type CheckState, type TaskFrame } from './TaskShell';

/**
 * Build a sentence by clicking scrambled words in the right order.
 *
 * The desktop app uses drag and drop; clicking is used here so the exercise
 * works the same on a phone. Clicking a placed word sends it back to the bank.
 */
export function AssemblyTask({
  items,
  onSolved,
  ...frame
}: TaskFrame & {
  items: AssemblyExercise[];
  onSolved: () => void;
}) {
  // Scramble once per mount so re-renders do not reshuffle mid-exercise.
  const banks = useMemo(() => items.map((item) => shuffle(item.words)), [items]);

  const [placed, setPlaced] = useState<number[][]>(() => items.map(() => []));
  const [state, setState] = useState<CheckState>('idle');
  const [checked, setChecked] = useState(false);
  const [revealed, setRevealed] = useState(false);

  const rowCorrect = (i: number) => {
    const built = placed[i].map((bankIndex) => banks[i][bankIndex]);
    return built.length === items[i].words.length && built.every((w, j) => w === items[i].words[j]);
  };

  const check = () => {
    setChecked(true);
    const allCorrect = items.every((_, i) => rowCorrect(i));
    setState(allCorrect ? 'ok' : 'wrong');
    if (allCorrect) onSolved();
  };

  const place = (item: number, bankIndex: number) => {
    setPlaced((prev) => prev.map((row, i) => (i === item ? [...row, bankIndex] : row)));
    setChecked(false);
    setState('idle');
  };

  const remove = (item: number, position: number) => {
    setPlaced((prev) =>
      prev.map((row, i) => (i === item ? row.filter((_, p) => p !== position) : row))
    );
    setChecked(false);
    setState('idle');
  };

  return (
    <TaskShell
      {...frame}
      state={state}
      onCheck={check}
      onReveal={() => setRevealed(true)}
      revealed={revealed}
    >
      {items.map((item, i) => {
        const status = checked ? (rowCorrect(i) ? ' slot--ok' : ' slot--wrong') : '';

        return (
          <div className="ex__item" key={i}>
            {item.prompt && (
              <p className="ex__prompt">
                <span className="ex__num">{i + 1}</span>
                {item.prompt}
              </p>
            )}

            <div className={`slot${status}`}>
              {placed[i].length === 0 && (
                <span className="slot__placeholder">…</span>
              )}
              {placed[i].map((bankIndex, position) => (
                <button className="chip" key={position} onClick={() => remove(i, position)}>
                  {banks[i][bankIndex]}
                </button>
              ))}
            </div>

            <div className="bank">
              {banks[i].map((word, bankIndex) => (
                <button
                  className={`chip${placed[i].includes(bankIndex) ? ' chip--used' : ''}`}
                  key={bankIndex}
                  disabled={placed[i].includes(bankIndex)}
                  onClick={() => place(i, bankIndex)}
                >
                  {word}
                </button>
              ))}
            </div>

            {item.meaning && <p className="ex__meaning">{item.meaning}</p>}
            {revealed && <p className="ex__model">{item.words.join(' ')}</p>}
          </div>
        );
      })}
    </TaskShell>
  );
}

/** Fisher-Yates, matching `shuffle_indices` in the desktop app. */
function shuffle<T>(input: readonly T[]): T[] {
  const out = [...input];
  for (let i = out.length - 1; i > 0; i--) {
    const j = Math.floor(Math.random() * (i + 1));
    [out[i], out[j]] = [out[j], out[i]];
  }
  return out;
}
