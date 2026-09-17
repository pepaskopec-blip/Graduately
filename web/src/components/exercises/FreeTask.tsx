'use client';

import { useState } from 'react';

import type { FreeExercise } from '@/content';
import { useApp } from '@/app/providers';
import { TaskShell, type CheckState, type TaskFrame } from './TaskShell';

/**
 * Open questions with a model answer.
 *
 * There is no single right string, so writing something for every question
 * counts as done and reveals the samples to compare against — the same rule the
 * desktop app applies.
 */
export function FreeTask({
  items,
  onSolved,
  ...frame
}: TaskFrame & {
  items: FreeExercise[];
  onSolved: () => void;
}) {
  const { t, lang } = useApp();
  const [values, setValues] = useState<string[]>(() => items.map(() => ''));
  const [state, setState] = useState<CheckState>('idle');
  const [revealed, setRevealed] = useState(false);

  const check = () => {
    const answered = values.every((v) => v.trim().length > 0);
    setState(answered ? 'ok' : 'wrong');
    if (answered) {
      setRevealed(true);
      onSolved();
    }
  };

  return (
    <TaskShell {...frame} state={state} onCheck={check}>
      {items.map((item, i) => (
        <div className="ex__item" key={i}>
          <p className="ex__prompt">
            <span className="ex__num">{i + 1}</span>
            {item.question}
          </p>

          {item.questionCs && <p className="ex__meaning">{t(item.questionCs)}</p>}

          <input
            className="gap"
            style={{ width: '100%', minWidth: 0 }}
            type="text"
            value={values[i]}
            placeholder={t('your_answer')}
            autoComplete="off"
            onChange={(e) => {
              const value = e.target.value;
              setValues((prev) => prev.map((v, j) => (j === i ? value : v)));
              setState('idle');
            }}
          />

          {revealed && (
            <p className="ex__model">
              {item.sample}
              {item.sampleCs && (
                <>
                  {' — '}
                  <span className="muted">{t(item.sampleCs)}</span>
                </>
              )}
            </p>
          )}
        </div>
      ))}

      {/* The umlaut tip only matters while typing German. */}
      {lang === 'cs' && <p className="ex__meaning">{t('hint_umlauts')}</p>}
    </TaskShell>
  );
}
