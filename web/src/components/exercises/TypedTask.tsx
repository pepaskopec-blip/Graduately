'use client';

import { useState } from 'react';

import type { TypedExercise } from '@/content';
import { useApp } from '@/app/providers';
import { containsAnswer, isCorrect } from '@/lib/answers';
import { TaskShell, type CheckState, type TaskFrame } from './TaskShell';

/** Type the answer; any of the accepted spellings counts. */
export function TypedTask({
  items,
  onSolved,
  ...frame
}: TaskFrame & {
  items: TypedExercise[];
  onSolved: () => void;
}) {
  const { t } = useApp();
  const [values, setValues] = useState<string[]>(() => items.map(() => ''));
  const [state, setState] = useState<CheckState>('idle');
  const [checked, setChecked] = useState(false);
  const [revealed, setRevealed] = useState(false);

  const ok = (i: number) => {
    const item = items[i];
    return item.substring
      ? containsAnswer(values[i], item.answers)
      : item.answers.some((a) => isCorrect(values[i], a));
  };

  const check = () => {
    setChecked(true);
    const allCorrect = items.every((_, i) => ok(i));
    setState(allCorrect ? 'ok' : 'wrong');
    if (allCorrect) onSolved();
  };

  return (
    <TaskShell
      {...frame}
      state={state}
      onCheck={check}
      onReveal={() => setRevealed(true)}
      revealed={revealed}
    >
      {items.map((item, i) => (
        <div className="ex__item" key={i}>
          <p className="ex__prompt">
            <span className="ex__num">{i + 1}</span>
            {item.prompt}
          </p>

          <input
            className={`gap${checked ? (ok(i) ? ' gap--ok' : ' gap--wrong') : ''}`}
            style={{ width: '100%', minWidth: 0 }}
            type="text"
            value={values[i]}
            placeholder={t('your_answer')}
            autoComplete="off"
            spellCheck={false}
            onChange={(e) => {
              const value = e.target.value;
              setValues((prev) => prev.map((v, j) => (j === i ? value : v)));
              setChecked(false);
              setState('idle');
            }}
          />

          {item.meaning && <p className="ex__meaning">{item.meaning}</p>}
          {revealed && <p className="ex__model">{item.model ?? item.answers[0]}</p>}
        </div>
      ))}
    </TaskShell>
  );
}
