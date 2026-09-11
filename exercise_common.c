#include "maturita.h"

/* ------------------------------------------------------------------ */
/* Exercise page shell                                                */
/* ------------------------------------------------------------------ */

GtkWidget *ex_page_shell(const char *back_target, const char *title,
                                const char *subtitle, const char *btn_label,
                                GtkWidget **body_out, GtkWidget **feedback_out,
                                GtkWidget **btn_out) {
    GtkWidget *page = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    GtkWidget *scroll;
    GtkWidget *body;
    GtkWidget *bottom;
    GtkWidget *feedback;
    GtkWidget *btn;

    gtk_widget_set_margin_start(page, 32);
    gtk_widget_set_margin_end(page, 32);
    gtk_widget_set_margin_top(page, 24);
    gtk_widget_set_margin_bottom(page, 24);

    gtk_box_append(GTK_BOX(page), top_bar(back_target, title, subtitle));

    scroll = gtk_scrolled_window_new();
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll),
                                   GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_widget_set_vexpand(scroll, TRUE);
    gtk_widget_set_margin_top(scroll, 14);
    gtk_widget_set_margin_bottom(scroll, 14);
    gtk_box_append(GTK_BOX(page), scroll);

    body = gtk_box_new(GTK_ORIENTATION_VERTICAL, 14);
    gtk_widget_set_vexpand(body, TRUE);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), body);
    *body_out = body;

    bottom = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 12);
    gtk_widget_set_margin_top(bottom, 4);
    gtk_box_append(GTK_BOX(page), bottom);

    feedback = gtk_label_new("");
    gtk_widget_set_halign(feedback, GTK_ALIGN_START);
    gtk_widget_set_valign(feedback, GTK_ALIGN_CENTER);
    gtk_widget_set_hexpand(feedback, TRUE);
    gtk_box_append(GTK_BOX(bottom), feedback);
    *feedback_out = feedback;

    btn = gtk_button_new();
    i18n_bind(btn, btn_label, 1);
    gtk_widget_add_css_class(btn, "btn-primary");
    gtk_widget_set_valign(btn, GTK_ALIGN_CENTER);
    gtk_box_append(GTK_BOX(bottom), btn);
    *btn_out = btn;

    return page;
}

/* ------------------------------------------------------------------ */
/* Word-selection (combo box) exercise                                */
/* ------------------------------------------------------------------ */


/* Build a hidden translation label ("meaning") appended under an item. */
GtkWidget *meaning_add(GtkWidget *body, const char *text) {
    GtkWidget *lbl = gtk_label_new(NULL);

    i18n_bind(lbl, text, 0);
    gtk_label_set_xalign(GTK_LABEL(lbl), 0.0);
    gtk_widget_set_halign(lbl, GTK_ALIGN_START);
    gtk_widget_add_css_class(lbl, "meaning");
    gtk_label_set_wrap(GTK_LABEL(lbl), TRUE);
    gtk_widget_set_visible(lbl, FALSE);
    gtk_widget_set_margin_top(lbl, 2);
    gtk_widget_set_margin_bottom(lbl, 2);
    gtk_widget_set_margin_start(lbl, 8);
    gtk_box_append(GTK_BOX(body), lbl);
    return lbl;
}

/* A hidden green label that shows the model German sentence after Check. */
GtkWidget *model_answer_add(GtkWidget *body, const char *german) {
    GtkWidget *lbl = gtk_label_new(german);

    gtk_widget_set_halign(lbl, GTK_ALIGN_START);
    gtk_label_set_xalign(GTK_LABEL(lbl), 0.0);
    gtk_label_set_wrap(GTK_LABEL(lbl), TRUE);
    gtk_widget_set_margin_top(lbl, 2);
    gtk_widget_set_margin_bottom(lbl, 2);
    gtk_widget_set_margin_start(lbl, 8);
    gtk_widget_add_css_class(lbl, "u3-answer");
    gtk_widget_set_visible(lbl, FALSE);
    gtk_box_append(GTK_BOX(body), lbl);
    return lbl;
}

