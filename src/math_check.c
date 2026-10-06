/* Numeric and construction checker for mathematics.
 * No GTK: the same rules are ported to Swift and Kotlin.
 * A rational matches exactly, or within 0.0005.
 * A square root also matches a decimal within 0.001.
 * Pi stays symbolic: 3.14 is not accepted for pi. */
#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glib.h>

#define MC_TOKS 96
#define MC_PARTS 8
#define MC_PART 192

typedef struct {
    int ok;
    long an, ad;
    long bn, bd;
    int rad;
    long pn, pd;
} Term;

typedef enum {
    TK_END, TK_NUM, TK_SQRT, TK_PI, TK_ADD, TK_SUB, TK_MUL, TK_DIV, TK_LP, TK_RP
} TokKind;

typedef struct {
    TokKind kind;
    long n, d;
} Tok;

static long igcd(long a, long b) {
    if (a < 0)
        a = -a;
    if (b < 0)
        b = -b;
    while (b) {
        long t = a % b;
        a = b;
        b = t;
    }
    return a ? a : 1;
}

static int safe_mul(long a, long b, long *out) {
    __int128 p = (__int128)a * (__int128)b;

    if (p > (__int128)LONG_MAX || p < (__int128)LONG_MIN)
        return 0;
    *out = (long)p;
    return 1;
}

static int set_frac(long n, long d, long *on, long *od) {
    long g;

    if (d == 0)
        return 0;
    if (d < 0) {
        n = -n;
        d = -d;
    }
    g = igcd(n, d);
    *on = n / g;
    *od = d / g;
    return 1;
}

static int mul_frac(long n1, long d1, long n2, long d2, long *on, long *od) {
    long n, d;

    if (!safe_mul(n1, n2, &n) || !safe_mul(d1, d2, &d))
        return 0;
    return set_frac(n, d, on, od);
}

static int add_frac(long n1, long d1, long n2, long d2, long *on, long *od) {
    long a, b, den, s;

    if (!safe_mul(n1, d2, &a) || !safe_mul(n2, d1, &b) || !safe_mul(d1, d2, &den))
        return 0;
    if ((b > 0 && a > LONG_MAX - b) || (b < 0 && a < LONG_MIN - b))
        return 0;
    s = a + b;
    return set_frac(s, den, on, od);
}

static Term term_zero(void) {
    Term t;

    memset(&t, 0, sizeof t);
    t.ok = 1;
    t.ad = t.bd = t.pd = 1;
    return t;
}

static int term_zero_p(Term t) {
    return t.an == 0 && t.bn == 0 && t.pn == 0;
}

static int norm_sqrt(Term *t) {
    int r, f;
    long pull, nb;

    if (t->bn == 0) {
        t->bd = 1;
        t->rad = 0;
        return 1;
    }
    if (t->rad < 0)
        return 0;
    if (t->rad == 0) {
        t->bn = 0;
        t->bd = 1;
        return 1;
    }
    if (t->rad > 1000000)
        return 0;
    r = t->rad;
    pull = 1;
    for (f = 2; f * f <= r; f++) {
        int sq = f * f;

        while (r % sq == 0) {
            r /= sq;
            if (!safe_mul(pull, f, &pull))
                return 0;
        }
    }
    if (!safe_mul(t->bn, pull, &nb))
        return 0;
    t->bn = nb;
    t->rad = r;
    if (t->rad == 1 || t->rad == 0) {
        if (!add_frac(t->an, t->ad, t->bn, t->bd, &t->an, &t->ad))
            return 0;
        t->bn = 0;
        t->bd = 1;
        t->rad = 0;
    }
    if (t->bn == 0) {
        t->bd = 1;
        t->rad = 0;
    }
    return 1;
}

static int term_add(Term a, Term b, Term *o) {
    Term r = term_zero();

    if (!a.ok || !b.ok)
        return 0;
    if (!add_frac(a.an, a.ad, b.an, b.ad, &r.an, &r.ad))
        return 0;
    if (!add_frac(a.pn, a.pd, b.pn, b.pd, &r.pn, &r.pd))
        return 0;
    if (a.bn == 0 && b.bn == 0) {
        r.rad = 0;
    } else if (a.bn == 0) {
        r.bn = b.bn;
        r.bd = b.bd;
        r.rad = b.rad;
    } else if (b.bn == 0) {
        r.bn = a.bn;
        r.bd = a.bd;
        r.rad = a.rad;
    } else if (a.rad != b.rad) {
        return 0;
    } else if (!add_frac(a.bn, a.bd, b.bn, b.bd, &r.bn, &r.bd)) {
        return 0;
    } else {
        r.rad = a.rad;
        if (r.bn == 0) {
            r.bd = 1;
            r.rad = 0;
        }
    }
    *o = r;
    return 1;
}

static int add_sqrt_piece(Term *dst, long cn, long cd, int rad) {
    Term piece = term_zero();

    if (cn == 0 || rad <= 0)
        return 1;
    if (!set_frac(cn, cd, &piece.bn, &piece.bd))
        return 0;
    piece.rad = rad;
    if (!norm_sqrt(&piece))
        return 0;
    return term_add(*dst, piece, dst);
}

