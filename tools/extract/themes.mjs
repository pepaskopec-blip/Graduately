import { readFileSync } from 'node:fs';
import { parseStruct } from '../lib/cparse.mjs';

/** Field order of `ThemePalette` in maturita.h, as the C initializers are positional. */
const FIELDS = [
  'crust', 'base', 'mantle', 'surface0', 'surface1', 'surface2',
  'overlay', 'text', 'subtext', 'accent', 'accent2', 'accent3',
  'success', 'success2', 'warning', 'error', 'onAccent',
  'node', 'lockedBg', 'lockedBorder', 'rail',
  'finishDark', 'finishLight', 'finishStroke',
];

/** slug, display name, and the C variable prefix used in theme.c. */
const THEMES = [
  ['catppuccin', 'Catppuccin', 'cat'],
  ['nord', 'Nord', 'nord'],
  ['dracula', 'Dracula', 'dra'],
  ['rose-pine', 'Rose Pine', 'rose'],
  ['ocean', 'Ocean', 'ocean'],
  ['gruvbox', 'Gruvbox', 'gruv'],
  ['solarized', 'Solarized', 'sol'],
  ['everforest', 'Everforest', 'ever'],
  ['monokai', 'Monokai', 'mono'],
  ['one-dark', 'One Dark', 'one'],
];

function toPalette(values, label) {
  if (values.length !== FIELDS.length) {
    throw new Error(`${label}: expected ${FIELDS.length} colors, got ${values.length}`);
  }
  const out = {};
  values.forEach((v, i) => {
    if (typeof v !== 'number') throw new Error(`${label}: field ${FIELDS[i]} is not a hex literal`);
    out[FIELDS[i]] = '#' + v.toString(16).padStart(6, '0');
  });
  return out;
}

export function extractThemes(desktopDir) {
  const src = readFileSync(`${desktopDir}/theme.c`, 'utf8');

  return THEMES.map(([id, name, prefix]) => ({
    id,
    name,
    dark: toPalette(parseStruct(src, `${prefix}_dark`), `${id} dark`),
    light: toPalette(parseStruct(src, `${prefix}_light`), `${id} light`),
  }));
}