void meaning_reveal_all(GtkWidget **labels, int n) {
    for (int i = 0; i < n; i++)
        if (labels[i])
            gtk_widget_set_visible(labels[i], TRUE);
}

ComboListCtx *combo_list_ctx_new(UnitCtx *unit, int ex_num,
                                        GtkWidget *feedback) {
    ComboListCtx *ctx = g_new0(ComboListCtx, 1);
    ctx->combos = g_array_new(FALSE, FALSE, sizeof(GtkComboBox *));
    ctx->answers = g_array_new(FALSE, FALSE, sizeof(const char *));
    ctx->trans = g_array_new(FALSE, FALSE, sizeof(GtkWidget *));
    ctx->models = g_array_new(FALSE, FALSE, sizeof(GtkWidget *));
    ctx->unit = unit;
    ctx->ex_num = ex_num;
    ctx->feedback = feedback;
    return ctx;
}

void combo_list_add(ComboListCtx *ctx, GtkWidget *combo, const char *ans) {
    g_array_append_val(ctx->combos, combo);
    g_array_append_val(ctx->answers, ans);
}

void combo_list_add_trans(ComboListCtx *ctx, GtkWidget *label) {
    g_array_append_val(ctx->trans, label);
}

GtkWidget *make_word_combo(const char **words, int n) {
    GtkWidget *combo = gtk_combo_box_text_new();
    int *order = g_new0(int, n);

    for (int i = 0; i < n; i++)
        order[i] = i;
    shuffle_indices(order, n);
    for (int i = 0; i < n; i++)
        gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(combo), NULL,
                                  words[order[i]]);
    gtk_combo_box_set_active(GTK_COMBO_BOX(combo), -1);
    g_free(order);
    return combo;
}

gboolean answer_accepts(const char *sel_norm, const char *ans) {
    gchar **parts = g_strsplit(ans, "|", -1);
    gboolean ok = FALSE;

    for (int i = 0; parts[i] != NULL; i++) {
        gchar *a = normalize_answer(parts[i]);
        if (g_strcmp0(sel_norm, a) == 0) {
            ok = TRUE;
            g_free(a);
            break;
        }
        g_free(a);
    }

    g_strfreev(parts);
    return ok;
}

void combo_list_check(GtkButton *button, gpointer data) {
    ComboListCtx *ctx = data;
    guint total = ctx->combos->len;
    guint ok = 0;

    (void)button;

    for (guint i = 0; i < total; i++) {
        GtkComboBox *combo = g_array_index(ctx->combos, GtkComboBox *, i);
        const char *ans = g_array_index(ctx->answers, const char *, i);
        gchar *sel = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(combo));
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

    if (ctx->trans->len > 0) {
        for (guint i = 0; i < ctx->trans->len; i++) {
            GtkWidget *lbl = g_array_index(ctx->trans, GtkWidget *, i);
            if (lbl)
                gtk_widget_set_visible(lbl, TRUE);
        }
    }

    if (ctx->models->len > 0) {
        for (guint i = 0; i < ctx->models->len; i++) {
            GtkWidget *lbl = g_array_index(ctx->models, GtkWidget *, i);
            if (lbl)
                gtk_widget_set_visible(lbl, TRUE);
        }
    }

    if (ok == total) {
        set_feedback(ctx->feedback, TRUE, tr("feedback_ok"));
        mark_done(ctx->unit, ctx->ex_num);
    } else {
        set_feedback(ctx->feedback, FALSE, tr("feedback_retry"));
    }
}

/* ------------------------------------------------------------------ */
/* Word-arrangement (chips) exercise                                  */
/* ------------------------------------------------------------------ */



