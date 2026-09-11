#include "maturita.h"

/* ------------------------------------------------------------------ */
/* Progress                                                           */
/* ------------------------------------------------------------------ */

void unit_save_progress(UnitCtx *u) {
    GKeyFile *kf = g_key_file_new();
    for (int i = 1; i <= MAX_UNIT_EX; i++) {
        gchar key[8];
        g_snprintf(key, sizeof(key), "%d", i);
        g_key_file_set_boolean(kf, "done", key, u->done[i]);
    }

    if (g_mkdir_with_parents(PROGRESS_DIR, 0755) != 0) {
        g_key_file_free(kf);
        return;
    }

    gchar *data = g_key_file_to_data(kf, NULL, NULL);
    g_key_file_free(kf);

    if (data) {
        GError *err = NULL;
        g_file_set_contents(u->progress_file, data, -1, &err);
        if (err)
            g_error_free(err);
        g_free(data);
    }
}

void unit_load_progress(UnitCtx *u) {
    GKeyFile *kf = g_key_file_new();
    GError *err = NULL;

    if (!g_key_file_load_from_file(kf, u->progress_file, G_KEY_FILE_NONE, &err)) {
        if (err)
            g_error_free(err);
        g_key_file_free(kf);
        return;
    }

    for (int i = 1; i <= MAX_UNIT_EX; i++) {
        gchar key[8];
        g_snprintf(key, sizeof(key), "%d", i);
        u->done[i] = g_key_file_get_boolean(kf, "done", key, NULL);
    }

    g_key_file_free(kf);
}

void load_progress(void) {
    for (int i = 0; i < NUM_UNLOCKED; i++)
        unit_load_progress(&units[i]);
    net_load_progress();
    hw_load_progress();
}

/* Redraw the exercise rails of every unit that already has a page. */
void all_rails_redraw(void) {
    for (int i = 0; i < NUM_UNLOCKED; i++) {
        if (units[i].ex_rail)
            gtk_widget_queue_draw(units[i].ex_rail);
    }
}

void refresh_completion_ui(void) {
    for (int uidx = 0; uidx < NUM_UNLOCKED; uidx++) {
        UnitCtx *u = &units[uidx];
        gboolean all = TRUE;

        for (int i = 1; i <= u->n_ex; i++) {
            if (!u->done[i]) {
                all = FALSE;
                break;
            }
        }

        /* per-bubble markers */
        if (u->ex_cells[0] != NULL) {
            for (int i = 1; i <= u->n_ex; i++) {
                GtkWidget *btn = u->ex_cells[i - 1];
                if (!btn)
                    continue;
                if (u->done[i]) {
                    gtk_widget_add_css_class(btn, "done");
                    if (u->ex_icons[i])
                        gtk_widget_set_visible(u->ex_icons[i], TRUE);
                } else {
                    gtk_widget_remove_css_class(btn, "done");
                    if (u->ex_icons[i])
                        gtk_widget_set_visible(u->ex_icons[i], FALSE);
                }
            }
        }

        /* off-path branch marker */
        if (u->has_branch && u->branch_cell) {
            if (u->done[u->branch_ex]) {
                gtk_widget_add_css_class(u->branch_cell, "done");
                if (u->branch_icon)
                    gtk_widget_set_visible(u->branch_icon, TRUE);
            } else {
                gtk_widget_remove_css_class(u->branch_cell, "done");
                if (u->branch_icon)
                    gtk_widget_set_visible(u->branch_icon, FALSE);
            }
        }

        /* roadmap node marker */
        if (u->node) {
            if (all && u->n_ex > 0) {
                gtk_widget_remove_css_class(u->node, "current");
                gtk_widget_add_css_class(u->node, "done");
                if (u->node_done_icon)
                    gtk_widget_set_visible(u->node_done_icon, TRUE);
            } else {
                gtk_widget_remove_css_class(u->node, "done");
                gtk_widget_add_css_class(u->node, "current");
                if (u->node_done_icon)
                    gtk_widget_set_visible(u->node_done_icon, FALSE);
            }
        }
    }

    all_rails_redraw();
    refresh_net_completion_ui();
    refresh_hw_completion_ui();
    refresh_stats_ui();
}

void mark_done(UnitCtx *u, int n) {
    if (!u || n < 1 || n > MAX_UNIT_EX)
        return;
    u->done[n] = TRUE;
    refresh_completion_ui();
    unit_save_progress(u);
}

void net_load_progress(void) {
    GKeyFile *kf = g_key_file_new();
    GError *err = NULL;

    if (!g_key_file_load_from_file(kf, PROGRESS_NET, G_KEY_FILE_NONE, &err)) {
        if (err)
            g_error_free(err);
        g_key_file_free(kf);
        return;
    }

    for (int i = 0; i < NET_LESSONS; i++) {
        gchar key[8];

        g_snprintf(key, sizeof(key), "%d", i + 1);
        net_lessons[i].done = g_key_file_get_boolean(kf, "done", key, NULL);
    }

    g_key_file_free(kf);
}

