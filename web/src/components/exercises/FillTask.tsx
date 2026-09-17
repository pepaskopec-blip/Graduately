'use client';

import { useState } from 'react';

import type { FillExercise } from '@/content';
import { useApp } from '@/app/providers';
import { isCorrect, primaryAnswer } from '@/lib/answers';
import { TaskShell, type CheckState, type TaskFrame } from './TaskShell';

/**
 * Sentences with gaps.
 *
 * Every gap variant in the desktop app collapses to this component: a dropdown
 * fed from the task's word pool, or a free text field when there is no pool.
 */
export function FillTask({
  items,
  pool,
  altPool,
  onSolved,
  ...frame
}: TaskFrame & {
  items: FillExercise[];
  pool?: string[];
  altPool?: string[];
  onSolved: () => void;
}) {
  const { t } = useApp();
  const [values, setValues] = useState<string[][]>(() => items.map((it) => it.answers.map(() => '')));
  const [state, setState] = useState<CheckState>('idle');
  const [checked, setChecked] = useState(false);
  const [revealed, setRevealed] = useState(false);

  const check = () => {
    setChecked(true);
    const allCorrect = items.every((item, i) =>
      item.answers.every((answer, g) => isCorrect(values[i][g] ?? '', answer))
    );
    setState(allCorrect ? 'ok' : 'wrong');
    if (allCorrect) onSolved();
  };

  const set = (item: number, gap: number, value: string) => {
    setValues((prev) =>
      prev.map((row, i) => (i === item ? row.map((v, g) => (g === gap ? value : v)) : row))
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
          {item.heading && <h2 className="ex__group">{item.heading}</h2>}

          <div className={`sentence${item.letters ? ' sentence--letters' : ''}`}>
            {item.label && <span className="ex__num">{item.label}</span>}
            {item.emoji && <span className="sentence__emoji">{item.emoji}</span>}
            {item.clue && <span className="sentence__clue">{item.clue}</span>}

            {chunk(item).map((group, g) =>
              group === SPACE ? (
                <span key={g}> </span>
              ) : (
                <span className="sentence__chunk" key={g}>
                  {group.map((node, n) =>
                    typeof node === 'string' ? (
                      <span key={n}>{node}</span>
                    ) : (
                      <Gap
                        key={n}
                        value={values[i][node.gap] ?? ''}
                        onChange={(v) => set(i, node.gap, v)}
                        pool={
                          item.input === 'select'
                            ? item.gapPool?.[node.gap] === 'alt'
                              ? altPool
                              : pool
                            : undefined
                        }
                        placeholder={item.letters ? '' : t('your_answer')}
                        letters={item.letters}
                        status={
                          checked
                            ? isCorrect(values[i][node.gap] ?? '', item.answers[node.gap])
                              ? 'ok'
                              : 'wrong'
                            : undefined
                        }
                      />
                    )
                  )}
                </span>
              )
            )}
          </div>

          {item.meaning && <p className="ex__meaning">{item.meaning}</p>}

          {item.glosses?.some(Boolean) && (
            <p className="ex__gloss">{item.glosses.filter(Boolean).join(' · ')}</p>
          )}

          {revealed && (
            <p className="ex__model">
              {buildSolution(item)}
            </p>
          )}
        </div>
      ))}
    </TaskShell>
  );
}

const SPACE = Symbol('space');
type GapRef = { gap: number };
type Chunk = (string | GapRef)[] | typeof SPACE;

/**
 * Split a sentence into groups that must not be broken across lines.
 *
 * A letter gap sits inside a word, so wrapping between the gap and the letters
 * around it would tear the word in half ("Öster[re]" / "ich."). Splitting only
 * at real spaces keeps each word — gaps included — on one line.
 */
function chunk(item: FillExercise): Chunk[] {
  const chunks: Chunk[] = [];
  let current: (string | GapRef)[] = [];

  const flush = () => {
    if (current.length) chunks.push(current);
    current = [];
  };

  item.segments.forEach((segment, s) => {
    for (const part of segment.split(/(\s+)/)) {
      if (!part) continue;
      if (/^\s+$/.test(part)) {
        flush();
        chunks.push(SPACE);
      } else {
        current.push(part);
      }
    }

    // One gap follows every segment except the last.
    if (s < item.answers.length) current.push({ gap: s });
  });

  flush();
  return chunks;
}

function Gap({
  value,
  onChange,
  pool,
  placeholder,
  letters,
  status,
}: {
  value: string;
  onChange: (value: string) => void;
  pool?: string[];
  placeholder: string;
  letters?: boolean;
  status?: 'ok' | 'wrong';
}) {
  const className =
    `gap${letters ? ' gap--letters' : ''}${status ? ` gap--${status}` : ''}`;

  if (pool?.length) {
    return (
      <select className={className} value={value} onChange={(e) => onChange(e.target.value)}>
        <option value="">—</option>
        {pool.map((word) => (
          <option key={word} value={word}>
            {word}
          </option>
        ))}
      </select>
    );
  }

  return (
    <input
      className={className}
      type="text"
      value={value}
      placeholder={placeholder}
      autoComplete="off"
      spellCheck={false}
      onChange={(e) => onChange(e.target.value)}
    />
  );
}

/** Reassemble the sentence with its correct answers filled in. */
function buildSolution(item: FillExercise): string {
  return item.segments
    .map((segment, s) => segment + (s < item.answers.length ? primaryAnswer(item.answers[s]) : ''))
    .join('')
    .replace(/\s+/g, ' ')
    .trim();
}