void sent_rebuild(SentBuilder *sb) {
    for (int i = 0; i < sb->n_words; i++) {
        GtkWidget *parent = gtk_widget_get_parent(sb->chips[i]);
        if (parent != NULL && GTK_IS_FLOW_BOX_CHILD(parent))
            gtk_flow_box_child_set_child(GTK_FLOW_BOX_CHILD(parent), NULL);
    }

    flow_clear(GTK_FLOW_BOX(sb->pool));
    flow_clear(GTK_FLOW_BOX(sb->target));

    for (int i = 0; i < sb->placed_count; i++)
        gtk_flow_box_insert(GTK_FLOW_BOX(sb->target),
                            sb->chips[sb->placed[i]], -1);

    for (int i = 0; i < sb->n_words; i++) {
        int w = sb->pool_order[i];
        gboolean in_target = FALSE;
        for (int j = 0; j < sb->placed_count; j++) {
            if (sb->placed[j] == w) {
                in_target = TRUE;
                break;
            }
        }
        if (!in_target)
            gtk_flow_box_insert(GTK_FLOW_BOX(sb->pool), sb->chips[w], -1);
    }
}


GdkContentProvider *asm_drag_prepare(GtkDragSource *source,
                                            double x, double y,
                                            gpointer data) {
    ChipRef *ref = data;
    (void)source;
    (void)x;
    (void)y;
    return gdk_content_provider_new_typed(G_TYPE_INT, ref->idx);
}

gboolean asm_rebuild_idle(gpointer data) {
    sent_rebuild(data);
    return G_SOURCE_REMOVE;
}

void asm_drag_end(GtkDragSource *source, GdkDrag *drag,
                         gboolean delete_data, gpointer data) {
    ChipRef *ref = data;
    (void)source;
    (void)drag;
    (void)delete_data;
    g_idle_add(asm_rebuild_idle, ref->sb);
}

gboolean asm_drop_to_sentence(GtkDropTarget *target, const GValue *value,
                                     double x, double y, gpointer data) {
    SentBuilder *sb = data;
    int idx;
    int pos = -1;

    (void)target;
    (void)x;
    (void)y;

    if (!G_VALUE_HOLDS_INT(value))
        return FALSE;

    idx = g_value_get_int(value);
    for (int i = 0; i < sb->placed_count; i++) {
        if (sb->placed[i] == idx) {
            pos = i;
            break;
        }
    }
    if (pos >= 0) {
        for (int i = pos; i < sb->placed_count - 1; i++)
            sb->placed[i] = sb->placed[i + 1];
        sb->placed_count--;
    }
    sb->placed[sb->placed_count++] = idx;
    return TRUE;
}

gboolean asm_drop_to_pool(GtkDropTarget *target, const GValue *value,
                                 double x, double y, gpointer data) {
    SentBuilder *sb = data;
    int idx;
    int pos = -1;

    (void)target;
    (void)x;
    (void)y;

    if (!G_VALUE_HOLDS_INT(value))
        return FALSE;

    idx = g_value_get_int(value);
    for (int i = 0; i < sb->placed_count; i++) {
        if (sb->placed[i] == idx) {
            pos = i;
            break;
        }
    }
    if (pos < 0)
        return FALSE;

    for (int i = pos; i < sb->placed_count - 1; i++)
        sb->placed[i] = sb->placed[i + 1];
    sb->placed_count--;
    return TRUE;
}

/* ------------------------------------------------------------------ */
/* Click-to-move with a fly ("glide") animation                        */
/* ------------------------------------------------------------------ */


void asm_finish_fly(FlyCtx *fc) {
    gtk_widget_remove_css_class(fc->chip, "ghost-host");
    gtk_fixed_remove(GTK_FIXED(fc->stage), fc->ghost);
    if (fc->flying)
        *fc->flying = FALSE;
    g_free(fc);
}

