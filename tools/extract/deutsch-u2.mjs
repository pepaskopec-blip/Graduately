import { readFileSync } from 'node:fs';
import { parseArray, parseString } from '../lib/cparse.mjs';
import {
  choiceItem,
  freeItem,
  opt,
  typedItem,
  verbFill,
  vocabSections,
} from './deutsch-helpers.mjs';

/**
 * Deutsch Unit 2 ("Familie und Freunde").
 *
 * Task order, titles and instruction keys come from the `u2exN` builders at the
 * bottom of exercise_u2.c; each one names the `const` arrays holding its data.
 */
export function extractDeutschUnit2(desktopDir) {
  const src = readFileSync(`${desktopDir}/exercise_u2.c`, 'utf8');
  const tasks = [];

  /** `build_verb_ex`: one pool feeding single-gap sentences. */
  const verbTask = (number, title, subtitleKey, prefix, { gloss = false } = {}) => {
    const meanings = parseArray(src, `${prefix}_mean`);
    const glosses = gloss ? parseArray(src, `${prefix}_gloss`) : null;
    return {
      number,
      title,
      subtitleKey,
      pool: parseArray(src, `${prefix}_pool`).map(String),
      items: parseArray(src, `${prefix}_rows`).map((row, i) =>
        verbFill(row, meanings[i], glosses && opt(glosses[i])
          ? { glosses: [String(glosses[i])] }
          : {})),
    };
  };

  const choiceTask = (number, title, subtitleKey, prefix) => {
    const meanings = parseArray(src, `${prefix}_mean`);
    return {
      number,
      title,
      subtitleKey,
      items: parseArray(src, `${prefix}_rows`).map((row, i) => choiceItem(row, meanings[i])),
    };
  };

  const typedTask = (number, title, subtitleKey, rowsName, bankName) => ({
    number,
    title,
    subtitleKey,
    ...(bankName ? { note: parseString(src, bankName) } : {}),
    items: parseArray(src, rowsName).map(typedItem),
  });

  /* 1 — Verben konjugieren: two people, each a headed group over one pool. */
  {
    const pool = parseArray(src, 'g01_pool').map(String);
    const groups = [
      ['Peter Fritsch:', 'g01_peter', 'g01_peter_mean'],
      ['Jana Nová und Pavol Korčák:', 'g01_jana', 'g01_jana_mean'],
    ];

    const items = groups.flatMap(([heading, rowsName, meanName]) => {
      const meanings = parseArray(src, meanName);
      return parseArray(src, rowsName).map((row, i) =>
        verbFill(row, meanings[i], i === 0 ? { heading } : {}));
    });

    tasks.push({
      number: 1,
      title: 'Verben konjugieren',
      subtitleKey: 'sub_verben',
      pool,
      items,
    });
  }

  tasks.push(choiceTask(2, 'aus oder in', 'sub_ausin', 'g02'));
  tasks.push(choiceTask(3, 'Fragewörter', 'sub_wer', 'g03'));
  tasks.push(verbTask(4, 'sprechen', 'sub_sprich', 'g04'));
  tasks.push(typedTask(5, 'Nationalitäten', 'sub_nation', 'g05_rows', 'g05_bank'));
  tasks.push(typedTask(6, 'Woher?', 'sub_woher', 'g06_rows'));
  tasks.push(typedTask(7, 'Länder schreiben', 'sub_laender', 'g07_rows'));
  tasks.push(verbTask(8, 'Verben einsetzen', 'sub_verb2', 'g08'));

  /* 9 — Freie Antwort */
  tasks.push({
    number: 9,
    title: 'Freie Antwort',
    subtitleKey: 'sub_free',
    items: parseArray(src, 'ex9_free_qs').map(freeItem),
  });

  tasks.push(verbTask(10, 'Euro', 'sub_euro', 'g10'));
  tasks.push(typedTask(11, 'Wörter suchen', 'sub_wortsuchen', 'g11_rows'));
  tasks.push(choiceTask(12, 'Was ist richtig?', 'sub_verb2', 'g12'));

  /* 13 — Ordne zu: a Fragewort dropdown and an answer dropdown per row. */
  {
    const meanings = parseArray(src, 'g13_mean');
    tasks.push({
      number: 13,
      title: 'Ordne zu',
      subtitleKey: 'sub_ordne',
      pool: parseArray(src, 'g13_fw_pool').map(String),
      altPool: parseArray(src, 'g13_ans_pool').map(String),
      items: parseArray(src, 'g13_rows').map(([mid, fw, ans], i) => ({
        kind: 'fill',
        segments: ['', ` ${mid} → `, ''],
        answers: [String(fw), String(ans)],
        input: 'select',
        gapPool: ['main', 'alt'],
        label: `${i + 1}.`,
        ...(opt(meanings[i]) ? { meaning: String(meanings[i]) } : {}),
      })),
    });
  }

  tasks.push(verbTask(14, 'Lückentext', 'sub_luecke', 'g14', { gloss: true }));
  tasks.push(verbTask(15, 'Verbinde', 'sub_verbinde', 'g15', { gloss: true }));
  tasks.push(verbTask(16, 'Zahlen', 'sub_zahlpaar', 'g16'));

  /* 17 — Steckbrief */
  tasks.push({
    number: 17,
    title: 'Steckbrief',
    subtitleKey: 'sub_steckbrief',
    items: parseArray(src, 's01_rows').map(([stem, sample]) => ({
      kind: 'profile',
      stem: String(stem),
      sample: String(sample),
    })),
  });

  /* 18 — Berufe: hangman, one round per profession. */
  {
    const words = parseArray(src, 'hm_words').map(String);
    const tips = parseArray(src, 'hm_tips').map(String);
    tasks.push({
      number: 18,
      title: 'Berufe',
      subtitleKey: 'sub_berufe',
      items: words.map((word, i) => ({ kind: 'hangman', word, hint: tips[i] })),
    });
  }

  tasks.push(typedTask(19, 'Nationalität', 'sub_bistdu', 's03_rows'));

  return {
    id: 'unit-02',
    subject: 'deutsch',
    number: 2,
    title: { cs: 'Aus aller Welt', en: 'Aus aller Welt' },
    slides: [],
    tasks,
    vocab: vocabSections(src, 'u2_trans_sections'),
  };
}
