import { readFileSync } from 'node:fs';
import { parseArray, trimNulls } from '../lib/cparse.mjs';

/**
 * Deutsch Unit 1 ("Neue Freunde").
 *
 * The desktop app defines each exercise's data as a `const` array next to its
 * `build_*` function; the task order and titles come from
 * `build_exercise_page` in main.c and `u1_ex_names` in globals.c.
 */

const opt = (v) => (v === null || v === undefined || v === '' ? undefined : String(v));

/** Split the pipe-separated accepted-answer lists used by TypedQ. */
const alternatives = (s) => String(s).split('|').map((a) => a.trim()).filter(Boolean);

/** `{before, after, answer}` becomes a single-gap fill sentence. */
function verbFill(row, meaning, extra = {}) {
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

export function extractDeutschUnit1(desktopDir) {
  const u1 = readFileSync(`${desktopDir}/exercise_u1.c`, 'utf8');
  const common = readFileSync(`${desktopDir}/exercise_common.c`, 'utf8');

  const tasks = [];

  /* 1 — Dialog: three dialogues sharing one word pool, one gap per line. */
  {
    const meanings = parseArray(common, 'ex1_meaning');
    const pool = parseArray(common, 'ex1_pool');
    const dialogues = [
      ['Sich vorstellen', parseArray(common, 'ex1_d1')],
      ['Begrüßung', parseArray(common, 'ex1_d2')],
      ['Verabschiedung', parseArray(common, 'ex1_d3')],
    ];

    let i = 0;
    const items = dialogues.map(([title, rows]) => ({
      kind: 'dialogue',
      title,
      rows: rows.map(([speaker, before, answer, after]) => ({
        speaker: speaker ?? '',
        before: before ?? '',
        answer: String(answer),
        after: after ?? '',
        meaning: opt(meanings[i++]),
      })),
    }));

    tasks.push({ number: 1, title: 'Dialog', subtitleKey: 'sub_dialog', pool, items });
  }

  /* 2 — Sätze bilden */
  {
    const meanings = parseArray(common, 'ex2_meaning');
    tasks.push({
      number: 2,
      title: 'Sätze bilden',
      subtitleKey: 'sub_assembly',
      items: parseArray(common, 'ex2_items').map(([prompt, words], i) => ({
        kind: 'assembly',
        prompt: opt(prompt) ?? '',
        words: trimNulls(words).map(String),
        meaning: opt(meanings[i]),
      })),
    });
  }

  /* 3 + 9 — multiple choice */
  const choiceTask = (number, title, subtitleKey, qsName, meanName) => {
    const meanings = parseArray(u1, meanName);
    return {
      number,
      title,
      subtitleKey,
      items: parseArray(u1, qsName).map(([prompt, options, n, correct], i) => ({
        kind: 'choice',
        prompt: String(prompt),
        options: trimNulls(options).map(String).slice(0, n),
        correct,
        meaning: opt(meanings[i]),
      })),
    };
  };
  tasks.push(choiceTask(3, 'Was ist richtig?', 'sub_choice_num', 'ex3_questions', 'ex3_meaning'));

  /* 4 — Freie Antwort */
  tasks.push({
    number: 4,
    title: 'Freie Antwort',
    subtitleKey: 'sub_free',
    items: parseArray(u1, 'ex4_questions').map(([question, questionCs, sample, sampleCs]) => ({
      kind: 'free',
      question: String(question),
      questionCs: opt(questionCs),
      sample: String(sample),
      sampleCs: opt(sampleCs),
    })),
  });

  /* 5 — Zahlen: digits shown as the clue, pick the German word. */
  {
    const meanings = parseArray(u1, 'ex5_meaning');
    tasks.push({
      number: 5,
      title: 'Zahlen',
      subtitleKey: 'sub_zahlen',
      pool: parseArray(u1, 'ex5_pool'),
      items: parseArray(u1, 'ex5_data').map(([digits, answer], i) => ({
        kind: 'fill',
        segments: ['', ''],
        answers: [String(answer)],
        input: 'select',
        clue: String(digits),
        meaning: opt(meanings[i]),
      })),
    });
  }

  /* 6 — Wie viel?: the emoji is repeated `count` times as the clue. */
  {
    const meanings = parseArray(u1, 'ex6_meaning');
    tasks.push({
      number: 6,
      title: 'Wie viel?',
      subtitleKey: 'sub_wieviel',
      pool: parseArray(u1, 'ex6_pool'),
      items: parseArray(u1, 'ex6_data').map(([emoji, count, noun, answer], i) => ({
        kind: 'fill',
        segments: ['', ` ${noun}`],
        answers: [String(answer)],
        input: 'select',
        emoji: Array.from({ length: count }, () => emoji).join(' '),
        meaning: opt(meanings[i]),
      })),
    });
  }

  /* 7 — Zahlenreihe */
  {
    const meanings = parseArray(u1, 'ex7_meaning');
    tasks.push({
      number: 7,
      title: 'Zahlenreihe',
      subtitleKey: 'sub_reihe',
      pool: parseArray(u1, 'ex7_pool'),
      items: parseArray(u1, 'ex7_data').map((row, i) => verbFill(row, meanings[i])),
    });
  }

  /* 8 — Verb einsetzen */
  {
    const meanings = parseArray(u1, 'ex8_meaning');
    tasks.push({
      number: 8,
      title: 'Verb einsetzen',
      subtitleKey: 'sub_verb',
      pool: parseArray(u1, 'ex8_pool'),
      items: parseArray(u1, 'ex8_data').map((row, i) => verbFill(row, meanings[i])),
    });
  }

  tasks.push(choiceTask(9, 'Wer? Wie? Wo?', 'sub_wer', 'ex9_questions', 'ex9_meaning'));

  /* 10 — Wörter trennen: split a run-together string into words. */
  {
    const meanings = parseArray(u1, 'ex10_meaning');
    tasks.push({
      number: 10,
      title: 'Wörter trennen',
      subtitleKey: 'sub_assembly',
      items: parseArray(u1, 'ex10_items').map(([prompt, words], i) => ({
        kind: 'assembly',
        prompt: opt(prompt) ?? '',
        words: trimNulls(words).map(String),
        meaning: opt(meanings[i]),
      })),
    });
  }

  /* 11 + 13 — sort items into groups */
  const assignTask = (number, title, subtitleKey, itemsName, groupsName, meanName) => {
    const meanings = parseArray(u1, meanName);
    return {
      number,
      title,
      subtitleKey,
      items: [{
        kind: 'assign',
        groups: parseArray(u1, groupsName).map(String),
        items: parseArray(u1, itemsName).map(([emoji, label, group], i) => ({
          emoji: emoji ?? null,
          label: String(label),
          group,
          meaning: opt(meanings[i]),
        })),
      }],
    };
  };
  tasks.push(assignTask(11, 'Grußformen', 'sub_gruss', 'ex11_items', 'ex11_groups', 'ex11_meaning'));

  /* 12 — Was macht er/sie? */
  {
    const meanings = parseArray(u1, 'ex12_meaning');
    tasks.push({
      number: 12,
      title: 'Was macht er/sie?',
      subtitleKey: 'sub_bild',
      pool: parseArray(u1, 'ex12_pool'),
      items: parseArray(u1, 'ex12_data').map(([emoji, before, after, answer], i) =>
        verbFill([before, after, answer], meanings[i], { emoji: String(emoji) })),
    });
  }

  tasks.push(assignTask(13, 'Länder', 'sub_land', 'ex13_items', 'ex13_groups', 'ex13_meaning'));

  /* Vocabulary trainer branch: one section per textbook page. */
  const vocab = parseArray(u1, 'u1_trans_sections').map(([header, arrayRef]) => {
    const name = arrayRef?.__c;
    if (!name) throw new Error(`vocab section '${header}' has no data array`);
    return {
      header: String(header),
      items: parseArray(u1, name).map(([prompt, answers, meaning]) => ({
        prompt: String(prompt),
        answers: alternatives(answers),
        meaning: opt(meaning),
      })),
    };
  });

  return {
    id: 'unit-01',
    subject: 'deutsch',
    number: 1,
    title: { cs: 'Neue Freunde', en: 'Neue Freunde' },
    slides: [],
    tasks,
    vocab,
  };
}
