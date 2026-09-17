import { parseArray, trimNulls } from '../lib/cparse.mjs';

/** Shared conversions for the three Deutsch units. */

export const opt = (v) => (v === null || v === undefined || v === '' ? undefined : String(v));

/** Split the pipe-separated accepted-answer lists used by TypedQ and friends. */
export const alternatives = (s) =>
  String(s ?? '').split('|').map((a) => a.trim()).filter(Boolean);

/** `VerbQ`/`SeqQ` `{before, after, answer}` becomes a single-gap fill sentence. */
export function verbFill(row, meaning, extra = {}) {
  const [before, after, answer] = row;
  return {
    kind: 'fill',
    segments: [before ?? '', after ?? ''],
    answers: [String(answer)],
    input: 'select',
    ...(meaning ? { meaning: String(meaning) } : {}),
    ...extra,
  };
}

/**
 * `U3Fill` alternates literal segments and answers: s0, a0, s1, a1, s2, a2, s3.
 * Trailing NULLs mean the row uses fewer than three gaps.
 */
export function u3Fill(row, { gloss } = {}) {
  const [num, ...rest] = row;
  const mean = rest.pop();
  const parts = rest;

  const segments = [];
  const answers = [];
  for (let i = 0; i < parts.length; i++) {
    if (i % 2 === 0) segments.push(parts[i] ?? '');
    else if (parts[i] !== null && parts[i] !== undefined) answers.push(String(parts[i]));
  }

  // Drop segments that only exist to pad unused gaps.
  while (segments.length > answers.length + 1) segments.pop();

  return {
    kind: 'fill',
    segments,
    answers,
    input: 'select',
    ...(opt(num) ? { label: String(num) } : {}),
    ...(opt(mean) ? { meaning: String(mean) } : {}),
    ...(gloss ? { glosses: [String(gloss)] } : {}),
  };
}

/** `U3Let`: s0, a0, s1, a1, s2, a2, s3, a3, s4 — gaps hold missing letters. */
export function u3Letters(row) {
  const [num, s0, a0, s1, a1, s2, a2, s3, a3, s4, mean] = row;
  const rawSegments = [s0, s1, s2, s3, s4];
  const rawAnswers = [a0, a1, a2, a3];

  const answers = rawAnswers.filter((a) => a !== null && a !== undefined).map(String);
  const segments = rawSegments.slice(0, answers.length + 1).map((s) => s ?? '');

  return {
    kind: 'fill',
    segments,
    answers,
    input: 'text',
    letters: true,
    ...(opt(num) ? { label: String(num) } : {}),
    ...(opt(mean) ? { meaning: String(mean) } : {}),
  };
}

/**
 * `Ex4Row`: seg[5], ans[4] (missing letters), czech, word[4], gmean[4], gaps.
 * Each gap's gloss pairs the completed German word with its Czech meaning.
 */
export function ex4Letters(row) {
  const [seg, ans, czech, word, gmean, gaps] = row;
  const answers = ans.slice(0, gaps).map(String);
  const segments = seg.slice(0, gaps + 1).map((s) => s ?? '');

  const glosses = answers.map((_, i) => {
    const w = opt(word?.[i]);
    const m = opt(gmean?.[i]);
    if (w && m) return `${w} — ${m}`;
    return w ?? m ?? null;
  });

  return {
    kind: 'fill',
    segments,
    answers,
    input: 'text',
    letters: true,
    ...(opt(czech) ? { meaning: String(czech) } : {}),
    glosses,
  };
}

/** `Ex2Row`: num, seg[4], ans[3], gaps — grouped under an `Ex2Item`'s translation. */
export function ex2Row(row, czech) {
  const [num, seg, ans, gaps] = row;
  return {
    kind: 'fill',
    segments: seg.slice(0, gaps + 1).map((s) => s ?? ''),
    answers: ans.slice(0, gaps).map(String),
    input: 'select',
    ...(opt(num) ? { label: String(num) } : {}),
    ...(opt(czech) ? { meaning: String(czech) } : {}),
  };
}

/** `ChoiceQ`: prompt, options[5], n_options, correct. */
export function choiceItem(row, meaning) {
  const [prompt, options, n, correct] = row;
  const list = trimNulls(options ?? []).map(String);
  const count = typeof n === 'number' ? n : list.length;

  if (typeof correct !== 'number' || correct < 0 || correct >= count) {
    throw new Error(`choice '${prompt}': correct index ${correct} out of range`);
  }

  return {
    kind: 'choice',
    prompt: String(prompt),
    options: list.slice(0, count),
    correct,
    ...(opt(meaning) ? { meaning: String(meaning) } : {}),
  };
}

/** `TypedQ`: prompt, pipe-separated answers, Czech meaning. */
export function typedItem(row) {
  const [prompt, answers, meaning] = row;
  return {
    kind: 'typed',
    prompt: String(prompt),
    answers: alternatives(answers),
    ...(opt(meaning) ? { meaning: String(meaning) } : {}),
  };
}

/** `FreeQ`: question, Czech question, sample, Czech sample. */
export function freeItem(row) {
  const [question, questionCs, sample, sampleCs] = row;
  return {
    kind: 'free',
    question: String(question),
    ...(opt(questionCs) ? { questionCs: String(questionCs) } : {}),
    sample: String(sample),
    ...(opt(sampleCs) ? { sampleCs: String(sampleCs) } : {}),
  };
}

/** `AssemblyItem`: prompt, words[6], n. */
export function assemblyItem(row, meaning) {
  const [prompt, words] = row;
  return {
    kind: 'assembly',
    prompt: opt(prompt) ?? '',
    words: trimNulls(words).map(String),
    ...(opt(meaning) ? { meaning: String(meaning) } : {}),
  };
}

/** One `assign` exercise built from `AssignItem[]`, group labels and meanings. */
export function assignExercise(src, itemsName, groupsName, meaningName) {
  const meanings = meaningName ? parseArray(src, meaningName) : [];
  return {
    kind: 'assign',
    groups: parseArray(src, groupsName).map(String),
    items: parseArray(src, itemsName).map(([emoji, label, group], i) => ({
      emoji: emoji ?? null,
      label: String(label),
      group,
      ...(opt(meanings[i]) ? { meaning: String(meanings[i]) } : {}),
    })),
  };
}

/** Vocabulary trainer sections: `TransSection[]` referencing `TypedQ[]` arrays. */
export function vocabSections(src, sectionsName) {
  return parseArray(src, sectionsName).map(([header, arrayRef]) => {
    const name = arrayRef?.__c;
    if (!name) throw new Error(`vocab section '${header}' has no data array`);
    return {
      header: String(header),
      items: parseArray(src, name).map(([prompt, answers, meaning]) => ({
        prompt: String(prompt),
        answers: alternatives(answers),
        ...(opt(meaning) ? { meaning: String(meaning) } : {}),
      })),
    };
  });
}
