'use client';

import { useMemo, useState } from 'react';

import type { AssignExercise } from '@/content';
import { useApp } from '@/app/providers';
import { TaskShell, type CheckState, type TaskFrame } from './TaskShell';

/**
 * Sort cards into named groups.
 *
 * Mirrors the desktop interaction: select a group, then click cards to move
 * them in; clicking a card inside a group returns it to the pool.
 */
export function AssignTask({
  exercise,
  onSolved,
  ...frame
}: TaskFrame & {
  exercise: AssignExercise;
  onSolved: () => void;
}) {
  const { t } = useApp();
  const order = useMemo(() => shuffle(exercise.items.map((_, i) => i)), [exercise]);

  /** Group index per card, or null while the card is still in the pool. */
  const [assigned, setAssigned] = useState<(number | null)[]>(() => exercise.items.map(() => null));
  const [activeGroup, setActiveGroup] = useState(0);
  const [state, setState] = useState<CheckState>('idle');
  const [checked, setChecked] = useState(false);

  const check = () => {
    setChecked(true);
    const allCorrect = exercise.items.every((item, i) => assigned[i] === item.group);
    setState(allCorrect ? 'ok' : 'wrong');
    if (allCorrect) onSolved();
  };

  const move = (card: number, group: number | null) => {
    setAssigned((prev) => prev.map((g, i) => (i === card ? group : g)));
    setChecked(false);
    setState('idle');
  };

  const cardLabel = (i: number) => {
    const item = exercise.items[i];
    return `${item.emoji ? `${item.emoji} ` : ''}${item.label}`;
  };

  const cardStatus = (i: number) => {
    if (!checked || assigned[i] === null) return '';
    return assigned[i] === exercise.items[i].group ? ' gap--ok' : ' gap--wrong';
  };

  return (
    <TaskShell {...frame} state={state} onCheck={check}>
      <div className="ex__item">
        <p className="ex__prompt">{t('assign_hint')}</p>

        <div className="segmented" role="group">
          {exercise.groups.map((group, gi) => (
            <button key={gi} aria-pressed={activeGroup === gi} onClick={() => setActiveGroup(gi)}>
              {group}
            </button>
          ))}
        </div>

        <div className="bank">
          {order
            .filter((i) => assigned[i] === null)
            .map((i) => (
              <button className="chip" key={i} onClick={() => move(i, activeGroup)}>
                {cardLabel(i)}
              </button>
            ))}
        </div>

        <div className="groups">
          {exercise.groups.map((group, gi) => (
            <div className="group" key={gi}>
              <span className="group__title">{group}</span>
              {order
                .filter((i) => assigned[i] === gi)
                .map((i) => (
                  <button className={`chip${cardStatus(i)}`} key={i} onClick={() => move(i, null)}>
                    {cardLabel(i)}
                  </button>
                ))}
            </div>
          ))}
        </div>

        {checked && (
          <ul className="ex__meaning" style={{ listStyle: 'none', padding: 0 }}>
            {exercise.items.map(
              (item, i) =>
                item.meaning && (
                  <li key={i}>
                    {item.label} — {item.meaning}
                  </li>
                )
            )}
          </ul>
        )}
      </div>
    </TaskShell>
  );
}

function shuffle<T>(input: readonly T[]): T[] {
  const out = [...input];
  for (let i = out.length - 1; i > 0; i--) {
    const j = Math.floor(Math.random() * (i + 1));
    [out[i], out[j]] = [out[j], out[i]];
  }
  return out;
}