void net_save_progress(void) {
    GKeyFile *kf = g_key_file_new();

    for (int i = 0; i < NET_LESSONS; i++) {
        gchar key[8];

        g_snprintf(key, sizeof(key), "%d", i + 1);
        g_key_file_set_boolean(kf, "done", key, net_lessons[i].done);
    }

    if (g_mkdir_with_parents(PROGRESS_DIR, 0755) != 0) {
        g_key_file_free(kf);
        return;
    }

    gchar *data = g_key_file_to_data(kf, NULL, NULL);
    g_key_file_free(kf);

    if (data) {
        GError *err = NULL;
        g_file_set_contents(PROGRESS_NET, data, -1, &err);
        if (err)
            g_error_free(err);
        g_free(data);
    }
}

void refresh_net_completion_ui(void) {
    for (int i = 0; i < NET_LESSONS; i++) {
        GtkWidget *node = net_nodes[i];
        GtkWidget *icon = net_lessons[i].done_icon;

        if (!node)
            continue;
        if (net_lessons[i].done) {
            gtk_widget_remove_css_class(node, "current");
            gtk_widget_add_css_class(node, "done");
            if (icon)
                gtk_widget_set_visible(icon, TRUE);
        } else {
            gtk_widget_remove_css_class(node, "done");
            gtk_widget_add_css_class(node, "current");
            if (icon)
                gtk_widget_set_visible(icon, FALSE);
        }
    }
}

void mark_net_done(int lesson_id) {
    if (lesson_id < 0 || lesson_id >= NET_LESSONS)
        return;
    if (net_lessons[lesson_id].done)
        return;
    net_lessons[lesson_id].done = TRUE;
    net_save_progress();
    refresh_net_completion_ui();
    refresh_stats_ui();
}

void progress_for_net(ProgressSum *out) {
    out->total_ex += NET_LESSONS;
    out->open_units += NET_LESSONS;
    for (int i = 0; i < NET_LESSONS; i++) {
        if (net_lessons[i].done) {
            out->done_ex++;
            out->done_units++;
        }
    }
}

void hw_load_progress(void) {
    GKeyFile *kf = g_key_file_new();
    GError *err = NULL;

    if (!g_key_file_load_from_file(kf, PROGRESS_HW, G_KEY_FILE_NONE, &err)) {
        if (err)
            g_error_free(err);
        g_key_file_free(kf);
        return;
    }

    for (int i = 0; i < HW_LESSONS; i++) {
        gchar key[8];

        g_snprintf(key, sizeof(key), "%d", i + 1);
        hw_lessons[i].done = g_key_file_get_boolean(kf, "done", key, NULL);
    }

    g_key_file_free(kf);
}

void hw_save_progress(void) {
    GKeyFile *kf = g_key_file_new();

    for (int i = 0; i < HW_LESSONS; i++) {
        gchar key[8];

        g_snprintf(key, sizeof(key), "%d", i + 1);
        g_key_file_set_boolean(kf, "done", key, hw_lessons[i].done);
    }

    if (g_mkdir_with_parents(PROGRESS_DIR, 0755) != 0) {
        g_key_file_free(kf);
        return;
    }

    gchar *data = g_key_file_to_data(kf, NULL, NULL);
    g_key_file_free(kf);

    if (data) {
        GError *err = NULL;
        g_file_set_contents(PROGRESS_HW, data, -1, &err);
        if (err)
            g_error_free(err);
        g_free(data);
    }
}

void refresh_hw_completion_ui(void) {
    for (int i = 0; i < HW_LESSONS; i++) {
        GtkWidget *node = hw_nodes[i];
        GtkWidget *icon = hw_lessons[i].done_icon;

        if (!node)
            continue;
        if (hw_lessons[i].done) {
            gtk_widget_remove_css_class(node, "current");
            gtk_widget_add_css_class(node, "done");
            if (icon)
                gtk_widget_set_visible(icon, TRUE);
        } else {
            gtk_widget_remove_css_class(node, "done");
            gtk_widget_add_css_class(node, "current");
            if (icon)
                gtk_widget_set_visible(icon, FALSE);
        }
    }
}

void mark_hw_done(int lesson_id) {
    if (lesson_id < 0 || lesson_id >= HW_LESSONS)
        return;
    if (hw_lessons[lesson_id].done)
        return;
    hw_lessons[lesson_id].done = TRUE;
    hw_save_progress();
    refresh_hw_completion_ui();
    refresh_stats_ui();
}

void progress_for_hw(ProgressSum *out) {
    out->total_ex += HW_LESSONS;
    out->open_units += HW_LESSONS;
    for (int i = 0; i < HW_LESSONS; i++) {
        if (hw_lessons[i].done) {
            out->done_ex++;
            out->done_units++;
        }
    }
}
