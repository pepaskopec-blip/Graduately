import { readFileSync } from 'node:fs';
import { parseArray, parseString } from '../lib/cparse.mjs';
import {
  alternatives,
  assemblyItem,
  assignExercise,
  ex2Row,
  ex4Letters,
  opt,
  typedItem,
  u3Fill,
  u3Letters,
  vocabSections,
} from './deutsch-helpers.mjs';

/**
 * Deutsch Unit 3 ("Bei uns zu Hause").
 *
 * Most tasks go through `u3_build_drop` (dropdown gaps over a shared pool);
 * the rest are hand-built pages whose data arrays are read here directly.
 */
export function extractDeutschUnit3(desktopDir) {
  const src = readFileSync(`${desktopDir}/exercise_u3.c`, 'utf8');
  const tasks = [];

  /** `u3_build_drop`: `U3Fill` rows sharing one dropdown pool. */
  const dropTask = (number, title, subtitleKey, prefix, { example, gloss } = {}) => {
    const glosses = gloss ? parseArray(src, `${prefix}_gloss`) : null;
    return {
      number,
      title,
      subtitleKey,
      ...(example ? { example } : {}),
      pool: parseArray(src, `${prefix}_pool`).map(String),
      items: parseArray(src, `${prefix}_rows`).map((row, i) =>
        u3Fill(row, { gloss: glosses ? opt(glosses[i]) : undefined })),
    };
  };

  tasks.push(dropTask(1, 'Familienpaare', 'sub3_pair', 'ex1_u3', {
    example: 'Beispiel: der Vater ↔ die Mutter',
  }));

  /* 2 — mein oder dein: rows grouped per item, each item has one translation. */
  tasks.push({
    number: 2,
    title: 'mein oder dein',
    subtitleKey: 'sub3_poss',
    example:
      'Beispiel: Ist das dein Vater? – Nein, das ist nicht mein Vater. ' +
      'Das ist sein Onkel.',
    pool: parseArray(src, 'ex2_u3_pool').map(String),
    items: parseArray(src, 'ex2_u3_items').flatMap(([czech, rowsRef]) => {
      const name = rowsRef?.__c;
      if (!name) throw new Error(`ex2_u3_items row group for '${czech}' has no data array`);
      return parseArray(src, name).map((row) => ex2Row(row, czech));
    }),
  });

  tasks.push(dropTask(3, 'ein / kein', 'sub3_haustier', 'ex3_u3', {
    example:
      'Beispiel: Hast du eine Katze? – Nein, ich habe keine Katze. ' +
      'Ich habe einen Hund.',
  }));

  /* 4 — Lückentext: missing letters inside words. */
  tasks.push({
    number: 4,
    title: 'Lückentext',
    subtitleKey: 'sub3_buchst',
    items: parseArray(src, 'ex4_u3_rows').map(ex4Letters),
  });

  /* 5 — Marcos Familie */
  tasks.push({
    number: 5,
    title: 'Marcos Familie',
    subtitleKey: 'sub3_marco',
    note: parseString(src, 'ex5_u3_bank'),
    items: parseArray(src, 'ex5_u3_rows').map(typedItem),
  });

  /* 6 — Sortieren */
  tasks.push({
    number: 6,
    title: 'Sortieren',
    subtitleKey: 'sub3_sort',
    items: [assignExercise(src, 'ex6_u3_items', 'ex6_u3_groups', 'ex6_u3_meaning')],
  });

  /* 7 — Possessivtabelle: nouns down the side, persons across the top. */
  {
    const persons = ['ich', 'du', 'er', 'sie', 'es', 'wir', 'ihr', 'sie / Sie'];
    const nouns = ['der Garten', 'die Idee', 'das Fest', 'die Geschwister'];
    const forms = [
      ['mein', 'dein', 'sein', 'ihr', 'sein', 'unser', 'euer', 'ihr'],
      ['meine', 'deine', 'seine', 'ihre', 'seine', 'unsere', 'eure', 'ihre'],
      ['mein', 'dein', 'sein', 'ihr', 'sein', 'unser', 'euer', 'ihr'],
      ['meine', 'deine', 'seine', 'ihre', 'seine', 'unsere', 'eure', 'ihre'],
    ];

    tasks.push({
      number: 7,
      title: 'Possessivtabelle',
      subtitleKey: 'sub3_tabelle',
      items: [{
        kind: 'table',
        columns: persons,
        rows: nouns,
        answers: forms.map((row) => row.map((value) => ({ value }))),
      }],
    });
  }

  tasks.push(dropTask(8, 'Akkusativ', 'sub3_akk', 'ex8_u3', {
    example: 'Beispiel: die Inliner (ich) → Ich brauche meine Inliner.',
    gloss: true,
  }));
  tasks.push(dropTask(9, 'kein / nicht', 'sub3_nicht', 'ex9_u3'));

  /* 10 — Sätze bauen */
  {
    const meanings = parseArray(src, 'ex10_u3_meaning');
    tasks.push({
      number: 10,
      title: 'Sätze bauen',
      subtitleKey: 'sub3_satz',
      items: parseArray(src, 'ex10_u3_items').map((row, i) => assemblyItem(row, meanings[i])),
    });
  }

  /* 11 + 14 — `u3_build_kw`: free sentence checked for the required keywords. */
  const keywordTask = (number, title, subtitleKey, qsName, example) => ({
    number,
    title,
    subtitleKey,
    example,
    items: parseArray(src, qsName).map(([prompt, answers, mean, german]) => ({
      kind: 'typed',
      prompt: String(prompt),
      answers: alternatives(answers),
      substring: true,
      ...(opt(mean) ? { meaning: String(mean) } : {}),
      ...(opt(german) ? { model: String(german) } : {}),
    })),
  });

  tasks.push(keywordTask(11, 'Was siehst du?', 'sub3_sehen', 'ex11_u3_qs',
    'Beispiel: Ich sehe eine Burg. / Ich sehe ein Schloss.'));

  tasks.push(dropTask(12, 'Wem gehört das?', 'sub3_wem', 'ex12_u3'));
  tasks.push(dropTask(13, 'Es gibt …', 'sub3_gibt', 'ex13_u3'));

  tasks.push(keywordTask(14, 'Beschreiben', 'sub3_saetze', 'ex14_u3_qs',
    'Beispiel: Ich sehe einen Jungen. Er liest ein Buch.'));

  /* 15 — Wochenende: missing letters again, but with no per-gap glosses. */
  tasks.push({
    number: 15,
    title: 'Wochenende',
    subtitleKey: 'sub3_wochen',
    items: parseArray(src, 'ex15_u3_rows').map(u3Letters),
  });

  return {
    id: 'unit-03',
    subject: 'deutsch',
    number: 3,
    title: { cs: 'Bei uns zu Hause', en: 'Bei uns zu Hause' },
    slides: [],
    tasks,
    vocab: vocabSections(src, 'u3_trans_sections'),
  };
}
