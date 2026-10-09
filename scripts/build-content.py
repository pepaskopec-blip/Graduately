#!/usr/bin/env python3
"""Build content.json from the Markdown files in content/.

The format is described in content/README.md. Every platform reads the
generated JSON; nothing under content/ is read at run time.
"""

from __future__ import annotations

import json
import re
import sys
import unicodedata
from dataclasses import dataclass, field
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
CONTENT = ROOT / "content"
OUTS = [ROOT / "android" / "app" / "src" / "main" / "assets" / "content.json"]


# ---------------------------------------------------------------------------
# Errors
# ---------------------------------------------------------------------------

class ContentError(Exception):
    pass


ERRORS: list[str] = []
WARNINGS: list[str] = []


def rel(path: Path) -> str:
    try:
        return str(path.relative_to(ROOT))
    except ValueError:
        return str(path)


def fail(path: Path, line: int | None, msg: str) -> None:
    where = f"{rel(path)}:{line}" if line else rel(path)
    ERRORS.append(f"{where}: {msg}")


def warn(path: Path, line: int | None, msg: str) -> None:
    where = f"{rel(path)}:{line}" if line else rel(path)
    WARNINGS.append(f"{where}: {msg}")


# ---------------------------------------------------------------------------
# Markdown subset
# ---------------------------------------------------------------------------

@dataclass
class Node:
    kind: str          # h1 h2 h3 bullet num quote answer table para
    text: str
    line: int
    checked: bool = False
    rows: list[list[str]] = field(default_factory=list)
    lines: list[str] = field(default_factory=list)


@dataclass
class Doc:
    path: Path
    meta: dict[str, str]
    meta_lines: dict[str, int]
    nodes: list[Node]


BULLET = re.compile(r"^- (\[([ xX])\] )?(.*)$")
NUMBERED = re.compile(r"^(\d+)\. (.*)$")
TABLE_SEP = re.compile(r"^\|?\s*:?-{3,}:?\s*(\|\s*:?-{3,}:?\s*)*\|?\s*$")


def parse_doc(path: Path) -> Doc:
    raw = path.read_text(encoding="utf-8").replace("\r\n", "\n")
    lines = raw.split("\n")
    meta: dict[str, str] = {}
    meta_lines: dict[str, int] = {}
    i = 0
    if lines and lines[0].strip() == "---":
        i = 1
        while i < len(lines) and lines[i].strip() != "---":
            ln = lines[i]
            if ln.strip() and not ln.lstrip().startswith("#"):
                key, sep, value = ln.partition(":")
                if not sep:
                    fail(path, i + 1, f"v hlavičce chybí dvojtečka: {ln!r}")
                else:
                    meta[key.strip()] = value.strip()
                    meta_lines[key.strip()] = i + 1
            i += 1
        if i >= len(lines):
            fail(path, 1, "hlavička začíná --- , ale nikde nekončí")
        i += 1

    nodes: list[Node] = []
    para: Node | None = None
    table: Node | None = None
    in_comment = False
    for n in range(i, len(lines)):
        ln = lines[n].rstrip()
        no = n + 1
        if in_comment:
            if "-->" in ln:
                in_comment = False
            continue
        if ln.lstrip().startswith("<!--"):
            if "-->" not in ln:
                in_comment = True
            para = table = None
            continue
        if not ln.strip():
            para = table = None
            continue
        if ln.startswith("|"):
            para = None
            if table is None:
                table = Node("table", "", no)
                nodes.append(table)
            if not TABLE_SEP.match(ln):
                table.rows.append(split_row(ln))
            continue
        table = None
        if ln.startswith("\\"):
            text = ln[1:]
            if para is None:
                para = Node("para", "", no)
                nodes.append(para)
            para.lines.append(text)
            continue
        m = re.match(r"^(#{1,3}) (.*)$", ln) or re.match(r"^(#{1,3})$", ln)
        if m:
            para = None
            nodes.append(Node(f"h{len(m.group(1))}", (m.group(2) if m.lastindex and m.lastindex > 1 else "").strip(), no))
            continue
        m = BULLET.match(ln)
        if m:
            para = None
            nodes.append(Node("bullet", m.group(3), no, checked=bool(m.group(2)) and m.group(2).lower() == "x"))
            continue
        m = NUMBERED.match(ln)
        if m:
            para = None
            nodes.append(Node("num", m.group(2), no))
            continue
        if ln.startswith("> ") or ln == ">":
            para = None
            nodes.append(Node("quote", ln[2:], no))
            continue
        if ln.startswith("= ") or ln == "=":
            para = None
            nodes.append(Node("answer", ln[2:], no))
            continue
        if para is None:
            para = Node("para", "", no)
            nodes.append(para)
        para.lines.append(ln)
    return Doc(path, meta, meta_lines, nodes)


def split_row(line: str) -> list[str]:
    s = line.strip()
    if s.startswith("|"):
        s = s[1:]
    if s.endswith("|") and not s.endswith("\\|"):
        s = s[:-1]
    cells: list[str] = []
    cur: list[str] = []
    i = 0
    while i < len(s):
        c = s[i]
        if c == "\\" and i + 1 < len(s):
            cur.append(s[i:i + 2])
            i += 2
            continue
        if c == "|":
            cells.append("".join(cur).strip())
            cur = []
        else:
            cur.append(c)
        i += 1
    cells.append("".join(cur).strip())
    return cells


