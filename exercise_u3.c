#include "maturita.h"

/* ------------------------------------------------------------------ */
/* Unit 3 exercises ("Bei uns zu Hause")                              */
/* ------------------------------------------------------------------ */

/* Sentence fill-ins: a row is made of literal text segments that
 * alternate with one or more blank widgets (dropdowns). `num` is the
 * optional item number shown at the start of the line. */

/* Build the complete German sentence of a fill row (segments spliced
 * with the correct answers) – used as the model shown after Check. */
char *u3_fill_sentence(const U3Fill *f) {
    const char *segs[4];
    const char *ans[3];
    GString *s;
    int i, g;

    segs[0] = f->s0;
    segs[1] = f->s1;
    segs[2] = f->s2;
    segs[3] = f->s3;
    ans[0] = f->a0;
    ans[1] = f->a1;
    ans[2] = f->a2;
    g = 0;
    while (g < 3 && ans[g])
        g++;

    s = g_string_new(NULL);
    for (i = 0; i <= g; i++) {
        if (segs[i] && segs[i][0])
            g_string_append(s, segs[i]);
        if (i < g && ans[i])
            g_string_append(s, ans[i]);
    }
    if (s->len == 0) {
        g_string_free(s, TRUE);
        return NULL;
    }
    return g_string_free(s, FALSE);
}

/* Append one horizontal fill row to `body`. The answer of blank i sits
 * between text segments s[i] and s[i+1]; empty segments are skipped.
 * `gloss` (may be NULL) is an i18n key for a small grey note shown at
 * the end of the line, e.g. the meaning of the expected possessive. */
void u3_fill_row(ComboListCtx *ctx, GtkWidget *body, const U3Fill *f,
                        const char **pool, int pool_n, const char *gloss) {
    const char *segs[4];
    const char *ans[3];
    int i, g;
    GtkWidget *line;

    segs[0] = f->s0;
    segs[1] = f->s1;
    segs[2] = f->s2;
    segs[3] = f->s3;
    ans[0] = f->a0;
    ans[1] = f->a1;
    ans[2] = f->a2;
    g = 0;
    while (g < 3 && ans[g])
        g++;

    line = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
    gtk_widget_set_halign(line, GTK_ALIGN_START);
    gtk_box_append(GTK_BOX(body), line);

    if (f->num && f->num[0]) {
        GtkWidget *nl = gtk_label_new(f->num);
        gtk_widget_set_valign(nl, GTK_ALIGN_CENTER);
        gtk_widget_add_css_class(nl, "ex-prompt");
        gtk_box_append(GTK_BOX(line), nl);
    }
    for (i = 0; i <= g; i++) {
        if (segs[i] && segs[i][0]) {
            GtkWidget *lb = gtk_label_new(segs[i]);
            gtk_widget_set_valign(lb, GTK_ALIGN_CENTER);
            gtk_widget_add_css_class(lb, "ex-prompt");
            gtk_box_append(GTK_BOX(line), lb);
        }
        if (i < g) {
            GtkWidget *combo = make_word_combo(pool, pool_n);
            gtk_widget_set_size_request(combo, 96, -1);
            gtk_widget_set_valign(combo, GTK_ALIGN_CENTER);
            gtk_box_append(GTK_BOX(line), combo);
            combo_list_add(ctx, combo, ans[i]);
        }
    }

    if (gloss && gloss[0]) {
        GtkWidget *gl = gtk_label_new(NULL);
        i18n_bind(gl, gloss, 0);
        gtk_widget_set_valign(gl, GTK_ALIGN_CENTER);
        gtk_widget_set_margin_start(gl, 8);
        gtk_widget_add_css_class(gl, "meaning");
        gtk_box_append(GTK_BOX(line), gl);
    }

    if (f->mean && f->mean[0]) {
        GtkWidget *m = meaning_add(body, f->mean);
        combo_list_add_trans(ctx, m);
        if (ctx->trans_upfront)
            gtk_widget_set_visible(m, TRUE);
    }

    if (ctx->reveal_german) {
        char *sentence = u3_fill_sentence(f);

        if (sentence && sentence[0]) {
            GtkWidget *w = model_answer_add(body, sentence);

            g_array_append_val(ctx->models, w);
        }
        g_free(sentence);
    }
}

void u3_sample_line(GtkWidget *body, const char *text) {
    GtkWidget *sl = gtk_label_new(text);

    gtk_widget_set_halign(sl, GTK_ALIGN_START);
    gtk_widget_set_margin_top(sl, 2);
    gtk_widget_set_margin_bottom(sl, 4);
    gtk_widget_add_css_class(sl, "hint");
    gtk_label_set_wrap(GTK_LABEL(sl), TRUE);
    gtk_box_append(GTK_BOX(body), sl);
}

GtkWidget *u3_build_drop(UnitCtx *unit, int ex_num, const char *title,
                                const char *sub, const char *sample,
                                const U3Fill *rows, int n,
                                const char **pool, int pool_n,
                                const char **glosses, gboolean show_german) {
    GtkWidget *body, *feedback, *check;
    GtkWidget *page = ex_page_shell(unit->page, title, sub, "check",
                                    &body, &feedback, &check);
    ComboListCtx *ctx = combo_list_ctx_new(unit, ex_num, feedback);

    ctx->trans_upfront = show_german;
    ctx->reveal_german = show_german;

    if (sample && sample[0])
        u3_sample_line(body, sample);

    for (int i = 0; i < n; i++)
        u3_fill_row(ctx, body, &rows[i], pool, pool_n,
                    glosses ? glosses[i] : NULL);

    g_signal_connect(check, "clicked", G_CALLBACK(combo_list_check), ctx);
    return page;
}

/* ---- ex1: Familienpaare (match the pair) ---------------------------- */

const char *ex1_u3_pool[] = {
    "die Cousine", "die Tante", "die Oma", "der Bruder",
    "die Freundin", "der Sohn",
};

const U3Fill ex1_u3_rows[] = {
    {"1.", "der Cousin", "die Cousine", NULL, NULL, NULL, NULL, NULL,
     "bratranec / sestřenice"},
    {"2.", "der Onkel", "die Tante", NULL, NULL, NULL, NULL, NULL,
     "strýc / teta"},
    {"3.", "der Opa", "die Oma", NULL, NULL, NULL, NULL, NULL,
     "dědeček / babička"},
    {"4.", "die Schwester", "der Bruder", NULL, NULL, NULL, NULL, NULL,
     "sestra / bratr"},
    {"5.", "der Freund", "die Freundin", NULL, NULL, NULL, NULL, NULL,
     "kamarád / kamarádka"},
    {"6.", "die Tochter", "der Sohn", NULL, NULL, NULL, NULL, NULL,
     "dcera / syn"},
};

GtkWidget *u3ex1(UnitCtx *unit) {
    return u3_build_drop(unit, 1, "Familienpaare", "sub3_pair",
                         "Beispiel: der Vater ↔ die Mutter",
                         ex1_u3_rows, G_N_ELEMENTS(ex1_u3_rows),
                         ex1_u3_pool, G_N_ELEMENTS(ex1_u3_pool), NULL, FALSE);
}

/* ---- ex2: Possessivpronomen im Nominativ ----------------------------- */

const char *ex2_u3_pool[] = {
    "dein", "deine", "mein", "meine", "sein", "seine",
};

/* The Czech sentence is shown straight away, and pressing "Check" only
 * adds the translations of the words the student picked in the blanks. */


const Ex2Row ex2_u3_rows2[] = {
    {"2.", {"Ist das ", " Mutter? – Nein, das ist nicht ", " Mutter.", NULL},
     {"deine", "meine", NULL}, 2},
    {NULL, {"Das ist ", " Tante.", NULL, NULL}, {"seine", NULL, NULL}, 1},
};

const Ex2Row ex2_u3_rows3[] = {
    {"3.", {"Ist das ", " Bruder? – Nein, das ist nicht ", " Bruder.", NULL},
     {"dein", "mein", NULL}, 2},
    {NULL, {"Das ist ", " Cousin.", NULL, NULL}, {"sein", NULL, NULL}, 1},
};

const Ex2Row ex2_u3_rows4[] = {
    {"4.",
     {"Sind das ", " Eltern? – Nein, das sind nicht ",
      " Eltern. Das sind die Eltern von Markus.", NULL},
     {"deine", "meine", NULL}, 2},
};

const Ex2Item ex2_u3_items[] = {
    {"Je to tvoje matka? – Ne, to není moje matka. To je jeho teta.",
     ex2_u3_rows2, G_N_ELEMENTS(ex2_u3_rows2)},
    {"Je to tvůj bratr? – Ne, to není můj bratr. To je jeho bratranec.",
     ex2_u3_rows3, G_N_ELEMENTS(ex2_u3_rows3)},
    {"Jsou to tvoji rodiče? – Ne, to nejsou moji rodiče. To jsou rodiče "
     "Markuse.",
     ex2_u3_rows4, G_N_ELEMENTS(ex2_u3_rows4)},
};


