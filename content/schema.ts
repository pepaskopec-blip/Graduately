/**
 * Shared lesson content schema.
 *
 * This is the single description of everything under `content/`. Both the
 * extractor in `tools/` and the Next.js app in `web/` type against it, so a
 * shape change here surfaces as a type error on both sides.
 */

export type Lang = 'cs' | 'en';

/** A short piece of text that may differ per UI language. */
export interface Localized {
  cs: string;
  en?: string;
}

/* ---------------------------------------------------------------- themes --- */

export interface ThemePalette {
  crust: string;
  base: string;
  mantle: string;
  surface0: string;
  surface1: string;
  surface2: string;
  overlay: string;
  text: string;
  subtext: string;
  accent: string;
  accent2: string;
  accent3: string;
  success: string;
  success2: string;
  warning: string;
  error: string;
  onAccent: string;
  node: string;
  lockedBg: string;
  lockedBorder: string;
  rail: string;
  finishDark: string;
  finishLight: string;
  finishStroke: string;
}

export interface Theme {
  id: string;
  name: string;
  dark: ThemePalette;
  light: ThemePalette;
}

/* -------------------------------------------------------------- subjects --- */

export interface Subject {
  /** Stable slug used in URLs, e.g. `deutsch`, `networks`. */
  id: string;
  /** i18n key holding the display name. */
  key: string;
  icon: 'flag-de' | 'wifi' | 'chip' | 'book' | null;
  unlocked: boolean;
  /** Route the subject node opens, or null when it is a locked placeholder. */
  route: string | null;
}

/* ----------------------------------------------------------------- units --- */

/**
 * One node on a serpentine learning path.
 *
 * A node carries either a literal `title` (German unit names are the same in
 * both languages) or a `titleKey` to look up in the translation tables.
 */
export interface UnitRef {
  /** 1-based position on the path. */
  number: number;
  /** Slug of the lesson file under `content/lessons/<subject>/`, if any. */
  lesson: string | null;
  title?: Localized;
  titleKey?: string;
  /** Destination for nodes that open another path rather than a lesson. */
  route?: string | null;
  unlocked: boolean;
}

export interface Path {
  subject: string;
  /** Optional school-year grouping, as the networks subject uses. */
  year?: number;
  /** Set on secondary paths within a subject, e.g. the Czech reading list. */
  branch?: string;
  units: UnitRef[];
}

/* ----------------------------------------------------------- theory slides - */

export interface Slide {
  /** Small label above the title, e.g. `1 / 4   •   VLSM`. */
  kicker: string;
  title: string;
  /** Highlighted aside shown under the title. */
  tip: string | null;
  lines: string[];
}

/* -------------------------------------------------------------- exercises -- */

/** Multiple choice. `correct` indexes into `options`. */
export interface ChoiceExercise {
  kind: 'choice';
  prompt: string;
  options: string[];
  correct: number;
  hint?: string;
  /** Czech gloss of the prompt, shown as a translation aid. */
  meaning?: string;
}

/**
 * A sentence split into literal segments with gaps between them.
 *
 * `segments.length === answers.length + 1`. Every gap variant in the desktop
 * app (verb insertion, number words, sequences, possessives, letters) collapses
 * into this one shape; `input` decides whether the gap is a dropdown fed from
 * `pool` or a free text field.
 */
export interface FillExercise {
  kind: 'fill';
  segments: string[];
  answers: string[];
  input: 'select' | 'text';
  /** Row label such as `1.` or `a)`. */
  label?: string;
  /** Group heading rendered above this row, e.g. a speaker's name. */
  heading?: string;
  /** Emoji or picture clue shown before the sentence. */
  emoji?: string;
  /** Extra clue such as the digits to spell out. */
  clue?: string;
  /** Czech translation revealed as a hint. */
  meaning?: string;
  /** Per-gap glosses, aligned with `answers`. */
  glosses?: (string | null)[];
  /**
   * Which of the task's word lists feeds each gap, aligned with `answers`.
   * Defaults to the task `pool`; `alt` selects `altPool`.
   */
  gapPool?: ('main' | 'alt')[];
  /** Gaps hold single letters rather than whole words, so render them narrow. */
  letters?: boolean;
}

/** Free-form answer checked against a model answer rather than exactly. */
export interface FreeExercise {
  kind: 'free';
  question: string;
  questionCs?: string;
  sample: string;
  sampleCs?: string;
}

