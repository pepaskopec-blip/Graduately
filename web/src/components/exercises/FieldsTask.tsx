'use client';

import { useState } from 'react';

import type { FieldsExercise } from '@/content';
import { isCorrect } from '@/lib/answers';
import { TaskShell, type CheckState, type TaskFrame } from './TaskShell';

/**
 * A prompt answered through several labelled fields.
 *
 * The Czech grammar exercises use this for questions with more than one part —
 * "determine the gender, number and case" fills three fields from one sentence.
 */
export function FieldsTask({
  items,
  onSolved,
  ...frame
}: TaskFrame & {
  items: FieldsExercise[];
  onSolved: () => void;
}) {
  const [values, setValues] = useState<string[][]>(() =>
    items.map((item) => item.fields.map(() => ''))
  );
  const [state, setState] = useState<CheckState>('idle');
  const [checked, setChecked] = useState(false);
  const [revealed, setRevealed] = useState(false);

  const ok = (i: number, f: number) =>
    items[i].fields[f].answers.some((a) => isCorrect(values[i][f] ?? '', a));

  const check = () => {
    setChecked(true);
    const allCorrect = items.every((item, i) => item.fields.every((_, f) => ok(i, f)));
    setState(allCorrect ? 'ok' : 'wrong');
    if (allCorrect) onSolved();
  };

  const set = (item: number, field: number, value: string) => {
    setValues((prev) =>
      prev.map((row, i) => (i === item ? row.map((v, f) => (f === field ? value : v)) : row))
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
      {items.map((item, i) => (
        <div className="ex__item" key={i}>
          <p className="ex__prompt">
            <span className="ex__num">{i + 1}</span>
            {item.prompt}
          </p>

          <div className="fields">
            {item.fields.map((field, f) => (
              <label className="fields__field" key={f}>
                <span className="fields__label">{field.label}</span>
                <input
                  className={`gap${checked ? (ok(i, f) ? ' gap--ok' : ' gap--wrong') : ''}`}
                  type="text"
                  value={values[i][f] ?? ''}
                  autoComplete="off"
                  spellCheck={false}
                  onChange={(e) => set(i, f, e.target.value)}
                />
              </label>
            ))}
          </div>

          {revealed && item.solution && <p className="ex__model">{item.solution}</p>}
        </div>
      ))}
    </TaskShell>
  );
}