const char *ex2_word_cs(const char *w) {
    if (!w)
        return NULL;
    if (g_strcmp0(w, "dein") == 0)  return "tvůj";
    if (g_strcmp0(w, "deine") == 0) return "tvoje";
    if (g_strcmp0(w, "mein") == 0)  return "můj";
    if (g_strcmp0(w, "meine") == 0) return "moje";
    if (g_strcmp0(w, "sein") == 0)  return "jeho";
    if (g_strcmp0(w, "seine") == 0) return "jeho";
    return NULL;
}

void ex2_check(GtkButton *button, gpointer data) {
    Ex2Ctx *ctx = data;
    guint total = ctx->combos->len;
    guint ok = 0;

    (void)button;

    for (guint i = 0; i < total; i++) {
        GtkComboBox *combo = g_array_index(ctx->combos, GtkComboBox *, i);
        const char *ans = g_array_index(ctx->answers, const char *, i);
        gchar *sel = gtk_combo_box_text_get_active_text(
            GTK_COMBO_BOX_TEXT(combo));
        gboolean good = FALSE;
        if (sel) {
            gchar *t = normalize_answer(sel);
            good = answer_accepts(t, ans);
            g_free(t);
            g_free(sel);
        }
        answer_mark(GTK_WIDGET(combo), good);
        if (good)
            ok++;
    }

    for (int it = 0; it < ctx->n_items; it++) {
        GArray *item = ctx->item_combos[it];
        GtkWidget *note = ctx->notes[it];
        GString *s = g_string_new(NULL);
        for (guint k = 0; k < item->len; k++) {
            GtkComboBox *combo = g_array_index(item, GtkComboBox *, k);
            gchar *sel = gtk_combo_box_text_get_active_text(
                GTK_COMBO_BOX_TEXT(combo));
            const char *cs;
            if (sel) {
                cs = ex2_word_cs(sel);
                if (cs) {
                    if (s->len > 0)
                        g_string_append(s, " · ");
                    g_string_append_printf(s, "%s = %s", sel, tr(cs));
                }
                g_free(sel);
            }
        }
        gtk_label_set_text(GTK_LABEL(note), s->str);
        g_string_free(s, TRUE);
        gtk_widget_set_visible(note, TRUE);
    }

    if (ok == total) {
        set_feedback(ctx->feedback, TRUE, tr("feedback_ok"));
        mark_done(ctx->unit, ctx->ex_num);
    } else {
        set_feedback(ctx->feedback, FALSE, tr("feedback_retry"));
    }
}

GtkWidget *u3ex2(UnitCtx *unit) {
    GtkWidget *body, *feedback, *check;
    GtkWidget *page = ex_page_shell(unit->page, "mein oder dein", "sub3_poss",
                                    "check", &body, &feedback, &check);
    Ex2Ctx *ctx = g_new0(Ex2Ctx, 1);

    ctx->combos = g_array_new(FALSE, FALSE, sizeof(GtkComboBox *));
    ctx->answers = g_array_new(FALSE, FALSE, sizeof(const char *));
    ctx->n_items = G_N_ELEMENTS(ex2_u3_items);
    ctx->notes = g_new0(GtkWidget *, ctx->n_items);
    ctx->item_combos = g_new0(GArray *, ctx->n_items);
    ctx->unit = unit;
    ctx->ex_num = 2;
    ctx->feedback = feedback;

    u3_sample_line(body, "Beispiel: Ist das dein Vater? – Nein, das ist "
                         "nicht mein Vater. Das ist sein Onkel.");

    for (int it = 0; it < ctx->n_items; it++) {
        const Ex2Item *item = &ex2_u3_items[it];
        GtkWidget *mean;
        GtkWidget *note;
        GArray *item_combos = g_array_new(FALSE, FALSE,
                                          sizeof(GtkComboBox *));
        ctx->item_combos[it] = item_combos;

        for (int r = 0; r < item->nrows; r++) {
            const Ex2Row *row = &item->rows[r];
            GtkWidget *line = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);

            gtk_widget_set_halign(line, GTK_ALIGN_START);
            if (row->num && row->num[0]) {
                GtkWidget *nl = gtk_label_new(row->num);
                gtk_widget_set_valign(nl, GTK_ALIGN_CENTER);
                gtk_widget_add_css_class(nl, "ex-prompt");
                gtk_box_append(GTK_BOX(line), nl);
            }
            for (int i = 0; i <= row->gaps; i++) {
                if (row->seg[i] && row->seg[i][0]) {
                    GtkWidget *lb = gtk_label_new(row->seg[i]);
                    gtk_widget_set_valign(lb, GTK_ALIGN_CENTER);
                    gtk_widget_add_css_class(lb, "ex-prompt");
                    gtk_box_append(GTK_BOX(line), lb);
                }
                if (i < row->gaps) {
                    GtkWidget *combo = make_word_combo(ex2_u3_pool,
                                       G_N_ELEMENTS(ex2_u3_pool));
                    gtk_widget_set_size_request(combo, 96, -1);
                    gtk_widget_set_valign(combo, GTK_ALIGN_CENTER);
                    gtk_box_append(GTK_BOX(line), combo);
                    g_array_append_val(ctx->combos, combo);
                    g_array_append_val(item_combos, combo);
                    g_array_append_val(ctx->answers, row->ans[i]);
                }
            }
            gtk_box_append(GTK_BOX(body), line);
        }

        /* Czech sentence is available before checking. */
        mean = meaning_add(body, item->czech);
        gtk_widget_set_visible(mean, TRUE);

        /* word translations appear only after pressing "Check". */
        note = gtk_label_new(NULL);
        gtk_widget_set_halign(note, GTK_ALIGN_START);
        gtk_widget_set_margin_bottom(note, 2);
        gtk_widget_add_css_class(note, "meaning");
        gtk_label_set_wrap(GTK_LABEL(note), TRUE);
        gtk_widget_set_visible(note, FALSE);
        gtk_box_append(GTK_BOX(body), note);
        ctx->notes[it] = note;
    }

    g_signal_connect(check, "clicked", G_CALLBACK(ex2_check), ctx);
    return page;
}

/* ---- ex3: Haustiere – ein/kein --------------------------------------- */

const char *ex3_u3_pool[] = {
    "ein", "eine", "einen", "kein", "keine", "keinen",
};

const U3Fill ex3_u3_rows[] = {
    {"2.", "Hast du ", "einen",
     " Hamster? – Nein, ich habe ", "keinen", " Hamster.", NULL, NULL,
     NULL},
    {NULL, "Ich habe ", "eine", " Schildkröte.", NULL, NULL, NULL, NULL,
     "Máš křečka? – Ne, nemám křečka. Mám želvu."},
    {"3.", "Hast du ", "ein",
     " Pferd? – Nein, ich habe ", "kein", " Pferd.", NULL, NULL, NULL},
    {NULL, "Ich habe ", "ein", " Kaninchen.", NULL, NULL, NULL, NULL,
     "Máš koně? – Ne, nemám koně. Mám králíka."},
    {"4.", "Hast du ", "einen",
     " Fisch? – Nein, ich habe ", "keinen", " Fisch.", NULL, NULL, NULL},
    {NULL, "Ich habe ", "einen", " Vogel.", NULL, NULL, NULL, NULL,
     "Máš rybu? – Ne, nemám rybu. Mám ptáka."},
    {"5.", "Hast du ", "ein",
     " Meerschweinchen? – Nein, ich habe ", "kein",
     " Meerschweinchen.", NULL, NULL, NULL},
    {NULL, "Ich habe ", "einen", " Wellensittich.", NULL, NULL, NULL, NULL,
     "Máš morče? – Ne, nemám morče. Mám andulku."},
};

GtkWidget *u3ex3(UnitCtx *unit) {
    return u3_build_drop(unit, 3, "ein / kein", "sub3_haustier",
                         "Beispiel: Hast du eine Katze? – Nein, ich habe "
                         "keine Katze. Ich habe einen Hund.",
                         ex3_u3_rows, G_N_ELEMENTS(ex3_u3_rows),
                         ex3_u3_pool, G_N_ELEMENTS(ex3_u3_pool), NULL, TRUE);
}

/* ---- inline letter-gap rows (ex15) ----------------------------------- */

/* One sentence row whose text is split into segments with up to four
 * small text entries (one per missing-letter group) inserted. */


U3LetCtx *u3_let_ctx_new(UnitCtx *unit, int ex_num,
                                GtkWidget *feedback) {
    U3LetCtx *ctx = g_new0(U3LetCtx, 1);
    ctx->entries = g_array_new(FALSE, FALSE, sizeof(GtkWidget *));
    ctx->answers = g_array_new(FALSE, FALSE, sizeof(const char *));
    ctx->trans = g_array_new(FALSE, FALSE, sizeof(GtkWidget *));
    ctx->models = g_array_new(FALSE, FALSE, sizeof(GtkWidget *));
    ctx->unit = unit;
    ctx->ex_num = ex_num;
    ctx->feedback = feedback;
    return ctx;
}

