'use client';

import { useState } from 'react';

import { useApp } from '@/app/providers';
import type { RevealExercise } from '@/content';
import { TaskShell, type CheckState, type TaskFrame } from './TaskShell';

/**
 * Self-assessed exercise: write your answer, then compare it with the model.
 *
 * The desktop app labels the button "Zobrazit řešení" for these, because there
 * is nothing meaningful to check automatically — morpheme analysis, punctuation
 * and rewriting direct speech all have too many valid phrasings.
 */
export function RevealTask({
  items,
  onSolved,
  ...frame
}: TaskFrame & {
  items: RevealExercise[];
  onSolved: () => void;
}) {
  const { t } = useApp();
  const [values, setValues] = useState<string[]>(() => items.map(() => ''));
  const [state, setState] = useState<CheckState>('idle');
  const [revealed, setRevealed] = useState(false);

  const reveal = () => {
    setRevealed(true);
    // Completion here means "you attempted every item", as on the desktop.
    const attempted = values.every((v) => v.trim().length > 0);
    setState(attempted ? 'ok' : 'wrong');
    if (attempted) onSolved();
  };

  return (
    <TaskShell
      {...frame}
      state={state}
      checkLabel="net_solution"
      onCheck={reveal}
    >
      {items.map((item, i) => (
        <div className="ex__item" key={i}>
          <p className="ex__prompt">
            <span className="ex__num">{i + 1}</span>
            {item.prompt}
          </p>

          <input
            className="gap"
            style={{ width: '100%', minWidth: 0 }}
            type="text"
            value={values[i]}
            placeholder={t('your_answer')}
            autoComplete="off"
            spellCheck={false}
            onChange={(e) => {
              const value = e.target.value;
              setValues((prev) => prev.map((v, j) => (j === i ? value : v)));
              setState('idle');
            }}
          />

          {revealed && <p className="ex__model">{item.solution}</p>}
        </div>
      ))}
    </TaskShell>
  );
}
