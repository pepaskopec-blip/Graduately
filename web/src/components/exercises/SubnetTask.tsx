'use client';

import { useState } from 'react';

import type { SubnetExercise } from '@/content';
import { useApp } from '@/app/providers';
import { TaskShell, type CheckState, type TaskFrame } from './TaskShell';

/** Columns of the VLSM worksheet, matching the desktop table headings. */
const FIELDS = [
  { key: 'network', cs: 'Síťová adresa', en: 'Network address' },
  { key: 'broadcast', cs: 'Broadcast', en: 'Broadcast' },
  { key: 'firstHost', cs: 'První uzel', en: 'First host' },
  { key: 'lastHost', cs: 'Poslední uzel', en: 'Last host' },
] as const;

type Entry = { prefix: string; network: string; broadcast: string; firstHost: string; lastHost: string };

const emptyEntry = (): Entry => ({
  prefix: '',
  network: '',
  broadcast: '',
  firstHost: '',
  lastHost: '',
});

/**
 * VLSM subnetting: read the scenario, then fill in prefix, network address,
 * broadcast and host range for each of the four subnets.
 */
export function SubnetTask({
  items,
  onSolved,
  ...frame
}: TaskFrame & {
  items: SubnetExercise[];
  onSolved: () => void;
}) {
  const { lang } = useApp();
  const subnetWord = lang === 'en' ? 'Subnet' : 'Podsíť';
  const [entries, setEntries] = useState<Entry[][]>(() =>
    items.map((item) => (item.blocks ?? []).map(emptyEntry))
  );
  const [state, setState] = useState<CheckState>('idle');
  const [checked, setChecked] = useState(false);
  const [revealed, setRevealed] = useState(false);

  const cellOk = (taskIndex: number, blockIndex: number, field: keyof Entry) => {
    const block = items[taskIndex].blocks?.[blockIndex];
    if (!block) return true;
    const typed = entries[taskIndex][blockIndex][field].trim();
    const expected = field === 'prefix' ? String(block.prefix) : block[field];
    return typed.replace(/^\//, '') === String(expected);
  };

  const check = () => {
    setChecked(true);
    const allCorrect = items.every((item, ti) =>
      // Scenarios without a valid allocation are informational only.
      !item.blocks ||
      item.blocks.every((_, bi) =>
        (['prefix', ...FIELDS.map((f) => f.key)] as (keyof Entry)[]).every((f) =>
          cellOk(ti, bi, f)
        )
      )
    );
    setState(allCorrect ? 'ok' : 'wrong');
    if (allCorrect) onSolved();
  };

  const set = (ti: number, bi: number, field: keyof Entry, value: string) => {
    setEntries((prev) =>
      prev.map((blocks, i) =>
        i === ti ? blocks.map((b, j) => (j === bi ? { ...b, [field]: value } : b)) : blocks
      )
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
      {items.map((item, ti) => (
        <div className="ex__item" key={ti}>
          <p className="ex__prompt" style={{ whiteSpace: 'pre-wrap' }}>
            {item.scenario}
          </p>

          {item.blocks ? (
            <div className="stack" style={{ gap: 12 }}>
              {item.blocks.map((_, bi) => (
                <div key={bi} className="stack" style={{ gap: 6 }}>
                  <span className="card__kicker">
                    {/* Subnets are labelled A-D in the scenario text. */}
                    {`${subnetWord} ${String.fromCharCode(65 + bi)}`}
                  </span>

                  <div className="row">
                    <label className="row" style={{ gap: 6 }}>
                      <span className="muted">Prefix /</span>
                      <input
                        className={`gap${checked ? (cellOk(ti, bi, 'prefix') ? ' gap--ok' : ' gap--wrong') : ''}`}
                        style={{ minWidth: 70 }}
                        inputMode="numeric"
                        value={entries[ti][bi].prefix}
                        onChange={(e) => set(ti, bi, 'prefix', e.target.value)}
                      />
                    </label>

                    {FIELDS.map((field) => (
                      <label className="row" style={{ gap: 6 }} key={field.key}>
                        <span className="muted">{field[lang]}</span>
                        <input
                          className={`gap${checked ? (cellOk(ti, bi, field.key) ? ' gap--ok' : ' gap--wrong') : ''}`}
                          style={{ minWidth: 140 }}
                          autoComplete="off"
                          spellCheck={false}
                          value={entries[ti][bi][field.key]}
                          onChange={(e) => set(ti, bi, field.key, e.target.value)}
                        />
                      </label>
                    ))}
                  </div>
                </div>
              ))}
            </div>
          ) : (
            <p className="ex__meaning">{item.solution}</p>
          )}

          {revealed && item.blocks && (
            <p className="ex__model" style={{ whiteSpace: 'pre-wrap' }}>
              {item.solution}
            </p>
          )}
        </div>
      ))}
    </TaskShell>
  );
}