/* Rebuild the full German sentence (letter gaps filled in). */
char *u3_let_sentence(const U3Let *row) {
    const char *segs[5];
    const char *ans[4];
    GString *s;
    int i, g;

    segs[0] = row->s0;
    segs[1] = row->s1;
    segs[2] = row->s2;
    segs[3] = row->s3;
    segs[4] = row->s4;
    ans[0] = row->a0;
    ans[1] = row->a1;
    ans[2] = row->a2;
    ans[3] = row->a3;
    g = 0;
    while (g < 4 && ans[g])
        g++;

    s = g_string_new(NULL);
    for (i = 0; i <= g; i++) {
        if (segs[i] && segs[i][0])
            g_string_append(s, segs[i]);
        if (i < g && ans[i])
            g_string_append(s, ans[i]);
    }
    if (s->len == 0) {
        g_string_free(s, TRUE);
        return NULL;
    }
    return g_string_free(s, FALSE);
}

void u3_let_check(GtkButton *button, gpointer data) {
    U3LetCtx *ctx = data;
    guint total = ctx->entries->len;
    guint ok = 0;

    (void)button;

    for (guint i = 0; i < total; i++) {
        GtkWidget *e = g_array_index(ctx->entries, GtkWidget *, i);
        const char *ans = g_array_index(ctx->answers, const char *, i);
        const gchar *txt = gtk_editable_get_text(GTK_EDITABLE(e));
        gchar *norm = normalize_answer(txt);
        gboolean good = txt && txt[0] && answer_accepts(norm, ans);
        g_free(norm);
        answer_mark(e, good);
        if (good)
            ok++;
    }

    for (guint i = 0; i < ctx->trans->len; i++) {
        GtkWidget *lbl = g_array_index(ctx->trans, GtkWidget *, i);
        if (lbl)
            gtk_widget_set_visible(lbl, TRUE);
    }

    for (guint i = 0; i < ctx->models->len; i++) {
        GtkWidget *lbl = g_array_index(ctx->models, GtkWidget *, i);
        if (lbl)
            gtk_widget_set_visible(lbl, TRUE);
    }

    if (ok == total) {
        set_feedback(ctx->feedback, TRUE, tr("feedback_ok"));
        mark_done(ctx->unit, ctx->ex_num);
    } else {
        set_feedback(ctx->feedback, FALSE, tr("feedback_retry"));
    }
}

void u3_let_row(U3LetCtx *ctx, GtkWidget *body, const U3Let *row) {
    const char *segs[5];
    const char *ans[4];
    int i, g;
    GtkWidget *line;

    segs[0] = row->s0;
    segs[1] = row->s1;
    segs[2] = row->s2;
    segs[3] = row->s3;
    segs[4] = row->s4;
    ans[0] = row->a0;
    ans[1] = row->a1;
    ans[2] = row->a2;
    ans[3] = row->a3;
    g = 0;
    while (g < 4 && ans[g])
        g++;

    line = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 4);
    gtk_widget_set_halign(line, GTK_ALIGN_START);
    gtk_box_append(GTK_BOX(body), line);

    if (row->num && row->num[0]) {
        GtkWidget *nl = gtk_label_new(row->num);
        gtk_widget_set_valign(nl, GTK_ALIGN_CENTER);
        gtk_widget_add_css_class(nl, "ex-prompt");
        gtk_box_append(GTK_BOX(line), nl);
    }
    for (i = 0; i <= g; i++) {
        if (segs[i] && segs[i][0]) {
            GtkWidget *lb = gtk_label_new(segs[i]);
            gtk_widget_set_valign(lb, GTK_ALIGN_CENTER);
            gtk_widget_add_css_class(lb, "ex-prompt");
            gtk_box_append(GTK_BOX(line), lb);
        }
        if (i < g) {
            GtkWidget *e = gtk_entry_new();
            gtk_editable_set_width_chars(GTK_EDITABLE(e), 4);
            gtk_widget_set_valign(e, GTK_ALIGN_CENTER);
            gtk_box_append(GTK_BOX(line), e);
            g_array_append_val(ctx->entries, e);
            g_array_append_val(ctx->answers, ans[i]);
        }
    }

    if (row->mean && row->mean[0]) {
        GtkWidget *m = meaning_add(body, row->mean);
        gtk_widget_set_visible(m, TRUE);
        g_array_append_val(ctx->trans, m);
    }

    {
        char *sentence = u3_let_sentence(row);

        if (sentence && sentence[0]) {
            GtkWidget *w = model_answer_add(body, sentence);

            g_array_append_val(ctx->models, w);
        }
        g_free(sentence);
    }
}

GtkWidget *u3_build_letters(UnitCtx *unit, int ex_num,
                                   const char *title, const char *sub,
                                   const U3Let *rows, int n) {
    GtkWidget *body, *feedback, *check;
    GtkWidget *page = ex_page_shell(unit->page, title, sub, "check",
                                    &body, &feedback, &check);
    U3LetCtx *ctx = u3_let_ctx_new(unit, ex_num, feedback);

    {
        gboolean need = FALSE;

        for (int i = 0; i < n && !need; i++) {
            const U3Let *r = &rows[i];

            need = text_has_german_umlaut(r->s0) ||
                   text_has_german_umlaut(r->s1) ||
                   text_has_german_umlaut(r->s2) ||
                   text_has_german_umlaut(r->s3) ||
                   text_has_german_umlaut(r->s4) ||
                   text_has_german_umlaut(r->a0) ||
                   text_has_german_umlaut(r->a1) ||
                   text_has_german_umlaut(r->a2) ||
                   text_has_german_umlaut(r->a3);
        }
        if (need)
            add_umlaut_note(body);
    }

    for (int i = 0; i < n; i++)
        u3_let_row(ctx, body, &rows[i]);

    g_signal_connect(check, "clicked", G_CALLBACK(u3_let_check), ctx);
    return page;
}

/* ---- ex4: missing letters in a paragraph ----------------------------- */

/* The Czech sentence is shown straight away; pressing "Check" only adds
 * the translations of the words the student had to complete. */

const Ex4Row ex4_u3_rows[] = {
    {{"Wir hab", "n Fre", "nde in Öster", "ich. Sie wo", "nen in Wien."},
     {"e", "u", "re", "h"},
     "Máme přátele v Rakousku. Bydlí ve Vídni.",
     {"haben", "Freunde", "Österreich", "wohnen"},
     {"mít", "přátelé", "Rakousko", "bydlet"}, 4},
    {{"Sie hei", "en Elfriede und Jiri. Sie haben auch ", "inder.", NULL,
      NULL},
     {"ß", "K", NULL, NULL},
     "Jmenují se Elfriede a Jiří. Mají také děti.",
     {"heißen", "Kinder", NULL, NULL},
     {"jmenovat se", "děti", NULL, NULL}, 2},
    {{"Ihr S", "hn heißt Philipp und ihr", " Tochter heißt Sabine.", NULL,
      NULL},
     {"o", "e", NULL, NULL},
     "Jejich syn se jmenuje Philipp a jejich dcera Sabine.",
     {"Sohn", "ihre", NULL, NULL},
     {"syn", "jejich", NULL, NULL}, 2},
    {{"Die Famili", " re", "st gern.", NULL, NULL},
     {"e", "i", NULL, NULL},
     "Rodina ráda cestuje.",
     {"Familie", "reist", NULL, NULL},
     {"rodina", "cestuje", NULL, NULL}, 2},
    {{"Sie ist o", "t bei uns zu Bes", "ch in Tsche", "ien.", NULL},
     {"f", "u", "ch", NULL},
     "Často je u nás na návštěvě v Česku.",
     {"oft", "Besuch", "Tschechien", NULL},
     {"často", "návštěva", "Česko", NULL}, 3},
    {{"Die Kinder ler", "en auch Tsche", "isch.", NULL, NULL},
     {"n", "ch", NULL, NULL},
     "Děti se učí také česky.",
     {"lernen", "Tschechisch", NULL, NULL},
     {"učit se", "čeština", NULL, NULL}, 2},
};


void ex4_check(GtkButton *button, gpointer data) {
    Ex4LetterCtx *ctx = data;
    guint total = ctx->entries->len;
    guint ok = 0;

    (void)button;

    for (guint i = 0; i < total; i++) {
        GtkWidget *e = g_array_index(ctx->entries, GtkWidget *, i);
        const char *ans = g_array_index(ctx->answers, const char *, i);
        const gchar *txt = gtk_editable_get_text(GTK_EDITABLE(e));
        gchar *norm = normalize_answer(txt);
        gboolean good = txt && txt[0] && answer_accepts(norm, ans);
        g_free(norm);
        answer_mark(e, good);
        if (good)
            ok++;
    }

    for (int r = 0; r < ctx->n_rows; r++) {
        const Ex4Row *row = &ctx->rows[r];
        GtkWidget *note = ctx->notes[r];
        GString *s = g_string_new(NULL);
        for (int k = 0; k < row->gaps; k++) {
            if (k > 0)
                g_string_append(s, " · ");
            if (row->word[k])
                g_string_append_printf(s, "%s = %s", row->word[k],
                                       tr(row->gmean[k]));
        }
        gtk_label_set_text(GTK_LABEL(note), s->str);
        g_string_free(s, TRUE);
        gtk_widget_set_visible(note, TRUE);
    }

    if (ok == total) {
        set_feedback(ctx->feedback, TRUE, tr("feedback_ok"));
        mark_done(ctx->unit, ctx->ex_num);
    } else {
        set_feedback(ctx->feedback, FALSE, tr("feedback_retry"));
    }
}