def unesc(s: str) -> str:
    out = []
    i = 0
    while i < len(s):
        if s[i] == "\\" and i + 1 < len(s):
            out.append("\n" if s[i + 1] == "n" else s[i + 1])
            i += 2
            continue
        out.append(s[i])
        i += 1
    return "".join(out)


def ungap(s: str) -> tuple[list[str], list[str]]:
    """Split 'a [x] b [y] c' into segments ['a ', ' b ', ' c'] and answers ['x', 'y']."""
    segs: list[str] = []
    answers: list[str] = []
    cur: list[str] = []
    in_gap = False
    i = 0
    while i < len(s):
        c = s[i]
        if c == "\\" and i + 1 < len(s):
            cur.append("\n" if s[i + 1] == "n" else s[i + 1])
            i += 2
            continue
        if c == "[" and not in_gap:
            segs.append("".join(cur))
            cur = []
            in_gap = True
        elif c == "]" and in_gap:
            answers.append("".join(cur))
            cur = []
            in_gap = False
        else:
            cur.append(c)
        i += 1
    if in_gap:
        raise ContentError(f"neuzavřená mezera [ ... ] v {s!r}")
    segs.append("".join(cur))
    return segs, answers


def norm(s: str) -> str:
    s = unicodedata.normalize("NFKD", s.replace("ß", "ss"))
    s = "".join(c for c in s if not unicodedata.combining(c))
    return re.sub(r"\s+", " ", s).strip().lower()


def slugify(s: str) -> str:
    return re.sub(r"[^a-z0-9]+", "-", norm(s)).strip("-")


# ---------------------------------------------------------------------------
# Structure helpers
# ---------------------------------------------------------------------------

@dataclass
class Part:
    title: str
    line: int
    nodes: list[Node]

    def items(self) -> list["Part"]:
        """Split on ## headings. Nodes before the first ## are dropped."""
        out: list[Part] = []
        for nd in self.nodes:
            if nd.kind == "h2":
                out.append(Part(nd.text, nd.line, []))
            elif out:
                out[-1].nodes.append(nd)
        return out

    def head(self) -> list[Node]:
        out = []
        for nd in self.nodes:
            if nd.kind == "h2":
                break
            out.append(nd)
        return out

    def subparts(self) -> list["Part"]:
        out: list[Part] = []
        for nd in self.nodes:
            if nd.kind == "h3":
                out.append(Part(nd.text, nd.line, []))
            elif out:
                out[-1].nodes.append(nd)
        return out


def parts(doc: Doc) -> list[Part]:
    out: list[Part] = []
    for nd in doc.nodes:
        if nd.kind == "h1":
            out.append(Part(nd.text, nd.line, []))
        elif out:
            out[-1].nodes.append(nd)
        elif nd.kind != "h1":
            fail(doc.path, nd.line, "text před první sekcí (# Název) se nepoužije")
    return out


def part(doc: Doc, *names: str) -> Part | None:
    wanted = {norm(n) for n in names}
    for p in parts(doc):
        if norm(p.title) in wanted:
            return p
    return None


def text_of(nodes: list[Node]) -> str | None:
    paras = ["\n".join(nd.lines) for nd in nodes if nd.kind == "para"]
    return "\n\n".join(paras) if paras else None


def bullets(nodes: list[Node]) -> list[str]:
    return [nd.text for nd in nodes if nd.kind == "bullet"]


def numbered(nodes: list[Node]) -> list[str]:
    return [nd.text for nd in nodes if nd.kind == "num"]


def quote(nodes: list[Node]) -> str | None:
    q = [nd.text for nd in nodes if nd.kind == "quote"]
    return "\n".join(q) if q else None


def answer(nodes: list[Node]) -> str | None:
    a = [nd.text for nd in nodes if nd.kind == "answer"]
    return "\n".join(a) if a else None


def table(doc: Doc, nodes: list[Node], cols: int, where: str) -> list[list[str]]:
    tables = [nd for nd in nodes if nd.kind == "table"]
    if not tables:
        return []
    t = tables[0]
    rows = t.rows[1:]
    for k, r in enumerate(rows):
        if len(r) != cols:
            fail(doc.path, t.line + k + 2,
                 f"{where}: řádek tabulky má {len(r)} sloupců, čekám {cols} "
                 "(svislítko uvnitř textu se píše jako \\|)")
    return [r + [""] * (cols - len(r)) for r in rows]


def flag(doc: Doc, key: str) -> bool | None:
    if key not in doc.meta:
        return None
    v = norm(doc.meta[key])
    if v in ("ano", "true", "yes", "1"):
        return True
    if v in ("ne", "false", "no", "0"):
        return False
    fail(doc.path, doc.meta_lines.get(key), f"{key}: čekám ano nebo ne, ne {doc.meta[key]!r}")
    return None


def need(doc: Doc, key: str) -> str:
    v = doc.meta.get(key)
    if v is None:
        fail(doc.path, None, f"v hlavičce chybí {key}:")
        return ""
    return v


def opt_int(s: str) -> int | None:
    s = s.strip()
    return int(s) if re.fullmatch(r"-?\d+", s) else None


# ---------------------------------------------------------------------------
# Shared readers
# ---------------------------------------------------------------------------

