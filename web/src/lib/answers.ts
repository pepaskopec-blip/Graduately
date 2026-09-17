/**
 * Answer comparison, ported from `normalize_answer` in `desktop/util.c`.
 *
 * The rules matter for fairness, so they follow the desktop app exactly:
 * German umlauts get their two-letter keyboard fallbacks, accents are dropped
 * so Czech diacritics are optional, punctuation is ignored and whitespace is
 * collapsed.
 */

const GERMAN_FALLBACKS: Record<string, string> = {
  ä: 'ae',
  ö: 'oe',
  ü: 'ue',
  ß: 'ss',
};

export function normalizeAnswer(input: string): string {
  const lowered = (input ?? '').toLowerCase();

  // German letters first, so ä does not decompose to a bare "a".
  let pre = '';
  for (const ch of lowered) pre += GERMAN_FALLBACKS[ch] ?? ch;

  // Strip combining marks, keep alphanumerics, collapse runs of whitespace.
  const decomposed = pre.normalize('NFD').replace(/\p{M}+/gu, '');

  return decomposed
    .replace(/[^\p{L}\p{N}\s]+/gu, '')
    .replace(/\s+/g, ' ')
    .trim();
}

/** Accepted answers are pipe-separated in the content, e.g. `bin|heiße`. */
export const splitAccepted = (answer: string): string[] =>
  answer.split('|').map((a) => a.trim()).filter(Boolean);

/** True when `input` matches any accepted spelling of `answer`. */
export function isCorrect(input: string, answer: string): boolean {
  const normalized = normalizeAnswer(input);
  if (!normalized) return false;
  return splitAccepted(answer).some((a) => normalizeAnswer(a) === normalized);
}

/** The spelling shown when revealing a solution. */
export const primaryAnswer = (answer: string): string => splitAccepted(answer)[0] ?? answer;

/**
 * True when `input` contains any of the required keywords.
 *
 * The Unit 3 "describe the picture" exercises accept whole free-form sentences
 * as long as the key noun appears, so they compare on containment rather than
 * on equality.
 */
export function containsAnswer(input: string, answers: string[]): boolean {
  const normalized = normalizeAnswer(input);
  if (!normalized) return false;
  return answers.some((a) => normalized.includes(normalizeAnswer(a)));
}