static int term_mul(Term a, Term b, Term *o) {
    Term r = term_zero();
    long n, d, n2, d2;

    if (!a.ok || !b.ok)
        return 0;
    if ((a.pn && (b.bn || b.pn)) || (b.pn && (a.bn || a.pn)))
        return 0;
    if (!mul_frac(a.an, a.ad, b.an, b.ad, &r.an, &r.ad))
        return 0;
    if (!mul_frac(a.an, a.ad, b.pn, b.pd, &n, &d))
        return 0;
    if (!mul_frac(a.pn, a.pd, b.an, b.ad, &n2, &d2))
        return 0;
    if (!add_frac(n, d, n2, d2, &r.pn, &r.pd))
        return 0;
    if (b.bn && b.rad > 0) {
        if (!mul_frac(a.an, a.ad, b.bn, b.bd, &n, &d))
            return 0;
        if (!add_sqrt_piece(&r, n, d, b.rad))
            return 0;
    }
    if (a.bn && a.rad > 0) {
        if (!mul_frac(b.an, b.ad, a.bn, a.bd, &n, &d))
            return 0;
        if (!add_sqrt_piece(&r, n, d, a.rad))
            return 0;
    }
    if (a.bn && a.rad > 0 && b.bn && b.rad > 0) {
        long radp;

        if (!mul_frac(a.bn, a.bd, b.bn, b.bd, &n, &d))
            return 0;
        if (!safe_mul(a.rad, b.rad, &radp) || radp <= 0 || radp > 1000000)
            return 0;
        if (!add_sqrt_piece(&r, n, d, (int)radp))
            return 0;
    }
    *o = r;
    return 1;
}

static int term_div(Term a, Term b, Term *o) {
    Term inv;

    if (!b.ok || term_zero_p(b))
        return 0;
    if (b.bn == 0 && b.pn == 0) {
        inv = term_zero();
        if (!set_frac(b.ad, b.an, &inv.an, &inv.ad))
            return 0;
        return term_mul(a, inv, o);
    }
    if (b.an == 0 && b.pn == 0 && b.bn != 0 && b.rad > 1) {
        long den;

        inv = term_zero();
        if (!safe_mul(b.bn, b.rad, &den))
            return 0;
        if (!set_frac(b.bd, den, &inv.bn, &inv.bd))
            return 0;
        inv.rad = b.rad;
        if (!norm_sqrt(&inv))
            return 0;
        return term_mul(a, inv, o);
    }
    if (b.an == 0 && b.bn == 0 && b.pn != 0) {
        long n, d;

        if (a.an || a.bn)
            return 0;
        if (!mul_frac(a.pn, a.pd, b.pd, b.pn, &n, &d))
            return 0;
        *o = term_zero();
        return set_frac(n, d, &o->an, &o->ad);
    }
    return 0;
}

static int terms_exact(Term a, Term b) {
    return a.ok && b.ok && a.an == b.an && a.ad == b.ad && a.bn == b.bn &&
           a.bd == b.bd && a.rad == b.rad && a.pn == b.pn && a.pd == b.pd;
}

static double term_double(Term t) {
    double v = (double)t.an / (double)t.ad + (double)t.pn / (double)t.pd * G_PI;

    if (t.bn && t.rad > 0)
        v += (double)t.bn / (double)t.bd * sqrt((double)t.rad);
    return v;
}

static int terms_close(Term a, Term b) {
    double diff;

    if (!a.ok || !b.ok)
        return 0;
    if (terms_exact(a, b))
        return 1;
    if (a.pn || b.pn)
        return 0;
    diff = fabs(term_double(a) - term_double(b));
    if (a.bn || b.bn)
        return diff <= 0.001;
    return diff <= 0.0005;
}

static int utf8_next(const char *s, int i, unsigned *cp) {
    const unsigned char *u = (const unsigned char *)s;
    unsigned c = u[i];

    if (c < 0x80) {
        *cp = c;
        return 1;
    }
    if ((c & 0xE0) == 0xC0 && u[i + 1]) {
        *cp = ((c & 0x1F) << 6) | (u[i + 1] & 0x3F);
        return 2;
    }
    if ((c & 0xF0) == 0xE0 && u[i + 1] && u[i + 2]) {
        *cp = ((c & 0x0F) << 12) | ((u[i + 1] & 0x3F) << 6) | (u[i + 2] & 0x3F);
        return 3;
    }
    *cp = c;
    return 1;
}

static char fold_char(unsigned cp) {
    switch (cp) {
    case 0xE1: case 0xC1: return 'a';
    case 0x10D: case 0x10C: return 'c';
    case 0x10F: case 0x10E: return 'd';
    case 0xE9: case 0xC9: case 0x11B: case 0x11A: return 'e';
    case 0xED: case 0xCD: return 'i';
    case 0x148: case 0x147: return 'n';
    case 0xF3: case 0xD3: return 'o';
    case 0x159: case 0x158: return 'r';
    case 0x161: case 0x160: return 's';
    case 0x165: case 0x164: return 't';
    case 0xFA: case 0xDA: case 0x16F: case 0x16E: return 'u';
    case 0xFD: case 0xDD: return 'y';
    case 0x17E: case 0x17D: return 'z';
    default:
        if (cp >= 'A' && cp <= 'Z')
            return (char)(cp - 'A' + 'a');
        if (cp >= 'a' && cp <= 'z')
            return (char)cp;
        if (cp >= '0' && cp <= '9')
            return (char)cp;
        return 0;
    }
}

