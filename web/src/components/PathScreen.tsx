'use client';

import type { Path } from '@/content';
import { useApp } from '@/app/providers';
import { useProgress } from '@/lib/progress';
import { PathMap, type PathNode } from './PathMap';
import { TopBar } from './TopBar';

export interface PathScreenProps {
  path: Path;
  /** i18n key for the heading; falls back to `title` when it has no key. */
  titleKey?: string;
  title?: string;
  subtitleKey?: string;
  subtitle?: string;
  backHref: string;
  /** Base URL that each unit slug is appended to. */
  hrefBase: string;
  /** Tasks per lesson, used to decide whether a unit counts as finished. */
  taskCounts: Record<string, number>;
  finish?: boolean;
}

/** Renders a subject's serpentine unit path with live completion state. */
export function PathScreen({
  path,
  titleKey,
  title,
  subtitleKey,
  subtitle,
  backHref,
  hrefBase,
  taskCounts,
  finish,
}: PathScreenProps) {
  const { tOpt } = useApp();
  const { doneCount, ready } = useProgress();

  const nodes: PathNode[] = path.units.map((unit) => {
    const total = unit.lesson ? taskCounts[unit.lesson] ?? 0 : 0;
    const done = unit.lesson ? doneCount(path.subject, unit.lesson) : 0;

    return {
      number: unit.number,
      label: tOpt(unit.titleKey) ?? unit.title?.cs ?? String(unit.number),
      // A node either opens a lesson under `hrefBase` or links on to its own path.
      href: unit.lesson ? `${hrefBase}/${unit.lesson}` : unit.route ?? null,
      locked: !unit.unlocked,
      // Only claim completion once progress has loaded, to avoid a flash of green.
      done: ready && total > 0 && done >= total,
    };
  });

  const heading = tOpt(titleKey) ?? title ?? '';
  const sub = tOpt(subtitleKey) ?? subtitle;

  return (
    <div className="shell">
      <TopBar title={heading} backHref={backHref} />

      <main className="page">
        <div className="page__head">
          <h1 className="page__title">{heading}</h1>
          {sub && <p className="page__sub">{sub}</p>}
        </div>

        <PathMap nodes={nodes} finish={finish} />
      </main>
    </div>
  );
}