GtkWidget *u3ex4(UnitCtx *unit) {
    GtkWidget *body, *feedback, *check;
    GtkWidget *page = ex_page_shell(unit->page, "Lückentext", "sub3_buchst",
                                    "check", &body, &feedback, &check);
    Ex4LetterCtx *ctx = g_new0(Ex4LetterCtx, 1);

    ctx->entries = g_array_new(FALSE, FALSE, sizeof(GtkWidget *));
    ctx->answers = g_array_new(FALSE, FALSE, sizeof(const char *));
    ctx->rows = ex4_u3_rows;
    ctx->n_rows = G_N_ELEMENTS(ex4_u3_rows);
    ctx->notes = g_new0(GtkWidget *, ctx->n_rows);
    ctx->unit = unit;
    ctx->ex_num = 4;
    ctx->feedback = feedback;

    {
        gboolean need = FALSE;

        for (int r = 0; r < ctx->n_rows && !need; r++) {
            for (int i = 0; i < ctx->rows[r].gaps; i++) {
                if (text_has_german_umlaut(ctx->rows[r].ans[i]) ||
                    text_has_german_umlaut(ctx->rows[r].word[i])) {
                    need = TRUE;
                    break;
                }
            }
        }
        if (need)
            add_umlaut_note(body);
    }

    for (int r = 0; r < ctx->n_rows; r++) {
        const Ex4Row *row = &ctx->rows[r];
        GtkWidget *line = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 4);
        GtkWidget *mean;
        GtkWidget *note;

        gtk_widget_set_halign(line, GTK_ALIGN_START);
        for (int i = 0; i <= row->gaps; i++) {
            if (row->seg[i] && row->seg[i][0]) {
                GtkWidget *lb = gtk_label_new(row->seg[i]);
                gtk_widget_set_valign(lb, GTK_ALIGN_CENTER);
                gtk_widget_add_css_class(lb, "ex-prompt");
                gtk_box_append(GTK_BOX(line), lb);
            }
            if (i < row->gaps) {
                GtkWidget *e = gtk_entry_new();
                gtk_editable_set_width_chars(GTK_EDITABLE(e), 4);
                gtk_widget_set_valign(e, GTK_ALIGN_CENTER);
                gtk_box_append(GTK_BOX(line), e);
                g_array_append_val(ctx->entries, e);
                g_array_append_val(ctx->answers, row->ans[i]);
            }
        }
        gtk_box_append(GTK_BOX(body), line);

        /* the Czech sentence is available before checking */
        mean = meaning_add(body, row->czech);
        gtk_widget_set_visible(mean, TRUE);

        /* gapped-word translations appear only after "Check" */
        note = gtk_label_new(NULL);
        gtk_widget_set_halign(note, GTK_ALIGN_START);
        gtk_widget_set_margin_bottom(note, 2);
        gtk_widget_add_css_class(note, "meaning");
        gtk_label_set_wrap(GTK_LABEL(note), TRUE);
        gtk_widget_set_visible(note, FALSE);
        gtk_box_append(GTK_BOX(body), note);
        ctx->notes[r] = note;
    }

    g_signal_connect(check, "clicked", G_CALLBACK(ex4_check), ctx);
    return page;
}

/* ---- ex5: Marcos Familie (word-bank typing) -------------------------- */

const char *ex5_u3_bank =
    "Tanten | Job | Vater | Ferien | Zwillinge | Eltern | Kroatien";

const TypedQ ex5_u3_rows[] = {
    {"Wir haben eine Männer-Wohngemeinschaft: Mein ________ Johann (50), "
     "Ivo (16) und ich.",
     "vater", "Máme mužskou domácnost: Můj otec Johann (50), Ivo (16) a já."},
    {"Ivo und ich sind gleich alt – wir sind nämlich ________.",
     "zwillinge", "Ivo a já jsme stejně staří – jsme totiž dvojčata."},
    {"Jetzt wohnen wir in Hannover, unser Vater hat hier einen neuen "
     "________.",
     "job", "Náš otec tu má novou práci."},
    {"Unsere ________ sind geschieden.",
     "eltern", "Naši rodiče jsou rozvedení."},
    {"Unsere Mutter Maja (50) lebt in Wien, aber sie kommt aus ________.",
     "kroatien", "Naše matka Maja (50) žije ve Vídni, ale pochází z "
     "Chorvatska."},
    {"Wir haben viele Onkel, ________, Cousinen und Cousins in Kroatien.",
     "tanten", "Máme v Chorvatsku hodně strýců, tet, sestřenic a bratranců."},
    {"Wir sehen sie manchmal in den ________.",
     "ferien", "Vídáme je někdy o prázdninách."},
};

GtkWidget *u3ex5(UnitCtx *unit) {
    return build_typed(unit, 5, "Marcos Familie", "sub3_marco",
                       ex5_u3_rows, G_N_ELEMENTS(ex5_u3_rows),
                       ex5_u3_bank, FALSE);
}

/* ---- ex6: Wortschatz sortieren --------------------------------------- */

const AssignItem ex6_u3_items[] = {
    {NULL, "die Oma", 0},
    {NULL, "der Bruder", 0},
    {NULL, "der Opa", 0},
    {NULL, "die Tochter", 0},
    {NULL, "die Tante", 0},
    {NULL, "der Hamster", 1},
    {NULL, "das Pferd", 1},
    {NULL, "die Katze", 1},
    {NULL, "der Hund", 1},
    {NULL, "der Wellensittich", 1},
};

const char *ex6_u3_groups[] = {"Familienmitglieder", "Haustiere"};

const char *ex6_u3_meaning[] = {
    "babička", "bratr", "dědeček", "dcera", "teta",
    "křeček", "kůň", "kočka", "pes", "andulka",
};

GtkWidget *u3ex6(UnitCtx *unit) {
    return build_assign(unit, "Sortieren", "sub3_sort", 6,
                        ex6_u3_items, G_N_ELEMENTS(ex6_u3_items),
                        ex6_u3_groups, G_N_ELEMENTS(ex6_u3_groups),
                        ex6_u3_meaning);
}

/* ---- ex7: Possessivpronomen-Tabelle ---------------------------------- */


void u3_table_check(GtkButton *button, gpointer data) {
    U3TableCtx *ctx = data;
    int ok = 0;

    (void)button;

    for (int i = 0; i < ctx->n; i++) {
        const gchar *txt = gtk_editable_get_text(
            GTK_EDITABLE(ctx->entries[i]));
        gchar *norm = normalize_answer(txt);
        gboolean good = txt && txt[0] &&
                        g_strcmp0(norm, ctx->answers[i]) == 0;
        g_free(norm);
        answer_mark(ctx->entries[i], good);
        if (good)
            ok++;
    }

    if (ok == ctx->n) {
        set_feedback(ctx->feedback, TRUE, tr("feedback_ok"));
        mark_done(ctx->unit, ctx->ex_num);
    } else {
        set_feedback(ctx->feedback, FALSE, tr("feedback_retry"));
    }
}

