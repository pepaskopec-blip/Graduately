#!/usr/bin/env node
/**
 * Regenerate `content/` from the desktop C sources.
 *
 * The C app remains the authoring surface for now, so this is a one-way
 * export: run it after changing lesson data in `desktop/` to refresh the JSON
 * the web app reads. It is deliberately strict — a shape it does not recognise
 * throws rather than emitting a half-empty lesson.
 */

import { mkdirSync, writeFileSync, rmSync } from 'node:fs';
import { dirname, resolve } from 'node:path';
import { fileURLToPath } from 'node:url';

import { parseArray } from './lib/cparse.mjs';
import { readFileSync } from 'node:fs';
import { extractThemes } from './extract/themes.mjs';
import { extractI18n } from './extract/i18n.mjs';
import { extractNetworks } from './extract/networks.mjs';
import { extractDeutschUnit1 } from './extract/deutsch.mjs';
import { extractDeutschUnit2 } from './extract/deutsch-u2.mjs';
import { extractDeutschUnit3 } from './extract/deutsch-u3.mjs';
import { extractHardware } from './extract/hardware.mjs';
import { extractMluvnice } from './extract/mluvnice.mjs';
import { extractCetba } from './extract/cetba.mjs';

const ROOT = resolve(dirname(fileURLToPath(import.meta.url)), '..');
const DESKTOP = resolve(ROOT, 'desktop');
const CONTENT = resolve(ROOT, 'content');

const write = (relPath, data) => {
  const file = resolve(CONTENT, relPath);
  mkdirSync(dirname(file), { recursive: true });
  writeFileSync(file, JSON.stringify(data, null, 2) + '\n');
  return relPath;
};

/* ------------------------------------------------------------- subjects --- */

/** Mirrors `sub_keys` in subjects.c; only four subjects have content today. */
const SUBJECTS = [
  { id: 'deutsch', icon: 'flag-de', route: '/deutsch' },
  { id: 'networks', icon: 'wifi', route: '/networks' },
  { id: 'hardware', icon: 'chip', route: '/hardware' },
  { id: 'czech', icon: 'book', route: '/czech' },
  { id: 'civics', icon: null, route: null },
  { id: 'english', icon: null, route: null },
  { id: 'math', icon: null, route: null },
  { id: 'physics', icon: null, route: null },
  { id: 'science', icon: null, route: null },
  { id: 'tech-graphics', icon: null, route: null },
  { id: 'presentation', icon: null, route: null },
  { id: 'programming', icon: null, route: null },
  { id: 'software', icon: null, route: null },
];

function extractSubjects() {
  const keys = parseArray(readFileSync(`${DESKTOP}/subjects.c`, 'utf8'), 'sub_keys').map(String);
  if (keys.length !== SUBJECTS.length) {
    throw new Error(`subjects.c has ${keys.length} keys but the extractor knows ${SUBJECTS.length}`);
  }
  return SUBJECTS.map((s, i) => ({ ...s, key: keys[i], unlocked: s.route !== null }));
}

/* ---------------------------------------------------------------- paths --- */

/** German unit titles from `unit_meta_init` in main.c. */
const DEUTSCH_TITLES = [
  'Neue Freunde', 'Aus aller Welt', 'Bei uns zu Hause',
  'Schule und Freizeit', 'Guten Appetit!', 'Mein Tagesablauf',
  'Meine Freunde', 'Wir treffen uns in Salzburg',
  'Mein Haus ist meine Burg', 'Urlaub in Österreich',
];

/**
 * `HW_UNITS` bubbles are drawn but only `HW_LESSONS` of them are playable, so
 * the trailing nodes stay locked placeholders as they do on the desktop map.
 */
const HW_UNITS = 10;

/** `cz_keys` in czech.c; "Literatura" has no content yet. */
const CZECH_NODES = [
  { number: 1, title: 'Literatura', route: null },
  { number: 2, title: 'Mluvnice', route: '/czech/mluvnice' },
  { number: 3, title: 'Maturitní četba', route: '/czech/reading' },
];

