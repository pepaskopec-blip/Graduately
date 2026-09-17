# content/

Shared lesson content for the whole project. One folder, one file per lesson —
instead of lesson text being buried inside each unit's C source file.

`schema.ts` is the single description of every shape here. Both the extractor in
`tools/` and the web app in `web/` type against it, so a change there shows up as
a type error on both sides.

## Layout

```
content/
  schema.ts              TypeScript types for everything below
  themes.json            10 palettes, dark + light (24 colors each)
  subjects.json          the 13 subjects, their i18n keys and routes
  paths.json             serpentine unit paths per subject
  manifest.json          subject and lesson ids
  i18n/
    ui.json              UI chrome strings, keyed by identifier
    content.json         lesson strings, keyed by the Czech text
  lessons/
    deutsch/unit-01.json … unit-03.json
    networks/unit-01.json … unit-27.json
    hardware/unit-01.json … unit-03.json
    czech/mluvnice.json, orwell-1984.json, fuks-spalovac-mrtvol.json
```

`paths.json` holds one entry per path. A subject usually has exactly one, but
Czech has two: the top-level area picker and the `reading` branch listing the
books, distinguished by the `branch` field.

## Lesson shape

A lesson holds theory `slides` and a list of `tasks`. Each task is one bubble on
the unit map and contains `items` of a single `kind`:

| kind | what it is |
| ---- | ---------- |
| `choice` | multiple choice, `correct` indexes `options` |
| `fill` | sentence split into `segments` with a gap between each; `input` picks dropdown or text |
| `dialogue` | dialogue lines with one missing word each |
| `assembly` | build a sentence from scrambled words |
| `assign` | sort labelled cards into named groups |
| `free` | open question with a model answer |
| `typed` | type the answer, `answers` lists accepted spellings |
| `fields` | one prompt, several labelled fields (Czech grammatical categories) |
| `reveal` | self-assessed: write an answer, then compare it with the model |
| `hangman` | guess the word letter by letter |
| `profile` | "Steckbrief" line with a model answer |
| `table` | editable grid, some cells prefilled |
| `subnet` | VLSM subnetting worksheet |

`fill` absorbs every gap variant in the desktop app. A few flags steer it:
`letters` marks gaps holding missing letters inside a word rather than whole
words, `gapPool` picks the task's `pool` or `altPool` per gap (the "Ordne zu"
question-word and answer columns), and `heading` starts a labelled group of rows.

Two conventions carried over from the desktop app:

- **Pipe-separated answers.** `"bin|heiße"` means either spelling is accepted.
- **Lenient comparison.** Answers are matched after lowercasing, expanding
  `ä ö ü ß` to `ae oe ue ss`, dropping accents and ignoring punctuation, so Czech
  diacritics and German umlauts are optional. See `web/src/lib/answers.ts`.

Titles come either as a literal `title` or as a `titleKey` to look up in the
i18n tables, because German unit names are identical in both languages while
network lesson names are translated.

## Regenerating

The C sources in `desktop/` are still where content is authored, so this folder
is a generated one-way export:

```bash
npm run content     # from the repo root, or: node tools/build-content.mjs
```

The extractor parses the C initializers directly and throws on any shape it does
not recognise, so a bad export fails loudly rather than emitting empty lessons.
