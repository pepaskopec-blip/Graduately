import { readFileSync } from 'node:fs';
import { parseArray, trimNulls } from '../lib/cparse.mjs';

/**
 * Czech grammar ("Mluvnice"), 20 exercises in one lesson.
 *
 * Unlike the other subjects, mluvnice.c passes its instructions as literal
 * Czech strings rather than i18n keys, so they are copied verbatim from the
 * `build_mluvnice_exercise_page` switch below.
 */

/** One row per `case` in `build_mluvnice_exercise_page`, in the same order. */
const EXERCISES = [
  ['typed', 'Pravopis – i/y po obojetných souhláskách', 'Doplňte i/í nebo y/ý.', null],
  ['choice', 'Pravopis – velká/malá písmena', 'Vyberte správnou variantu.', null],
  ['typed', 'Slovní druhy', 'Určete slovní druh podtržených slov.',
    'Do pole napište název slovního druhu.'],
  ['typed', 'Mluvnické kategorie – podstatná jména',
    'Určete rod, číslo a pád podtržených slov.',
    'Vyplňte tři pole: rod, číslo a pád.'],
  ['typed', 'Mluvnické kategorie – slovesa',
    'Určete osobu, číslo, čas a způsob u těchto sloves.',
    'U rozkazovacího a podmiňovacího způsobu napište do pole čas „neurčitý“.'],
  ['reveal', 'Slovotvorba', 'Rozeberte slova na morfémy.',
    'Určete předponu, kořen, příponu a koncovku.'],
  ['typed', 'Větné členy', 'Určete větný člen podtržených výrazů.', null],
  ['typed', 'Druhy vedlejších vět', 'Určete druh vedlejší věty.', null],
  ['typed', 'Synonyma', 'Ke každému slovu napište synonymum.', null],
  ['typed', 'Antonyma', 'Napište opak.', null],
  ['typed', 'Pravopis – s/z na začátku slova', 'Vyberte správnou předponu s-/z-.', null],
  ['reveal', 'Skladba – najděte chybu',
    'Každá věta obsahuje jednu chybu. Najděte ji a opravte.', null],
  ['typed', 'Tvarosloví – správný tvar', 'Dejte slovo do správného tvaru.', null],
  ['choice', 'Výběr ze čtyř možností', 'Vyberte správnou odpověď.', null],
  ['reveal', 'Přímá a nepřímá řeč', 'Přepište přímou řeč na nepřímou.', null],
  ['reveal', 'Slovní zásoba – rozdíly', 'Vysvětlete rozdíl mezi těmito dvojicemi slov.', null],
  ['reveal', 'Interpunkce', 'Doplňte čárky tam, kde patří.', null],
  ['typed', 'Obrazná pojmenování',
    'Určete, o jaký druh obrazného pojmenování jde.', null],
  ['typed', 'Stylistika – slohové útvary',
    'Přiřaďte ukázku ke správnému slohovému útvaru.', null],
  ['choice', 'Souhrnné opakování', 'Vyberte správnou odpověď.', null],
];

/** Only exercise 14 ships per-question explanations. */
const CHOICE_EXPLANATIONS = { 14: 'e14_expl' };

/** `MluvItem`: prompt, shown, nblank, hint[5], answers[5]. */
function fieldsItem(row, label) {
  const [prompt, shown, nblank, hints, answers] = row;
  if (typeof nblank !== 'number' || nblank < 1) {
    throw new Error(`${label}: expected at least one blank, got ${nblank}`);
  }

  return {
    kind: 'fields',
    prompt: String(prompt),
    fields: Array.from({ length: nblank }, (_, i) => ({
      label: String(hints[i] ?? ''),
      answers: String(answers[i] ?? '').split('|').map((a) => a.trim()).filter(Boolean),
    })),
    ...(shown ? { solution: String(shown) } : {}),
  };
}

function choiceItem(row, expl, label) {
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
    ...(expl ? { hint: String(expl) } : {}),
  };
}

export function extractMluvnice(desktopDir) {
  const src = readFileSync(`${desktopDir}/mluvnice.c`, 'utf8');
  const names = parseArray(src, 'mluv_names').map(String);

  if (names.length !== EXERCISES.length) {
    throw new Error(`mluv_names has ${names.length} entries but the extractor knows ${EXERCISES.length}`);
  }

  const tasks = EXERCISES.map(([type, heading, subtitle, note], index) => {
    const number = index + 1;
    const label = `mluvnice ex${number}`;
    const base = {
      number,
      // Short label for the map bubble; the page shows the full heading.
      title: names[index],
      heading,
      subtitleKey: null,
      subtitle,
      ...(note ? { note } : {}),
    };

    if (type === 'choice') {
      const explName = CHOICE_EXPLANATIONS[number];
      const expls = explName ? parseArray(src, explName).map(String) : [];
      return {
        ...base,
        items: parseArray(src, `e${number}_qs`)
          .map((row, i) => choiceItem(row, expls[i], `${label} q${i + 1}`)),
      };
    }

    if (type === 'reveal') {
      return {
        ...base,
        items: parseArray(src, `e${number}_items`).map(([prompt, solution]) => ({
          kind: 'reveal',
          prompt: String(prompt),
          solution: String(solution),
        })),
      };
    }

    return {
      ...base,
      items: parseArray(src, `e${number}_items`)
        .map((row, i) => fieldsItem(row, `${label} item ${i + 1}`)),
    };
  });

  return {
    id: 'mluvnice',
    subject: 'czech',
    number: 1,
    title: { cs: 'Mluvnice', en: 'Czech grammar' },
    subtitle: {
      cs: 'Vyberte cvičení a dokončete je.',
      en: 'Pick an exercise and complete it.',
    },
    slides: [],
    tasks,
  };
}
