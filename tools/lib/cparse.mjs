/**
 * Minimal reader for the C initializer syntax used by the desktop sources.
 *
 * It understands only what the lesson data actually uses: string literals with
 * adjacent concatenation, char literals, integers, `NULL`, identifiers,
 * designated initializers and nested braces. Anything else throws so a bad
 * extraction fails loudly instead of silently producing empty lessons.
 */

const IDENT_START = /[A-Za-z_]/;
const IDENT_PART = /[A-Za-z0-9_]/;

/** Strip comments while preserving string/char literals verbatim. */
export function stripComments(src) {
  let out = '';
  let i = 0;

  while (i < src.length) {
    const c = src[i];

    if (c === '"' || c === "'") {
      const quote = c;
      out += c;
      i++;
      while (i < src.length) {
        if (src[i] === '\\') {
          out += src[i] + (src[i + 1] ?? '');
          i += 2;
          continue;
        }
        out += src[i];
        if (src[i] === quote) {
          i++;
          break;
        }
        i++;
      }
      continue;
    }

    if (c === '/' && src[i + 1] === '*') {
      const end = src.indexOf('*/', i + 2);
      i = end === -1 ? src.length : end + 2;
      out += ' ';
      continue;
    }

    if (c === '/' && src[i + 1] === '/') {
      const end = src.indexOf('\n', i);
      i = end === -1 ? src.length : end;
      out += ' ';
      continue;
    }

    out += c;
    i++;
  }

  return out;
}

const ESCAPES = {
  n: '\n',
  t: '\t',
  r: '\r',
  '0': '\0',
  '\\': '\\',
  '"': '"',
  "'": "'",
  a: '\x07',
  b: '\b',
  f: '\f',
  v: '\v',
};

class Reader {
  constructor(src, label) {
    this.src = src;
    this.label = label;
    this.i = 0;
  }

  fail(msg) {
    const at = this.src.slice(Math.max(0, this.i - 60), this.i + 60);
    throw new Error(`${this.label}: ${msg} near\n...${at}...`);
  }

  ws() {
    while (this.i < this.src.length && /\s/.test(this.src[this.i])) this.i++;
  }

  peek() {
    this.ws();
    return this.src[this.i];
  }

  eat(ch) {
    this.ws();
    if (this.src[this.i] !== ch) this.fail(`expected '${ch}', got '${this.src[this.i]}'`);
    this.i++;
  }

  tryEat(ch) {
    this.ws();
    if (this.src[this.i] === ch) {
      this.i++;
      return true;
    }
    return false;
  }

  /** One string literal, honouring C's adjacent-literal concatenation. */
  readString() {
    let value = '';
    let first = true;

    for (;;) {
      this.ws();
      if (this.src[this.i] !== '"') {
        if (first) this.fail('expected string literal');
        return value;
      }
      first = false;
      this.i++;
      while (this.i < this.src.length && this.src[this.i] !== '"') {
        if (this.src[this.i] === '\\') {
          const esc = this.src[this.i + 1];
          if (esc in ESCAPES) {
            value += ESCAPES[esc];
            this.i += 2;
            continue;
          }
          if (esc === 'u' || esc === 'x') {
            const m = /^\\(u[0-9a-fA-F]{4}|x[0-9a-fA-F]+)/.exec(this.src.slice(this.i));
            if (!m) this.fail('bad escape');
            const hex = m[1].slice(1);
            value += String.fromCodePoint(parseInt(hex, 16));
            this.i += m[0].length;
            continue;
          }
          value += esc;
          this.i += 2;
          continue;
        }
        value += this.src[this.i];
        this.i++;
      }
      if (this.src[this.i] !== '"') this.fail('unterminated string');
      this.i++;
    }
  }

  readIdent() {
    this.ws();
    if (!IDENT_START.test(this.src[this.i] ?? '')) this.fail('expected identifier');
    let start = this.i;
    while (this.i < this.src.length && IDENT_PART.test(this.src[this.i])) this.i++;
    return this.src.slice(start, this.i);
  }

  /**
   * A single initializer value. Braces become arrays, or plain objects when the
   * body uses designated initializers.
   */
  readValue() {
    this.ws();
    const c = this.src[this.i];

    if (c === undefined) this.fail('unexpected end of input');
    if (c === '"') return this.readString();
    if (c === '{') return this.readBraced();

    // Cast prefix such as `(int)` or `(const char *)`: skip it and read on.
    if (c === '(') {
      const cast = /^\(\s*(?:const\s+|unsigned\s+|signed\s+)*[A-Za-z_]\w*\s*\**\s*\)/.exec(
        this.src.slice(this.i)
      );
      if (cast) {
        this.i += cast[0].length;
        return this.readValue();
      }
    }

    if (c === "'") {
      this.i++;
      let ch;
      if (this.src[this.i] === '\\') {
        const esc = this.src[this.i + 1];
        ch = esc in ESCAPES ? ESCAPES[esc] : esc;
        this.i += 2;
      } else {
        ch = this.src[this.i];
        this.i++;
      }
      this.eat("'");
      return ch;
    }

    // Numeric or simple arithmetic/negation, e.g. `-1`, `4`, `G_N_ELEMENTS(x)`.
    const numMatch = /^[+-]?(?:0[xX][0-9a-fA-F]+|\d+)(?![\w.])/.exec(this.src.slice(this.i));
    if (numMatch) {
      this.i += numMatch[0].length;
      return Number(numMatch[0]);
    }

    if (IDENT_START.test(c)) {
      const name = this.readIdent();
      if (name === 'NULL') return null;
      if (name === 'TRUE') return true;
      if (name === 'FALSE') return false;
      // Macro call or bare constant: keep the source text for the caller.
      if (this.peek() === '(') {
        let depth = 0;
        const start = this.i;
        while (this.i < this.src.length) {
          if (this.src[this.i] === '(') depth++;
          else if (this.src[this.i] === ')') {
            depth--;
            if (depth === 0) {
              this.i++;
              break;
            }
          }
          this.i++;
        }
        return { __c: name + this.src.slice(start, this.i) };
      }
      return { __c: name };
    }

    this.fail(`unexpected character '${c}'`);
  }

