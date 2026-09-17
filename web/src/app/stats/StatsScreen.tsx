'use client';

import { useApp } from '@/app/providers';
import { TopBar } from '@/components/TopBar';
import { useProgress } from '@/lib/progress';

export interface SubjectStats {
  id: string;
  key: string;
  lessons: {
    id: string;
    number: number;
    titleKey: string | null;
    title: string | null;
    taskCount: number;
  }[];
}

/**
 * Progress overview: totals at the top, then a bar per subject and a per-lesson
 * breakdown, following the layout of the desktop statistics page.
 */
export function StatsScreen({
  tracked,
  locked,
}: {
  tracked: SubjectStats[];
  locked: { id: string; key: string }[];
}) {
  const { t, tOpt } = useApp();
  const { doneCount, ready } = useProgress();

  const totals = tracked.reduce(
    (acc, subject) => {
      const total = subject.lessons.reduce((a, l) => a + l.taskCount, 0);
      const done = subject.lessons.reduce((a, l) => a + doneCount(subject.id, l.id), 0);
      const finished = subject.lessons.filter(
        (l) => l.taskCount > 0 && doneCount(subject.id, l.id) >= l.taskCount
      ).length;

      return {
        done: acc.done + done,
        total: acc.total + total,
        finishedUnits: acc.finishedUnits + finished,
        openUnits: acc.openUnits + subject.lessons.length,
      };
    },
    { done: 0, total: 0, finishedUnits: 0, openUnits: 0 }
  );

  const percent = totals.total ? Math.round((totals.done / totals.total) * 100) : 0;

  return (
    <div className="shell">
      <TopBar title={t('stats_title')} backHref="/subjects" showTools={false} />

      <main className="page">
        <div className="page__head">
          <h1 className="page__title">{t('stats_title')}</h1>
          <p className="page__sub">{t('stats_sub')}</p>
        </div>

        <div className="summary">
          <Cell label={t('stats_ex_label')} value={ready ? `${totals.done} / ${totals.total}` : '—'} />
          <Cell label={t('stats_pct_label')} value={ready ? `${percent} %` : '—'} />
          <Cell
            label={t('stats_units_label')}
            value={ready ? `${totals.finishedUnits} / ${totals.openUnits}` : '—'}
          />
        </div>

        <h2 className="settings__label">{t('stats_section')}</h2>

        {tracked.map((subject) => {
          const total = subject.lessons.reduce((a, l) => a + l.taskCount, 0);
          const done = subject.lessons.reduce((a, l) => a + doneCount(subject.id, l.id), 0);

          return (
            <div key={subject.id}>
              <Row
                name={t(subject.key)}
                count={total ? `${done} / ${total}` : t('stats_locked')}
                ratio={total ? done / total : 0}
              />

              {/* Per-lesson detail, only where a lesson has tasks. */}
              <div style={{ paddingLeft: 16 }}>
                {subject.lessons
                  .filter((lesson) => lesson.taskCount > 0)
                  .map((lesson) => {
                    const lessonDone = doneCount(subject.id, lesson.id);
                    return (
                      <Row
                        key={lesson.id}
                        name={`${lesson.number}. ${
                          tOpt(lesson.titleKey) ?? lesson.title ?? lesson.id
                        }`}
                        count={`${lessonDone} / ${lesson.taskCount}`}
                        ratio={lessonDone / lesson.taskCount}
                        small
                      />
                    );
                  })}
              </div>
            </div>
          );
        })}

        {locked.map((subject) => (
          <Row key={subject.id} name={t(subject.key)} count={t('stats_locked')} ratio={0} dim />
        ))}
      </main>
    </div>
  );
}

const Cell = ({ label, value }: { label: string; value: string }) => (
  <div className="summary__cell">
    <div className="summary__value tabular">{value}</div>
    <div className="summary__label">{label}</div>
  </div>
);

function Row({
  name,
  count,
  ratio,
  small,
  dim,
}: {
  name: string;
  count: string;
  ratio: number;
  small?: boolean;
  dim?: boolean;
}) {
  const complete = ratio >= 1;

  return (
    <div className="stat-row" style={{ opacity: dim ? 0.5 : 1 }}>
      <div className="stat-row__head">
        <span className="stat-row__name" style={small ? { fontSize: '0.9rem' } : undefined}>
          {name}
        </span>
        <span className="stat-row__count">{count}</span>
      </div>
      <div className="bar">
        <div
          className={`bar__fill${complete ? ' bar__fill--done' : ''}`}
          style={{ width: `${Math.min(100, ratio * 100)}%` }}
        />
      </div>
    </div>
  );
}
