#!/usr/bin/env python3
"""Pull educational content out of the GTK C sources into content.json."""

from __future__ import annotations

import json
import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
SRC = ROOT / "src"
# Default target is the Android asset; Xcode (iOS / native macOS) passes
# `--out` and copies the file into the app bundle itself, so nothing
# generated lives under ios/ or macos/.
OUTS = [
    ROOT / "android" / "app" / "src" / "main" / "assets" / "content.json",
]
if "--out" in sys.argv:
    OUTS = [Path(a) for a in sys.argv[sys.argv.index("--out") + 1:] if a]
    if not OUTS:
        sys.exit("usage: extract-android-content.py [--out PATH ...]")
OUT = OUTS[0]

IDENT = re.compile(r"[A-Za-z_][A-Za-z0-9_]*")


def strip_comments(src: str) -> str:
    out = []
    i, n = 0, len(src)
    while i < n:
        c = src[i]
        if c == '"' or c == "'":
            q = c
            out.append(c)
            i += 1
            while i < n:
                ch = src[i]
                out.append(ch)
                if ch == "\\" and i + 1 < n:
                    out.append(src[i + 1])
                    i += 2
                    continue
                i += 1
                if ch == q:
                    break
            continue
        if c == "/" and i + 1 < n and src[i + 1] == "/":
            i += 2
            while i < n and src[i] != "\n":
                i += 1
            continue
        if c == "/" and i + 1 < n and src[i + 1] == "*":
            i += 2
            while i + 1 < n and not (src[i] == "*" and src[i + 1] == "/"):
                i += 1
            i = min(n, i + 2)
            out.append(" ")
            continue
        out.append(c)
        i += 1
    return "".join(out)


def unescape(s: str) -> str:
    out = []
    i = 0
    while i < len(s):
        if s[i] == "\\" and i + 1 < len(s):
            n = s[i + 1]
            mapping = {
                "n": "\n",
                "t": "\t",
                "r": "\r",
                "\\": "\\",
                '"': '"',
                "'": "'",
            }
            if n in mapping:
                out.append(mapping[n])
                i += 2
                continue
            if n == "x":
                j = i + 2
                hexdigits = ""
                while j < len(s) and s[j] in "0123456789abcdefABCDEF" and len(hexdigits) < 2:
                    hexdigits += s[j]
                    j += 1
                out.append(chr(int(hexdigits, 16)) if hexdigits else "x")
                i = j
                continue
        out.append(s[i])
        i += 1
    return "".join(out)


class Tok:
    def __init__(self, kind: str, value, pos: int):
        self.kind = kind
        self.value = value
        self.pos = pos


def tokenize(src: str) -> list[Tok]:
    toks: list[Tok] = []
    i, n = 0, len(src)
    while i < n:
        c = src[i]
        if c.isspace():
            i += 1
            continue
        if c == '"':
            i += 1
            raw = []
            while i < n:
                ch = src[i]
                if ch == "\\" and i + 1 < n:
                    raw.append(ch)
                    raw.append(src[i + 1])
                    i += 2
                    continue
                if ch == '"':
                    i += 1
                    break
                raw.append(ch)
                i += 1
            while i < n and src[i].isspace():
                i += 1
            while i < n and src[i] == '"':
                i += 1
                while i < n:
                    ch = src[i]
                    if ch == "\\" and i + 1 < n:
                        raw.append(ch)
                        raw.append(src[i + 1])
                        i += 2
                        continue
                    if ch == '"':
                        i += 1
                        break
                    raw.append(ch)
                    i += 1
                while i < n and src[i].isspace():
                    i += 1
            toks.append(Tok("str", unescape("".join(raw)), i))
            continue
        if c in "{}[],;=":
            toks.append(Tok(c, c, i))
            i += 1
            continue
        if c == "-" and i + 1 < n and src[i + 1].isdigit():
            j = i + 1
            while j < n and (src[j].isdigit() or src[j] == "."):
                j += 1
            toks.append(Tok("num", int(float(src[i:j])), i))
            i = j
            continue
        if c.isdigit():
            j = i
            while j < n and (src[j].isdigit() or src[j] == "."):
                j += 1
            text = src[i:j]
            toks.append(Tok("num", int(float(text)) if "." not in text else int(float(text)), i))
            i = j
            continue
        if c == "'" and i + 2 < n:
            j = i + 1
            raw = []
            while j < n and src[j] != "'":
                if src[j] == "\\" and j + 1 < n:
                    raw.append(src[j + 1])
                    j += 2
                    continue
                raw.append(src[j])
                j += 1
            j += 1
            toks.append(Tok("num", ord(raw[0]) if raw else 0, i))
            i = j
            continue
        m = IDENT.match(src, i)
        if m:
            name = m.group(0)
            i = m.end()
            if name == "NULL":
                toks.append(Tok("null", None, i))
            elif name in ("TRUE", "FALSE"):
                toks.append(Tok("num", 1 if name == "TRUE" else 0, i))
            else:
                toks.append(Tok("id", name, i))
            continue
        i += 1
    return toks


def parse_value(toks: list[Tok], i: int):
    if i >= len(toks):
        return None, i
    t = toks[i]
    if t.kind == "str":
        return t.value, i + 1
    if t.kind == "num":
        return t.value, i + 1
    if t.kind == "null":
        return None, i + 1
    if t.kind == "id":
        name = t.value
        i += 1
        if i < len(toks) and toks[i].kind == "(":
            depth = 1
            i += 1
            while i < len(toks) and depth:
                if toks[i].kind == "(":
                    depth += 1
                elif toks[i].kind == ")":
                    depth -= 1
                i += 1
            return None, i
        return {"$ref": name}, i
    if t.kind == "{":
        return parse_list(toks, i)
    return None, i + 1


def parse_list(toks: list[Tok], i: int):
    assert toks[i].kind == "{"
    i += 1
    items = []
    while i < len(toks) and toks[i].kind != "}":
        if toks[i].kind == ",":
            i += 1
            continue
        val, i = parse_value(toks, i)
        items.append(val)
        if i < len(toks) and toks[i].kind == ",":
            i += 1
    if i < len(toks) and toks[i].kind == "}":
        i += 1
    return items, i


ARRAY_RE = re.compile(
    r"(?:static\s+)?const\s+"
    r"(?:struct\s+)?(?P<type>[A-Za-z_][A-Za-z0-9_]*)"
    r"(?:\s+\*|\s+const\s+\*|\s*)\s*"
    r"(?P<name>[A-Za-z_][A-Za-z0-9_]*)"
    r"(?:\s*\[[^\]]*\])+\s*=\s*\{",
    re.M,
)

CHARPTR_RE = re.compile(
    r"(?:static\s+)?const\s+char\s*\*\s*(?:const\s+)?"
    r"(?P<name>[A-Za-z_][A-Za-z0-9_]*)"
    r"(?:\s*\[[^\]]*\])+\s*=\s*\{",
    re.M,
)

STRING_ASSIGN_RE = re.compile(
    r"(?:static\s+)?const\s+char\s*\*\s*"
    r"(?P<name>[A-Za-z_][A-Za-z0-9_]*)\s*=\s*(?P<body>\"(?:\\.|[^\"\\])*\"(?:\s*\"(?:\\.|[^\"\\])*\")*)\s*;",
    re.S,
)