GtkWidget *u3ex7(UnitCtx *unit) {
    static const char *persons[] = {
        "ich", "du", "er", "sie", "es", "wir", "ihr", "sie / Sie",
    };
    static const char *nouns[] = {
        "der Garten", "die Idee", "das Fest", "die Geschwister",
    };
    static const char *ans[4][8] = {
        {"mein",  "dein",  "sein",  "ihr", "sein",  "unser",  "euer",
         "ihr"},
        {"meine", "deine", "seine", "ihre", "seine", "unsere", "eure",
         "ihre"},
        {"mein",  "dein",  "sein",  "ihr", "sein",  "unser",  "euer",
         "ihr"},
        {"meine", "deine", "seine", "ihre", "seine", "unsere", "eure",
         "ihre"},
    };
    const int np = G_N_ELEMENTS(persons);
    const int nn = G_N_ELEMENTS(nouns);
    GtkWidget *body, *feedback, *check;
    GtkWidget *page = ex_page_shell(unit->page, "Possessivtabelle",
                                    "sub3_tabelle", "check",
                                    &body, &feedback, &check);
    GtkWidget *grid;
    GtkWidget *sample_note;
    GtkWidget *bank_row;
    GtkWidget *bank_hdr;
    GtkWidget *bank1;
    GtkWidget *bank2;
    U3TableCtx *ctx = g_new0(U3TableCtx, 1);

    ctx->n = nn * np;
    ctx->entries = g_new0(GtkWidget *, ctx->n);
    ctx->answers = g_new0(const char *, ctx->n);
    ctx->unit = unit;
    ctx->ex_num = 7;
    ctx->feedback = feedback;

    sample_note = gtk_label_new("Beispiel: mein (ich × der Garten)");
    gtk_widget_set_halign(sample_note, GTK_ALIGN_START);
    gtk_widget_set_margin_top(sample_note, 2);
    gtk_widget_set_margin_bottom(sample_note, 4);
    gtk_widget_add_css_class(sample_note, "hint");
    gtk_box_append(GTK_BOX(body), sample_note);

    bank_row = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
    gtk_widget_set_halign(bank_row, GTK_ALIGN_START);
    gtk_box_append(GTK_BOX(body), bank_row);
    bank_hdr = gtk_label_new(NULL);
    i18n_bind(bank_hdr, "wordbank", 0);
    gtk_widget_add_css_class(bank_hdr, "hint");
    gtk_box_append(GTK_BOX(bank_row), bank_hdr);
    bank1 = gtk_label_new("(der Garten / das Fest)  "
                          "mein · dein · sein · ihr · unser · euer");
    gtk_widget_add_css_class(bank1, "hint");
    gtk_box_append(GTK_BOX(bank_row), bank1);

    bank_row = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
    gtk_widget_set_halign(bank_row, GTK_ALIGN_START);
    gtk_box_append(GTK_BOX(body), bank_row);
    bank2 = gtk_label_new("(die Idee / die Geschwister)  "
                          "meine · deine · seine · ihre · unsere · eure");
    gtk_widget_add_css_class(bank2, "hint");
    gtk_box_append(GTK_BOX(bank_row), bank2);

    grid = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(grid), 8);
    gtk_grid_set_row_spacing(GTK_GRID(grid), 8);
    gtk_widget_set_halign(grid, GTK_ALIGN_START);
    gtk_box_append(GTK_BOX(body), grid);

    for (int c = 0; c < np; c++) {
        GtkWidget *h = gtk_label_new(persons[c]);
        gtk_widget_set_halign(h, GTK_ALIGN_CENTER);
        gtk_widget_add_css_class(h, "ex-sub");
        gtk_grid_attach(GTK_GRID(grid), h, c + 1, 0, 1, 1);
    }
    for (int r = 0; r < nn; r++) {
        GtkWidget *h = gtk_label_new(nouns[r]);
        gtk_widget_set_halign(h, GTK_ALIGN_START);
        gtk_widget_add_css_class(h, "ex-sub");
        gtk_grid_attach(GTK_GRID(grid), h, 0, r + 1, 1, 1);
    }
    for (int r = 0; r < nn; r++) {
        for (int c = 0; c < np; c++) {
            GtkWidget *e = gtk_entry_new();
            gtk_editable_set_width_chars(GTK_EDITABLE(e), 6);
            gtk_widget_set_halign(e, GTK_ALIGN_CENTER);
            gtk_grid_attach(GTK_GRID(grid), e, c + 1, r + 1, 1, 1);
            ctx->entries[r * np + c] = e;
            ctx->answers[r * np + c] = ans[r][c];
        }
    }
    /* pre-filled sample cell (ich × der Garten) */
    gtk_editable_set_text(GTK_EDITABLE(ctx->entries[0]), "mein");
    gtk_widget_set_sensitive(ctx->entries[0], FALSE);

    g_signal_connect(check, "clicked", G_CALLBACK(u3_table_check), ctx);
    return page;
}

/* ---- ex8: Possessivpronomen im Akkusativ ------------------------------ */

const char *ex8_u3_pool[] = {
    "mein", "meine", "meinen", "dein", "deine", "deinen",
    "sein", "seine", "seinen", "ihr", "ihre", "ihren",
    "unser", "unsere", "unseren", "euer", "eure", "euren",
};

const U3Fill ex8_u3_rows[] = {
    {"2.", "das Handy → Du brauchst ", "dein", " Handy.", NULL, NULL, NULL,
     NULL, "Potřebuješ svůj mobil."},
    {"3.", "das Pferd → Er hat ", "sein", " Pferd.", NULL, NULL, NULL, NULL,
     "Má svého koně."},
    {"4.", "die Party → Sie macht ", "ihre", " Party.", NULL, NULL, NULL,
     NULL, "Dělá svou párty."},
    {"5.", "der Lehrer → Es fragt ", "seinen", " Lehrer.", NULL, NULL, NULL,
     NULL, "Ptá se svého učitele."},
    {"6.", "das Wochenendhaus → Wir besuchen ", "unser", " Wochenendhaus.",
     NULL, NULL, NULL, NULL, "Navštěvujeme náš víkendový dům."},
    {"7.", "der Hund → Ihr seht ", "euren", " Hund.", NULL, NULL, NULL, NULL,
     "Vy vidíte svého psa."},
    {"8.", "die Instrumente → Sie haben ", "ihre", " Instrumente.", NULL,
     NULL, NULL, NULL, "Mají své nástroje."},
};

const char *ex8_u3_gloss[] = {
    "u3g_dein", "u3g_sein", "u3g_ihr_sie", "u3g_sein_es",
    "u3g_unser", "u3g_euer", "u3g_ihr_sie_pl",
};

GtkWidget *u3ex8(UnitCtx *unit) {
    return u3_build_drop(unit, 8, "Akkusativ", "sub3_akk",
                         "Beispiel: die Inliner (ich) → Ich brauche meine "
                         "Inliner.",
                         ex8_u3_rows, G_N_ELEMENTS(ex8_u3_rows),
                         ex8_u3_pool, G_N_ELEMENTS(ex8_u3_pool),
                         ex8_u3_gloss, TRUE);
}

/* ---- ex9: kein / keine / keinen / nicht ------------------------------ */

const char *ex9_u3_pool[] = {
    "kein", "keine", "keinen", "nicht",
};

const U3Fill ex9_u3_rows[] = {
    {"1.", "● Sprichst du Spanisch?  ○ Nein, ich spreche ", "kein",
     " Spanisch.", NULL, NULL, NULL, NULL,
     "Mluvíš španělsky? – Ne, nemluvím španělsky."},
    {"2.", "● Hast du einen Bruder?  ○ Nein, ich habe ", "keinen",
     " Bruder.", NULL, NULL, NULL, NULL, "Máš bratra? – Ne, nemám bratra."},
    {"3.", "● Gibt es in Opava einen Zoo?  ○ Nein, in Opava gibt es ",
     "keinen", " Zoo.", NULL, NULL, NULL, NULL,
     "Je v Opavě zoo? – Ne, v Opavě žádná zoo není."},
    {"4.", "● Gehst du ins Schwimmbad?  ○ Nein, ich gehe ", "nicht",
     " ins Schwimmbad.", NULL, NULL, NULL, NULL,
     "Jdeš do bazénu? – Ne, do bazénu nejdu."},
    {"5.", "● Wandert er gern?  ○ Nein, er wandert ", "nicht", " gern.",
     NULL, NULL, NULL, NULL, "Rád chodí na túry? – Ne, nerad chodí na túry."},
    {"6.", "● Besuchst du heute ein Schloss?  ○ Nein, heute besuche ich ",
     "kein", " Schloss.", NULL, NULL, NULL, NULL,
     "Navštívíš dnes zámek? – Ne, dnes nenavštívím žádný zámek."},
};

GtkWidget *u3ex9(UnitCtx *unit) {
    return u3_build_drop(unit, 9, "kein / nicht", "sub3_nicht", NULL,
                         ex9_u3_rows, G_N_ELEMENTS(ex9_u3_rows),
                         ex9_u3_pool, G_N_ELEMENTS(ex9_u3_pool), NULL, TRUE);
}

/* ---- ex10: Sätze bauen ------------------------------------------------ */

const AssemblyItem ex10_u3_items[] = {
    {NULL, {"Sie", "haben", "eine", "Schildkröte", "."}, 5},
    {NULL, {"Was", "macht", "deine", "Freundin", "am", "Wochenende?"}, 6},
    {NULL, {"Mein", "Vater", "ist", "Lehrer", "von", "Beruf."}, 6},
    {NULL, {"Seid", "ihr", "viele", "zu", "Hause?"}, 5},
    {NULL, {"Julia", "stellt", "ihren", "Hund", "Trixi", "vor."}, 6},
};

const char *ex10_u3_meaning[] = {
    "Mají želvu.",
    "Co dělá tvoje kamarádka o víkendu?",
    "Můj otec je povoláním učitel.",
    "Je vás doma hodně?",
    "Julie představuje svého psa Trixiho.",
};

GtkWidget *u3ex10(UnitCtx *unit) {
    return build_assembly(unit, "Sätze bauen", "sub3_satz", 10,
                          ex10_u3_items, ex10_u3_meaning,
                          G_N_ELEMENTS(ex10_u3_items));
}

/* ---- keyword-typed builder (ex11 + ex14) ----------------------------- */



