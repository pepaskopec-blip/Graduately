'use client';

import { useState } from 'react';

import type { ProfileExercise } from '@/content';
import { useApp } from '@/app/providers';
import { TaskShell, type CheckState, type TaskFrame } from './TaskShell';

/**
 * "Steckbrief" profile questions.
 *
 * Like the free-answer exercise there is no single right string, so answering
 * every line counts as done and reveals the model sentences.
 */
export function ProfileTask({
  items,
  onSolved,
  ...frame
}: TaskFrame & {
  items: ProfileExercise[];
  onSolved: () => void;
}) {
  const { t } = useApp();
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
          <div className="sentence">
            <span className="ex__num">{i + 1}</span>
            <span>{item.stem}</span>
            <input
              className="gap"
              style={{ minWidth: 200 }}
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
          </div>

          {revealed && <p className="ex__model">{item.sample}</p>}
        </div>
      ))}
    </TaskShell>
  );
}