def read_quiz(doc: Doc, p: Part | None, expl_key: str | None = "expl") -> list[dict]:
    if p is None:
        return []
    out = []
    for it in p.items():
        opts = [nd for nd in it.nodes if nd.kind == "bullet"]
        if len(opts) < 2:
            fail(doc.path, it.line, f"otázka {it.title!r} má méně než dvě možnosti")
        right = [k for k, nd in enumerate(opts) if nd.checked]
        if len(right) != 1:
            fail(doc.path, it.line, f"otázka {it.title!r} musí mít právě jednu správnou možnost (- [x] …)")
        q = {"prompt": it.title, "options": [nd.text for nd in opts],
             "correct": right[0] if right else 0}
        ex = quote(it.nodes)
        if ex is not None and expl_key:
            q[expl_key] = ex
        out.append(q)
    return out


def read_slides(doc: Doc, p: Part | None) -> list[dict]:
    if p is None:
        return []
    items = p.items()
    out = []
    for k, it in enumerate(items):
        label = None
        for nd in it.nodes:
            if nd.kind == "para":
                m = re.fullmatch(r"\*(.+)\*", nd.lines[0])
                if m:
                    label = m.group(1)
                break
        if label is None:
            fail(doc.path, it.line, f"snímek {it.title!r}: pod nadpisem chybí téma (*Téma*)")
            label = ""
        out.append({
            "kicker": f"{k + 1} / {len(items)}   •   {label}",
            "title": it.title,
            "tip": quote(it.nodes),
            "lines": bullets(it.nodes),
        })
    return out


def read_typed(doc: Doc, p: Part | None, meaning_key: str = "meaning") -> list[dict]:
    if p is None:
        return []
    out = []
    for it in p.items():
        a = answer(it.nodes)
        if a is None:
            fail(doc.path, it.line, f"úloha {it.title!r} nemá odpověď (= …)")
        out.append({"prompt": it.title, "answers": a or "", meaning_key: quote(it.nodes)})
    return out


# ---------------------------------------------------------------------------
# i18n
# ---------------------------------------------------------------------------

class I18n:
    def __init__(self) -> None:
        self.map: dict[str, dict] = {}
        self.origin: dict[str, str] = {}

    def put(self, key: str, cs: str | None, en: str | None, src: Path) -> str:
        if key in self.map and self.origin[key] != rel(src):
            prev = self.map[key]
            if (prev["cs"], prev["en"]) != (cs, en):
                warn(src, None, f"klíč {key!r} už definuje {self.origin[key]}")
        self.map[key] = {"cs": cs, "en": en}
        self.origin[key] = rel(src)
        return key

    def text(self, cs: str, en: str | None, src: Path) -> None:
        """Translation of a content string: the Czech text itself is the key."""
        if cs in self.map:
            prev = self.map[cs]
            if prev["en"] and en and prev["en"] != en:
                warn(src, None, f"text {cs[:40]!r} má jiný překlad v {self.origin[cs]}")
            if prev["en"] and not en:
                return
        self.map[cs] = {"cs": cs, "en": en}
        self.origin[cs] = rel(src)


I18N = I18n()


def en_doc(path: Path) -> Doc | None:
    p = path.with_name(path.stem + ".en.md")
    return parse_doc(p) if p.exists() else None


def load_translations(doc: Doc | None) -> None:
    if doc is None:
        return
    p = part(doc, "Překlady", "Translations")
    if p is None:
        return
    for r in table(doc, p.nodes, 2, "Překlady"):
        cs, en = unesc(r[0]), unesc(r[1])
        if cs:
            I18N.text(cs, en or None, doc.path)


def keyed(key: str, doc: Doc, en: Doc | None, field_name: str, required: bool = True) -> str | None:
    cs = doc.meta.get(field_name)
    if cs is None:
        if required:
            fail(doc.path, None, f"v hlavičce chybí {field_name}:")
        return None
    e = en.meta.get(field_name) if en else None
    return I18N.put(key, cs or None, e or None, doc.path)


# ---------------------------------------------------------------------------
# Progress keys
# ---------------------------------------------------------------------------

PROGRESS: dict[str, str] = {}
LEGACY: dict[str, str] = {}
IDS: dict[str, Path] = {}


def lesson_id(doc: Doc, address: str) -> str:
    lid = need(doc, "id")
    if lid in IDS:
        fail(doc.path, doc.meta_lines.get("id"), f"id {lid!r} už má {rel(IDS[lid])}")
    IDS[lid] = doc.path
    PROGRESS[address] = lid
    old = doc.meta.get("puvodni")
    if old:
        for o in old.split(","):
            LEGACY[o.strip()] = lid
    return lid


# ---------------------------------------------------------------------------
# Lesson kinds
# ---------------------------------------------------------------------------

def by_order(p: Path) -> tuple[int, str]:
    m = re.match(r"^(\d+)-", p.name)
    return (int(m.group(1)) if m else 1 << 30, p.name)


def lesson_files(directory: Path) -> list[Path]:
    if not directory.is_dir():
        fail(directory, None, "složka chybí")
        return []
    files = sorted((p for p in directory.glob("*.md")
                    if not p.name.endswith(".en.md") and not p.name.startswith("_")), key=by_order)
    for p in files:
        if not re.match(r"^\d+-", p.name):
            fail(p, None, "název souboru musí začínat pořadím, např. 03-nazev.md")
    return files