/** Type the answer; `answers` lists all accepted spellings. */
export interface TypedExercise {
  kind: 'typed';
  prompt: string;
  answers: string[];
  meaning?: string;
  /** Full model sentence revealed after checking, where one exists. */
  model?: string;
  /** Accepted answers are substrings of a longer sentence, not the whole input. */
  substring?: boolean;
}

/**
 * A prompt answered through several labelled fields, as the Czech grammar
 * exercises use ("determine gender, number and case").
 */
export interface FieldsExercise {
  kind: 'fields';
  prompt: string;
  fields: { label: string; answers: string[] }[];
  /** The full correct answer, revealed after checking. */
  solution?: string;
}

/**
 * Self-assessed exercise: think of the answer, then reveal the model solution.
 * Used where no automatic check is meaningful (morpheme analysis, punctuation,
 * rewriting direct speech).
 */
export interface RevealExercise {
  kind: 'reveal';
  prompt: string;
  solution: string;
}

/** Drag or click words into order to build a sentence. */
export interface AssemblyExercise {
  kind: 'assembly';
  prompt: string;
  words: string[];
  meaning?: string;
}

/** Sort labelled items into named groups. */
export interface AssignExercise {
  kind: 'assign';
  groups: string[];
  items: { emoji: string | null; label: string; group: number; meaning?: string }[];
}

/**
 * A dialogue where some replies are missing.
 *
 * `answer` may list several accepted words separated by `|`, matching the
 * desktop app's answer comparison.
 */
export interface DialogueExercise {
  kind: 'dialogue';
  title: string;
  rows: {
    speaker: string;
    before: string;
    answer: string;
    after: string;
    meaning?: string;
  }[];
}

/** Guess the word letter by letter. */
export interface HangmanExercise {
  kind: 'hangman';
  word: string;
  hint: string;
}

/** Open profile question with a model answer ("Steckbrief"). */
export interface ProfileExercise {
  kind: 'profile';
  stem: string;
  sample: string;
}

/** Editable grid, e.g. the possessive-pronoun table. */
export interface TableExercise {
  kind: 'table';
  columns: string[];
  rows: string[];
  /** `answers[row][col]`; a prefilled cell has `given: true`. */
  answers: { value: string; given?: boolean }[][];
}

/** VLSM subnetting: split a network into four blocks. */
export interface SubnetExercise {
  kind: 'subnet';
  scenario: string;
  solution: string;
  blocks: {
    prefix: number;
    network: string;
    broadcast: string;
    firstHost: string;
    lastHost: string;
  }[] | null;
}

export type Exercise =
  | ChoiceExercise
  | FillExercise
  | FreeExercise
  | TypedExercise
  | FieldsExercise
  | RevealExercise
  | AssemblyExercise
  | AssignExercise
  | DialogueExercise
  | HangmanExercise
  | ProfileExercise
  | TableExercise
  | SubnetExercise;

/* ---------------------------------------------------------------- lessons -- */

/** A group of exercises presented as one task on the unit map. */
export interface Task {
  /** 1-based bubble number on the unit map. */
  number: number;
  /** Short label for the map bubble. */
  title: string;
  /** Longer heading for the exercise page, where it differs from `title`. */
  heading?: string;
  /** i18n key for the instruction line. */
  subtitleKey: string | null;
  /** Literal instruction, where the desktop app passes text instead of a key. */
  subtitle?: string;
  /** Worked example shown above the items. */
  example?: string;
  /** Extra note under the instruction. */
  note?: string;
  /** Words offered to every `select` gap in this task. */
  pool?: string[];
  /** Second word list, selected per gap via `FillExercise.gapPool`. */
  altPool?: string[];
  items: Exercise[];
}

export interface VocabSection {
  header: string;
  items: { prompt: string; answers: string[]; meaning?: string }[];
}

export interface Lesson {
  /** File slug, unique within the subject. */
  id: string;
  subject: string;
  /** Position on the subject path. */
  number: number;
  /** Literal title, or `titleKey` when the text comes from the i18n tables. */
  title?: Localized;
  titleKey?: string;
  subtitle?: Localized;
  subtitleKey?: string;
  /** Theory shown before the exercises. */
  slides: Slide[];
  tasks: Task[];
  /** Optional vocabulary trainer branch. */
  vocab?: VocabSection[];
}

/* --------------------------------------------------------------- manifest -- */

export interface ContentManifest {
  subjects: Subject[];
  paths: Path[];
  /** Lesson ids per subject, in path order. */
  lessons: Record<string, string[]>;
}