gboolean asm_fly_tick(gpointer data) {
    FlyCtx *fc = data;
    gint64 now = g_get_monotonic_time() / 1000;

    if (fc->t0 < 0) {
        double nx = 0.0, ny = 0.0;
        gboolean ok;

        fc->wait++;
        ok = gtk_widget_translate_coordinates(fc->chip, fc->stage,
                                              0, 0, &nx, &ny);
        if (fc->wait < 2)
            return G_SOURCE_CONTINUE;

        if (ok && fc->wait < 14 &&
            fabs(nx - fc->sx) + fabs(ny - fc->sy) < 0.5)
            return G_SOURCE_CONTINUE;   /* still waiting for relayout   */

        if (ok) {
            fc->tx = nx;
            fc->ty = ny;
        } else {
            fc->tx = fc->sx;
            fc->ty = fc->sy;
        }
        fc->t0 = now;
        return G_SOURCE_CONTINUE;
    }

    {
        double t = (double)(now - fc->t0) / 240.0;
        double e, x, y;

        if (t > 1.0)
            t = 1.0;
        e = 1.0 - pow(1.0 - t, 3.0);   /* ease-out cubic                */
        x = fc->sx + (fc->tx - fc->sx) * e;
        y = fc->sy + (fc->ty - fc->sy) * e;
        gtk_fixed_move(GTK_FIXED(fc->stage), fc->ghost, (int)x, (int)y);

        if (t >= 1.0) {
            asm_finish_fly(fc);
            return G_SOURCE_REMOVE;
        }
    }
    return G_SOURCE_CONTINUE;
}

/* Start a fly animation: hide `chip` in place and animate a ghost from
 * (sx, sy) to the chip's new location. */
void asm_begin_fly(GtkWidget *chip, GtkWidget *stage,
                          gboolean *flying, double sx, double sy) {
    GtkWidget *child = gtk_button_get_child(GTK_BUTTON(chip));
    const char *txt = (child && GTK_IS_LABEL(child))
                          ? gtk_label_get_text(GTK_LABEL(child)) : NULL;
    GtkWidget *ghost = gtk_button_new_with_label(txt ? txt : "");
    FlyCtx *fc;

    gtk_widget_add_css_class(ghost, "chip");
    gtk_widget_set_sensitive(ghost, FALSE);
    gtk_widget_set_can_focus(ghost, FALSE);
    gtk_fixed_put(GTK_FIXED(stage), ghost, (int)sx, (int)sy);

    fc = g_new0(FlyCtx, 1);
    fc->chip = chip;
    fc->ghost = ghost;
    fc->stage = stage;
    fc->flying = flying;
    fc->sx = sx;
    fc->sy = sy;
    fc->t0 = -1;

    if (flying)
        *flying = TRUE;
    gtk_widget_add_css_class(chip, "ghost-host");

    g_timeout_add(16, asm_fly_tick, fc);
}

void asm_move_by_click(GtkButton *button, gpointer data) {
    ChipRef *ref = data;
    SentBuilder *sb = ref->sb;
    int idx = ref->idx;
    int pos = -1;
    double sx = 0.0, sy = 0.0;
    GtkWidget *chip = GTK_WIDGET(button);

    (void)button;

    if (sb->flying)
        return;

    for (int i = 0; i < sb->placed_count; i++) {
        if (sb->placed[i] == idx) {
            pos = i;
            break;
        }
    }

    if (sb->stage &&
        gtk_widget_translate_coordinates(chip, sb->stage, 0, 0, &sx, &sy)) {
        if (pos >= 0) {
            for (int i = pos; i < sb->placed_count - 1; i++)
                sb->placed[i] = sb->placed[i + 1];
            sb->placed_count--;
        } else {
            sb->placed[sb->placed_count++] = idx;
        }

        sent_rebuild(sb);
        asm_begin_fly(chip, sb->stage, &sb->flying, sx, sy);
    } else {
        /* No usable animation layer yet – just move instantly. */
        if (pos >= 0) {
            for (int i = pos; i < sb->placed_count - 1; i++)
                sb->placed[i] = sb->placed[i + 1];
            sb->placed_count--;
        } else {
            sb->placed[sb->placed_count++] = idx;
        }
        sent_rebuild(sb);
    }
}