def std_lessons(directory: Path, prefix: str) -> list[dict]:
    """Lessons with slides and a choice quiz (IT, civics, literature, sciences, physics)."""
    out = []
    for i, path in enumerate(lesson_files(directory), start=1):
        doc, en = parse_doc(path), en_doc(path)
        load_translations(en)
        lid = lesson_id(doc, f"{prefix}.{i}")
        ex_key = "net_ex_title" if prefix == "net" and i == 1 else f"{prefix}_ex{i}_title"
        lesson = {
            "id": i,
            "key": lid,
            "titleKey": keyed(f"{prefix}_unit{i}", doc, en, "nazev"),
            "subKey": keyed(f"{prefix}_unit{i}_sub", doc, en, "popis"),
            "exTitleKey": keyed(ex_key, doc, en, "nazev_cviceni"),
            "quizHeadKey": keyed(f"{prefix}_quiz{i}_head", doc, en, "nadpis_kvizu", required=False),
            "slides": read_slides(doc, part(doc, "Výklad")),
            "quiz": read_quiz(doc, part(doc, "Kvíz")),
        }
        out.append(lesson)
        if prefix == "net" and i == 1:
            NET_TASKS.extend(read_net_tasks(doc, part(doc, "Úlohy na podsítě")))
    return out


NET_TASKS: list[tuple[dict, list[dict]]] = []


def read_net_tasks(doc: Doc, p: Part | None) -> list[tuple[dict, list[dict]]]:
    if p is None:
        return []
    out = []
    for it in p.items():
        head = []
        for nd in it.nodes:
            if nd.kind == "h3":
                break
            head.append(nd)
        sub = {norm(s.title): s for s in it.subparts()}
        sol = sub.get(norm("Řešení"))
        chk = sub.get(norm("Kontrola"))
        rows = []
        for r in table(doc, chk.nodes if chk else [], 5, "Kontrola"):
            pfx = opt_int(r[0])
            rows.append({"pfx": pfx if pfx is not None else -1, "net": r[1] or None,
                         "bcast": r[2] or None, "lo": r[3] or None, "hi": r[4] or None})
        out.append(({"prompt": text_of(head) or "",
                     "solution": text_of(sol.nodes) if sol else ""}, rows))
    return out


def math_lessons(directory: Path, prefix: str) -> list[dict]:
    out = []
    for i, path in enumerate(lesson_files(directory), start=1):
        doc, en = parse_doc(path), en_doc(path)
        load_translations(en)
        lid = lesson_id(doc, f"{prefix}.{i}")
        problems = []
        for it in (part(doc, "Příklady").items() if part(doc, "Příklady") else []):
            a = answer(it.nodes)
            if a is None:
                fail(doc.path, it.line, f"příklad {it.title!r} nemá výsledek (= …)")
            problems.append({"prompt": it.title, "answer": a or "", "hint": quote(it.nodes) or ""})
        out.append({
            "id": i,
            "key": lid,
            "titleKey": keyed(f"{prefix}_unit{i}", doc, en, "nazev"),
            "subKey": keyed(f"{prefix}_unit{i}_sub", doc, en, "popis"),
            "exTitleKey": keyed(f"{prefix}_ex{i}_title", doc, en, "nazev_cviceni"),
            "quizHeadKey": keyed(f"{prefix}_quiz{i}_head", doc, en, "nadpis_kvizu"),
            "slides": read_slides(doc, part(doc, "Výklad")),
            "problems": problems,
        })
    return out


def skill_lessons(directory: Path, tag: str, year: int) -> list[dict]:
    """English and German years: slides, reading, listening, gaps, writing, quiz."""
    out = []
    for i, path in enumerate(lesson_files(directory), start=1):
        doc, en = parse_doc(path), en_doc(path)
        load_translations(en)
        lid = lesson_id(doc, f"{tag}.{year}.{i}")
        read = part(doc, "Čtení")
        listen = part(doc, "Poslech")
        write = part(doc, "Psaní")
        writing = None
        if write is not None:
            sub = {norm(s.title): s for s in write.items()}
            model = sub.get(norm("Vzorová odpověď"))
            keys = sub.get(norm("Musí obsahovat"))
            words = sub.get(norm("Minimum slov"))
            n = opt_int(text_of(words.nodes) or "") if words else None
            writing = {
                "prompt": text_of(write.head()) or "",
                "model": text_of(model.nodes) if model else "",
                "keys": "|".join(bullets(keys.nodes)) if keys else "",
                "minWords": n if n is not None else 20,
            }
        out.append({
            "id": i,
            "key": lid,
            "titleKey": keyed(f"{tag}_y{year}_u{i}", doc, en, "nazev"),
            "subKey": keyed(f"{tag}_y{year}_u{i}_sub", doc, en, "popis"),
            "slides": read_slides(doc, part(doc, "Výklad")),
            "reading": text_of(read.head()) if read else None,
            "readQuiz": read_quiz(doc, read),
            "listening": text_of(listen.head()) if listen else None,
            "listenQuiz": read_quiz(doc, listen),
            "gaps": read_typed(doc, part(doc, "Doplňování")),
            "writing": writing,
            "quiz": read_quiz(doc, part(doc, "Kvíz")),
        })
    return out


MLUV_TYPES = {"doplnovani": "typed", "vyber": "choice", "reseni": "reveal"}