static void fold_word(const char *s, char *out, int cap) {
    int i = 0, n = 0;

    while (s[i] && n + 1 < cap) {
        unsigned cp;
        int len = utf8_next(s, i, &cp);
        char f = fold_char(cp);

        if (f)
            out[n++] = f;
        i += len > 0 ? len : 1;
    }
    out[n] = 0;
}

static int word_ok(const char *alts, const char *user) {
    char folded[128];
    char buf[256];
    char *save = NULL;
    char *part;

    fold_word(user ? user : "", folded, sizeof folded);
    if (!folded[0])
        return 0;
    g_strlcpy(buf, alts ? alts : "", sizeof buf);
    for (part = strtok_r(buf, "|", &save); part; part = strtok_r(NULL, "|", &save)) {
        char one[128];

        fold_word(part, one, sizeof one);
        if (one[0] && strcmp(one, folded) == 0)
            return 1;
    }
    return 0;
}

static void strip_units(char *s) {
    static const char *units[] = {
        "procenta", "procent", "cm", "mm", "dm", "km", "m", NULL
    };

    for (;;) {
        int n = (int)strlen(s);
        int cut = 0;
        unsigned cp;
        int i;

        while (n > 0 && isspace((unsigned char)s[n - 1]))
            s[--n] = 0;
        if (n >= 2) {
            utf8_next(s + n - 2, 0, &cp);
            if (cp == 0xB0) { /* degree sign */
                s[n - 2] = 0;
                continue;
            }
        }
        if (n > 0 && s[n - 1] == '%') {
            s[n - 1] = 0;
            continue;
        }
        if (n > 0 && s[n - 1] == '.') {
            s[n - 1] = 0;
            continue;
        }
        for (i = 0; units[i]; i++) {
            int m = (int)strlen(units[i]);
            unsigned char prev;

            if (n <= m)
                continue;
            if (g_ascii_strncasecmp(s + n - m, units[i], m) != 0)
                continue;
            prev = (unsigned char)s[n - m - 1];
            if (g_ascii_isalpha(prev))
                continue;
            s[n - m] = 0;
            cut = 1;
            break;
        }
        if (!cut)
            break;
    }
}

static void trim_copy(const char *src, char *dst, int cap) {
    int n;

    while (src && isspace((unsigned char)*src))
        src++;
    g_strlcpy(dst, src ? src : "", cap);
    n = (int)strlen(dst);
    while (n > 0 && isspace((unsigned char)dst[n - 1]))
        dst[--n] = 0;
    if (n >= 2 && (dst[0] == '{' || dst[0] == '[' || dst[0] == '(') &&
        (dst[n - 1] == '}' || dst[n - 1] == ']' || dst[n - 1] == ')')) {
        memmove(dst, dst + 1, n - 2);
        dst[n - 2] = 0;
        trim_copy(dst, dst, cap);
    }
}

static int lex_number(const char *s, int i, Tok *tok) {
    int start = i;
    long whole = 0, frac = 0, den = 1;
    int neg = 0;
    int saw = 0;

    if (s[i] == '+')
        i++;
    else if (s[i] == '-') {
        neg = 1;
        i++;
    }
    while (isdigit((unsigned char)s[i])) {
        if (whole > LONG_MAX / 10)
            return -1;
        whole = whole * 10 + (s[i] - '0');
        saw = 1;
        i++;
    }
    if (s[i] == '.' || s[i] == ',') {
        i++;
        while (isdigit((unsigned char)s[i])) {
            if (frac > LONG_MAX / 10 || den > LONG_MAX / 10)
                return -1;
            frac = frac * 10 + (s[i] - '0');
            den *= 10;
            saw = 1;
            i++;
        }
    }
    if (!saw || i == start)
        return -1;
    if (!safe_mul(whole, den, &whole))
        return -1;
    if ((frac > 0 && whole > LONG_MAX - frac) || frac < 0)
        return -1;
    whole += frac;
    if (neg)
        whole = -whole;
    tok->kind = TK_NUM;
    if (!set_frac(whole, den, &tok->n, &tok->d))
        return -1;
    return i;
}

static int starts_ident(unsigned cp) {
    return (cp >= 'A' && cp <= 'Z') || (cp >= 'a' && cp <= 'z') || cp == 0x221A ||
           cp == 0x3C0;
}