void assembly_check(GtkButton *button, gpointer data) {
    AsmCtx *ctx = data;
    int ok = 0;

    (void)button;

    for (int i = 0; i < ctx->n; i++) {
        SentBuilder *sb = ctx->sbs[i];
        gboolean good = (sb->placed_count == sb->n_words);
        if (good) {
            for (int j = 0; j < sb->placed_count; j++) {
                if (sb->placed[j] != j) {
                    good = FALSE;
                    break;
                }
            }
        }
        if (good) {
            ok++;
            gtk_widget_remove_css_class(sb->group, "wrong");
            gtk_widget_add_css_class(sb->group, "ok");
        } else {
            gtk_widget_remove_css_class(sb->group, "ok");
            gtk_widget_add_css_class(sb->group, "wrong");
        }
    }

    for (int i = 0; i < ctx->n; i++) {
        if (ctx->sbs[i]->trans)
            gtk_widget_set_visible(ctx->sbs[i]->trans, TRUE);
    }

    if (ok == ctx->n) {
        set_feedback(ctx->feedback, TRUE, tr("feedback_ok"));
        mark_done(ctx->unit, ctx->ex_num);
    } else {
        set_feedback(ctx->feedback, FALSE, tr("feedback_sentences"));
    }
}

GtkWidget *build_assembly(UnitCtx *unit, const char *title,
                                 const char *subtitle, int ex_num,
                                 const AssemblyItem *items,
                                 const char **meanings, int n) {
    GtkWidget *body, *feedback, *check;
    GtkWidget *page = ex_page_shell(unit->page, title, subtitle, "check",
                                    &body, &feedback, &check);
    AsmCtx *ctx = g_new0(AsmCtx, 1);

    ctx->sbs = g_new0(SentBuilder *, n);
    ctx->n = n;
    ctx->unit = unit;
    ctx->ex_num = ex_num;
    ctx->feedback = feedback;

    for (int s = 0; s < n; s++) {
        const AssemblyItem *item = &items[s];
        SentBuilder *sb = g_new0(SentBuilder, 1);
        GtkWidget *holder;
        GtkWidget *group;
        GtkWidget *target;
        GtkWidget *pool;
        GtkWidget *stage;

        sb->n_words = item->n;
        ctx->sbs[s] = sb;

        if (item->prompt) {
            GtkWidget *prompt = gtk_label_new(item->prompt);
            gtk_widget_set_halign(prompt, GTK_ALIGN_START);
            gtk_widget_add_css_class(prompt, "chain");
            gtk_box_append(GTK_BOX(body), prompt);
        }

        holder = gtk_overlay_new();
        gtk_widget_set_hexpand(holder, TRUE);

        group = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
        gtk_widget_add_css_class(group, "sent-group");
        sb->group = group;
        gtk_overlay_set_child(GTK_OVERLAY(holder), group);

        target = gtk_flow_box_new();
        gtk_flow_box_set_selection_mode(GTK_FLOW_BOX(target), GTK_SELECTION_NONE);
        gtk_flow_box_set_min_children_per_line(GTK_FLOW_BOX(target), 2);
        gtk_widget_set_size_request(target, -1, 44);
        sb->target = target;
        gtk_box_append(GTK_BOX(group), target);

        pool = gtk_flow_box_new();
        gtk_flow_box_set_selection_mode(GTK_FLOW_BOX(pool), GTK_SELECTION_NONE);
        gtk_flow_box_set_min_children_per_line(GTK_FLOW_BOX(pool), 2);
        gtk_widget_set_size_request(pool, -1, 44);
        sb->pool = pool;
        gtk_box_append(GTK_BOX(group), pool);

        stage = gtk_fixed_new();
        gtk_widget_set_halign(stage, GTK_ALIGN_FILL);
        gtk_widget_set_valign(stage, GTK_ALIGN_FILL);
        gtk_widget_set_can_target(stage, FALSE);
        gtk_overlay_add_overlay(GTK_OVERLAY(holder), stage);
        sb->stage = stage;

        {
            GtkDropTarget *dt = gtk_drop_target_new(G_TYPE_INT, GDK_ACTION_COPY);
            GtkDropTarget *dp = gtk_drop_target_new(G_TYPE_INT, GDK_ACTION_COPY);
            g_signal_connect(dt, "drop", G_CALLBACK(asm_drop_to_sentence), sb);
            gtk_widget_add_controller(target, GTK_EVENT_CONTROLLER(dt));
            g_signal_connect(dp, "drop", G_CALLBACK(asm_drop_to_pool), sb);
            gtk_widget_add_controller(pool, GTK_EVENT_CONTROLLER(dp));
        }

        for (int w = 0; w < item->n; w++) {
            GtkWidget *chip = gtk_button_new_with_label(item->words[w]);
            GtkDragSource *src = gtk_drag_source_new();
            ChipRef *ref = g_new(ChipRef, 1);
            gtk_widget_add_css_class(chip, "chip");
            g_object_ref(chip);
            sb->chips[w] = chip;
            ref->sb = sb;
            ref->idx = w;
            gtk_drag_source_set_actions(src, GDK_ACTION_COPY);
            g_signal_connect(src, "prepare", G_CALLBACK(asm_drag_prepare), ref);
            g_signal_connect(src, "drag-end", G_CALLBACK(asm_drag_end), ref);
            g_signal_connect(chip, "clicked", G_CALLBACK(asm_move_by_click), ref);
            gtk_widget_add_controller(chip, GTK_EVENT_CONTROLLER(src));
        }

        {
            for (int w = 0; w < item->n; w++)
                sb->pool_order[w] = w;
            shuffle_indices(sb->pool_order, item->n);
            for (int w = 0; w < item->n; w++)
                gtk_flow_box_insert(GTK_FLOW_BOX(pool),
                                    sb->chips[sb->pool_order[w]], -1);
        }

        gtk_box_append(GTK_BOX(body), holder);

        if (meanings[s])
            sb->trans = meaning_add(body, meanings[s]);
    }

    g_signal_connect(check, "clicked", G_CALLBACK(assembly_check), ctx);
    return page;
}