def mluvnice(directory: Path) -> list[dict]:
    out = []
    for n, path in enumerate(lesson_files(directory), start=1):
        doc, en = parse_doc(path), en_doc(path)
        load_translations(en)
        lid = lesson_id(doc, f"mluv.{n}")
        kind = MLUV_TYPES.get(norm(need(doc, "typ")))
        if kind is None:
            fail(path, doc.meta_lines.get("typ"), f"typ musí být jeden z: {', '.join(MLUV_TYPES)}")
            kind = "typed"
        item = {"id": n, "key": lid, "name": need(doc, "zkratka"), "type": kind,
                "title": need(doc, "nazev"), "sub": need(doc, "popis"),
                "note": doc.meta.get("poznamka") or None}
        p = part(doc, "Úlohy")
        its = p.items() if p else []
        if kind == "typed":
            rows = []
            for it in its:
                hints, answers = [], []
                for b in bullets(it.nodes):
                    h, sep, a = b.partition(" = ")
                    if not sep:
                        fail(path, it.line, f"mezera {b!r} musí mít tvar „nápověda = odpověď“")
                    hints.append(h)
                    answers.append(a)
                rows.append({"prompt": it.title, "shown": quote(it.nodes) or "",
                             "hints": hints, "answers": answers})
            item["items"] = rows
        elif kind == "reveal":
            item["items"] = [{"prompt": it.title, "solution": answer(it.nodes) or ""} for it in its]
        else:
            qs = read_quiz(doc, p, expl_key=None)
            item["questions"] = qs
            expls = [quote(it.nodes) for it in its]
            if any(e is not None for e in expls):
                item["expls"] = [e or "" for e in expls]
        out.append(item)
    return out


BOOK_ICONS = ["book", "globe", "people", "bulb"]


def books(directory: Path) -> list[dict]:
    out = []
    for path in lesson_files(directory):
        doc, en = parse_doc(path), en_doc(path)
        load_translations(en)
        bid = need(doc, "id")
        if bid in IDS:
            fail(path, doc.meta_lines.get("id"), f"id {bid!r} už má {rel(IDS[bid])}")
        IDS[bid] = path
        PROGRESS[f"book.{bid}.quiz"] = f"{bid}.kviz"
        PROGRESS[f"book.{bid}.plot"] = f"{bid}.dej"
        LEGACY[f"book.{bid}.quiz"] = f"{bid}.kviz"
        LEGACY[f"book.{bid}.plot"] = f"{bid}.dej"
        author, title = need(doc, "autor"), need(doc, "nazev")
        notes, quiz, plot, plot_meaning = [], [], [], []
        for p in parts(doc):
            t = norm(p.title)
            if t == norm("Kvíz"):
                quiz = read_quiz(doc, p)
            elif t == norm("Děj"):
                for it in p.items():
                    plot.append({"prompt": it.title, "words": numbered(it.nodes)})
                    m = quote(it.nodes)
                    if m is not None:
                        plot_meaning.append(m)
            else:
                icon = BOOK_ICONS[len(notes)] if len(notes) < len(BOOK_ICONS) else "book"
                notes.append({"title": p.title, "icon": icon, "lines": bullets(p.nodes)})

        def label(field_name: str, default: str) -> str:
            if field_name in doc.meta:
                return keyed(f"book_{slugify(bid)}_{field_name}", doc, en, field_name) or default
            return default

        out.append({
            "id": bid,
            "title": f"{author} – {title}",
            "subKey": label("popis", "cetba1984_sub" if quiz else "cetba_entry_sub"),
            "page": doc.meta.get("stranka") or f"cetba_{bid.replace('-', '_')}",
            "genre": need(doc, "zanr"),
            "quizTitle": label("kviz_nadpis", "book_quiz_heading") if quiz else "",
            "quizSub": label("kviz_popis", "lit_quiz_sub") if quiz else "",
            "plotTitle": label("dej_nadpis", "lit_plot_title") if plot else "",
            "plotSub": label("dej_popis", "lit_plot_sub") if plot else "",
            "notes": notes,
            "quiz": quiz,
            "plot": plot,
            "plotMeaning": plot_meaning,
        })
    return out


# ---------------------------------------------------------------------------
# German textbook
# ---------------------------------------------------------------------------

def gap_rows(doc: Doc, rows: list[list[str]], col: int, where: str) -> list[tuple[str, str, str]]:
    out = []
    for r in rows:
        try:
            segs, ans = ungap(r[col])
        except ContentError as e:
            fail(doc.path, None, f"{where}: {e}")
            segs, ans = [r[col]], [""]
        if len(ans) != 1:
            fail(doc.path, None, f"{where}: věta {r[col]!r} musí mít právě jednu mezeru [ … ]")
            ans = (ans + [""])[:1]
            segs = (segs + ["", ""])[:2]
        out.append((segs[0], ans[0], segs[1]))
    return out


def seg_rows(doc: Doc, text: str, pad: int, where: str) -> tuple[list[str], list[str]]:
    try:
        segs, ans = ungap(text)
    except ContentError as e:
        fail(doc.path, None, f"{where}: {e}")
        return [text], []
    while len(segs) < pad:
        segs.append("")
    return segs, ans


def blist(doc: Doc, name: str) -> list[str]:
    p = part(doc, name)
    return bullets(p.nodes) if p else []


def rows_of(doc: Doc, name: str, cols: int) -> list[list[str]]:
    p = part(doc, name)
    return table(doc, p.nodes, cols, name) if p else []


