import { readFileSync } from 'node:fs';
import { parseArray } from '../lib/cparse.mjs';

/**
 * `tr_ui` holds UI chrome keyed by short identifiers; `tr_content` holds
 * lesson text keyed by the Czech string itself (its `cs` column is usually
 * NULL, meaning "the key already is the Czech text").
 */
function toTable(entries, { keyIsCzech }) {
  const cs = {};
  const en = {};

  for (const row of entries) {
    if (!Array.isArray(row)) continue;
    const [key, csText, enText] = row;
    if (key === null || key === undefined) continue; // terminator

    cs[key] = csText ?? (keyIsCzech ? key : null);
    if (enText !== null && enText !== undefined) en[key] = enText;
  }

  // A null Czech value in tr_ui would render as an empty label; drop it.
  for (const k of Object.keys(cs)) if (cs[k] === null) delete cs[k];

  return { cs, en };
}

export function extractI18n(desktopDir) {
  const src = readFileSync(`${desktopDir}/i18n.c`, 'utf8');

  return {
    ui: toTable(parseArray(src, 'tr_ui'), { keyIsCzech: false }),
    content: toTable(parseArray(src, 'tr_content'), { keyIsCzech: true }),
  };
}