static int lex(const char *s, Tok *toks, int cap) {
    int i = 0, n = 0;

    while (s[i]) {
        unsigned cp;
        int len;
        Tok tok;

        if (n + 2 >= cap)
            return -1;
        if (isspace((unsigned char)s[i])) {
            i++;
            continue;
        }
        memset(&tok, 0, sizeof tok);
        tok.d = 1;
        if (isdigit((unsigned char)s[i]) ||
            ((s[i] == '.' || s[i] == ',') && isdigit((unsigned char)s[i + 1]))) {
            int ni = lex_number(s, i, &tok);

            if (ni < 0)
                return -1;
            toks[n++] = tok;
            i = ni;
            continue;
        }
        len = utf8_next(s, i, &cp);
        if (cp == '+' || cp == '-' || cp == 0x2212 || cp == 0x2013) {
            tok.kind = cp == '+' ? TK_ADD : TK_SUB;
            toks[n++] = tok;
            i += cp < 128 ? 1 : len;
            continue;
        }
        if (cp == '*' || cp == 0xD7 || cp == 0xB7) {
            tok.kind = TK_MUL;
            toks[n++] = tok;
            i += cp < 128 ? 1 : len;
            continue;
        }
        if (cp == '/' || cp == ':') {
            tok.kind = TK_DIV;
            toks[n++] = tok;
            i++;
            continue;
        }
        if (cp == '(') {
            tok.kind = TK_LP;
            toks[n++] = tok;
            i++;
            continue;
        }
        if (cp == ')') {
            tok.kind = TK_RP;
            toks[n++] = tok;
            i++;
            continue;
        }
        if (cp == 0x221A) {
            tok.kind = TK_SQRT;
            toks[n++] = tok;
            i += len;
            continue;
        }
        if (cp == 0x3C0) {
            tok.kind = TK_PI;
            toks[n++] = tok;
            i += len;
            continue;
        }
        if (starts_ident(cp)) {
            char ident[16];
            int k = 0;

            while (s[i] && k + 1 < (int)sizeof ident) {
                unsigned c2;
                int l2 = utf8_next(s, i, &c2);

                if (!((c2 >= 'A' && c2 <= 'Z') || (c2 >= 'a' && c2 <= 'z')))
                    break;
                ident[k++] = (char)((c2 >= 'A' && c2 <= 'Z') ? c2 - 'A' + 'a' : c2);
                i += l2;
            }
            ident[k] = 0;
            if (strcmp(ident, "sqrt") == 0)
                tok.kind = TK_SQRT;
            else if (strcmp(ident, "pi") == 0)
                tok.kind = TK_PI;
            else
                return -1;
            toks[n++] = tok;
            continue;
        }
        return -1;
    }
    toks[n].kind = TK_END;
    toks[n].d = 1;
    return n;
}

typedef struct {
    const Tok *t;
    int i;
    int n;
    int bad;
} Parse;

static Term parse_expr(Parse *p);

static int at_primary(const Tok *t) {
    return t->kind == TK_NUM || t->kind == TK_SQRT || t->kind == TK_PI ||
           t->kind == TK_LP;
}

static Term parse_primary(Parse *p) {
    Term r = term_zero();
    Tok tok;

    if (p->bad || p->i >= p->n) {
        p->bad = 1;
        r.ok = 0;
        return r;
    }
    tok = p->t[p->i];
        if (tok.kind == TK_NUM) {
        /* Mixed number: 1 1/2 means 1 + 1/2. A leading minus is unary. */
        if (p->i + 3 < p->n && p->t[p->i + 1].kind == TK_NUM &&
            p->t[p->i + 2].kind == TK_DIV && p->t[p->i + 3].kind == TK_NUM &&
            p->t[p->i + 1].d == 1 && p->t[p->i + 3].d == 1 && tok.d == 1) {
            Term whole = term_zero();
            Term frac = term_zero();

            whole.an = tok.n;
            whole.ad = tok.d;
            if (!set_frac(p->t[p->i + 1].n, p->t[p->i + 3].n, &frac.an, &frac.ad)) {
                p->bad = 1;
                r.ok = 0;
                return r;
            }
            if (!term_add(whole, frac, &r)) {
                p->bad = 1;
                r.ok = 0;
                return r;
            }
            p->i += 4;
            return r;
        }
        r.an = tok.n;
        r.ad = tok.d;
        p->i++;
        return r;
    }
    if (tok.kind == TK_PI) {
        r.pn = 1;
        r.pd = 1;
        p->i++;
        return r;
    }
    if (tok.kind == TK_SQRT) {
        Term inner;

        p->i++;
        /* √2 and sqrt2 are the root of the following whole number. */
        if (p->i < p->n && p->t[p->i].kind == TK_NUM && p->t[p->i].d == 1 &&
            p->t[p->i].n >= 0) {
            r.bn = 1;
            r.bd = 1;
            r.rad = (int)p->t[p->i].n;
            p->i++;
            if (!norm_sqrt(&r)) {
                p->bad = 1;
                r.ok = 0;
            }
            return r;
        }
        if (p->i >= p->n || p->t[p->i].kind != TK_LP) {
            p->bad = 1;
            r.ok = 0;
            return r;
        }
        p->i++;
        inner = parse_expr(p);
        if (p->bad || p->i >= p->n || p->t[p->i].kind != TK_RP || !inner.ok ||
            inner.bn || inner.pn || inner.an < 0) {
            p->bad = 1;
            r.ok = 0;
            return r;
        }
        p->i++;
        if (inner.ad != 1) {
            /* sqrt of a fraction is not asked; reject rather than guess. */
            p->bad = 1;
            r.ok = 0;
            return r;
        }
        r.bn = 1;
        r.bd = 1;
        r.rad = (int)inner.an;
        if (!norm_sqrt(&r)) {
            p->bad = 1;
            r.ok = 0;
        }
        return r;
    }
    if (tok.kind == TK_LP) {
        p->i++;
        r = parse_expr(p);
        if (p->bad || p->i >= p->n || p->t[p->i].kind != TK_RP) {
            p->bad = 1;
            r.ok = 0;
            return r;
        }
        p->i++;
        return r;
    }
    p->bad = 1;
    r.ok = 0;
    return r;
}