def nullable(s: str) -> str | None:
    return unesc(s) if s != "" else None


def german_exercise(doc: Doc, kind: str) -> dict:
    ex: dict = {"type": kind}
    pool = lambda: blist(doc, "Nabídka")
    meanings = lambda: blist(doc, "Překlad")
    if kind == "dialog":
        dialogues = []
        p = part(doc, "Rozhovory")
        for it in (p.items() if p else []):
            rows = []
            for r in table(doc, it.nodes, 2, it.title):
                (b, a, af), = gap_rows(doc, [r], 1, it.title)
                rows.append({"speaker": unesc(r[0]), "before": b, "answer": a, "after": af})
            dialogues.append({"name": it.title, "rows": rows})
        ex.update(pool=pool(), rows=[], meanings=meanings(), dialogues=dialogues)
    elif kind == "assembly":
        items = [{"prompt": None, "words": b.split(" / ")} for b in blist(doc, "Věty")]
        items += [{"prompt": nullable(r[0]), "words": unesc(r[1]).split(" / ")}
                  for r in rows_of(doc, "Úlohy", 2)]
        ex.update(items=items, meanings=meanings())
    elif kind == "choice":
        ex.update(questions=read_quiz(doc, part(doc, "Otázky"), expl_key=None), meanings=meanings())
    elif kind == "free":
        ex["questions"] = [{"question": unesc(r[0]), "qCs": unesc(r[1]), "sample": unesc(r[2]),
                            "aCs": unesc(r[3])} for r in rows_of(doc, "Otázky", 4)]
    elif kind == "number":
        ex.update(pool=pool(), rows=[{"digits": unesc(r[0]), "answer": unesc(r[1])}
                                     for r in rows_of(doc, "Úlohy", 2)], meanings=meanings())
    elif kind == "count":
        ex.update(pool=pool(), rows=[{"emoji": unesc(r[0]), "count": opt_int(r[1]) or 0,
                                      "noun": unesc(r[2]), "answer": unesc(r[3])}
                                     for r in rows_of(doc, "Úlohy", 4)], meanings=meanings())
    elif kind in ("seq", "verb"):
        rows = [{"before": b, "after": af, "answer": a}
                for b, a, af in gap_rows(doc, rows_of(doc, "Úlohy", 1), 0, "Úlohy")]
        ex.update(pool=pool(), rows=rows, meanings=meanings())
        if part(doc, "Glosář") is not None:
            ex["gloss"] = blist(doc, "Glosář")
    elif kind == "verb_sections":
        sections = []
        p = part(doc, "Oddíly")
        for it in (p.items() if p else []):
            rows = [{"before": b, "after": af, "answer": a}
                    for b, a, af in gap_rows(doc, table(doc, it.nodes, 1, it.title), 0, it.title)]
            sections.append({"title": it.title, "rows": rows, "meanings": bullets(it.nodes)})
        ex.update(pool=pool(), rows=[], meanings=[], sections=sections)
    elif kind == "assign":
        groups = blist(doc, "Skupiny")
        items = []
        for r in rows_of(doc, "Slova", 3):
            g = unesc(r[2])
            if g not in groups:
                fail(doc.path, None, f"skupina {g!r} není v seznamu # Skupiny")
            items.append({"emoji": nullable(r[0]), "label": unesc(r[1]),
                          "group": groups.index(g) if g in groups else 0})
        ex.update(items=items, groups=groups, meanings=meanings())
    elif kind == "verbclue":
        rs = rows_of(doc, "Úlohy", 2)
        rows = [{"emoji": unesc(r[0]), "before": b, "after": af, "answer": a}
                for r, (b, a, af) in zip(rs, gap_rows(doc, rs, 1, "Úlohy"))]
        ex.update(pool=pool(), rows=rows, meanings=meanings())
    elif kind == "typed":
        ex["rows"] = [{"prompt": unesc(r[0]), "answers": unesc(r[1]), "meaning": nullable(r[2])}
                      for r in rows_of(doc, "Úlohy", 3)]
        p = part(doc, "Nabídka")
        if p is not None:
            ex["bank"] = text_of(p.nodes)
    elif kind == "profile":
        ex["questions"] = [{"stem": unesc(r[0]), "sample": unesc(r[1])}
                           for r in rows_of(doc, "Otázky", 2)]
    elif kind == "hangman":
        rs = rows_of(doc, "Slova", 2)
        letters = text_of(part(doc, "Písmena").nodes) if part(doc, "Písmena") else ""
        ex.update(words=[unesc(r[0]) for r in rs], tips=[unesc(r[1]) for r in rs],
                  letters=(letters or "").split())
    elif kind == "ordne":
        ex.update(pool=[], rows=[{"mid": unesc(r[1]), "fw": unesc(r[0]), "ans": unesc(r[2])}
                                 for r in rows_of(doc, "Úlohy", 3)],
                  meanings=meanings(), fwPool=blist(doc, "Tázací slova"),
                  ansPool=blist(doc, "Odpovědi"))
    elif kind in ("fill", "letters"):
        pad = 4 if kind == "fill" else 5
        rows = []
        for r in rows_of(doc, "Úlohy", 3):
            segs, ans = seg_rows(doc, r[1], pad, "Úlohy")
            rows.append({"num": nullable(r[0]), "segs": segs, "answers": ans, "mean": nullable(r[2])})
        ex["rows"] = rows
        if kind == "fill":
            ex.update(pool=pool(), meanings=meanings())
    elif kind == "ex2":
        items = []
        p = part(doc, "Úlohy")
        for it in (p.items() if p else []):
            rows = []
            for r in table(doc, it.nodes, 2, it.title):
                segs, ans = seg_rows(doc, r[1], 0, it.title)
                rows.append({"num": nullable(r[0]), "segs": segs, "answers": ans})
            items.append({"czech": it.title, "rows": rows})
        ex.update(pool=pool(), items=items)
    elif kind == "letters_gap":
        rows = []
        p = part(doc, "Věty")
        for it in (p.items() if p else []):
            segs, ans = seg_rows(doc, it.title, 0, "Věty")
            words, gmean = [], []
            for b in bullets(it.nodes):
                w, _, m = b.partition(" = ")
                words.append(w)
                gmean.append(m)
            rows.append({"segs": segs, "answers": ans, "czech": quote(it.nodes),
                         "words": words, "gmean": gmean})
        ex["rows"] = rows
    elif kind == "table":
        p = part(doc, "Tabulka")
        t = [nd for nd in (p.nodes if p else []) if nd.kind == "table"]
        rows = t[0].rows if t else [[""]]
        ex.update(persons=[unesc(c) for c in rows[0][1:]],
                  nouns=[unesc(r[0]) for r in rows[1:]],
                  answers=[[unesc(c) for c in r[1:]] for r in rows[1:]])
    elif kind == "kw":
        ex["rows"] = [{"prompt": unesc(r[0]), "answers": unesc(r[1]), "mean": nullable(r[2]),
                       "german": nullable(r[3])} for r in rows_of(doc, "Úlohy", 4)]
    elif kind == "vocab":
        sections = []
        p = part(doc, "Slovíčka")
        for it in (p.items() if p else []):
            sections.append({"header": it.title, "rows": [
                {"prompt": unesc(r[0]), "answers": unesc(r[1]), "meaning": nullable(r[2])}
                for r in table(doc, it.nodes, 3, it.title)]})
        ex["sections"] = sections
    else:
        fail(doc.path, doc.meta_lines.get("typ"), f"neznámý typ cvičení {kind!r}")
    for fm, js in (("ukazka", "sample"),):
        if fm in doc.meta:
            ex[js] = doc.meta[fm]
    for fm, js in (("vyznam_predem", "preMeaning"), ("preklad_predem", "transUpfront"),
                   ("odhalit_nemcinu", "revealGerman"), ("ukazat_nemcinu", "showGerman"),
                   ("poznamka_k_odpovedim", "answersNote")):
        v = flag(doc, fm)
        if v is not None:
            ex[js] = v
    return ex