/* ------------------------------------------------------------------ */
/* Exercise 1: Dialog (word selection)                                */
/* ------------------------------------------------------------------ */



const DialogRow ex1_d1[] = {
    {"A:", "Hallo! Ich ", "bin|heiße", " Anna."},
    {"B:", "Hallo! Ich ", "bin|heiße", " Peter."},
    {"A:", "Woher ", "kommst", " du?"},
    {"B:", "Ich komme ", "aus", " Tschechien."},
};

const DialogRow ex1_d2[] = {
    {"A:", "Guten ", "Morgen", "!"},
    {"B:", "Wie ", "geht", " es dir?"},
    {"A:", "Mir geht es gut, ", "danke", "."},
};

const DialogRow ex1_d3[] = {
    {"A:", "Ich muss gehen. Auf ", "Wiedersehen", "!"},
    {"B:", "", "Bis", " bald!"},
    {"A:", "", "Tschüss", "!"},
};

const Dialogue ex1_dialogues[] = {
    {"Sich vorstellen", ex1_d1, G_N_ELEMENTS(ex1_d1)},
    {"Begrüßung", ex1_d2, G_N_ELEMENTS(ex1_d2)},
    {"Verabschiedung", ex1_d3, G_N_ELEMENTS(ex1_d3)},
};

const char *ex1_pool[] = {
    "bin", "heiße", "kommst", "aus", "Morgen",
    "geht", "danke", "Wiedersehen", "Bis", "Tschüss",
};

