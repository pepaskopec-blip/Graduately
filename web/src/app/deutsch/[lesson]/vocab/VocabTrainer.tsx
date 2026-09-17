'use client';

import { useMemo, useState } from 'react';

import type { VocabSection } from '@/content';
import { useApp } from '@/app/providers';
import { TopBar } from '@/components/TopBar';
import { isCorrect } from '@/lib/answers';

interface Card {
  section: string;
  prompt: string;
  answers: string[];
  meaning?: string;
}

/**
 * Vocabulary drill.
 *
 * Follows the desktop rule from `sub_translate`: a wrong answer goes back to the
 * end of the queue, so the round only finishes once every word was translated
 * correctly at least once.
 */
export function VocabTrainer({
  sections,
  backHref,
}: {
  sections: VocabSection[];
  backHref: string;
}) {
  const { t } = useApp();

  const cards = useMemo<Card[]>(
    () =>
      sections.flatMap((section) =>
        section.items.map((item) => ({ section: section.header, ...item }))
      ),
    [sections]
  );

  const [queue, setQueue] = useState<number[]>(() => cards.map((_, i) => i));
  const [value, setValue] = useState('');
  const [verdict, setVerdict] = useState<'idle' | 'ok' | 'wrong' | 'done'>('idle');
  const [solvedCount, setSolvedCount] = useState(0);
  /** Answer of the card just missed, kept because the queue has already rotated. */
  const [missed, setMissed] = useState('');

  const current = queue.length > 0 ? cards[queue[0]] : null;
  const total = cards.length;

  const submit = () => {
    if (!current || verdict === 'done') return;

    if (current.answers.some((a) => isCorrect(value, a))) {
      const rest = queue.slice(1);
      setSolvedCount((n) => n + 1);
      setQueue(rest);
      setValue('');
      setVerdict(rest.length === 0 ? 'done' : 'ok');
      return;
    }

    // Wrong: show the expected answer and requeue the card at the back.
    setMissed(current.answers[0] ?? '');
    setVerdict('wrong');
    setQueue((q) => [...q.slice(1), q[0]]);
    setValue('');
  };

  return (
    <div className="shell">
      <TopBar title="Vokabeltraining" backHref={backHref} />

      <main className="page">
        <div className="page__head">
          <h1 className="page__title">Vokabeltraining</h1>
          <p className="page__sub">{t('sub_translate')}</p>
        </div>

        <div className="stack">
          <div className="bar" aria-hidden>
            <div
              className="bar__fill"
              style={{ width: `${total ? (solvedCount / total) * 100 : 0}%` }}
            />
          </div>
          <p className="muted tabular">
            {t('trans_progress').replace('%d', String(solvedCount)).replace('%d', String(total))}
          </p>

          {verdict === 'done' || !current ? (
            <p className="feedback feedback--ok" role="status">
              {t('trans_done')}
            </p>
          ) : (
            <div className="ex__item">
              <span className="card__kicker">{current.section}</span>
              <p className="ex__prompt" style={{ fontSize: '1.3rem' }}>
                {current.prompt}
              </p>

              <form
                onSubmit={(e) => {
                  e.preventDefault();
                  submit();
                }}
              >
                <div className="row">
                  <input
                    className="gap"
                    style={{ flex: 1, minWidth: 200 }}
                    type="text"
                    value={value}
                    placeholder={t('your_answer')}
                    autoComplete="off"
                    autoFocus
                    spellCheck={false}
                    onChange={(e) => setValue(e.target.value)}
                  />
                  <button className="btn" type="submit">
                    {t('check')}
                  </button>
                </div>
              </form>

              {verdict === 'ok' && (
                <p className="feedback feedback--ok" role="status">
                  {t('feedback_ok')}
                </p>
              )}
              {verdict === 'wrong' && (
                <p className="feedback feedback--err" role="status">
                  {t('trans_wrong').replace('%s', missed)}
                </p>
              )}
            </div>
          )}
        </div>
      </main>
    </div>
  );
}