static Term parse_unary(Parse *p) {
    if (p->i < p->n && (p->t[p->i].kind == TK_ADD || p->t[p->i].kind == TK_SUB)) {
        int neg = p->t[p->i].kind == TK_SUB;
        Term r;

        p->i++;
        r = parse_unary(p);
        if (!r.ok)
            return r;
        if (neg) {
            r.an = -r.an;
            r.bn = -r.bn;
            r.pn = -r.pn;
        }
        return r;
    }
    return parse_primary(p);
}

static Term parse_term(Parse *p) {
    Term left = parse_unary(p);

    if (!left.ok)
        return left;
    while (!p->bad && p->i < p->n) {
        TokKind k = p->t[p->i].kind;
        Term right;
        int juxt = 0;

        if (k != TK_MUL && k != TK_DIV) {
            if (at_primary(&p->t[p->i]) &&
                (p->t[p->i].kind == TK_SQRT || p->t[p->i].kind == TK_PI ||
                 p->t[p->i].kind == TK_LP))
                juxt = 1;
            else
                break;
        }
        if (!juxt)
            p->i++;
        right = parse_unary(p);
        if (!right.ok) {
            left.ok = 0;
            return left;
        }
        if (k == TK_DIV && !juxt) {
            if (!term_div(left, right, &left)) {
                p->bad = 1;
                left.ok = 0;
                return left;
            }
        } else if (!term_mul(left, right, &left)) {
            p->bad = 1;
            left.ok = 0;
            return left;
        }
    }
    return left;
}

static Term parse_expr(Parse *p) {
    Term left = parse_term(p);

    while (!p->bad && left.ok && p->i < p->n &&
           (p->t[p->i].kind == TK_ADD || p->t[p->i].kind == TK_SUB)) {
        int sub = p->t[p->i].kind == TK_SUB;
        Term right;

        p->i++;
        right = parse_term(p);
        if (!right.ok) {
            left.ok = 0;
            return left;
        }
        if (sub) {
            right.an = -right.an;
            right.bn = -right.bn;
            right.pn = -right.pn;
        }
        if (!term_add(left, right, &left)) {
            p->bad = 1;
            left.ok = 0;
            return left;
        }
    }
    return left;
}

static int parse_term_text(const char *text, Term *out) {
    char buf[MC_PART];
    Tok toks[MC_TOKS];
    Parse p;
    int n;

    trim_copy(text, buf, sizeof buf);
    strip_units(buf);
    trim_copy(buf, buf, sizeof buf);
    if (!buf[0])
        return 0;
    n = lex(buf, toks, MC_TOKS);
    if (n < 0)
        return 0;
    p.t = toks;
    p.i = 0;
    p.n = n;
    p.bad = 0;
    *out = parse_expr(&p);
    if (p.bad || !out->ok || p.i != n)
        return 0;
    return 1;
}

static int split_parts(const char *text, char parts[][MC_PART], int max) {
    char buf[512];
    int n = 0;
    const char *s;
    int use_semi;

    trim_copy(text, buf, sizeof buf);
    if (!buf[0])
        return -1;
    use_semi = strchr(buf, ';') != NULL;
    s = buf;
    while (*s && n < max) {
        const char *end = s;
        int len;

        if (use_semi) {
            end = strchr(s, ';');
            if (!end)
                end = s + strlen(s);
        } else {
            end = strstr(s, ", ");
            if (!end)
                end = s + strlen(s);
        }
        len = (int)(end - s);
        if (len >= MC_PART)
            return -1;
        memcpy(parts[n], s, len);
        parts[n][len] = 0;
        trim_copy(parts[n], parts[n], MC_PART);
        if (!parts[n][0])
            return -1;
        n++;
        s = end;
        if (*s == 0)
            break;
        s += use_semi ? 1 : 2;
    }
    if (*s)
        return -1;
    return n;
}

static int num_ok(const char *spec, const char *user) {
    Term a, b;

    if (!parse_term_text(spec, &a) || !parse_term_text(user, &b))
        return 0;
    return terms_close(a, b);
}

static int list_ok(const char *spec, const char *user, int ordered) {
    char sp[MC_PARTS][MC_PART];
    char up[MC_PARTS][MC_PART];
    Term st[MC_PARTS], ut[MC_PARTS];
    int ns, nu, i, j;
    int used[MC_PARTS];

    ns = split_parts(spec, sp, MC_PARTS);
    nu = split_parts(user, up, MC_PARTS);
    if (ns < 1 || ns != nu)
        return 0;
    for (i = 0; i < ns; i++) {
        if (!parse_term_text(sp[i], &st[i]) || !parse_term_text(up[i], &ut[i]))
            return 0;
        used[i] = 0;
    }
    if (ordered) {
        for (i = 0; i < ns; i++)
            if (!terms_close(st[i], ut[i]))
                return 0;
        return 1;
    }
    for (i = 0; i < ns; i++) {
        int hit = -1;

        for (j = 0; j < nu; j++) {
            if (!used[j] && terms_exact(st[i], ut[j])) {
                hit = j;
                break;
            }
        }
        if (hit < 0) {
            for (j = 0; j < nu; j++) {
                if (!used[j] && terms_close(st[i], ut[j])) {
                    hit = j;
                    break;
                }
            }
        }
        if (hit < 0)
            return 0;
        used[hit] = 1;
    }
    return 1;
}