void u3_kw_check(GtkButton *button, gpointer data) {
    U3KwCtx *ctx = data;
    int ok = 0;

    (void)button;

    for (int i = 0; i < ctx->n; i++) {
        const gchar *txt = gtk_editable_get_text(GTK_EDITABLE(ctx->entries[i]));
        gchar *norm = normalize_answer(txt);
        gboolean good = txt && txt[0];
        if (good) {
            gchar **parts = g_strsplit(ctx->qs[i].answers, "|", -1);
            good = FALSE;
            for (int j = 0; parts[j]; j++) {
                gchar *a = normalize_answer(parts[j]);
                if (a[0] && g_strrstr(norm, a)) {
                    good = TRUE;
                    g_free(a);
                    break;
                }
                g_free(a);
            }
            g_strfreev(parts);
        }
        g_free(norm);
        answer_mark(ctx->entries[i], good);
        if (good)
            ok++;
    }

    for (int i = 0; i < ctx->n; i++)
        if (ctx->models[i])
            gtk_widget_set_visible(ctx->models[i], TRUE);

    if (ok == ctx->n) {
        set_feedback(ctx->feedback, TRUE, tr("feedback_ok"));
        mark_done(ctx->unit, ctx->ex_num);
    } else {
        set_feedback(ctx->feedback, FALSE, tr("feedback_retry"));
    }
}

GtkWidget *u3_build_kw(UnitCtx *unit, int ex_num, const char *title,
                              const char *sub, const char *sample,
                              const U3Kw *qs, int n) {
    GtkWidget *body, *feedback, *check;
    GtkWidget *page = ex_page_shell(unit->page, title, sub, "check",
                                    &body, &feedback, &check);
    U3KwCtx *ctx = g_new0(U3KwCtx, 1);

    ctx->qs = qs;
    ctx->n = n;
    ctx->unit = unit;
    ctx->ex_num = ex_num;
    ctx->feedback = feedback;
    ctx->entries = g_new0(GtkWidget *, n);
    ctx->trans = g_new0(GtkWidget *, n);
    ctx->models = g_new0(GtkWidget *, n);

    {
        gboolean need = FALSE;

        for (int i = 0; i < n && !need; i++) {
            need = text_has_german_umlaut(qs[i].prompt) ||
                   text_has_german_umlaut(qs[i].answers) ||
                   text_has_german_umlaut(qs[i].german);
        }
        if (need)
            add_umlaut_note(body);
    }

    if (sample && sample[0])
        u3_sample_line(body, sample);

    for (int i = 0; i < n; i++) {
        GtkWidget *prompt = gtk_label_new(qs[i].prompt);
        GtkWidget *entry;

        gtk_widget_set_halign(prompt, GTK_ALIGN_START);
        gtk_widget_add_css_class(prompt, "ex-prompt");
        gtk_label_set_wrap(GTK_LABEL(prompt), TRUE);
        gtk_box_append(GTK_BOX(body), prompt);

        entry = gtk_entry_new();
        gtk_widget_set_hexpand(entry, FALSE);
        gtk_widget_set_halign(entry, GTK_ALIGN_START);
        gtk_box_append(GTK_BOX(body), entry);
        ctx->entries[i] = entry;

        if (qs[i].mean)
            ctx->trans[i] = meaning_add(body, qs[i].mean);

        /* Czech meaning acts as a hint and stays visible. */
        if (ctx->trans[i])
            gtk_widget_set_visible(ctx->trans[i], TRUE);

        /* The model German sentence is only shown after clicking Check. */
        if (qs[i].german && qs[i].german[0]) {
            GtkWidget *model = gtk_label_new(qs[i].german);

            gtk_widget_set_halign(model, GTK_ALIGN_START);
            gtk_label_set_xalign(GTK_LABEL(model), 0.0);
            gtk_label_set_wrap(GTK_LABEL(model), TRUE);
            gtk_widget_set_margin_top(model, 2);
            gtk_widget_set_margin_bottom(model, 2);
            gtk_widget_set_margin_start(model, 8);
            gtk_widget_add_css_class(model, "u3-answer");
            gtk_widget_set_visible(model, FALSE);
            gtk_box_append(GTK_BOX(body), model);
            ctx->models[i] = model;
        }
    }

    g_signal_connect(check, "clicked", G_CALLBACK(u3_kw_check), ctx);
    return page;
}

/* ---- ex11: Was siehst du? -------------------------------------------- */

const U3Kw ex11_u3_qs[] = {
    {"Ein altes Gebäude aus dem Mittelalter. Es steht auf einem Berg.",
     "burg|schloss", "Vidím hrad / zámek.",
     "Ich sehe eine Burg. / Ich sehe ein Schloss."},
    {"Wasser, Bäume und Berge. Es ist sehr ruhig.",
     "see", "Vidím jezero.", "Ich sehe einen See."},
    {"Eine Frau, ein Mann und zwei Kinder essen zusammen.",
     "familie", "Vidím rodinu.", "Ich sehe eine Familie."},
};

GtkWidget *u3ex11(UnitCtx *unit) {
    return u3_build_kw(unit, 11, "Was siehst du?", "sub3_sehen",
                       "Beispiel: Ich sehe eine Burg. / Ich sehe ein Schloss.",
                       ex11_u3_qs, G_N_ELEMENTS(ex11_u3_qs));
}

/* ---- ex12: Wem gehört das? ------------------------------------------- */

const char *ex12_u3_pool[] = {
    "mein", "meine", "dein", "deine", "sein", "seine",
    "ihr", "ihre", "unser", "unsere", "euer", "eure",
};

const U3Fill ex12_u3_rows[] = {
    {"1.", "Das ist Tom. → Das ist ", "sein", " Handy.", NULL, NULL, NULL,
     NULL, "To je jeho mobil."},
    {"2.", "Das ist Maria. → Das ist ", "ihr", " Kaninchen.", NULL, NULL,
     NULL, NULL, "To je její králík."},
    {"3.", "Das sind Max und Lisa. → Das sind ", "ihre", " Inliner.", NULL,
     NULL, NULL, NULL, "To jsou jejich brusle."},
    {"4.", "Das ist Frau Berger. → Das ist ", "ihr", " Kuchen.", NULL, NULL,
     NULL, NULL, "To je její dort."},
    {"5.", "Das ist Herr Müller. → Das ist ", "sein", " Haus.", NULL, NULL,
     NULL, NULL, "To je jeho dům."},
    {"6.", "Das sind die Schmidts. → Das sind ", "ihre", " Kinder.", NULL,
     NULL, NULL, NULL, "To jsou jejich děti."},
};

GtkWidget *u3ex12(UnitCtx *unit) {
    return u3_build_drop(unit, 12, "Wem gehört das?", "sub3_wem", NULL,
                         ex12_u3_rows, G_N_ELEMENTS(ex12_u3_rows),
                         ex12_u3_pool, G_N_ELEMENTS(ex12_u3_pool), NULL, TRUE);
}

/* ---- ex13: Es gibt + unbestimmter Artikel ---------------------------- */

const char *ex13_u3_pool[] = {"ein", "eine", "einen"};

const U3Fill ex13_u3_rows[] = {
    {"1.", "(das Konzert) → Es gibt ", "ein", " Konzert.", NULL, NULL, NULL,
     NULL, "Je tu koncert."},
    {"2.", "(die Ausstellung) → Es gibt ", "eine", " Ausstellung.", NULL,
     NULL, NULL, NULL, "Je tu výstava."},
    {"3.", "(das Sportfest) → Es gibt ", "ein", " Sportfest.", NULL, NULL,
     NULL, NULL, "Je tu sportovní slavnost."},
    {"4.", "(das Familientreffen) → Es gibt ", "ein", " Familientreffen.",
     NULL, NULL, NULL, NULL, "Je tu rodinné setkání."},
    {"5.", "(die Theatervorstellung) → Es gibt ", "eine",
     " Theatervorstellung.", NULL, NULL, NULL, NULL,
     "Je tu divadelní představení."},
    {"6.", "(der Flohmarkt) → Es gibt ", "einen", " Flohmarkt.", NULL, NULL,
     NULL, NULL, "Je tu bleší trh."},
};

GtkWidget *u3ex13(UnitCtx *unit) {
    return u3_build_drop(unit, 13, "Es gibt …", "sub3_gibt", NULL,
                         ex13_u3_rows, G_N_ELEMENTS(ex13_u3_rows),
                         ex13_u3_pool, G_N_ELEMENTS(ex13_u3_pool), NULL, TRUE);
}

/* ---- ex14: Beschreiben (sentence pairs) ------------------------------ */

const U3Kw ex14_u3_qs[] = {
    {"Eine Frau fährt Fahrrad.  →  Ich sehe ________.",
     "eine frau|frau", "Vidím ženu.", "Ich sehe eine Frau."},
    {"→ Sie ________.",
     "sie fährt fahrrad|sie fährt|fährt fahrrad|fährt", "Jezdí na kole.",
     "Sie fährt Fahrrad."},
    {"Ein Mann wandert im Wald.  →  Ich sehe ________.",
     "einen mann|mann", "Vidím muže.", "Ich sehe einen Mann."},
    {"→ Er ________.",
     "er wandert im wald|er wandert|wandert im wald|wandert",
     "Chodí po lese.", "Er wandert im Wald."},
    {"Zwei Kinder spielen Fußball.  →  Ich sehe ________.",
     "zwei kinder|kinder", "Vidím dvě děti.", "Ich sehe zwei Kinder."},
    {"→ Sie ________.",
     "sie spielen fußball|sie spielen fussball|spielen fußball|spielen "
     "fussball|spielen", "Hrají fotbal.", "Sie spielen Fußball."},
};