  readBraced() {
    this.eat('{');
    const list = [];
    const obj = {};
    let designated = false;

    for (;;) {
      if (this.tryEat('}')) break;

      if (this.peek() === '.') {
        this.i++;
        const field = this.readIdent();
        this.eat('=');
        obj[field] = this.readValue();
        designated = true;
      } else {
        list.push(this.readValue());
      }

      if (this.tryEat(',')) continue;
      this.eat('}');
      break;
    }

    return designated ? obj : list;
  }
}

/**
 * Find an array/struct definition by name and parse its initializer.
 * Matches e.g. `static const ChoiceQ net2_qs[] = { ... };`
 */
export function parseArray(src, name, { label = name } = {}) {
  const clean = stripComments(src);
  const re = new RegExp(
    `\\b${name}\\s*(?:\\[[^\\]]*\\]\\s*)+=\\s*(?=\\{)`,
    'g'
  );
  const m = re.exec(clean);
  if (!m) throw new Error(`${label}: array '${name}' not found`);

  const reader = new Reader(clean, label);
  reader.i = m.index + m[0].length;
  const value = reader.readValue();
  if (!Array.isArray(value)) throw new Error(`${label}: '${name}' is not a braced list`);
  return value;
}

/** Parse a scalar struct definition, e.g. `static const ThemePalette nord_dark = {...};` */
export function parseStruct(src, name, { label = name } = {}) {
  const clean = stripComments(src);
  const re = new RegExp(`\\b${name}\\s*=\\s*(?=\\{)`, 'g');
  const m = re.exec(clean);
  if (!m) throw new Error(`${label}: struct '${name}' not found`);

  const reader = new Reader(clean, label);
  reader.i = m.index + m[0].length;
  return reader.readValue();
}

/**
 * Same as parseArray but scoped to the body of a single function, so the many
 * identically-named `static const NetSlide slides[]` locals stay distinct.
 */
export function parseArrayInFunction(src, fnName, arrayName) {
  const clean = stripComments(src);
  const fnRe = new RegExp(`\\b${fnName}\\s*\\([^)]*\\)\\s*\\{`, 'g');
  const m = fnRe.exec(clean);
  if (!m) throw new Error(`function '${fnName}' not found`);

  // Walk to the matching closing brace so we only search this body.
  let depth = 0;
  let i = m.index + m[0].length - 1;
  const start = i;
  for (; i < clean.length; i++) {
    if (clean[i] === '"' || clean[i] === "'") {
      const q = clean[i++];
      while (i < clean.length && clean[i] !== q) i += clean[i] === '\\' ? 2 : 1;
      continue;
    }
    if (clean[i] === '{') depth++;
    else if (clean[i] === '}') {
      depth--;
      if (depth === 0) break;
    }
  }

  const body = clean.slice(start, i + 1);
  return parseArray(body, arrayName, { label: `${fnName}/${arrayName}` });
}

/** Parse a scalar string definition, e.g. `const char *g05_bank = "a, b, c";` */
export function parseString(src, name, { label = name } = {}) {
  const clean = stripComments(src);
  const re = new RegExp(`\\b${name}\\s*=\\s*(?=")`, 'g');
  const m = re.exec(clean);
  if (!m) throw new Error(`${label}: string '${name}' not found`);

  const reader = new Reader(clean, label);
  reader.i = m.index + m[0].length;
  return reader.readString();
}

/** Resolve `#define NAME value` integer macros. */
export function parseDefines(src) {
  const out = {};
  const re = /^[ \t]*#[ \t]*define[ \t]+([A-Za-z_]\w*)[ \t]+([^\n/]+)/gm;
  let m;
  while ((m = re.exec(src))) {
    const raw = m[2].trim();
    if (/^-?\d+$/.test(raw)) out[m[1]] = Number(raw);
  }
  return out;
}

/** Trailing NULLs are array terminators in the C data, not content. */
export function trimNulls(list) {
  const out = [...list];
  while (out.length && (out.at(-1) === null || out.at(-1) === undefined)) out.pop();
  return out;
}
