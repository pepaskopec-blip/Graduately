import { readFile, readdir } from 'node:fs/promises';
import { join } from 'node:path';

import type {
  Lesson,
  Path,
  Subject,
  Theme,
} from '../../../content/schema';

/**
 * Server-side access to the shared `content/` folder.
 *
 * Everything is read at build time and cached per process, so pages can stay
 * static. Lesson JSON is the same data the desktop app compiles in, exported by
 * `tools/build-content.mjs`.
 */

const CONTENT_DIR = join(process.cwd(), '..', 'content');

const cache = new Map<string, unknown>();

async function readJson<T>(relPath: string): Promise<T> {
  const hit = cache.get(relPath);
  if (hit) return hit as T;

  const raw = await readFile(join(CONTENT_DIR, relPath), 'utf8');
  const parsed = JSON.parse(raw) as T;
  cache.set(relPath, parsed);
  return parsed;
}

export interface TranslationTable {
  cs: Record<string, string>;
  en: Record<string, string>;
}

/** Merged UI + lesson translations, so callers need only one lookup table. */
export async function getTranslations(): Promise<TranslationTable> {
  const [ui, content] = await Promise.all([
    readJson<TranslationTable>('i18n/ui.json'),
    readJson<TranslationTable>('i18n/content.json'),
  ]);

  return {
    cs: { ...content.cs, ...ui.cs },
    en: { ...content.en, ...ui.en },
  };
}

export const getThemes = () => readJson<Theme[]>('themes.json');
export const getSubjects = () => readJson<Subject[]>('subjects.json');
export const getPaths = () => readJson<Path[]>('paths.json');

/**
 * The main path for a subject, or one of its branches.
 *
 * Czech has two: the top-level area picker and the reading list hanging off it.
 */
export async function getPath(subject: string, branch?: string): Promise<Path | undefined> {
  const paths = await getPaths();
  return paths.find((p) => p.subject === subject && p.branch === branch);
}

export function getLesson(subject: string, id: string): Promise<Lesson> {
  return readJson<Lesson>(`lessons/${subject}/${id}.json`);
}

/** Lesson ids for a subject, in path order. */
export async function getLessonIds(subject: string): Promise<string[]> {
  try {
    const files = await readdir(join(CONTENT_DIR, 'lessons', subject));
    return files.filter((f) => f.endsWith('.json')).map((f) => f.replace(/\.json$/, '')).sort();
  } catch {
    return [];
  }
}

export async function getLessons(subject: string): Promise<Lesson[]> {
  const ids = await getLessonIds(subject);
  return Promise.all(ids.map((id) => getLesson(subject, id)));
}

export type * from '../../../content/schema';
