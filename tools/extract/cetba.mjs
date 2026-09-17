import { readFileSync } from 'node:fs';
import { parseArray, parseArrayInFunction, trimNulls } from '../lib/cparse.mjs';

/**
 * Maturita reading list ("Četba"): one lesson per book.
 *
 * Each book page shows four reference cards followed by two exercises, so the
 * cards become the lesson's theory slides and the quiz and plot ordering
 * become its two tasks.
 */

/**
 * Lesson numbers continue after Mluvnice, so every Czech lesson has a distinct
 * number for the statistics page; the reading path numbers its own nodes.
 */
const BOOKS = [
  {
    id: 'orwell-1984',
    number: 2,
    title: 'George Orwell – 1984',
    page: 'build_cetba1984_page',
    quiz: 'lit_qs',
    plot: 'lit_plot_items',
    plotMeaning: 'lit_plot_meaning',
    cards: [
      ['about', 'O knize'],
      ['world', 'Svět a strana'],
      ['people', 'Postavy'],
      ['terms', 'Klíčové pojmy'],
    ],
  },
  {
    id: 'fuks-spalovac-mrtvol',
    number: 3,
    title: 'Ladislav Fuks – Spalovač mrtvol',
    page: 'build_cetba_fuks_page',
    quiz: 'fuks_qs',
    plot: 'fuks_plot_items',
    plotMeaning: 'fuks_plot_meaning',
    cards: [
      ['about', 'O knize'],
      ['world', 'Doba a svět'],
      ['people', 'Postavy'],
      ['terms', 'Klíčové motivy'],
    ],
  },
];

/** `LitQ`: question, opts[4], correct, explanation. */
function litChoice(row, label) {
  const [prompt, options, correct, expl] = row;
  const list = trimNulls(options ?? []).map(String);

  if (typeof correct !== 'number' || correct < 0 || correct >= list.length) {
    throw new Error(`${label}: correct index ${correct} out of range for ${list.length} options`);
  }

  return {
    kind: 'choice',
    prompt: String(prompt),
    options: list,
    correct,
    ...(expl ? { hint: String(expl) } : {}),
  };
}

export function extractCetba(desktopDir) {
  const src = readFileSync(`${desktopDir}/cetba.c`, 'utf8');

  return BOOKS.map((book) => {
    const slides = book.cards.map(([arrayName, cardTitle]) => ({
      kicker: book.title,
      title: cardTitle,
      tip: null,
      lines: trimNulls(parseArrayInFunction(src, book.page, arrayName)).map(String),
    }));

    const plotMeanings = parseArray(src, book.plotMeaning).map(String);

    return {
      id: book.id,
      subject: 'czech',
      number: book.number,
      title: { cs: book.title, en: book.title },
      subtitle: {
        cs: 'Projděte si přehled a pak si vyzkoušejte kvíz a řazení děje.',
        en: 'Read the overview, then try the quiz and the plot ordering.',
      },
      slides,
      tasks: [
        {
          number: 1,
          title: 'Kvíz',
          heading: 'Kvíz ke knize',
          subtitleKey: 'lit_quiz_sub',
          items: parseArray(src, book.quiz)
            .map((row, i) => litChoice(row, `${book.id} q${i + 1}`)),
        },
        {
          number: 2,
          title: 'Děj',
          heading: 'Seřaďte děj',
          subtitleKey: 'lit_plot_sub',
          items: parseArray(src, book.plot).map(([prompt, words], i) => ({
            kind: 'assembly',
            prompt: prompt ? String(prompt) : '',
            words: trimNulls(words).map(String),
            ...(plotMeanings[i] ? { meaning: plotMeanings[i] } : {}),
          })),
        },
      ],
    };
  });
}