def german_textbook(directory: Path) -> list[dict]:
    units = []
    unit_dirs = sorted((p for p in directory.iterdir() if p.is_dir()), key=by_order) \
        if directory.is_dir() else []
    for u, udir in enumerate(unit_dirs):
        info_path = udir / "_lekce.md"
        if not info_path.exists():
            fail(udir, None, "chybí _lekce.md")
            continue
        info, info_en = parse_doc(info_path), en_doc(info_path)
        load_translations(info_en)
        n = u + 1
        title = need(info, "nazev")
        if flag(info, "zamceno"):
            units.append({"id": u, "title": title, "page": None, "tag": None, "sub": None,
                          "unlocked": False, "names": [], "exercises": {}})
            continue
        exercises: dict[str, dict] = {}
        names: list[str] = []
        vocab = None
        files = sorted((p for p in udir.glob("*.md")
                        if not p.name.endswith(".en.md") and not p.name.startswith("_")), key=by_order)
        k = 0
        for path in files:
            doc, en = parse_doc(path), en_doc(path)
            load_translations(en)
            kind = need(doc, "typ")
            if kind == "vocab":
                lesson_id(doc, f"g.{u}.vocab")
                vocab = german_exercise(doc, kind)
                vocab = {"type": "vocab", "title": need(doc, "nazev"),
                         "sub": keyed(f"u{n}vocab_sub", doc, en, "popis"),
                         "sections": vocab["sections"]}
                continue
            if not re.match(r"^\d+-", path.name):
                fail(path, None, "název souboru musí začínat pořadím, např. 03-nazev.md")
            k += 1
            lesson_id(doc, f"g.{u}.{k}")
            ex = german_exercise(doc, kind)
            ex["title"] = need(doc, "nazev")
            ex["sub"] = keyed(f"u{n}e{k}_sub", doc, en, "popis")
            if "tip" in doc.meta:
                ex["tip"] = keyed(f"u{n}e{k}_tip", doc, en, "tip")
            exercises[str(k)] = ex
            names.append(ex["title"])
        unit = {"id": u, "title": title, "page": f"unit{n}", "tag": f"u{n}",
                "sub": keyed(f"unit{n}_sub", info, info_en, "popis"), "unlocked": True,
                "names": names, "branch": f"u{n}vocab", "exercises": exercises}
        if vocab is not None:
            unit["vocab"] = vocab
        units.append(unit)
    return units


# ---------------------------------------------------------------------------
# App-level files
# ---------------------------------------------------------------------------