FUNC_RE = re.compile(
    r"^(?:static\s+)?(?:GtkWidget\s*\*|void|gboolean|int|char\s*\*)\s*"
    r"([A-Za-z_][A-Za-z0-9_]*)\s*\(",
    re.M,
)


def find_arrays(src: str) -> dict[str, tuple[str, list]]:
    found: dict[str, tuple[str, list]] = {}
    funcs = [(m.start(), m.group(1)) for m in FUNC_RE.finditer(src)]

    def func_at(pos: int) -> str | None:
        current = None
        for start, name in funcs:
            if start < pos:
                current = name
            else:
                break
        if current is None:
            return None
        # Only treat as local if the array sits after the function's opening brace
        # and before the next top-level function. Good enough for this codebase.
        return current

    def add(typ: str, name: str, pos_brace: int):
        toks = tokenize(src[pos_brace:])
        if not toks or toks[0].kind != "{":
            return
        val, _ = parse_list(toks, 0)
        fn = func_at(pos_brace)
        key = f"{fn}.{name}" if fn and name in {
            "slides", "qs", "about", "world", "people", "terms", "persons",
            "nouns", "ans",
        } else name
        # Prefer the first global, overwrite locals with prefixed names.
        if key in found and not key.startswith(fn or "\0"):
            return
        found[key] = (typ, val)

    for m in ARRAY_RE.finditer(src):
        add(m.group("type"), m.group("name"), m.end() - 1)
    for m in CHARPTR_RE.finditer(src):
        add("char*", m.group("name"), m.end() - 1)
    for m in STRING_ASSIGN_RE.finditer(src):
        toks = tokenize(m.group("body"))
        text = "".join(t.value for t in toks if t.kind == "str")
        found[m.group("name")] = ("char", [text])
    return found


def as_list(v):
    return v if isinstance(v, list) else []


def as_str(v) -> str | None:
    if v is None:
        return None
    if isinstance(v, str):
        return v
    return None


def resolve(arrays: dict, val, depth=0):
    if depth > 8:
        return val
    if isinstance(val, dict) and "$ref" in val:
        name = val["$ref"]
        if name in arrays:
            return resolve(arrays, arrays[name][1], depth + 1)
        return None
    if isinstance(val, list):
        return [resolve(arrays, x, depth + 1) for x in val]
    return val


def choice_items(rows) -> list[dict]:
    out = []
    for row in as_list(rows):
        row = as_list(row)
        if len(row) < 4:
            continue
        opts = [o for o in as_list(row[1]) if isinstance(o, str)]
        correct = row[3] if isinstance(row[3], int) else 0
        out.append({
            "prompt": as_str(row[0]) or "",
            "options": opts,
            "correct": correct,
        })
    return out


def typed_items(rows) -> list[dict]:
    out = []
    for row in as_list(rows):
        row = as_list(row)
        if len(row) < 2:
            continue
        out.append({
            "prompt": as_str(row[0]) or "",
            "answers": as_str(row[1]) or "",
            "meaning": as_str(row[2]) if len(row) > 2 else None,
        })
    return out


def assembly_items(rows) -> list[dict]:
    out = []
    for row in as_list(rows):
        row = as_list(row)
        if len(row) < 2:
            continue
        words = [w for w in as_list(row[1]) if isinstance(w, str)]
        n = row[2] if len(row) > 2 and isinstance(row[2], int) else len(words)
        out.append({
            "prompt": as_str(row[0]),
            "words": words[:n],
        })
    return out


def str_list(rows) -> list[str]:
    return [x for x in as_list(rows) if isinstance(x, str)]


def verb_items(rows) -> list[dict]:
    out = []
    for row in as_list(rows):
        row = as_list(row)
        if len(row) < 3:
            continue
        out.append({
            "before": as_str(row[0]) or "",
            "after": as_str(row[1]) or "",
            "answer": as_str(row[2]) or "",
        })
    return out


def verb_clue_items(rows) -> list[dict]:
    out = []
    for row in as_list(rows):
        row = as_list(row)
        if len(row) < 4:
            continue
        out.append({
            "emoji": as_str(row[0]) or "",
            "before": as_str(row[1]) or "",
            "after": as_str(row[2]) or "",
            "answer": as_str(row[3]) or "",
        })
    return out


def assign_items(rows) -> list[dict]:
    out = []
    for row in as_list(rows):
        row = as_list(row)
        if len(row) < 3:
            continue
        out.append({
            "emoji": as_str(row[0]),
            "label": as_str(row[1]) or "",
            "group": row[2] if isinstance(row[2], int) else 0,
        })
    return out


def free_items(rows) -> list[dict]:
    out = []
    for row in as_list(rows):
        row = as_list(row)
        if len(row) < 4:
            continue
        out.append({
            "question": as_str(row[0]) or "",
            "qCs": as_str(row[1]) or "",
            "sample": as_str(row[2]) or "",
            "aCs": as_str(row[3]) or "",
        })
    return out


def profile_items(rows) -> list[dict]:
    out = []
    for row in as_list(rows):
        row = as_list(row)
        if len(row) < 2:
            continue
        out.append({
            "stem": as_str(row[0]) or "",
            "sample": as_str(row[1]) or "",
        })
    return out


def slides_items(rows) -> list[dict]:
    out = []
    for row in as_list(rows):
        row = as_list(row)
        if len(row) < 4:
            continue
        lines = [x for x in as_list(row[3]) if isinstance(x, str)]
        out.append({
            "kicker": as_str(row[0]) or "",
            "title": as_str(row[1]) or "",
            "tip": as_str(row[2]),
            "lines": lines,
        })
    return out


def fill_items(rows) -> list[dict]:
    out = []
    for row in as_list(rows):
        row = as_list(row)
        if len(row) < 9:
            continue
        segs = [as_str(row[1]), as_str(row[3]), as_str(row[5]), as_str(row[7])]
        ans = [as_str(row[2]), as_str(row[4]), as_str(row[6])]
        out.append({
            "num": as_str(row[0]),
            "segs": [s or "" for s in segs],
            "answers": [a for a in ans if a],
            "mean": as_str(row[8]),
        })
    return out


def let_items(rows) -> list[dict]:
    out = []
    for row in as_list(rows):
        row = as_list(row)
        if len(row) < 11:
            continue
        segs = [as_str(row[i]) or "" for i in (1, 3, 5, 7, 9)]
        ans = [as_str(row[i]) for i in (2, 4, 6, 8)]
        out.append({
            "num": as_str(row[0]),
            "segs": segs,
            "answers": [a for a in ans if a],
            "mean": as_str(row[10]),
        })
    return out


def ex4row_items(rows) -> list[dict]:
    out = []
    for row in as_list(rows):
        row = as_list(row)
        if len(row) < 6:
            continue
        segs = [s for s in as_list(row[0]) if s is None or isinstance(s, str)]
        ans = [a for a in as_list(row[1]) if isinstance(a, str)]
        words = [w for w in as_list(row[3]) if isinstance(w, str)] if len(row) > 3 else []
        gmean = [g for g in as_list(row[4]) if isinstance(g, str)] if len(row) > 4 else []
        gaps = row[5] if len(row) > 5 and isinstance(row[5], int) else len(ans)
        out.append({
            "segs": [s or "" for s in segs],
            "answers": ans[:gaps],
            "czech": as_str(row[2]) if len(row) > 2 else None,
            "words": words[:gaps],
            "gmean": gmean[:gaps],
        })
    return out


