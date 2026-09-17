'use client';

import { useState } from 'react';

import type { TableExercise } from '@/content';
import { isCorrect } from '@/lib/answers';
import { TaskShell, type CheckState, type TaskFrame } from './TaskShell';

/** Editable grid such as the possessive-pronoun table; some cells come prefilled. */
export function TableTask({
  exercise,
  onSolved,
  ...frame
}: TaskFrame & {
  exercise: TableExercise;
  onSolved: () => void;
}) {
  const [values, setValues] = useState<string[][]>(() =>
    exercise.answers.map((row) => row.map((cell) => (cell.given ? cell.value : '')))
  );
  const [state, setState] = useState<CheckState>('idle');
  const [checked, setChecked] = useState(false);
  const [revealed, setRevealed] = useState(false);

  const cellOk = (r: number, c: number) =>
    isCorrect(values[r][c] ?? '', exercise.answers[r][c].value);

  const check = () => {
    setChecked(true);
    const allCorrect = exercise.answers.every((row, r) => row.every((_, c) => cellOk(r, c)));
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
      <div className="ex__item" style={{ overflowX: 'auto' }}>
        <table style={{ borderCollapse: 'collapse', minWidth: '100%' }}>
          <thead>
            <tr>
              <th />
              {exercise.columns.map((column) => (
                <th key={column} className="card__kicker" style={{ padding: '6px 10px' }}>
                  {column}
                </th>
              ))}
            </tr>
          </thead>
          <tbody>
            {exercise.rows.map((rowLabel, r) => (
              <tr key={rowLabel}>
                <th
                  scope="row"
                  style={{ padding: '6px 10px', textAlign: 'left', fontWeight: 620 }}
                >
                  {rowLabel}
                </th>
                {exercise.columns.map((_, c) => {
                  const cell = exercise.answers[r][c];
                  return (
                    <td key={c} style={{ padding: 4 }}>
                      <input
                        className={`gap${checked ? (cellOk(r, c) ? ' gap--ok' : ' gap--wrong') : ''}`}
                        style={{ minWidth: 110 }}
                        value={values[r][c]}
                        readOnly={cell.given}
                        autoComplete="off"
                        spellCheck={false}
                        onChange={(e) => {
                          const value = e.target.value;
                          setValues((prev) =>
                            prev.map((row, i) =>
                              i === r ? row.map((v, j) => (j === c ? value : v)) : row
                            )
                          );
                          setChecked(false);
                          setState('idle');
                        }}
                      />
                    </td>
                  );
                })}
              </tr>
            ))}
          </tbody>
        </table>

        {revealed && (
          <p className="ex__model">
            {exercise.answers.map((row) => row.map((cell) => cell.value).join(' · ')).join(' | ')}
          </p>
        )}
      </div>
    </TaskShell>
  );
}
