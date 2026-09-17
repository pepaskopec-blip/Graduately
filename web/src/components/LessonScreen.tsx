'use client';

import { useState } from 'react';

import type { Lesson } from '@/content';
import { useApp } from '@/app/providers';
import { useProgress } from '@/lib/progress';
import { PathMap, type PathNode } from './PathMap';
import { Slides } from './Slides';
import { TopBar } from './TopBar';
import { TaskRenderer } from './exercises/TaskRenderer';

/**
 * A lesson: optional theory slides, a map of exercise bubbles and the exercise
 * itself. All three live on one route and swap in place, which keeps the
 * student's progress state without a round trip.
 */
export function LessonScreen({
  lesson,
  backHref,
  vocabHref,
}: {
  lesson: Lesson;
  backHref: string;
  vocabHref?: string;
}) {
  const { t, tOpt } = useApp();
  const { isDone, markDone, ready } = useProgress();

  const hasSlides = lesson.slides.length > 0;
  const [view, setView] = useState<'slides' | 'map'>(hasSlides ? 'slides' : 'map');
  const [taskNumber, setTaskNumber] = useState<number | null>(null);

  const title = tOpt(lesson.titleKey) ?? lesson.title?.cs ?? lesson.id;
  const subtitle = tOpt(lesson.subtitleKey) ?? lesson.subtitle?.cs;

  const task = taskNumber === null ? null : lesson.tasks.find((x) => x.number === taskNumber);

  /* --- exercise view --- */
  if (task) {
    const heading = task.heading ?? task.title;
    return (
      <div className="shell">
        <TopBar title={heading} />
        <main className="page">
          <div className="page__head">
            <h1 className="page__title">{heading}</h1>
          </div>

          <TaskRenderer
            key={task.number}
            task={task}
            subtitle={tOpt(task.subtitleKey) ?? task.subtitle}
            onSolved={() => markDone(lesson.subject, lesson.id, task.number)}
          />

          <div className="actions" style={{ marginTop: 28 }}>
            <button className="btn btn--ghost" onClick={() => setTaskNumber(null)}>
              {t('back')}
            </button>
          </div>
        </main>
      </div>
    );
  }

  /* --- theory view --- */
  if (view === 'slides') {
    return (
      <div className="shell">
        <TopBar title={title} backHref={backHref} />
        <main className="page">
          <div className="page__head">
            <h1 className="page__title">{title}</h1>
            {subtitle && <p className="page__sub">{subtitle}</p>}
          </div>

          <Slides slides={lesson.slides} onFinish={() => setView('map')} />
        </main>
      </div>
    );
  }

  /* --- exercise map --- */
  const nodes: PathNode[] = lesson.tasks.map((x) => ({
    number: x.number,
    label: x.title,
    locked: false,
    done: ready && isDone(lesson.subject, lesson.id, x.number),
  }));

  return (
    <div className="shell">
      <TopBar title={title} backHref={backHref} />
      <main className="page">
        <div className="page__head">
          <h1 className="page__title">{title}</h1>
          {subtitle && <p className="page__sub">{subtitle}</p>}
        </div>

        {/* A single-task lesson (every network quiz) needs no bubble map. */}
        {lesson.tasks.length === 1 ? (
          <div className="stack">
            <button className="btn" onClick={() => setTaskNumber(lesson.tasks[0].number)}>
              {lesson.tasks[0].title}
            </button>
          </div>
        ) : (
          <PathMap nodes={nodes} onSelect={(node) => setTaskNumber(node.number)} />
        )}

        <div className="actions" style={{ marginTop: 32 }}>
          {hasSlides && (
            <button className="btn btn--ghost" onClick={() => setView('slides')}>
              {t('net_slide_prev')}
            </button>
          )}
          {vocabHref && (
            <a className="btn btn--ghost" href={vocabHref}>
              Vokabeltraining
            </a>
          )}
        </div>
      </main>
    </div>
  );
}