def kw_items(rows) -> list[dict]:
    out = []
    for row in as_list(rows):
        row = as_list(row)
        if len(row) < 2:
            continue
        out.append({
            "prompt": as_str(row[0]) or "",
            "answers": as_str(row[1]) or "",
            "mean": as_str(row[2]) if len(row) > 2 else None,
            "german": as_str(row[3]) if len(row) > 3 else None,
        })
    return out


def mluv_items(rows) -> list[dict]:
    out = []
    for row in as_list(rows):
        row = as_list(row)
        if len(row) < 5:
            continue
        nblank = row[2] if isinstance(row[2], int) else 1
        out.append({
            "prompt": as_str(row[0]) or "",
            "shown": as_str(row[1]) or "",
            "hints": [h for h in as_list(row[3]) if isinstance(h, str)][:nblank],
            "answers": [a for a in as_list(row[4]) if isinstance(a, str)][:nblank],
        })
    return out


def reveal_items(rows) -> list[dict]:
    out = []
    for row in as_list(rows):
        row = as_list(row)
        if len(row) < 2:
            continue
        out.append({
            "prompt": as_str(row[0]) or "",
            "solution": as_str(row[1]) or "",
        })
    return out


def lit_items(rows) -> list[dict]:
    out = []
    for row in as_list(rows):
        row = as_list(row)
        if len(row) < 4:
            continue
        opts = [o for o in as_list(row[1]) if isinstance(o, str)]
        out.append({
            "prompt": as_str(row[0]) or "",
            "options": opts,
            "correct": row[2] if isinstance(row[2], int) else 0,
            "expl": as_str(row[3]),
        })
    return out


def trans_sections(rows, arrays) -> list[dict]:
    out = []
    for row in as_list(rows):
        row = as_list(row)
        if len(row) < 2:
            continue
        data = resolve(arrays, row[1])
        out.append({
            "header": as_str(row[0]) or "",
            "rows": typed_items(data),
        })
    return out


def dialogs(arrays) -> list[dict]:
    out = []
    for key in ("ex1_d1", "ex1_d2", "ex1_d3"):
        if key not in arrays:
            continue
        rows = []
        for row in as_list(arrays[key][1]):
            row = as_list(row)
            if len(row) < 4:
                continue
            rows.append({
                "speaker": as_str(row[0]) or "",
                "before": as_str(row[1]) or "",
                "answer": as_str(row[2]) or "",
                "after": as_str(row[3]) or "",
            })
        titles = {"ex1_d1": "Sich vorstellen", "ex1_d2": "Begrüßung", "ex1_d3": "Verabschiedung"}
        out.append({"name": titles[key], "rows": rows})
    return out


def ordne_items(rows) -> list[dict]:
    out = []
    for row in as_list(rows):
        row = as_list(row)
        if len(row) < 3:
            continue
        out.append({
            "mid": as_str(row[0]) or "",
            "fw": as_str(row[1]) or "",
            "ans": as_str(row[2]) or "",
        })
    return out


def ex2_items(arrays) -> list[dict]:
    out = []
    if "ex2_u3_items" not in arrays:
        return out
    for row in as_list(arrays["ex2_u3_items"][1]):
        row = as_list(row)
        if len(row) < 2:
            continue
        raw_rows = resolve(arrays, row[1])
        parsed = []
        for r in as_list(raw_rows):
            r = as_list(r)
            if len(r) < 4:
                continue
            segs = [s or "" for s in as_list(r[1]) if s is None or isinstance(s, str)]
            ans = [a for a in as_list(r[2]) if isinstance(a, str)]
            gaps = r[3] if isinstance(r[3], int) else len(ans)
            parsed.append({
                "num": as_str(r[0]),
                "segs": segs,
                "answers": ans[:gaps],
            })
        out.append({"czech": as_str(row[0]) or "", "rows": parsed})
    return out


def net_ans(rows) -> list[list[dict]]:
    out = []
    for block in as_list(rows):
        block = as_list(block)
        inner = []
        for row in block:
            row = as_list(row)
            if len(row) < 5:
                continue
            inner.append({
                "pfx": row[0] if isinstance(row[0], int) else -1,
                "net": as_str(row[1]),
                "bcast": as_str(row[2]),
                "lo": as_str(row[3]),
                "hi": as_str(row[4]),
            })
        out.append(inner)
    return out


def net_tasks(rows) -> list[dict]:
    out = []
    for row in as_list(rows):
        row = as_list(row)
        if len(row) < 2:
            continue
        out.append({"prompt": as_str(row[0]) or "", "solution": as_str(row[1]) or ""})
    return out


def parse_changelog(path: Path) -> list[dict]:
    if not path.is_file():
        return []
    entries: list[dict] = []
    current = None
    for raw in path.read_text(encoding="utf-8").splitlines():
        line = raw.strip()
        if not line or line.startswith("#"):
            continue
        if line.startswith("---"):
            current = {"date": line[3:].strip(), "cs": [], "en": []}
            entries.append(current)
            continue
        if current is None:
            continue
        if line.startswith("cs:"):
            current["cs"].append(line[3:].strip())
        elif line.startswith("en:"):
            current["en"].append(line[3:].strip())
    return [e for e in entries if e["cs"] or e["en"]]


def parse_i18n(src: str) -> dict:
    arrays = find_arrays(src)
    out = {}
    for name in ("tr_ui", "tr_content"):
        if name not in arrays:
            continue
        for row in as_list(arrays[name][1]):
            row = as_list(row)
            if not row or not as_str(row[0]):
                continue
            key = as_str(row[0])
            cs = as_str(row[1]) if len(row) > 1 else None
            en = as_str(row[2]) if len(row) > 2 else None
            if key:
                out[key] = {"cs": cs, "en": en}
    return out


def combo(title, sub, pool, rows, meanings=None, kind="combo", **extra):
    spec = {
        "type": kind,
        "title": title,
        "sub": sub,
        "pool": pool or [],
        "rows": rows,
        "meanings": meanings or [],
    }
    spec.update(extra)
    return spec


