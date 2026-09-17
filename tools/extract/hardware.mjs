import { readFileSync } from 'node:fs';
import { parseArrayInFunction, trimNulls } from '../lib/cparse.mjs';

/**
 * Hardware ("Technické vybavení").
 *
 * Structurally identical to the network lessons: each `build_hw_unitN_page`
 * holds a local `NetSlide slides[]` and each `build_hw_unitN_exercise_page`
 * holds a local `ChoiceQ qs[]` with a parallel `hints[]`.
 */

const HW_LESSONS = 3;

function toSlide(row, label) {
  if (!Array.isArray(row)) throw new Error(`${label}: slide is not a braced list`);
  const [kicker, title, tip, lines] = row;
  return {
    kicker: kicker ?? '',
    title: title ?? '',
    tip: tip ?? null,
    lines: trimNulls(lines ?? []).map(String),
  };
}

function toChoice(row, hint, label) {
  if (!Array.isArray(row)) throw new Error(`${label}: question is not a braced list`);
  const [prompt, options, nOptions, correct] = row;
  const list = trimNulls(options ?? []).map(String);
  const n = typeof nOptions === 'number' ? nOptions : list.length;

  if (typeof correct !== 'number' || correct < 0 || correct >= n) {
    throw new Error(`${label}: correct index ${correct} out of range for ${n} options`);
  }

  return {
    kind: 'choice',
    prompt: String(prompt),
    options: list.slice(0, n),
    correct,
    ...(hint ? { hint: String(hint) } : {}),
  };
}

export function extractHardware(desktopDir) {
  const src = readFileSync(`${desktopDir}/hw.c`, 'utf8');
  const lessons = [];

  for (let n = 1; n <= HW_LESSONS; n++) {
    const slides = parseArrayInFunction(src, `build_hw_unit${n}_page`, 'slides')
      .map((row, i) => toSlide(row, `hw unit ${n} slide ${i + 1}`));

    const exFn = `build_hw_unit${n}_exercise_page`;
    const qs = parseArrayInFunction(src, exFn, 'qs');
    const hints = parseArrayInFunction(src, exFn, 'hints');

    lessons.push({
      id: `unit-${String(n).padStart(2, '0')}`,
      subject: 'hardware',
      number: n,
      titleKey: `hw_unit${n}`,
      subtitleKey: `hw_unit${n}_sub`,
      slides,
      tasks: [{
        number: 1,
        title: 'Quiz',
        subtitleKey: `hw_quiz${n}_head`,
        items: qs.map((row, i) => toChoice(row, hints[i], `hw unit ${n} q${i + 1}`)),
      }],
    });
  }

  return lessons;
}