def ui_strings(path: Path) -> None:
    doc = parse_doc(path)
    for t in (nd for nd in doc.nodes if nd.kind == "table"):
        for r in table(doc, [t], 3, "Texty"):
            key = unesc(r[0])
            if not key:
                continue
            if key in I18N.map:
                warn(path, None, f"klíč {key!r} je v textech dvakrát")
            I18N.put(key, nullable(r[1]), nullable(r[2]), path)


def subjects(path: Path) -> list[dict]:
    doc = parse_doc(path)
    out = []
    p = part(doc, "Předměty")
    for r in table(doc, p.nodes if p else [], 6, "Předměty"):
        key = unesc(r[0])
        if r[1] or r[2]:
            I18N.put(key, nullable(r[1]), nullable(r[2]), path)
        out.append({"key": key, "open": flag_cell(r[5]), "target": nullable(r[4]),
                    "icon": unesc(r[3])})
    return out


def flag_cell(s: str) -> bool:
    return norm(s) in ("ano", "true", "yes", "1")


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


# ---------------------------------------------------------------------------
# Main
# ---------------------------------------------------------------------------

STD = [
    ("pocitacove-site/2-rocnik", "net2"), ("pocitacove-site/3-rocnik", "net3"),
    ("pocitacove-site/4-rocnik", "net4"),
    ("technicke-vybaveni/1-rocnik", "hw"), ("technicke-vybaveni/2-rocnik", "hw2"),
    ("technicke-vybaveni/3-rocnik", "hw3"), ("technicke-vybaveni/4-rocnik", "hw4"),
    ("obcanska-nauka/1-rocnik", "on"), ("obcanska-nauka/2-rocnik", "on2"),
    ("obcanska-nauka/3-rocnik", "on3"), ("obcanska-nauka/4-rocnik", "on4"),
    ("cestina/literatura/1-rocnik", "lit"), ("cestina/literatura/2-rocnik", "lit2"),
    ("cestina/literatura/3-rocnik", "lit3"), ("cestina/literatura/4-rocnik", "lit4"),
    ("prirodni-vedy/chemie", "chem"), ("prirodni-vedy/biologie", "bio"),
    ("fyzika/1-rocnik", "fyz"), ("fyzika/2-rocnik", "fyz2"),
    ("fyzika/3-rocnik", "fyz3"), ("fyzika/4-rocnik", "fyz4"),
]
MATH = [
    ("matematika/0-opakovani", "mat0"), ("matematika/1-rocnik", "mat"),
    ("matematika/2-rocnik", "mat2"), ("matematika/3-rocnik", "mat3"),
    ("matematika/4-rocnik", "mat4"),
]
SKILLS = [("anglictina", "en"), ("nemcina", "de")]


def build(content: Path = CONTENT) -> dict:
    ui_strings(content / "aplikace" / "texty.md")
    payload: dict = {"i18n": None}
    payload["subjects"] = subjects(content / "aplikace" / "predmety.md")
    payload["german"] = german_textbook(content / "nemcina" / "ucebnice")
    net = std_lessons(content / "pocitacove-site" / "1-rocnik", "net")
    payload["net"] = {
        "lessons": net,
        "tasks": [t for t, _ in NET_TASKS],
        "answers": [a for _, a in NET_TASKS],
    }
    for d, key in STD:
        payload[key] = std_lessons(content / d, key)
    for d, key in MATH:
        payload[key] = math_lessons(content / d, key)
    for d, tag in SKILLS:
        for year in range(1, 5):
            payload[tag if year == 1 else f"{tag}{year}"] = skill_lessons(
                content / d / f"{year}-rocnik", tag, year)
    payload["mluvnice"] = mluvnice(content / "cestina" / "mluvnice")
    payload["books"] = books(content / "cestina" / "cetba")
    payload["changelog"] = parse_changelog(content / "aplikace" / "novinky.txt")
    payload["progress"] = {"keys": PROGRESS, "legacy": LEGACY}
    payload["i18n"] = I18N.map
    check(payload)
    return payload


def check(payload: dict) -> None:
    for sec, lessons in payload.items():
        if not isinstance(lessons, list):
            continue
        for L in lessons:
            if not isinstance(L, dict):
                continue
            for q in L.get("quiz") or []:
                if isinstance(q, dict) and q.get("options") and not 0 <= q["correct"] < len(q["options"]):
                    ERRORS.append(f"{sec} {L.get('id')}: správná odpověď mimo možnosti")


def main() -> int:
    outs = OUTS
    if "--out" in sys.argv:
        outs = [Path(a) for a in sys.argv[sys.argv.index("--out") + 1:] if a]
        if not outs:
            sys.exit("usage: build-content.py [--out PATH ...]")
    payload = build()
    for w in WARNINGS:
        print(f"upozornění: {w}", file=sys.stderr)
    if ERRORS:
        for e in ERRORS:
            print(f"chyba: {e}", file=sys.stderr)
        print(f"{len(ERRORS)} chyb, content.json nevznikl", file=sys.stderr)
        return 1
    text = json.dumps(payload, ensure_ascii=False, indent=1)
    for dest in outs:
        dest.parent.mkdir(parents=True, exist_ok=True)
        if not dest.exists() or dest.read_text(encoding="utf-8") != text:
            dest.write_text(text, encoding="utf-8")
    print(f"content.json: {len(IDS)} lekcí, {len(payload['i18n'])} textů → "
          + ", ".join(rel(d) for d in outs))
    return 0


if __name__ == "__main__":
    sys.exit(main())