gboolean math_answer_ok(const char *spec, const char *user) {
    if (!spec || !user)
        return FALSE;
    while (*spec && isspace((unsigned char)*spec))
        spec++;
    if (g_str_has_prefix(spec, "word:"))
        return word_ok(spec + 5, user);
    if (g_str_has_prefix(spec, "set:"))
        return list_ok(spec + 4, user, 0);
    if (g_str_has_prefix(spec, "pair:"))
        return list_ok(spec + 5, user, 1);
    if (g_str_has_prefix(spec, "num:"))
        return num_ok(spec + 4, user);
    return FALSE;
}

static int on_board(int x, int y) {
    return x >= 0 && x <= 10 && y >= 0 && y <= 8;
}

static int parse_xy(const char **ps, int *x, int *y) {
    char *end;
    long vx, vy;
    const char *s = *ps;

    while (*s && (isspace((unsigned char)*s) || *s == ';'))
        s++;
    if (!*s)
        return 0;
    vx = strtol(s, &end, 10);
    if (end == s || *end != ',')
        return 0;
    s = end + 1;
    vy = strtol(s, &end, 10);
    if (end == s)
        return 0;
    if (vx < -100 || vx > 100 || vy < -100 || vy > 100)
        return 0;
    *x = (int)vx;
    *y = (int)vy;
    *ps = end;
    return 1;
}

static const char *draw_body(const char *spec, const char *kind) {
    int n = (int)strlen(kind);

    if (!spec || strncmp(spec, kind, n) != 0 || spec[n] != ':')
        return NULL;
    return spec + n + 1;
}

gboolean math_is_draw(const char *spec) {
    return draw_body(spec, "square") || draw_body(spec, "mid") ||
           draw_body(spec, "right") || draw_body(spec, "para") ||
           draw_body(spec, "points");
}

static int points_scan(const char *body, int *fx, int *fy, int *fn,
                       int *rx, int *ry, int *rn, int cap) {
    const char *s = body;

    *fn = 0;
    *rn = 0;
    while (*s) {
        int fix = 0;
        int x, y;

        while (*s && (isspace((unsigned char)*s) || *s == ';'))
            s++;
        if (!*s)
            break;
        if (g_str_has_prefix(s, "fix:")) {
            fix = 1;
            s += 4;
        }
        if (!parse_xy(&s, &x, &y))
            return 0;
        if (fix) {
            if (*fn >= cap)
                return 0;
            fx[*fn] = x;
            fy[*fn] = y;
            (*fn)++;
        } else {
            if (*rn >= cap)
                return 0;
            rx[*rn] = x;
            ry[*rn] = y;
            (*rn)++;
        }
    }
    return 1;
}

int math_draw_need(const char *spec) {
    int fx[8], fy[8], rx[8], ry[8], fn = 0, rn = 0;

    if (draw_body(spec, "square"))
        return 2;
    if (draw_body(spec, "mid") || draw_body(spec, "right") || draw_body(spec, "para"))
        return 1;
    if (draw_body(spec, "points")) {
        if (!points_scan(draw_body(spec, "points"), fx, fy, &fn, rx, ry, &rn, 8))
            return -1;
        return rn;
    }
    return -1;
}

int math_draw_fixed(const char *spec, int *xs, int *ys, int cap) {
    const char *body;
    int n = 0;
    int x, y;

    if (!xs || !ys || cap <= 0)
        return 0;
    if ((body = draw_body(spec, "square")) || (body = draw_body(spec, "mid")) ||
        (body = draw_body(spec, "right")) || (body = draw_body(spec, "para"))) {
        const char *s = body;

        while (*s && n < cap) {
            while (*s && (isspace((unsigned char)*s) || *s == ';'))
                s++;
            if (!*s)
                break;
            if (!parse_xy(&s, &x, &y))
                return 0;
            xs[n] = x;
            ys[n] = y;
            n++;
        }
        return n;
    }
    if ((body = draw_body(spec, "points"))) {
        int fx[8], fy[8], rx[8], ry[8], fn = 0, rn = 0, i;

        if (!points_scan(body, fx, fy, &fn, rx, ry, &rn, 8))
            return 0;
        for (i = 0; i < fn && i < cap; i++) {
            xs[i] = fx[i];
            ys[i] = fy[i];
        }
        return fn < cap ? fn : cap;
    }
    return 0;
}

static int same_pair(int ax, int ay, int bx, int by, int cx, int cy, int dx, int dy) {
    return (ax == cx && ay == cy && bx == dx && by == dy) ||
           (ax == dx && ay == dy && bx == cx && by == cy);
}