GtkWidget *u3ex14(UnitCtx *unit) {
    return u3_build_kw(unit, 14, "Beschreiben", "sub3_saetze",
                       "Beispiel: Ich sehe einen Jungen. Er liest ein Buch.",
                       ex14_u3_qs, G_N_ELEMENTS(ex14_u3_qs));
}

/* ---- ex15: missing letters (Wochenende) ------------------------------ */

const U3Let ex15_u3_rows[] = {
    {"1.", "Meine Gr", "oß", "eltern fahr", "en", " ins Wochen", "end",
     "haus.", NULL, NULL,
     "Moji prarodiče jedou o víkendu do víkendového domu."},
    {"2.", "Mein ", "Va", "ter und mein", "e", " Mutter besuch", "en",
     " ein Konz", "er", "t.",
     "Můj otec a moje matka navštíví koncert."},
    {"3.", "Mein", "e", " Sch", "w", "ester geh", "t", " ins K", "in", "o.",
     "Moje sestra jde do kina."},
    {"4.", "Mein Bru", "d", "er f", "äh", "rt an ", "ei", "nen S", "ee",
     ".",
     "Můj bratr jede k jezeru."},
    {"5.", "Und ich? Ich besuch", "e", " ein Einka", "ufs", "zentrum.",
     NULL, NULL, NULL, NULL,
     "A já? Navštívím obchodní centrum."},
};

GtkWidget *u3ex15(UnitCtx *unit) {
    return u3_build_letters(unit, 15, "Wochenende", "sub3_wochen",
                            ex15_u3_rows, G_N_ELEMENTS(ex15_u3_rows));
}

/* ------------------------------------------------------------------ */
/* Unit 3 vocabulary (off-path Vokabeltraining branch)                */
/* ------------------------------------------------------------------ */

const TypedQ u3_trans_p32[] = {
    {"die Geburtstagsparty, -s",
     "narozeninová slavnost|narozeninová oslava|birthday party",
     "narozeninová slavnost"},
    {"das Haustier, -e", "domácí zvíře|domácí mazlíček|pet", "domácí zvíře"},
};

const TypedQ u3_trans_p33[] = {
    {"die Aktivität, -en", "aktivita|činnost|activity", "aktivita"},
    {"der Stammbaum, Stammbäume", "rodokmen|family tree", "rodokmen"},
};

const TypedQ u3_trans_p34[] = {
    {"der Bruder, Brüder", "bratr|brother", "bratr"},
    {"der Cousin, -s", "bratranec|cousin", "bratranec"},
    {"die Cousine, -n", "sestřenice|cousin", "sestřenice"},
    {"die Ehefrau, -en", "manželka|žena|wife", "manželka"},
    {"der Ehemann, Ehemänner", "manžel|muž|husband", "manžel"},
    {"das Einzelkind, -er", "jedináček|only child", "jedináček"},
    {"die Eltern (mn. č.)", "rodiče|parents", "rodiče"},
    {"die Familie, -n", "rodina|family", "rodina"},
    {"feiern (Infinitiv)", "slavit|oslavovat|to celebrate",
     "slavit, oslavovat"},
    {"das Fest, -e", "oslava|slavnost|svátek|celebration|festival",
     "oslava, slavnost"},
    {"der Garten, Gärten", "zahrada|garden", "zahrada"},
    {"gehören (Infinitiv)", "patřit|to belong", "patřit"},
    {"Er gehört zur Familie.", "patří k rodině|he belongs to the family",
     "Patří k rodině."},
    {"geschieden", "rozvedený|rozvedená|divorced", "rozvedený"},
    {"Meine Eltern sind geschieden.",
     "moji rodiče jsou rozvedení|my parents are divorced",
     "Moji rodiče jsou rozvedení."},
    {"die Geschwister (mn. č.)", "sourozenci|siblings", "sourozenci"},
    {"gleich", "stejný|rovný|same|equal", "stejný"},
    {"groß", "velký|big|large|tall", "velký"},
    {"haben (Infinitiv)", "mít|to have", "mít"},
    {"Unser Haus hat einen großen Garten.",
     "náš dům má velkou zahradu|our house has a big garden",
     "Náš dům má velkou zahradu."},
    {"das Haus, Häuser", "dům|house", "dům"},
    {"hier", "zde|tady|tu|here", "zde, tady"},
    {"der Hund, -e", "pes|dog", "pes"},
    {"ihr, ihre, ihr", "její|jejich|její, jejich|her|their", "její"},
    {"der Iran", "írán|iran", "Írán"},
    {"der Job, -s", "pracovní místo|zaměstnání|práce|job",
     "(pracovní) místo, zaměstnání"},
    {"der Kater, -", "kocour|tomcat", "kocour"},
    {"lange", "dlouho|long", "dlouho"},
    {"manchmal", "někdy|občas|sometimes", "někdy, občas"},
    {"mehr", "více|víc|již|more", "více, již"},
    {"Ich bin kein Einzelkind mehr.",
     "již nejsem jedináček|už nejsem jedináček|i am no longer an only child",
     "Již nejsem jedináček."},
    {"nämlich", "totiž|namely", "totiž"},
    {"natürlich", "samozřejmě|přirozeně|of course|naturally",
     "samozřejmě"},
    {"neu", "nový|new", "nový"},
    {"die Oma, -s", "babička|babi|grandma|grandmother", "babička"},
    {"der Onkel, -", "strýc|uncle", "strýc"},
    {"der Opa, -s", "dědeček|děda|grandpa|grandfather", "dědeček"},
    {"die Patchworkfamilie, -n",
     "patchworková rodina|patchwork family|blended family",
     "patchworková rodina"},
    {"Persisch", "perština|persky|persian", "perština, persky"},
    {"die Person, -en", "osoba|person", "osoba"},
    {"die Schwester, -n", "sestra|sister", "sestra"},
    {"sehen (Infinitiv)", "vidět|to see", "vidět"},
    {"Wir sehen sie manchmal in den Ferien.",
     "vídáme je občas o prázdninách|vidíme je někdy o prázdninách|"
     "we see them sometimes during the holidays",
     "Vídáme je občas o prázdninách."},
    {"die Stiefmutter, Stiefmütter",
     "nevlastní matka|macecha|stepmother", "nevlastní matka"},
    {"die Stiefschwester, -n", "nevlastní sestra|stepsister",
     "nevlastní sestra"},
    {"die Tante, -n", "teta|téta|aunt", "teta"},
    {"die Tochter, Töchter", "dcera|daughter", "dcera"},
    {"tot", "mrtvý|dead", "mrtvý"},
    {"Er ist leider schon tot.",
     "on je již bohužel mrtvý|je bohužel už mrtvý|"
     "he is unfortunately already dead", "(On) Je již bohužel mrtvý."},
    {"typisch", "typický|klasický|typical", "typický, klasický"},
    {"unser, unsere, unser", "náš|naše|our", "náš, naše, naše"},
    {"der Vater, Väter", "otec|táta|father|dad", "otec"},
    {"verstehen sich",
     "rozumět si|rozumí si|to get along|to understand each other",
     "rozumět si"},
    {"Wir verstehen uns alle sehr gut.",
     "všichni si moc dobře rozumíme|všichni si velmi dobře rozumíme|"
     "we all get along very well", "Všichni si moc dobře rozumíme."},
    {"der/die Verwandte, -n", "příbuzný|příbuzná|relative",
     "příbuzný/příbuzná"},
    {"die Wohngemeinschaft, -en",
     "společné bydlení|spolubydlení|flat share|shared apartment",
     "společné bydlení (studentů)"},
    {"die Männer-Wohngemeinschaft",
     "mužská domácnost|mužské spolubydlení|men's shared flat",
     "mužská domácnost"},
    {"zusammen", "spolu|dohromady|together", "spolu, dohromady"},
    {"der Zwilling, -e", "dvojče|dvojčata|twin|twins", "dvojče"},
};

const TypedQ u3_trans_p35[] = {
    {"dein, deine, dein", "tvůj|tvoje|your", "tvůj, tvoje, tvoje"},
    {"euer, eure, euer", "váš|vaše|your", "váš, vaše, vaše"},
    {"ihr, ihre, ihr", "jejich|její|their|her", "jejich"},
    {"sein, seine, sein", "jeho|his", "jeho"},
};