function buildPaths({ networkLessons, deutschLessons, hardwareLessons, cetbaLessons }) {
  const done = new Set(deutschLessons.map((l) => l.number));

  const deutsch = {
    subject: 'deutsch',
    units: DEUTSCH_TITLES.map((title, i) => ({
      number: i + 1,
      // Units 1-3 are playable in the desktop app; the rest are placeholders.
      lesson: done.has(i + 1) ? `unit-${String(i + 1).padStart(2, '0')}` : null,
      title: { cs: title, en: title },
      unlocked: i < 3 && done.has(i + 1),
    })),
  };

  const networks = {
    subject: 'networks',
    year: 1,
    units: networkLessons.map((l) => ({
      number: l.number,
      lesson: l.id,
      titleKey: l.titleKey,
      unlocked: true,
    })),
  };

  const byNumber = new Map(hardwareLessons.map((l) => [l.number, l]));
  const hardware = {
    subject: 'hardware',
    units: Array.from({ length: HW_UNITS }, (_, i) => {
      const lesson = byNumber.get(i + 1);
      return {
        number: i + 1,
        lesson: lesson?.id ?? null,
        titleKey: lesson?.titleKey ?? `hw_unit${i + 1}`,
        unlocked: Boolean(lesson),
      };
    }),
  };

  const czech = {
    subject: 'czech',
    units: CZECH_NODES.map((node) => ({
      number: node.number,
      lesson: null,
      route: node.route,
      title: { cs: node.title, en: node.title },
      unlocked: node.route !== null,
    })),
  };

  const reading = {
    subject: 'czech',
    branch: 'reading',
    units: cetbaLessons.map((l, i) => ({
      number: i + 1,
      lesson: l.id,
      title: l.title,
      unlocked: true,
    })),
  };

  return [deutsch, networks, hardware, czech, reading];
}

/* ----------------------------------------------------------------- main --- */

function main() {
  rmSync(resolve(CONTENT, 'lessons'), { recursive: true, force: true });

  const written = [];

  written.push(write('themes.json', extractThemes(DESKTOP)));

  const i18n = extractI18n(DESKTOP);
  written.push(write('i18n/ui.json', i18n.ui));
  written.push(write('i18n/content.json', i18n.content));

  const subjects = extractSubjects();
  written.push(write('subjects.json', subjects));

  const networkLessons = extractNetworks(DESKTOP);
  for (const lesson of networkLessons) {
    written.push(write(`lessons/networks/${lesson.id}.json`, lesson));
  }

  const deutschLessons = [
    extractDeutschUnit1(DESKTOP),
    extractDeutschUnit2(DESKTOP),
    extractDeutschUnit3(DESKTOP),
  ];
  for (const lesson of deutschLessons) {
    written.push(write(`lessons/deutsch/${lesson.id}.json`, lesson));
  }

  const hardwareLessons = extractHardware(DESKTOP);
  for (const lesson of hardwareLessons) {
    written.push(write(`lessons/hardware/${lesson.id}.json`, lesson));
  }

  const czechLessons = [extractMluvnice(DESKTOP), ...extractCetba(DESKTOP)];
  for (const lesson of czechLessons) {
    written.push(write(`lessons/czech/${lesson.id}.json`, lesson));
  }

  const cetbaLessons = czechLessons.filter((l) => l.id !== 'mluvnice');

  const paths = buildPaths({
    networkLessons,
    deutschLessons,
    hardwareLessons,
    cetbaLessons,
  });
  written.push(write('paths.json', paths));

  written.push(write('manifest.json', {
    subjects: subjects.map((s) => s.id),
    lessons: {
      deutsch: deutschLessons.map((l) => l.id),
      networks: networkLessons.map((l) => l.id),
      hardware: hardwareLessons.map((l) => l.id),
      czech: czechLessons.map((l) => l.id),
    },
  }));

  const stats = {
    'network lessons': networkLessons.length,
    'network slides': networkLessons.reduce((a, l) => a + l.slides.length, 0),
    'network items': networkLessons.reduce(
      (a, l) => a + l.tasks.reduce((b, t) => b + t.items.length, 0), 0),
    'deutsch tasks': deutschLessons.reduce((a, l) => a + l.tasks.length, 0),
    'deutsch items': deutschLessons.reduce(
      (a, l) => a + l.tasks.reduce((b, t) => b + t.items.length, 0), 0),
    'vocab words': deutschLessons.reduce(
      (a, l) => a + (l.vocab ?? []).reduce((b, s) => b + s.items.length, 0), 0),
    'hardware lessons': hardwareLessons.length,
    'hardware slides': hardwareLessons.reduce((a, l) => a + l.slides.length, 0),
    'hardware items': hardwareLessons.reduce(
      (a, l) => a + l.tasks.reduce((b, t) => b + t.items.length, 0), 0),
    'czech lessons': czechLessons.length,
    'czech tasks': czechLessons.reduce((a, l) => a + l.tasks.length, 0),
    'czech items': czechLessons.reduce(
      (a, l) => a + l.tasks.reduce((b, t) => b + t.items.length, 0), 0),
    'ui strings': Object.keys(i18n.ui.cs).length,
    'content strings': Object.keys(i18n.content.cs).length,
    themes: 10,
  };

  console.log(`wrote ${written.length} files to content/`);
  for (const [k, v] of Object.entries(stats)) console.log(`  ${k}: ${v}`);
}

main();