static int user_ok_board(const int *xs, const int *ys, int n) {
    int i, j;

    for (i = 0; i < n; i++) {
        if (!on_board(xs[i], ys[i]))
            return 0;
        for (j = i + 1; j < n; j++)
            if (xs[i] == xs[j] && ys[i] == ys[j])
                return 0;
    }
    return 1;
}

gboolean math_draw_ok(const char *spec, const int *xs, const int *ys, int n) {
    const char *body;
    int ax, ay, bx, by, cx, cy;
    const char *s;

    if (!spec || n < 0 || (n > 0 && (!xs || !ys)))
        return FALSE;
    if (!user_ok_board(xs, ys, n))
        return FALSE;
    if ((body = draw_body(spec, "square"))) {
        int dx, dy;
        int p1x, p1y, q1x, q1y, p2x, p2y, q2x, q2y;

        s = body;
        if (n != 2 || !parse_xy(&s, &ax, &ay) || !parse_xy(&s, &bx, &by))
            return FALSE;
        dx = bx - ax;
        dy = by - ay;
        if (dx == 0 && dy == 0)
            return FALSE;
        p1x = bx - dy;
        p1y = by + dx;
        q1x = ax - dy;
        q1y = ay + dx;
        p2x = bx + dy;
        p2y = by - dx;
        q2x = ax + dy;
        q2y = ay - dx;
        if (on_board(p1x, p1y) && on_board(q1x, q1y) &&
            same_pair(xs[0], ys[0], xs[1], ys[1], p1x, p1y, q1x, q1y))
            return TRUE;
        if (on_board(p2x, p2y) && on_board(q2x, q2y) &&
            same_pair(xs[0], ys[0], xs[1], ys[1], p2x, p2y, q2x, q2y))
            return TRUE;
        return FALSE;
    }
    if ((body = draw_body(spec, "mid"))) {
        s = body;
        if (n != 1 || !parse_xy(&s, &ax, &ay) || !parse_xy(&s, &bx, &by))
            return FALSE;
        if (((ax + bx) & 1) || ((ay + by) & 1))
            return FALSE;
        return xs[0] == (ax + bx) / 2 && ys[0] == (ay + by) / 2 && on_board(xs[0], ys[0]);
    }
    if ((body = draw_body(spec, "right"))) {
        long v1x, v1y, v2x, v2y;

        s = body;
        if (n != 1 || !parse_xy(&s, &ax, &ay) || !parse_xy(&s, &bx, &by))
            return FALSE;
        cx = xs[0];
        cy = ys[0];
        if ((cx == ax && cy == ay) || (cx == bx && cy == by))
            return FALSE;
        v1x = ax - cx;
        v1y = ay - cy;
        v2x = bx - cx;
        v2y = by - cy;
        return v1x * v2x + v1y * v2y == 0;
    }
    if ((body = draw_body(spec, "para"))) {
        int dx, dy;

        s = body;
        if (n != 1 || !parse_xy(&s, &ax, &ay) || !parse_xy(&s, &bx, &by) ||
            !parse_xy(&s, &cx, &cy))
            return FALSE;
        dx = bx + cx - ax;
        dy = by + cy - ay;
        return xs[0] == dx && ys[0] == dy && on_board(dx, dy);
    }
    if ((body = draw_body(spec, "points"))) {
        int fx[8], fy[8], rx[8], ry[8], fn = 0, rn = 0, i, j;
        int used[8];

        if (!points_scan(body, fx, fy, &fn, rx, ry, &rn, 8) || n != rn)
            return FALSE;
        for (i = 0; i < n; i++)
            used[i] = 0;
        for (i = 0; i < rn; i++) {
            int hit = 0;

            for (j = 0; j < n; j++) {
                if (!used[j] && xs[j] == rx[i] && ys[j] == ry[i]) {
                    used[j] = 1;
                    hit = 1;
                    break;
                }
            }
            if (!hit)
                return FALSE;
        }
        return TRUE;
    }
    return FALSE;
}

#ifdef MATH_CHECK_TEST
static int fails;

static void expect(const char *spec, const char *user, int want) {
    int got = math_answer_ok(spec, user);

    if (got != want) {
        fprintf(stderr, "FAIL %s vs %s got %d want %d\n", spec, user, got, want);
        fails++;
    }
}

static void expect_draw(const char *spec, const int *xs, const int *ys, int n, int want) {
    int got = math_draw_ok(spec, xs, ys, n);

    if (got != want) {
        fprintf(stderr, "FAIL draw %s got %d want %d\n", spec, got, want);
        fails++;
    }
}

static int run_file(const char *path) {
    FILE *f = fopen(path, "r");
    char line[512];

    if (!f) {
        perror(path);
        return 1;
    }
    while (fgets(line, sizeof line, f)) {
        char *spec, *user, *flag, *nl;
        int want;

        if (line[0] == '#' || line[0] == '\n')
            continue;
        nl = strchr(line, '\n');
        if (nl)
            *nl = 0;
        spec = line;
        user = strchr(spec, '\t');
        if (!user)
            continue;
        *user++ = 0;
        flag = strchr(user, '\t');
        if (!flag)
            continue;
        *flag++ = 0;
        want = atoi(flag);
        if (g_str_has_prefix(spec, "draw ")) {
            int xs[8], ys[8], n = 0;
            const char *s = user;

            while (*s && n < 8) {
                while (*s == ' ' || *s == ';')
                    s++;
                if (!*s)
                    break;
                if (!parse_xy(&s, &xs[n], &ys[n])) {
                    fprintf(stderr, "bad draw user %s\n", user);
                    fails++;
                    n = -1;
                    break;
                }
                n++;
            }
            if (n >= 0)
                expect_draw(spec + 5, xs, ys, n, want);
        } else {
            expect(spec, user, want);
        }
    }
    fclose(f);
    return fails ? 1 : 0;
}

