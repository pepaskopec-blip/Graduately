'use client';

import { useCallback, useEffect, useState } from 'react';

/**
 * Exercise completion, stored in localStorage.
 *
 * This is the browser equivalent of the desktop app's `progress/*.conf` files:
 * one boolean per task, keyed by subject and lesson.
 */

const STORAGE_KEY = 'maturita.progress';

/** `{ [subject]: { [lessonId]: taskNumber[] } }` */
export type ProgressMap = Record<string, Record<string, number[]>>;

function read(): ProgressMap {
  if (typeof window === 'undefined') return {};
  try {
    const raw = window.localStorage.getItem(STORAGE_KEY);
    return raw ? (JSON.parse(raw) as ProgressMap) : {};
  } catch {
    return {};
  }
}

/** Notifies every hook instance in the tab, which `storage` events do not. */
const CHANGE_EVENT = 'maturita.progress.change';

function write(next: ProgressMap) {
  window.localStorage.setItem(STORAGE_KEY, JSON.stringify(next));
  window.dispatchEvent(new Event(CHANGE_EVENT));
}

export function useProgress() {
  const [progress, setProgress] = useState<ProgressMap>({});
  const [ready, setReady] = useState(false);

  useEffect(() => {
    setProgress(read());
    setReady(true);

    const sync = () => setProgress(read());
    window.addEventListener(CHANGE_EVENT, sync);
    window.addEventListener('storage', sync);
    return () => {
      window.removeEventListener(CHANGE_EVENT, sync);
      window.removeEventListener('storage', sync);
    };
  }, []);

  const markDone = useCallback((subject: string, lesson: string, task: number) => {
    const next = read();
    const lessons = (next[subject] ??= {});
    const done = (lessons[lesson] ??= []);
    if (!done.includes(task)) {
      done.push(task);
      done.sort((a, b) => a - b);
      write(next);
    }
  }, []);

  const reset = useCallback(() => write({}), []);

  const isDone = useCallback(
    (subject: string, lesson: string, task: number) =>
      progress[subject]?.[lesson]?.includes(task) ?? false,
    [progress]
  );

  const doneCount = useCallback(
    (subject: string, lesson?: string) => {
      const lessons = progress[subject];
      if (!lessons) return 0;
      if (lesson) return lessons[lesson]?.length ?? 0;
      return Object.values(lessons).reduce((a, tasks) => a + tasks.length, 0);
    },
    [progress]
  );

  return { progress, ready, markDone, isDone, reset, doneCount };
}