const TypedQ u3_trans_p36[] = {
    {"bitte", "prosím|please", "prosím"},
    {"dann", "tak|pak|then", "tak, pak"},
    {"dauern (Infinitiv)", "trvat|to last|to take", "trvat"},
    {"Die Vorbereitung dauert nicht so lange.",
     "příprava netrvá moc dlouho|příprava netrvá tak dlouho|"
     "the preparation doesn't take that long",
     "Příprava netrvá moc dlouho."},
    {"doch", "přece|přeci|ale ano|yet|but", "přece"},
    {"einladen (Infinitiv)", "pozvat|zvát|to invite", "pozvat"},
    {"Wir laden Oma und Opa ein.",
     "pozveme babičku a dědečka|zveme babičku a dědečka|"
     "we invite grandma and grandpa", "Pozveme babičku a dědečka."},
    {"die Feier, -n", "oslava|slavnost|celebration|party",
     "oslava, slavnost"},
    {"fragen (Infinitiv)", "ptát se|zeptat se|to ask",
     "ptát se, zeptat se"},
    {"Ich frage meine Schwester.",
     "zeptám se své sestry|ptám se své sestry|i ask my sister",
     "Zeptám se své sestry."},
    {"der Geburtstag, -e", "narozeniny|birthday", "narozeniny"},
    {"grillen (Infinitiv)", "grilovat|to grill|to barbecue", "grilovat"},
    {"die Idee, -n", "nápad|myšlenka|idea", "nápad, myšlenka"},
    {"die Köchin, -nen", "kuchařka|cook|chef", "kuchařka"},
    {"der Kuchen, -", "koláč|moučník|dort|cake", "koláč, moučník"},
    {"das Menü, -s", "menu|jídelní lístek", "menu"},
    {"5-Gänge-Menü", "pětichodové menu|pětchodové menu|five-course menu",
     "pětichodové menu"},
    {"ob", "zda|jestli|whether|if", "zda, jestli"},
    {"der Papa, -s", "tatínek|táta|dad|daddy", "tatínek"},
    {"die Party, -s", "party|večírek|oslava", "party, večírek"},
    {"das Restaurant", "restaurace|restaurant", "restaurace"},
    {"schrecklich", "strašný|hrozný|terrible|awful", "strašný, hrozný"},
    {"die Schwarzwälder Kirschtorte",
     "schwarzwaldský dort|black forest cake", "Schwarzwaldský dort"},
    {"singen (Infinitiv)", "zpívat|to sing", "zpívat"},
    {"Wir singen Happy Birthday.",
     "zpíváme happy birthday|we sing happy birthday",
     "Zpíváme Happy Birthday."},
    {"stimmen (Infinitiv)", "souhlasit|to be right|to be correct",
     "souhlasit"},
    {"Ja, stimmt.",
     "ano, správně|ano, souhlasí|yes, that's right|yes, right",
     "Ano, správně."},
    {"die Vorbereitung, -en", "příprava|preparation", "příprava"},
    {"wieder", "opět|zase|again", "opět, zase"},
    {"zu Hause", "doma|at home", "doma"},
};

const TypedQ u3_trans_p37[] = {
    {"das Chamäleon, -s", "chameleon|chameleón", "chameleon"},
    {"der Goldfisch, -e", "zlatá rybka|karas|goldfish",
     "zlatá rybka (karas)"},
    {"der Hamster, -", "křeček|hamster", "křeček"},
    {"der Kanarienvogel, Kanarienvögel", "kanárek|kanár|canary",
     "kanárek"},
    {"das Kaninchen, -", "králík|rabbit", "králík"},
    {"die Katze, -n", "kočka|cat", "kočka"},
    {"das Meerschweinchen, -", "morče|guinea pig", "morče"},
    {"das Pferd, -e", "kůň|horse", "kůň"},
    {"die Schildkröte, -n", "želva|turtle|tortoise", "želva"},
    {"das Tier, -e", "zvíře|animal", "zvíře"},
};

const TypedQ u3_trans_p38[] = {
    {"der Ausflug, Ausflüge", "výlet|trip|excursion", "výlet"},
    {"die Ausstellung, -en", "výstava|exhibition", "výstava"},
    {"die Burg, -en", "hrad|castle", "hrad"},
    {"cool", "cool|skvělý|super", "cool, skvělý"},
    {"es gibt", "je|existuje|there is|there are", "je, existuje"},
    {"das Hochzeitsfest, -e",
     "svatební party|svatební oslava|wedding party", "svatební party"},
    {"die Hochzeitstorte, -n", "svatební dort|wedding cake",
     "svatební dort"},
    {"nach", "do|to", "do"},
    {"Ich fahre nach Wien.",
     "jedu do vídně|i go to vienna|i'm going to vienna",
     "Jedu do Vídně."},
    {"nächster, nächste, nächstes", "příští|next", "příští"},
    {"nächsten Monat / nächste Woche / nächstes Jahr",
     "příští měsíc / příští týden / příští rok|"
     "příští měsíc, příští týden, příští rok|"
     "next month / next week / next year|next month, next week, next year",
     "příští měsíc / příští týden / příští rok"},
    {"das Schloss, Schlösser", "zámek|castle|palace", "zámek"},
    {"das Schwimmbad, Schwimmbäder",
     "koupaliště|plavecký bazén|swimming pool",
     "koupaliště, plavecký bazén"},
    {"der Schwimmbadbesuch, -e",
     "návštěva koupaliště|návštěva plaveckého bazénu|"
     "visit to the swimming pool",
     "návštěva koupaliště, plaveckého bazénu"},
    {"schwimmen (Infinitiv)", "plavat|to swim", "plavat"},
    {"Meine Mutter schwimmt gern.",
     "moje matka ráda plave|my mother likes swimming",
     "Moje matka ráda plave."},
    {"shoppen (Infinitiv)", "nakupovat|to shop", "nakupovat"},
    {"Wir gehen shoppen.", "jdeme nakupovat|jdem nakupovat|we go shopping",
     "Jdeme nakupovat."},
    {"das Sportevent, -s",
     "sportovní akce|sportovní událost|sports event",
     "sportovní akce (zápas, závody)"},
    {"der Vergnügungspark, -s",
     "zábavní park|amusement park|theme park", "zábavní park"},
    {"Wir gehen in den Vergnügungspark.",
     "jdeme do zábavního parku|we go to the amusement park",
     "Jdeme do zábavního parku."},
    {"wandern (Infinitiv)", "putovat|cestovat|to hike|to wander",
     "putovat, cestovat"},
    {"Ich wandere gern.",
     "rád(a) putuji|rád putuji|ráda putuji|i like hiking",
     "Rád(a) putuji."},
    {"die Woche, -n", "týden|week", "týden"},
    {"diese Woche", "tento týden|this week", "tento týden"},
    {"der Zoo, -s", "zoo|zoologická zahrada", "zoo"},
    {"der Zoobesuch, -e", "návštěva zoo|visit to the zoo", "návštěva zoo"},
};

const TypedQ u3_trans_p39[] = {
    {"das Einkaufszentrum",
     "nákupní centrum|shopping centre|shopping mall|shopping center",
     "nákupní centrum"},
    {"die Inliner (mn. č.)",
     "inline brusle|inline bruslení|inliners", "inline brusle"},
    {"Sie fährt Inliner.",
     "(ona) jezdí na inline bruslích|jezdí na inline bruslích|"
     "she goes inline skating", "(Ona) Jezdí na inline bruslích."},
    {"der See, -n", "jezero|lake", "jezero"},
    {"Am Wochenende fahren wir an einen See.",
     "o víkendu pojedeme k jezeru|o víkendu jedeme k jezeru|"
     "at the weekend we drive to a lake",
     "O víkendu pojedeme k (nějakému) jezeru."},
    {"das Wochenende, -n", "víkend|weekend", "víkend"},
    {"am Wochenende", "o víkendu|on the weekend", "o víkendu"},
    {"dieses Wochenende", "tento víkend|this weekend", "tento víkend"},
    {"das Wochenendhaus, Wochenendhäuser",
     "chata|chalupa|víkendový dům|weekend house", "chata, chalupa"},
    {"Wir fahren dieses Wochenende ins Wochenendhaus.",
     "tento víkend jedeme na chatu|tento víkend jedeme na chalupu|"
     "this weekend we go to the cottage", "Tento víkend jedeme na chatu."},
};

const TransSection u3_trans_sections[] = {
    {"Strana 32", u3_trans_p32, (int)G_N_ELEMENTS(u3_trans_p32)},
    {"Strana 33", u3_trans_p33, (int)G_N_ELEMENTS(u3_trans_p33)},
    {"Strana 34", u3_trans_p34, (int)G_N_ELEMENTS(u3_trans_p34)},
    {"Strana 35", u3_trans_p35, (int)G_N_ELEMENTS(u3_trans_p35)},
    {"Strana 36", u3_trans_p36, (int)G_N_ELEMENTS(u3_trans_p36)},
    {"Strana 37", u3_trans_p37, (int)G_N_ELEMENTS(u3_trans_p37)},
    {"Strana 38", u3_trans_p38, (int)G_N_ELEMENTS(u3_trans_p38)},
    {"Strana 39", u3_trans_p39, (int)G_N_ELEMENTS(u3_trans_p39)},
};
const int u3_trans_sections_n = (int)G_N_ELEMENTS(u3_trans_sections);
