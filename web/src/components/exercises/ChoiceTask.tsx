'use client';

import { useState } from 'react';

import type { ChoiceExercise } from '@/content';
import { TaskShell, type CheckState, type TaskFrame } from './TaskShell';

/** Multiple choice, one correct option per question. */
export function ChoiceTask({
  items,
  onSolved,
  ...frame
}: TaskFrame & {
  items: ChoiceExercise[];
  onSolved: () => void;
}) {
  const [picked, setPicked] = useState<(number | null)[]>(() => items.map(() => null));
  const [state, setState] = useState<CheckState>('idle');
  const [checked, setChecked] = useState(false);

  const check = () => {
    setChecked(true);
    const allCorrect = items.every((q, i) => picked[i] === q.correct);
    setState(allCorrect ? 'ok' : 'wrong');
    if (allCorrect) onSolved();
  };

  const pick = (question: number, option: number) => {
    setPicked((prev) => prev.map((v, i) => (i === question ? option : v)));
    // Editing an answer clears the previous verdict so the colours stay honest.
    setChecked(false);
    setState('idle');
  };

  return (
    <TaskShell {...frame} state={state} onCheck={check}>
      {items.map((q, qi) => (
        <div className="ex__item" key={qi}>
          <p className="ex__prompt">
            <span className="ex__num">{qi + 1}</span>
            {q.prompt}
          </p>

          <div className="opts">
            {q.options.map((option, oi) => {
              const selected = picked[qi] === oi;
              let modifier = '';
              if (checked && selected) modifier = oi === q.correct ? ' opt--ok' : ' opt--wrong';
              // After a wrong guess, also point at the right answer.
              if (checked && !selected && oi === q.correct && picked[qi] !== null) {
                modifier = ' opt--ok';
              }

              return (
                <button
                  className={`opt${modifier}`}
                  key={oi}
                  aria-pressed={selected}
                  onClick={() => pick(qi, oi)}
                >
                  {option}
                </button>
              );
            })}
          </div>

          {q.hint && checked && <p className="ex__meaning">{q.hint}</p>}
          {q.meaning && checked && <p className="ex__meaning">{q.meaning}</p>}
        </div>
      ))}
    </TaskShell>
  );
}