/* One Czech meaning per dialog line, in the same order as the rows. */
const char *ex1_meaning[] = {
    "Ahoj! Jmenuji se Anna.",
    "Ahoj! Jmenuji se Petr.",
    "Odkud jsi?",
    "Pocházím z Česka.",
    "Dobré ráno!",
    "Jak se máš?",
    "Mám se dobře, děkuji.",
    "Musím jít. Na shledanou!",
    "Brzy na viděnou!",
    "Čau!",
};

GtkWidget *build_ex1(UnitCtx *unit) {
    GtkWidget *body, *feedback, *check;
    GtkWidget *page = ex_page_shell(unit->page, "Dialog",
                                    "sub_dialog",
                                    "check", &body, &feedback, &check);
    ComboListCtx *ctx = combo_list_ctx_new(unit, 1, feedback);
    int ri = 0;

    for (guint d = 0; d < G_N_ELEMENTS(ex1_dialogues); d++) {
        const Dialogue *diag = &ex1_dialogues[d];
        GtkWidget *name = gtk_label_new(diag->name);
        gtk_widget_set_halign(name, GTK_ALIGN_START);
        gtk_widget_add_css_class(name, "ex-sub");
        gtk_widget_set_margin_top(name, d > 0 ? 10 : 0);
        gtk_box_append(GTK_BOX(body), name);

        for (int r = 0; r < diag->n; r++) {
            const DialogRow *row = &diag->rows[r];
            GtkWidget *line = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
            GtkWidget *combo;
            gtk_widget_set_halign(line, GTK_ALIGN_START);
            gtk_box_append(GTK_BOX(body), line);

            if (row->speaker && row->speaker[0]) {
                GtkWidget *spk = gtk_label_new(row->speaker);
                gtk_widget_add_css_class(spk, "dlg-speaker");
                gtk_widget_set_valign(spk, GTK_ALIGN_CENTER);
                gtk_box_append(GTK_BOX(line), spk);
            }
            if (row->before && row->before[0]) {
                GtkWidget *lb = gtk_label_new(row->before);
                gtk_widget_set_valign(lb, GTK_ALIGN_CENTER);
                gtk_widget_add_css_class(lb, "ex-prompt");
                gtk_box_append(GTK_BOX(line), lb);
            }

            combo = make_word_combo(ex1_pool, G_N_ELEMENTS(ex1_pool));
            gtk_widget_set_size_request(combo, 130, -1);
            gtk_widget_set_valign(combo, GTK_ALIGN_CENTER);
            gtk_box_append(GTK_BOX(line), combo);
            combo_list_add(ctx, combo, row->answer);

            if (row->after && row->after[0]) {
                GtkWidget *la = gtk_label_new(row->after);
                gtk_widget_set_valign(la, GTK_ALIGN_CENTER);
                gtk_widget_add_css_class(la, "ex-prompt");
                gtk_box_append(GTK_BOX(line), la);
            }

            if (ri < (int)G_N_ELEMENTS(ex1_meaning))
                combo_list_add_trans(ctx, meaning_add(body, ex1_meaning[ri]));
            ri++;
        }
    }

    g_signal_connect(check, "clicked", G_CALLBACK(combo_list_check), ctx);
    return page;
}

/* ------------------------------------------------------------------ */
/* Exercise 2: Sätze bilden (word arrangement)                        */
/* ------------------------------------------------------------------ */

const AssemblyItem ex2_items[] = {
    {NULL, {"Ich", "heiße", "Anna", "."}, 4},
    {NULL, {"Woher", "kommst", "du", "?"}, 4},
    {NULL, {"Ich", "komme", "aus", "Tschechien", "."}, 5},
    {NULL, {"Wie", "geht", "es", "dir", "?"}, 5},
};

const char *ex2_meaning[] = {
    "Jmenuji se Anna.",
    "Odkud jsi?",
    "Pocházím z Česka.",
    "Jak se máš?",
};