def main() -> int:
    arrays: dict[str, tuple[str, list]] = {}
    i18n = {}
    for path in sorted(SRC.glob("*.c")):
        text = strip_comments(path.read_text(encoding="utf-8"))
        arrays.update(find_arrays(text))
        if path.name == "i18n.c":
            i18n = parse_i18n(text)

    def A(name):
        if name not in arrays:
            return []
        return resolve(arrays, arrays[name][1])

    german_units = [
        {
            "id": 0,
            "title": "Neue Freunde",
            "page": "unit1",
            "tag": "u1",
            "sub": "unit1_sub",
            "unlocked": True,
            "names": [
                "Dialog", "Sätze bilden", "Was ist richtig?", "Freie Antwort",
                "Zahlen", "Wie viel?", "Zahlenreihe", "Verb einsetzen",
                "Wer? Wie? Wo?", "Wörter trennen", "Grußformen",
                "Was macht er/sie?", "Länder",
            ],
            "branch": "u1vocab",
            "exercises": {
                "1": combo("Dialog", "sub_dialog", str_list(A("ex1_pool")),
                           [], str_list(A("ex1_meaning")), "dialog",
                           dialogues=dialogs(arrays)),
                "2": {
                    "type": "assembly", "title": "Sätze bilden", "sub": "sub_assembly",
                    "items": assembly_items(A("ex2_items")),
                    "meanings": str_list(A("ex2_meaning")),
                },
                "3": {
                    "type": "choice", "title": "Was ist richtig?", "sub": "sub_choice_num",
                    "questions": choice_items(A("ex3_questions")),
                    "meanings": str_list(A("ex3_meaning")),
                },
                "4": {
                    "type": "free", "title": "Freie Antwort", "sub": "sub_free",
                    "tip": "tip_ss", "questions": free_items(A("ex4_questions")),
                },
                "5": combo("Zahlen", "sub_zahlen", str_list(A("ex5_pool")),
                           [{"digits": as_list(r)[0], "answer": as_list(r)[1]}
                            for r in as_list(A("ex5_data")) if len(as_list(r)) >= 2],
                           str_list(A("ex5_meaning")), "number"),
                "6": combo("Wie viel?", "sub_wieviel", str_list(A("ex6_pool")),
                           [{"emoji": as_list(r)[0], "count": as_list(r)[1],
                             "noun": as_list(r)[2], "answer": as_list(r)[3]}
                            for r in as_list(A("ex6_data")) if len(as_list(r)) >= 4],
                           str_list(A("ex6_meaning")), "count"),
                "7": combo("Zahlenreihe", "sub_reihe", str_list(A("ex7_pool")),
                           [{"before": as_list(r)[0], "after": as_list(r)[1],
                             "answer": as_list(r)[2]}
                            for r in as_list(A("ex7_data")) if len(as_list(r)) >= 3],
                           str_list(A("ex7_meaning")), "seq"),
                "8": combo("Verb einsetzen", "sub_verb", str_list(A("ex8_pool")),
                           verb_items(A("ex8_data")), str_list(A("ex8_meaning")),
                           "verb"),
                "9": {
                    "type": "choice", "title": "Wer? Wie? Wo?", "sub": "sub_wer",
                    "questions": choice_items(A("ex9_questions")),
                    "meanings": str_list(A("ex9_meaning")),
                },
                "10": {
                    "type": "assembly", "title": "Wörter trennen", "sub": "sub_assembly",
                    "items": assembly_items(A("ex10_items")),
                    "meanings": str_list(A("ex10_meaning")),
                },
                "11": {
                    "type": "assign", "title": "Grußformen", "sub": "sub_gruss",
                    "items": assign_items(A("ex11_items")),
                    "groups": str_list(A("ex11_groups")),
                    "meanings": str_list(A("ex11_meaning")),
                },
                "12": combo("Was macht er/sie?", "sub_bild", str_list(A("ex12_pool")),
                            verb_clue_items(A("ex12_data")),
                            str_list(A("ex12_meaning")), "verbclue"),
                "13": {
                    "type": "assign", "title": "Länder", "sub": "sub_land",
                    "items": assign_items(A("ex13_items")),
                    "groups": str_list(A("ex13_groups")),
                    "meanings": str_list(A("ex13_meaning")),
                },
            },
            "vocab": {
                "type": "vocab", "title": "Vokabeltraining", "sub": "sub_translate",
                "sections": trans_sections(A("u1_trans_sections"), arrays),
            },
        },
        {
            "id": 1,
            "title": "Aus aller Welt",
            "page": "unit2",
            "tag": "u2",
            "sub": "unit2_sub",
            "unlocked": True,
            "names": [
                "Verben konjugieren", "aus oder in", "Fragewörter", "sprechen",
                "Nationalitäten", "Woher?", "Länder schreiben", "Verben einsetzen",
                "Freie Antwort", "Euro", "Wörter suchen", "Was ist richtig?",
                "Ordne zu", "Lückentext", "Verbinde", "Zahlen",
                "Steckbrief", "Berufe", "Nationalität",
            ],
            "branch": "u2vocab",
            "exercises": {
                "1": combo("Verben konjugieren", "sub_verben", str_list(A("g01_pool")),
                           [], [], "verb_sections",
                           sections=[
                               {"title": "Peter Fritsch:",
                                "rows": verb_items(A("g01_peter")),
                                "meanings": str_list(A("g01_peter_mean"))},
                               {"title": "Jana Nová und Pavol Korčák:",
                                "rows": verb_items(A("g01_jana")),
                                "meanings": str_list(A("g01_jana_mean"))},
                           ],
                           transUpfront=True, revealGerman=True),
                "2": {
                    "type": "choice", "title": "aus oder in", "sub": "sub_ausin",
                    "questions": choice_items(A("g02_rows")),
                    "meanings": str_list(A("g02_mean")),
                },
                "3": {
                    "type": "choice", "title": "Fragewörter", "sub": "sub_wer",
                    "questions": choice_items(A("g03_rows")),
                    "meanings": str_list(A("g03_mean")),
                },
                "4": combo("sprechen", "sub_sprich", str_list(A("g04_pool")),
                           verb_items(A("g04_rows")), str_list(A("g04_mean")), "verb"),
                "5": {
                    "type": "typed", "title": "Nationalitäten", "sub": "sub_nation",
                    "rows": typed_items(A("g05_rows")),
                    "bank": as_str(A("g05_bank")[0]) if A("g05_bank") else
                            (as_str(arrays["g05_bank"][1][0]) if "g05_bank" in arrays
                             and arrays["g05_bank"][1] and isinstance(arrays["g05_bank"][1][0], str)
                             else None),
                    "answersNote": False,
                },
                "6": {
                    "type": "typed", "title": "Woher?", "sub": "sub_woher",
                    "rows": typed_items(A("g06_rows")),
                },
                "7": {
                    "type": "typed", "title": "Länder schreiben", "sub": "sub_laender",
                    "rows": typed_items(A("g07_rows")),
                },
                "8": combo("Verben einsetzen", "sub_verb2", str_list(A("g08_pool")),
                           verb_items(A("g08_rows")), str_list(A("g08_mean")), "verb"),
                "9": {
                    "type": "free", "title": "Freie Antwort", "sub": "sub_free",
                    "tip": "tip_ss", "questions": free_items(A("ex9_free_qs")),
                },
                "10": combo("Euro", "sub_euro", str_list(A("g10_pool")),
                            verb_items(A("g10_rows")), str_list(A("g10_mean")),
                            "verb", preMeaning=True),
                "11": {
                    "type": "typed", "title": "Wörter suchen", "sub": "sub_wortsuchen",
                    "rows": typed_items(A("g11_rows")),
                },
                "12": {
                    "type": "choice", "title": "Was ist richtig?", "sub": "sub_verb2",
                    "questions": choice_items(A("g12_rows")),
                    "meanings": str_list(A("g12_mean")),
                },
                "13": combo("Ordne zu", "sub_ordne", [],
                            ordne_items(A("g13_rows")), str_list(A("g13_mean")),
                            "ordne",
                            fwPool=str_list(A("g13_fw_pool")),
                            ansPool=str_list(A("g13_ans_pool"))),
                "14": combo("Lückentext", "sub_luecke", str_list(A("g14_pool")),
                            verb_items(A("g14_rows")), str_list(A("g14_mean")),
                            "verb", preMeaning=True, gloss=str_list(A("g14_gloss"))),
                "15": combo("Verbinde", "sub_verbinde", str_list(A("g15_pool")),
                            verb_items(A("g15_rows")), str_list(A("g15_mean")),
                            "verb", preMeaning=True, gloss=str_list(A("g15_gloss"))),
                "16": combo("Zahlen", "sub_zahlpaar", str_list(A("g16_pool")),
                            verb_items(A("g16_rows")), str_list(A("g16_mean")),
                            "verb"),
                "17": {
                    "type": "profile", "title": "Steckbrief", "sub": "sub_steckbrief",
                    "questions": profile_items(A("s01_rows")),
                },
                "18": {
                    "type": "hangman", "title": "Berufe", "sub": "sub_berufe",
                    "words": str_list(A("hm_words")),
                    "tips": str_list(A("hm_tips")),
                    "letters": str_list(A("hm_letters")),
                },
                "19": {
                    "type": "typed", "title": "Nationalität", "sub": "sub_bistdu",
                    "rows": typed_items(A("s03_rows")),
                    "answersNote": True,
                },
            },
            "vocab": {
                "type": "vocab", "title": "Vokabeltraining", "sub": "sub_translate",
                "sections": trans_sections(A("u2_trans_sections"), arrays),
            },
        },
        {
            "id": 2,
            "title": "Bei uns zu Hause",
            "page": "unit3",
            "tag": "u3",
            "sub": "unit3_sub",
            "unlocked": True,
            "names": [
                "Familienpaare", "mein oder dein", "ein / kein", "Lückentext",
                "Marcos Familie", "Sortieren", "Possessivtabelle", "Akkusativ",
                "kein / nicht", "Sätze bauen", "Was siehst du?", "Wem gehört das?",
                "Es gibt …", "Beschreiben", "Wochenende",
            ],
            "branch": "u3vocab",
            "exercises": {
                "1": combo("Familienpaare", "sub3_pair", str_list(A("ex1_u3_pool")),
                           fill_items(A("ex1_u3_rows")), [], "fill",
                           sample="Beispiel: der Vater ↔ die Mutter",
                           showGerman=False),
                "2": {
                    "type": "ex2", "title": "mein oder dein", "sub": "sub3_poss",
                    "pool": str_list(A("ex2_u3_pool")),
                    "items": ex2_items(arrays),
                },
                "3": combo("ein / kein", "sub3_haustier", str_list(A("ex3_u3_pool")),
                           fill_items(A("ex3_u3_rows")), [], "fill",
                           sample="Beispiel: Hast du eine Katze? – Nein, ich habe keine Katze. Ich habe einen Hund.",
                           showGerman=True),
                "4": {
                    "type": "letters_gap", "title": "Lückentext", "sub": "sub3_buchst",
                    "rows": ex4row_items(A("ex4_u3_rows")),
                },
                "5": {
                    "type": "typed", "title": "Marcos Familie", "sub": "sub3_marco",
                    "rows": typed_items(A("ex5_u3_rows")),
                    "bank": None,
                },
                "6": {
                    "type": "assign", "title": "Sortieren", "sub": "sub3_sort",
                    "items": assign_items(A("ex6_u3_items")),
                    "groups": str_list(A("ex6_u3_groups")),
                    "meanings": str_list(A("ex6_u3_meaning")),
                },
                "7": {
                    "type": "table", "title": "Possessivtabelle", "sub": "sub3_tabelle",
                    "persons": [
                        "ich", "du", "er", "sie", "es", "wir", "ihr", "sie / Sie",
                    ],
                    "nouns": ["der Garten", "die Idee", "das Fest", "die Geschwister"],
                    "answers": [
                        ["mein", "dein", "sein", "ihr", "sein", "unser", "euer", "ihr"],
                        ["meine", "deine", "seine", "ihre", "seine", "unsere", "eure", "ihre"],
                        ["mein", "dein", "sein", "ihr", "sein", "unser", "euer", "ihr"],
                        ["meine", "deine", "seine", "ihre", "seine", "unsere", "eure", "ihre"],
                    ],
                },
                "8": combo("Akkusativ", "sub3_akk", str_list(A("ex8_u3_pool")),
                           fill_items(A("ex8_u3_rows")), str_list(A("ex8_u3_gloss")),
                           "fill", showGerman=True),
                "9": combo("kein / nicht", "sub3_nicht", str_list(A("ex9_u3_pool")),
                           fill_items(A("ex9_u3_rows")), [], "fill", showGerman=True),
                "10": {
                    "type": "assembly", "title": "Sätze bauen", "sub": "sub3_satz",
                    "items": assembly_items(A("ex10_u3_items")),
                    "meanings": str_list(A("ex10_u3_meaning")),
                },
                "11": {
                    "type": "kw", "title": "Was siehst du?", "sub": "sub3_sehen",
                    "sample": "Beispiel: Ich sehe eine Burg. / Ich sehe ein Schloss.",
                    "rows": kw_items(A("ex11_u3_qs")),
                },
                "12": combo("Wem gehört das?", "sub3_wem", str_list(A("ex12_u3_pool")),
                            fill_items(A("ex12_u3_rows")), [], "fill", showGerman=True),
                "13": combo("Es gibt …", "sub3_gibt", str_list(A("ex13_u3_pool")),
                            fill_items(A("ex13_u3_rows")), [], "fill", showGerman=True),
                "14": {
                    "type": "kw", "title": "Beschreiben", "sub": "sub3_saetze",
                    "sample": "Beispiel: Ich sehe einen Jungen. Er liest ein Buch.",
                    "rows": kw_items(A("ex14_u3_qs")),
                },
                "15": {
                    "type": "letters", "title": "Wochenende", "sub": "sub3_wochen",
                    "rows": let_items(A("ex15_u3_rows")),
                },
            },
            "vocab": {
                "type": "vocab", "title": "Vokabeltraining", "sub": "sub_translate",
                "sections": trans_sections(A("u3_trans_sections"), arrays),
            },
        },
    ]

    locked_german = [
        "Schule und Freizeit", "Guten Appetit!", "Mein Tagesablauf",
        "Meine Freunde", "Wir treffen uns in Salzburg",
        "Mein Haus ist meine Burg", "Urlaub in Österreich",
    ]
    for i, title in enumerate(locked_german, start=3):
        german_units.append({
            "id": i, "title": title, "page": None, "tag": None,
            "sub": None, "unlocked": False, "names": [],
            "exercises": {},
        })

    # g05_bank is a single string, not an array of strings
    if "g05_bank" in arrays:
        raw = arrays["g05_bank"][1]
        bank = None
        if isinstance(raw, list) and raw and isinstance(raw[0], str) and len(raw) == 1:
            bank = raw[0]
        elif isinstance(raw, str):
            bank = raw
        if bank:
            german_units[1]["exercises"]["5"]["bank"] = bank

    if "ex5_u3_bank" in arrays:
        raw = arrays["ex5_u3_bank"][1]
        if isinstance(raw, list) and raw and isinstance(raw[0], str):
            german_units[2]["exercises"]["5"]["bank"] = raw[0] if len(raw) == 1 else " ".join(
                x for x in raw if isinstance(x, str)
            )

    net_lessons = []
    for i in range(1, 28):
        slides = slides_items(A(f"build_net_unit{i}_page.slides"))
        qs = choice_items(A(f"net{i}_qs")) if i > 1 else []
        net_lessons.append({
            "id": i,
            "titleKey": f"net_unit{i}",
            "subKey": f"net_unit{i}_sub",
            "exTitleKey": f"net_ex{i}_title" if i > 1 else "net_ex_title",
            "quizHeadKey": f"net_quiz{i}_head" if i > 1 else None,
            "slides": slides,
            "quiz": qs,
        })

    hw_lessons = []
    for i in range(1, 30):
        hw_lessons.append({
            "id": i,
            "titleKey": f"hw_unit{i}",
            "subKey": f"hw_unit{i}_sub",
            "exTitleKey": f"hw_ex{i}_title",
            "quizHeadKey": f"hw_quiz{i}_head",
            "slides": slides_items(A(f"build_hw_unit{i}_page.slides")),
            "quiz": choice_items(A(f"build_hw_unit{i}_exercise_page.qs")),
        })

    mluv_meta = [
        (1, "typed", "Pravopis – i/y po obojetných souhláskách",
         "Doplňte i/í nebo y/ý.", None, "e1_items"),
        (2, "choice", "Pravopis – velká/malá písmena",
         "Vyberte správnou variantu.", None, "e2_qs"),
        (3, "typed", "Slovní druhy",
         "Určete slovní druh podtržených slov.",
         "Do pole napište název slovního druhu.", "e3_items"),
        (4, "typed", "Mluvnické kategorie – podstatná jména",
         "Určete rod, číslo a pád podtržených slov.",
         "Vyplňte tři pole: rod, číslo a pád.", "e4_items"),
        (5, "typed", "Mluvnické kategorie – slovesa",
         "Určete osobu, číslo, čas a způsob u těchto sloves.",
         "U rozkazovacího a podmiňovacího způsobu napište do pole čas „neurčitý“.",
         "e5_items"),
        (6, "reveal", "Slovotvorba", "Rozeberte slova na morfémy.",
         "Určete předponu, kořen, příponu a koncovku.", "e6_items"),
        (7, "typed", "Větné členy",
         "Určete větný člen podtržených výrazů.", None, "e7_items"),
        (8, "typed", "Druhy vedlejších vět",
         "Určete druh vedlejší věty.", None, "e8_items"),
        (9, "typed", "Synonyma", "Ke každému slovu napište synonymum.", None, "e9_items"),
        (10, "typed", "Antonyma", "Napište opak.", None, "e10_items"),
        (11, "typed", "Pravopis – s/z na začátku slova",
         "Vyberte správnou předponu s-/z-.", None, "e11_items"),
        (12, "reveal", "Skladba – najděte chybu",
         "Každá věta obsahuje jednu chybu. Najděte ji a opravte.", None, "e12_items"),
        (13, "typed", "Tvarosloví – správný tvar",
         "Dejte slovo do správného tvaru.", None, "e13_items"),
        (14, "choice", "Výběr ze čtyř možností",
         "Vyberte správnou odpověď.", None, "e14_qs"),
        (15, "reveal", "Přímá a nepřímá řeč",
         "Přepište přímou řeč na nepřímou.", None, "e15_items"),
        (16, "reveal", "Slovní zásoba – rozdíly",
         "Vysvětlete rozdíl mezi těmito dvojicemi slov.", None, "e16_items"),
        (17, "reveal", "Interpunkce",
         "Doplňte čárky tam, kde patří.", None, "e17_items"),
        (18, "typed", "Obrazná pojmenování",
         "Určete, o jaký druh obrazného pojmenování jde.", None, "e18_items"),
        (19, "typed", "Stylistika – slohové útvary",
         "Přiřaďte ukázku ke správnému slohovému útvaru.", None, "e19_items"),
        (20, "choice", "Souhrnné opakování",
         "Vyberte správnou odpověď.", None, "e20_qs"),
    ]
    mluv_names = [
        "i/y", "Velká písmena", "Slovní druhy", "Podst. jména", "Slovesa",
        "Slovotvorba", "Větné členy", "Vedlejší věty", "Synonyma", "Antonyma",
        "s/z", "Najdi chybu", "Tvary", "Test A–D", "Přímá řeč",
        "Rozdíly", "Interpunkce", "Obrazná pojmen.", "Slohové útvary",
        "Opakování",
    ]
    mluvnice = []
    for n, kind, title, sub, note, key in mluv_meta:
        item = {"id": n, "name": mluv_names[n - 1], "type": kind,
                "title": title, "sub": sub, "note": note}
        if kind == "typed":
            item["items"] = mluv_items(A(key))
        elif kind == "reveal":
            item["items"] = reveal_items(A(key))
        else:
            item["questions"] = choice_items(A(key))
            expl_key = key.replace("_qs", "_expl")
            if expl_key in arrays:
                item["expls"] = str_list(A(expl_key))
        mluvnice.append(item)

    def note_lines(fn, name):
        return str_list(A(f"{fn}.{name}"))

    catalog_path = ROOT / "data" / "cetba-catalog.json"
    catalog = json.loads(catalog_path.read_text(encoding="utf-8"))

    full_books = {
        "1984": {
            "subKey": "cetba1984_sub",
            "page": "cetba1984",
            "quizTitle": "lit_quiz_title",
            "quizSub": "lit_quiz_sub",
            "plotTitle": "lit_plot_title",
            "plotSub": "lit_plot_sub",
            "notes": [
                {"title": "O knize", "icon": "book",
                 "lines": note_lines("build_cetba1984_page", "about")},
                {"title": "Svět a strana", "icon": "globe",
                 "lines": note_lines("build_cetba1984_page", "world")},
                {"title": "Postavy", "icon": "people",
                 "lines": note_lines("build_cetba1984_page", "people")},
                {"title": "Klíčové pojmy", "icon": "bulb",
                 "lines": note_lines("build_cetba1984_page", "terms")},
            ],
            "quiz": lit_items(A("lit_qs")),
            "plot": assembly_items(A("lit_plot_items")),
            "plotMeaning": str_list(A("lit_plot_meaning")),
        },
        "fuks": {
            "subKey": "cetbaFuks_sub",
            "page": "cetbaFuks",
            "quizTitle": "lit_fuks_quiz_title",
            "quizSub": "lit_fuks_quiz_sub",
            "plotTitle": "lit_fuks_plot_title",
            "plotSub": "lit_fuks_plot_sub",
            "notes": [
                {"title": "O knize", "icon": "book",
                 "lines": note_lines("build_cetba_fuks_page", "about")},
                {"title": "Doba a svět", "icon": "globe",
                 "lines": note_lines("build_cetba_fuks_page", "world")},
                {"title": "Postavy", "icon": "people",
                 "lines": note_lines("build_cetba_fuks_page", "people")},
                {"title": "Klíčové motivy", "icon": "bulb",
                 "lines": note_lines("build_cetba_fuks_page", "terms")},
            ],
            "quiz": lit_items(A("fuks_qs")),
            "plot": assembly_items(A("fuks_plot_items")),
            "plotMeaning": str_list(A("fuks_plot_meaning")),
        },
    }

    books = []
    cetba_dir = ROOT / "data" / "cetba"
    for entry in catalog:
        bid = entry["id"]
        display = f"{entry['author']} – {entry['title']}"
        page = entry.get("page") or f"cetba_{bid.replace('-', '_')}"
        content_path = cetba_dir / f"{bid}.json"
        disk = None
        if content_path.exists():
            disk = json.loads(content_path.read_text(encoding="utf-8"))

        if bid in full_books and not disk:
            book = {"id": bid, "title": display, **full_books[bid]}
            book["genre"] = entry.get("genre", "")
            books.append(book)
            continue

        if disk:
            notes = disk.get("notes") or []
            quiz = disk.get("quiz") or []
            plot = disk.get("plot") or []
            plot_meaning = disk.get("plotMeaning") or []
            book = {
                "id": bid,
                "title": display,
                "subKey": "cetba1984_sub" if quiz else "cetba_entry_sub",
                "page": page if not entry.get("full") else (entry.get("page") or page),
                "genre": entry.get("genre", ""),
                "quizTitle": "book_quiz_heading" if quiz else "",
                "quizSub": "lit_quiz_sub" if quiz else "",
                "plotTitle": "lit_plot_title" if plot else "",
                "plotSub": "lit_plot_sub" if plot else "",
                "notes": notes,
                "quiz": quiz,
                "plot": plot,
                "plotMeaning": plot_meaning,
            }
            if bid in full_books:
                # Keep stable page ids for the two classic GTK screens
                book["page"] = full_books[bid]["page"]
                book["subKey"] = full_books[bid]["subKey"]
                book["quizTitle"] = full_books[bid]["quizTitle"]
                book["quizSub"] = full_books[bid]["quizSub"]
                book["plotTitle"] = full_books[bid]["plotTitle"]
                book["plotSub"] = full_books[bid]["plotSub"]
            books.append(book)
            continue

        about = [f"Autor: {entry['author']}", f"Žánr: {entry['genre']}"]
        if entry.get("translator"):
            about.append(f"Překlad: {entry['translator']}")
        about.append("Ze školního seznamu maturitní četby.")
        books.append({
            "id": bid,
            "title": display,
            "subKey": "cetba_entry_sub",
            "page": page,
            "genre": entry["genre"],
            "quizTitle": "",
            "quizSub": "",
            "plotTitle": "",
            "plotSub": "",
            "notes": [
                {"title": "O knize", "icon": "book", "lines": about},
            ],
            "quiz": [],
            "plot": [],
            "plotMeaning": [],
        })

    # Prefer disk content for 1984/fuks when present (already handled above).
    # GTK catalog: mark full when we have quiz content on disk or classic pages.
    has_full = {b["id"] for b in books if b.get("quiz")}

    # GTK catalog include for the reading-list path map
    inc_lines = [
        "/* Generated from data/cetba-catalog.json – do not edit by hand. */",
        f"#define BOOK_NODES {len(catalog)}",
        "",
        "typedef struct {",
        "    const char *title;",
        "    const char *target;",
        "    const char *author;",
        "    const char *work;",
        "    const char *genre;",
        "    const char *translator;",
        "    int full;",
        "} BookDef;",
        "",
        "static const BookDef book_defs[BOOK_NODES] = {",
    ]
    for entry in catalog:
        bid = entry["id"]
        display = f"{entry['author']} – {entry['title']}"
        page = entry.get("page") or f"cetba_{bid.replace('-', '_')}"
        if bid in ("1984", "fuks") or entry.get("full"):
            target = entry.get("page") or page
            full = 1
        elif bid in has_full:
            # Data-driven books still open the shared stub page for now;
            # Swift/Android use content.json fully.
            target = "cetbastub"
            full = 0
        else:
            target = "cetbastub"
            full = 0
        transl = entry.get("translator") or ""
        def c_str(s: str) -> str:
            return '"' + s.replace("\\", "\\\\").replace('"', '\\"') + '"'
        transl_c = "NULL" if not transl else c_str(transl)
        inc_lines.append(
            f"    {{{c_str(display)}, {c_str(target)}, {c_str(entry['author'])}, "
            f"{c_str(entry['title'])}, {c_str(entry['genre'])}, {transl_c}, {full}}},"
        )
    inc_lines.append("};")
    inc_lines.append("")
    (SRC / "cetba_catalog.inc").write_text("\n".join(inc_lines) + "\n", encoding="utf-8")

    # Full note packs for the GTK stub/detail page (Swift/Android use content.json).
    def c_escape(s: str) -> str:
        return s.replace("\\", "\\\\").replace('"', '\\"')

    notes_lines = [
        "/* Generated from data/cetba JSON packs – do not edit by hand. */",
        "",
        "typedef struct {",
        "    const char *title;",
        "    const char *const *lines;",
        "} BookNoteSec;",
        "",
        "typedef struct {",
        "    const BookNoteSec *secs;",
        "    int nsec;",
        "} BookNotePack;",
        "",
    ]
    pack_entries = []
    for i, entry in enumerate(catalog):
        bid = entry["id"]
        safe = re.sub(r"[^a-zA-Z0-9_]", "_", bid)
        content_path = cetba_dir / f"{bid}.json"
        notes = []
        if content_path.exists():
            disk = json.loads(content_path.read_text(encoding="utf-8"))
            notes = disk.get("notes") or []
        if not notes:
            about = [f"Autor: {entry['author']}", f"Žánr: {entry['genre']}"]
            if entry.get("translator"):
                about.append(f"Překlad: {entry['translator']}")
            notes = [{"title": "O knize", "lines": about}]
        sec_names = []
        for si, note in enumerate(notes):
            arr = f"book_{safe}_n{si}"
            notes_lines.append(f"static const char *const {arr}[] = {{")
            for line in note.get("lines") or []:
                notes_lines.append(f'    "{c_escape(line)}",')
            notes_lines.append("    NULL")
            notes_lines.append("};")
            notes_lines.append("")
            sec_names.append((note.get("title") or "Poznámky", arr))
        secs_arr = f"book_{safe}_secs"
        notes_lines.append(f"static const BookNoteSec {secs_arr}[] = {{")
        for title, arr in sec_names:
            notes_lines.append(f'    {{"{c_escape(title)}", {arr}}},')
        notes_lines.append("};")
        notes_lines.append("")
        pack_entries.append(f"    {{{secs_arr}, {len(sec_names)}}}")
    notes_lines.append(f"static const BookNotePack book_note_packs[BOOK_NODES] = {{")
    notes_lines.append(",\n".join(pack_entries))
    notes_lines.append("};")
    notes_lines.append("")
    (SRC / "cetba_book_notes.inc").write_text("\n".join(notes_lines) + "\n", encoding="utf-8")

    # Quiz + plot packs for the GTK stub pages (shared by every catalog book).
    ex_lines = [
        "/* Generated from data/cetba JSON packs – do not edit by hand. */",
        "/* Requires LitQ (cetba.c) and AssemblyItem (maturita.h). */",
        "",
        "typedef struct {",
        "    const LitQ *qs;",
        "    int nq;",
        "    const AssemblyItem *plot;",
        "    const char **plot_meaning;",
        "    int nplot;",
        "} BookExPack;",
        "",
    ]
    ex_pack_entries = []
    for entry in catalog:
        bid = entry["id"]
        safe = re.sub(r"[^a-zA-Z0-9_]", "_", bid)
        content_path = cetba_dir / f"{bid}.json"
        disk = {}
        if content_path.exists():
            disk = json.loads(content_path.read_text(encoding="utf-8"))
        quiz = disk.get("quiz") or []
        plot = disk.get("plot") or []
        plot_meaning = disk.get("plotMeaning") or []

        qs_name = f"book_{safe}_qs"
        if quiz:
            ex_lines.append(f"static const LitQ {qs_name}[] = {{")
            for q in quiz[:32]:
                opts = (q.get("options") or []) + ["", "", "", ""]
                ex_lines.append("    {")
                ex_lines.append(f'        "{c_escape(q.get("prompt") or "")}",')
                ex_lines.append("        {")
                for o in opts[:4]:
                    ex_lines.append(f'            "{c_escape(o)}",')
                ex_lines.append("        },")
                ex_lines.append(f'        {int(q.get("correct") or 0)},')
                ex_lines.append(f'        "{c_escape(q.get("expl") or "")}",')
                ex_lines.append("    },")
            ex_lines.append("};")
            ex_lines.append("")
            nq = min(len(quiz), 32)
            qs_ref = qs_name
        else:
            nq = 0
            qs_ref = "NULL"

        plot_name = f"book_{safe}_plot"
        mean_name = f"book_{safe}_plot_meaning"
        if plot and isinstance(plot[0], dict):
            words = (plot[0].get("words") or [])[:6]
            while len(words) < 6:
                words.append("")
            prompt = plot[0].get("prompt") or "Přetáhněte části příběhu do správného pořadí:"
            nwords = sum(1 for w in words if w)
            ex_lines.append(f"static const AssemblyItem {plot_name}[] = {{")
            ex_lines.append("    {")
            ex_lines.append(f'        "{c_escape(prompt)}",')
            ex_lines.append("        {")
            for w in words:
                ex_lines.append(f'            "{c_escape(w)}",')
            ex_lines.append("        },")
            ex_lines.append(f"        {nwords},")
            ex_lines.append("    },")
            ex_lines.append("};")
            ex_lines.append("")
            meaning = plot_meaning[0] if plot_meaning else ""
            ex_lines.append(f"static const char *{mean_name}[] = {{")
            ex_lines.append(f'    "{c_escape(meaning)}",')
            ex_lines.append("};")
            ex_lines.append("")
            plot_ref = plot_name
            mean_ref = mean_name
            nplot = 1
        else:
            plot_ref = "NULL"
            mean_ref = "NULL"
            nplot = 0

        ex_pack_entries.append(
            f"    {{{qs_ref}, {nq}, {plot_ref}, {mean_ref}, {nplot}}}"
        )

    ex_lines.append("static const BookExPack book_ex_packs[BOOK_NODES] = {")
    ex_lines.append(",\n".join(ex_pack_entries))
    ex_lines.append("};")
    ex_lines.append("")
    (SRC / "cetba_book_exercises.inc").write_text("\n".join(ex_lines) + "\n", encoding="utf-8")

    subjects = [
        {"key": "Deutsch", "open": True, "target": "roadmap", "icon": "de"},
        {"key": "Správa počítačových sítí", "open": True, "target": "netyears", "icon": "wifi"},
        {"key": "Technické vybavení", "open": True, "target": "hwyears", "icon": "chip"},
        {"key": "Český jazyk a literatura", "open": True, "target": "czechmap", "icon": "cz"},
        {"key": "Občanská nauka", "open": False, "target": None, "icon": "lock"},
        {"key": "English", "open": False, "target": None, "icon": "lock"},
        {"key": "Matematika", "open": False, "target": None, "icon": "lock"},
        {"key": "Fyzika", "open": False, "target": None, "icon": "lock"},
        {"key": "Základy Přírodopisných věd", "open": False, "target": None, "icon": "lock"},
        {"key": "Technická grafika", "open": False, "target": None, "icon": "lock"},
        {"key": "Prezentační grafika", "open": False, "target": None, "icon": "lock"},
        {"key": "Programování", "open": False, "target": None, "icon": "lock"},
        {"key": "Programové vybavení", "open": False, "target": None, "icon": "lock"},
    ]

    payload = {
        "i18n": i18n,
        "subjects": subjects,
        "german": german_units,
        "net": {
            "lessons": net_lessons,
            "tasks": net_tasks(A("net_tasks")),
            "answers": net_ans(A("net_ans")),
        },
        "hw": hw_lessons,
        "mluvnice": mluvnice,
        "books": books,
        "changelog": parse_changelog(ROOT / "data" / "changelog.txt"),
    }

    # Fix single-string banks stored as char* "arrays"
    for key in ("g05_bank", "ex5_u3_bank"):
        if key not in arrays:
            continue
        raw = arrays[key][1]
        if isinstance(raw, list) and len(raw) == 1 and isinstance(raw[0], str) and " " in raw[0]:
            if key == "g05_bank":
                german_units[1]["exercises"]["5"]["bank"] = raw[0]
            else:
                german_units[2]["exercises"]["5"]["bank"] = raw[0]

    text = json.dumps(payload, ensure_ascii=False, indent=2)
    for dest in OUTS:
        dest.parent.mkdir(parents=True, exist_ok=True)
        dest.write_text(text, encoding="utf-8")

    def count(path, pred):
        node = payload
        for p in path:
            node = node[p]
        return sum(1 for x in node if pred(x))

    print("wrote " + ", ".join(str(dest) for dest in OUTS))
    print(f"  i18n keys: {len(i18n)}")
    print(f"  german unlocked exercises: "
          f"{sum(len(u.get('exercises', {})) for u in german_units if u['unlocked'])}")
    print(f"  net lessons with slides: {sum(1 for L in net_lessons if L['slides'])}")
    print(f"  net quizzes: {sum(1 for L in net_lessons if L['quiz'])}")
    print(f"  hw lessons: {sum(1 for L in hw_lessons if L['slides'])}")
    print(f"  mluvnice: {len(mluvnice)}")
    print(f"  books quiz items: {[len(b['quiz']) for b in books]}")
    print(f"  changelog entries: {len(payload.get('changelog', []))}")
    missing = []
    for u in german_units:
        if not u["unlocked"]:
            continue
        for k, ex in u["exercises"].items():
            t = ex["type"]
            if t == "choice" and not ex.get("questions"):
                missing.append(f"{u['tag']}e{k} choice")
            if t == "typed" and not ex.get("rows"):
                missing.append(f"{u['tag']}e{k} typed")
            if t == "assembly" and not ex.get("items"):
                missing.append(f"{u['tag']}e{k} assembly")
            if t == "vocab" and not ex.get("sections"):
                missing.append(f"{u['tag']} vocab")
            if t == "fill" and not ex.get("rows"):
                missing.append(f"{u['tag']}e{k} fill")
            if t == "hangman" and not ex.get("words"):
                missing.append(f"{u['tag']}e{k} hangman")
        if u.get("vocab") and not u["vocab"].get("sections"):
            missing.append(f"{u['tag']} vocab sections")
    for L in net_lessons:
        if not L["slides"]:
            missing.append(f"net{L['id']} slides")
        if L["id"] > 1 and not L["quiz"]:
            missing.append(f"net{L['id']} quiz")
    for L in hw_lessons:
        if not L["slides"]:
            missing.append(f"hw{L['id']} slides")
        if not L["quiz"]:
            missing.append(f"hw{L['id']} quiz")
    for m in mluvnice:
        if m["type"] in ("typed", "reveal") and not m.get("items"):
            missing.append(f"mluv{m['id']}")
        if m["type"] == "choice" and not m.get("questions"):
            missing.append(f"mluv{m['id']}")
    if missing:
        print("MISSING:")
        for m in missing:
            print(" ", m)
        return 1
    return 0


if __name__ == "__main__":
    sys.exit(main())
