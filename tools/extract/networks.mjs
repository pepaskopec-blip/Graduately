import { readFileSync } from 'node:fs';
import { parseArray, parseArrayInFunction, trimNulls } from '../lib/cparse.mjs';

const NET_LESSONS = 27;

/** `NetSlide` is positional: kicker, title, tip, line[8]. */
function toSlide(row, label) {
  if (!Array.isArray(row)) throw new Error(`${label}: slide is not a braced list`);
  const [kicker, title, tip, lines] = row;
  return {
    kicker: kicker ?? '',
    title: title ?? '',
    tip: tip ?? null,
    lines: trimNulls(lines ?? []).map((l) => String(l)),
  };
}

/** `ChoiceQ` is positional: prompt, options[5], n_options, correct. */
function toChoice(row, hint, label) {
  if (!Array.isArray(row)) throw new Error(`${label}: question is not a braced list`);
  const [prompt, options, nOptions, correct] = row;
  const list = trimNulls(options ?? []).map((o) => String(o));
  const n = typeof nOptions === 'number' ? nOptions : list.length;

  if (typeof correct !== 'number' || correct < 0 || correct >= n) {
    throw new Error(`${label}: correct index ${correct} out of range for ${n} options`);
  }

  const out = { kind: 'choice', prompt: String(prompt), options: list.slice(0, n), correct };
  if (hint) out.hint = String(hint);
  return out;
}

/** Unit 1 is the VLSM walkthrough: four scenarios plus a per-block answer key. */
function subnetTasks(src) {
  const tasks = parseArray(src, 'net_tasks');
  const answers = parseArray(src, 'net_ans');

  return tasks.map(([scenario, solution], i) => {
    const rows = answers[i] ?? [];
    const blocks = rows
      .map(([prefix, network, broadcast, firstHost, lastHost]) => ({
        prefix, network, broadcast, firstHost, lastHost,
      }))
      // A prefix of -1 marks a scenario that has no valid allocation.
      .filter((b) => typeof b.prefix === 'number' && b.prefix > 0);

    return {
      kind: 'subnet',
      scenario: String(scenario),
      solution: String(solution),
      blocks: blocks.length ? blocks : null,
    };
  });
}

export function extractNetworks(desktopDir) {
  const src = readFileSync(`${desktopDir}/net.c`, 'utf8');
  const lessons = [];

  for (let n = 1; n <= NET_LESSONS; n++) {
    const slides = parseArrayInFunction(src, `build_net_unit${n}_page`, 'slides')
      .map((row, i) => toSlide(row, `net unit ${n} slide ${i + 1}`));

    let tasks;
    if (n === 1) {
      tasks = [{
        number: 1,
        title: 'VLSM',
        subtitleKey: 'net_ex1_title',
        items: subnetTasks(src),
      }];
    } else {
      const qs = parseArray(src, `net${n}_qs`);
      const hints = parseArray(src, `net${n}_hints`);
      tasks = [{
        number: 1,
        title: 'Quiz',
        subtitleKey: `net_quiz${n}_head`,
        items: qs.map((row, i) => toChoice(row, hints[i], `net unit ${n} q${i + 1}`)),
      }];
    }

    lessons.push({
      id: `unit-${String(n).padStart(2, '0')}`,
      subject: 'networks',
      number: n,
      // Titles live in the i18n tables; the web app resolves these keys.
      titleKey: `net_unit${n}`,
      subtitleKey: `net_unit${n}_sub`,
      slides,
      tasks,
    });
  }

  return lessons;
}