int main(int argc, char **argv) {
    const int sq_hi[] = {5, 2};
    const int sq_hi_y[] = {6, 6};
    const int sq_lo[] = {5, 2};
    const int sq_lo_y[] = {0, 0};
    const int badpt[] = {5, 2};
    const int badpt_y[] = {6, 5};
    const int midp[] = {4};
    const int midy[] = {3};
    const int th1[] = {5};
    const int th1y[] = {7};
    const int th2[] = {5};
    const int th2y[] = {1};
    const int thbad[] = {5};
    const int thbady[] = {4};
    const int par[] = {5};
    const int pary[] = {3};
    const int pt[] = {1};
    const int pty[] = {4};
    const int ptoff[] = {1};
    const int ptoffy[] = {-2};

    expect("num:5/6", "5/6", 1);
    expect("num:5/6", "0,833", 1);
    expect("num:5/6", "0.833", 1);
    expect("num:5/6", "0,83", 0);
    expect("num:-3/4", "-3/4", 1);
    expect("num:-3/4", "-(3/4)", 1);
    expect("num:3/4", "0,75", 1);
    expect("num:3/4", "0.75", 1);
    expect("num:14*pi", "14pi", 1);
    expect("num:14*pi", "14*pi", 1);
    expect("num:14*pi", "14π", 1);
    expect("num:14*pi", "43,98", 0);
    expect("num:14*pi", "44", 0);
    expect("num:pi", "3,14", 0);
    expect("num:pi", "22/7", 0);
    expect("num:pi", "3", 0);
    expect("num:2*sqrt(3)", "2√3", 1);
    expect("num:2*sqrt(3)", "sqrt(12)", 1);
    expect("num:sqrt(12)", "2*sqrt(3)", 1);
    expect("num:3/2", "1 1/2", 1);
    expect("num:sqrt(2)/2", "√2/2", 1);
    expect("num:sqrt(2)/2", "1/sqrt(2)", 1);
    expect("num:sqrt(2)", "1,414", 1);
    expect("num:sqrt(2)", "1.414", 1);
    expect("num:sqrt(2)", "1,41", 0);
    expect("num:1/3", "0,333", 1);
    expect("num:1/3", "0,33", 0);
    expect("num:12", "12 cm", 1);
    expect("num:25", "25%", 1);
    expect("num:25", "25 %", 1);
    expect("num:50", "50 m", 1);
    expect("num:70", "70°", 1);
    expect("num:0", "0", 1);
    expect("num:14", "2+3*4", 1);
    expect("num:8/3", "8/3", 1);
    expect("num:8/3", "2,667", 1);
    expect("num:8/3", "2,66", 0);
    expect("num:-1", "-1", 1);
    expect("set:2;3", "3;2", 1);
    expect("set:2;3", "2; 3", 1);
    expect("set:-2;2", "2;-2", 1);
    expect("set:2;3", "2;4", 0);
    expect("pair:6;4", "6;4", 1);
    expect("pair:6;4", "6; 4", 1);
    expect("pair:6;4", "4;6", 0);
    expect("pair:1;-2", "1;-2", 1);
    expect("word:pravouhly", "pravoúhlý", 1);
    expect("word:ano|ne", "Ano", 1);
    expect("word:ano|ne", "ne", 1);
    expect("word:ano", "ne", 0);
    expect("num:5/6", "", 0);
    expect("num:5/6", "abc", 0);

    expect_draw("square:2,3;5,3", sq_hi, sq_hi_y, 2, 1);
    expect_draw("square:2,3;5,3", sq_lo, sq_lo_y, 2, 1);
    expect_draw("square:2,3;5,3", badpt, badpt_y, 2, 0);
    expect_draw("mid:1,2;7,4", midp, midy, 1, 1);
    expect_draw("right:2,4;8,4", th1, th1y, 1, 1);
    expect_draw("right:2,4;8,4", th2, th2y, 1, 1);
    expect_draw("right:2,4;8,4", thbad, thbady, 1, 0);
    expect_draw("para:1,1;4,1;2,3", par, pary, 1, 1);
    expect_draw("points:1,4;fix:1,1;fix:5,1", pt, pty, 1, 1);
    expect_draw("points:1,4;fix:1,1;fix:5,1", ptoff, ptoffy, 1, 0);
    if (math_draw_need("square:2,3;5,3") != 2 || math_draw_need("right:2,4;8,4") != 1)
        fails++;

    if (argc > 1)
        return run_file(argv[1]) || fails;
    if (fails) {
        fprintf(stderr, "%d failed\n", fails);
        return 1;
    }
    printf("math_check ok\n");
    return 0;
}
#endif
