'use client';

import { useState } from 'react';

import type { DialogueExercise } from '@/content';
import { isCorrect, primaryAnswer } from '@/lib/answers';
import { TaskShell, type CheckState, type TaskFrame } from './TaskShell';

/** Short dialogues with one missing word per line, picked from a shared pool. */
export function DialogueTask({
  items,
  pool,
  onSolved,
  ...frame
}: TaskFrame & {
  items: DialogueExercise[];
  pool?: string[];
  onSolved: () => void;
}) {
  const [values, setValues] = useState<string[][]>(() => items.map((d) => d.rows.map(() => '')));
  const [state, setState] = useState<CheckState>('idle');
  const [checked, setChecked] = useState(false);
  const [revealed, setRevealed] = useState(false);

  const check = () => {
    setChecked(true);
    const allCorrect = items.every((dialogue, d) =>
      dialogue.rows.every((row, r) => isCorrect(values[d][r] ?? '', row.answer))
    );
    setState(allCorrect ? 'ok' : 'wrong');
    if (allCorrect) onSolved();
  };

  const set = (dialogue: number, row: number, value: string) => {
    setValues((prev) =>
      prev.map((rows, d) => (d === dialogue ? rows.map((v, r) => (r === row ? value : v)) : rows))
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
      {items.map((dialogue, d) => (
        <div className="ex__item" key={d}>
          <p className="ex__prompt">{dialogue.title}</p>

          {dialogue.rows.map((row, r) => {
            const status = checked
              ? isCorrect(values[d][r] ?? '', row.answer)
                ? 'ok'
                : 'wrong'
              : undefined;

            return (
              <div key={r} style={{ marginBottom: 10 }}>
                <div className="sentence">
                  {row.speaker && <span className="speaker">{row.speaker}</span>}
                  {row.before && <span>{row.before}</span>}

                  <select
                    className={`gap${status ? ` gap--${status}` : ''}`}
                    value={values[d][r] ?? ''}
                    onChange={(e) => set(d, r, e.target.value)}
                  >
                    <option value="">—</option>
                    {(pool ?? []).map((word) => (
                      <option key={word} value={word}>
                        {word}
                      </option>
                    ))}
                  </select>

                  {row.after && <span>{row.after}</span>}
                </div>

                {row.meaning && <p className="ex__meaning">{row.meaning}</p>}
                {revealed && (
                  <p className="ex__model">
                    {`${row.before}${primaryAnswer(row.answer)}${row.after}`.trim()}
                  </p>
                )}
              </div>
            );
          })}
        </div>
      ))}
    </TaskShell>
  );
}
