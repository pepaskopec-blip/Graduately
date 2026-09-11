#include "maturita.h"

/* ------------------------------------------------------------------ */
/* Statistics                                                         */
/* ------------------------------------------------------------------ */

GtkWidget *make_back_button(const char *target);
void on_nav_clicked(GtkButton *button, gpointer user_data);


StatsUi stats_ui;
char *stats_return_page;

int unit_done_count(const UnitCtx *u) {
    int n = 0;

    if (!u)
        return 0;
    for (int i = 1; i <= u->n_ex; i++) {
        if (u->done[i])
            n++;
    }
    return n;
}


/* Sum progress over units[first .. first+n-1]. Locked placeholder units and
 * the finish node (no playable exercises) are ignored. */
void progress_for_units(int first, int n, ProgressSum *out) {
    for (int j = 0; j < n; j++) {
        UnitCtx *u;
        int d;

        if (first < 0 || first + j >= NUM_UNITS)
            continue;
        u = &units[first + j];
        if (!u->unlocked || u->n_ex <= 0)
            continue;
        d = unit_done_count(u);
        out->done_ex += d;
        out->total_ex += u->n_ex;
        out->open_units++;
        if (d == u->n_ex)
            out->done_units++;
    }
}

void refresh_stats_ui(void) {
    ProgressSum all = {0};
    int pct;
    char *tmp;

    if (!stats_ui.ex_value)
        return;

    for (int s = 0; s < NUM_SUBJECTS; s++) {
        if (s == NET_SUBJ)
            progress_for_net(&all);
        else
            progress_for_units(sub_unit_start[s], sub_unit_count[s], &all);
    }

    pct = all.total_ex > 0 ? (all.done_ex * 100) / all.total_ex : 0;

    tmp = g_strdup_printf(tr("stats_ex_fmt"), all.done_ex, all.total_ex);
    gtk_label_set_text(GTK_LABEL(stats_ui.ex_value), tmp);
    g_free(tmp);

    tmp = g_strdup_printf(tr("stats_pct_fmt"), pct);
    gtk_label_set_text(GTK_LABEL(stats_ui.pct_value), tmp);
    g_free(tmp);

    tmp = g_strdup_printf(tr("stats_units_fmt"), all.done_units,
                          all.open_units);
    gtk_label_set_text(GTK_LABEL(stats_ui.units_value), tmp);
    g_free(tmp);

    gtk_progress_bar_set_fraction(
        GTK_PROGRESS_BAR(stats_ui.overall_bar),
        all.total_ex > 0 ? (double)all.done_ex / (double)all.total_ex : 0.0);

    for (int s = 0; s < NUM_SUBJECTS; s++) {
        GtkWidget *count = stats_ui.subj[s].count;
        GtkWidget *bar = stats_ui.subj[s].bar;
        ProgressSum sp = {0};
        gboolean has_content = (s == NET_SUBJ) || sub_unit_count[s] > 0;

        if (!count || !bar)
            continue;
        if (s == NET_SUBJ)
            progress_for_net(&sp);
        else
            progress_for_units(sub_unit_start[s], sub_unit_count[s], &sp);
        if (has_content) {
            tmp = g_strdup_printf(tr("stats_ex_fmt"), sp.done_ex, sp.total_ex);
            gtk_label_set_text(GTK_LABEL(count), tmp);
            g_free(tmp);
            gtk_progress_bar_set_fraction(
                GTK_PROGRESS_BAR(bar),
                sp.total_ex > 0 ? (double)sp.done_ex / (double)sp.total_ex
                                : 0.0);
        } else {
            gtk_label_set_text(GTK_LABEL(count), tr("stats_locked"));
            gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(bar), 0.0);
        }
    }

    for (int i = 0; i < NUM_UNITS; i++) {
        UnitCtx *u = &units[i];
        GtkWidget *count = stats_ui.unit_count[i];
        GtkWidget *badge = stats_ui.unit_badge[i];
        GtkWidget *bar = stats_ui.unit_bar[i];
        GtkWidget *row = stats_ui.unit_row[i];
        gboolean unlocked = u->unlocked && u->n_ex > 0;
        int d = unlocked ? unit_done_count(u) : 0;
        gboolean complete = unlocked && d == u->n_ex;

        if (!count || !bar || !row)
            continue;

        gtk_widget_remove_css_class(row, "done");
        gtk_widget_remove_css_class(row, "locked");
        gtk_widget_remove_css_class(bar, "done");

        if (!unlocked) {
            gtk_widget_add_css_class(row, "locked");
            gtk_label_set_text(GTK_LABEL(count), tr("stats_locked"));
            gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(bar), 0.0);
            if (badge)
                gtk_widget_set_visible(badge, FALSE);
            continue;
        }

        tmp = g_strdup_printf(tr("stats_ex_fmt"), d, u->n_ex);
        gtk_label_set_text(GTK_LABEL(count), tmp);
        g_free(tmp);
        gtk_progress_bar_set_fraction(
            GTK_PROGRESS_BAR(bar),
            u->n_ex > 0 ? (double)d / (double)u->n_ex : 0.0);

        if (complete) {
            gtk_widget_add_css_class(row, "done");
            gtk_widget_add_css_class(bar, "done");
            if (badge) {
                gtk_label_set_text(GTK_LABEL(badge), tr("stats_complete"));
                gtk_widget_set_visible(badge, TRUE);
            }
        } else if (badge) {
            gtk_widget_set_visible(badge, FALSE);
        }
    }
}

void on_stats_back_clicked(GtkButton *button, gpointer user_data) {
    const char *target = stats_return_page;

    (void)button;
    (void)user_data;
    if (!target || !target[0])
        target = "subjects";
    gtk_stack_set_visible_child_name(main_stack, target);
}

void on_stats_clicked(GtkButton *button, gpointer user_data) {
    const char *cur;

    (void)button;
    (void)user_data;

    cur = gtk_stack_get_visible_child_name(main_stack);
    if (cur && g_strcmp0(cur, "stats") != 0) {
        g_free(stats_return_page);
        stats_return_page = g_strdup(cur);
    }
    refresh_stats_ui();
    gtk_stack_set_visible_child_name(main_stack, "stats");
}

GtkWidget *stats_metric_card(const char *label_key, GtkWidget **value_out) {
    GtkWidget *card;
    GtkWidget *value;
    GtkWidget *label;

    card = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_add_css_class(card, "stats-metric");
    gtk_widget_set_hexpand(card, TRUE);

    value = gtk_label_new("0");
    gtk_widget_set_halign(value, GTK_ALIGN_START);
    gtk_widget_add_css_class(value, "stats-metric-value");
    gtk_box_append(GTK_BOX(card), value);

    label = gtk_label_new(NULL);
    i18n_bind(label, label_key, 0);
    gtk_widget_set_halign(label, GTK_ALIGN_START);
    gtk_widget_add_css_class(label, "stats-metric-label");
    gtk_box_append(GTK_BOX(card), label);

    *value_out = value;
    return card;
}

/* Build one unit row (index, name, badge, count, bar) for the stats list.
 * `num` is the row number shown (1-based within its subject). */
GtkWidget *make_unit_stats_row(UnitCtx *u, int num, int global) {
    GtkWidget *row;
    GtkWidget *head;
    GtkWidget *left;
    GtkWidget *idx;
    GtkWidget *name;
    GtkWidget *right;
    GtkWidget *count;
    GtkWidget *badge;
    GtkWidget *bar;
    char text[8];

    row = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
    gtk_widget_add_css_class(row, "stats-unit-row");
    stats_ui.unit_row[global] = row;

    head = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_box_append(GTK_BOX(row), head);

    left = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
    gtk_widget_set_hexpand(left, TRUE);
    gtk_box_append(GTK_BOX(head), left);

    g_snprintf(text, sizeof(text), "%d", num);
    idx = gtk_label_new(text);
    gtk_widget_add_css_class(idx, "stats-unit-index");
    gtk_widget_set_valign(idx, GTK_ALIGN_CENTER);
    gtk_box_append(GTK_BOX(left), idx);

    name = gtk_label_new(u->title);
    gtk_widget_set_halign(name, GTK_ALIGN_START);
    gtk_widget_set_hexpand(name, TRUE);
    gtk_label_set_xalign(GTK_LABEL(name), 0.0);
    gtk_label_set_ellipsize(GTK_LABEL(name), PANGO_ELLIPSIZE_END);
    gtk_widget_add_css_class(name, "stats-unit-name");
    gtk_box_append(GTK_BOX(left), name);

    right = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
    gtk_widget_set_valign(right, GTK_ALIGN_CENTER);
    gtk_box_append(GTK_BOX(head), right);

    badge = gtk_label_new(NULL);
    gtk_widget_add_css_class(badge, "stats-unit-badge");
    gtk_widget_set_visible(badge, FALSE);
    gtk_box_append(GTK_BOX(right), badge);
    stats_ui.unit_badge[global] = badge;

    count = gtk_label_new("");
    gtk_widget_add_css_class(count, "stats-unit-count");
    gtk_box_append(GTK_BOX(right), count);
    stats_ui.unit_count[global] = count;

    bar = gtk_progress_bar_new();
    gtk_widget_add_css_class(bar, "stats-unit-bar");
    gtk_widget_set_hexpand(bar, TRUE);
    gtk_box_append(GTK_BOX(row), bar);
    stats_ui.unit_bar[global] = bar;

    return row;
}

GtkWidget *build_stats_page(void) {
    GtkWidget *outer;
    GtkWidget *scroll;
    GtkWidget *page;
    GtkWidget *top;
    GtkWidget *back;
    GtkWidget *center;
    GtkWidget *heading;
    GtkWidget *sub;
    GtkWidget *summary;
    GtkWidget *section;
    GtkWidget *list;

    outer = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_add_css_class(outer, "stats-page");

    top = gtk_center_box_new();
    gtk_widget_set_margin_top(top, 2);
    gtk_widget_set_margin_bottom(top, 6);
    gtk_box_append(GTK_BOX(outer), top);

    back = make_back_button("subjects");
    g_signal_handlers_disconnect_matched(back, G_SIGNAL_MATCH_FUNC,
                                         0, 0, NULL, on_nav_clicked, NULL);
    g_signal_connect(back, "clicked", G_CALLBACK(on_stats_back_clicked), NULL);
    gtk_center_box_set_start_widget(GTK_CENTER_BOX(top), back);

    center = gtk_box_new(GTK_ORIENTATION_VERTICAL, 4);
    gtk_widget_set_valign(center, GTK_ALIGN_CENTER);
    gtk_center_box_set_center_widget(GTK_CENTER_BOX(top), center);

    heading = gtk_label_new(NULL);
    i18n_bind(heading, "stats_title", 0);
    gtk_widget_set_halign(heading, GTK_ALIGN_CENTER);
    gtk_widget_add_css_class(heading, "roadmap-title");
    gtk_box_append(GTK_BOX(center), heading);

    sub = gtk_label_new(NULL);
    i18n_bind(sub, "stats_sub", 0);
    gtk_widget_set_halign(sub, GTK_ALIGN_CENTER);
    gtk_widget_add_css_class(sub, "roadmap-sub");
    gtk_label_set_wrap(GTK_LABEL(sub), TRUE);
    gtk_box_append(GTK_BOX(center), sub);

    scroll = gtk_scrolled_window_new();
    gtk_widget_set_vexpand(scroll, TRUE);
    gtk_widget_set_hexpand(scroll, TRUE);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll),
                                   GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
    gtk_box_append(GTK_BOX(outer), scroll);

    page = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_halign(page, GTK_ALIGN_FILL);
    gtk_widget_set_margin_start(page, 4);
    gtk_widget_set_margin_end(page, 4);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), page);

    summary = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_widget_add_css_class(summary, "stats-summary");
    gtk_widget_set_halign(summary, GTK_ALIGN_FILL);
    gtk_box_append(GTK_BOX(page), summary);

    gtk_box_append(GTK_BOX(summary),
                   stats_metric_card("stats_ex_label", &stats_ui.ex_value));
    gtk_box_append(GTK_BOX(summary),
                   stats_metric_card("stats_pct_label", &stats_ui.pct_value));
    gtk_box_append(GTK_BOX(summary),
                   stats_metric_card("stats_units_label",
                                     &stats_ui.units_value));

    stats_ui.overall_bar = gtk_progress_bar_new();
    gtk_widget_add_css_class(stats_ui.overall_bar, "stats-overall-bar");
    gtk_widget_set_hexpand(stats_ui.overall_bar, TRUE);
    gtk_box_append(GTK_BOX(page), stats_ui.overall_bar);

    section = gtk_label_new(NULL);
    i18n_bind(section, "stats_section", 0);
    gtk_widget_set_halign(section, GTK_ALIGN_START);
    gtk_widget_add_css_class(section, "stats-section");
    gtk_box_append(GTK_BOX(page), section);

    list = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_box_append(GTK_BOX(page), list);

    for (int s = 0; s < NUM_SUBJECTS; s++) {
        GtkWidget *subject;
        GtkWidget *head;
        GtkWidget *name;
        GtkWidget *count;
        GtkWidget *bar;
        gboolean has_units = (s == NET_SUBJ) || sub_unit_count[s] > 0;
        gboolean has_deutsch_units = sub_unit_count[s] > 0;

        subject = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
        gtk_widget_add_css_class(subject, "stats-subject");
        gtk_box_append(GTK_BOX(list), subject);

        head = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
        gtk_box_append(GTK_BOX(subject), head);

        name = gtk_label_new(NULL);
        i18n_bind(name, sub_keys[s], 0);
        gtk_widget_set_halign(name, GTK_ALIGN_START);
        gtk_widget_set_hexpand(name, TRUE);
        gtk_label_set_xalign(GTK_LABEL(name), 0.0);
        gtk_label_set_ellipsize(GTK_LABEL(name), PANGO_ELLIPSIZE_END);
        gtk_widget_add_css_class(name, "stats-subject-name");
        if (!has_units)
            gtk_widget_add_css_class(name, "unit-name-locked");
        gtk_box_append(GTK_BOX(head), name);
        stats_ui.subj[s].name = name;

        count = gtk_label_new(NULL);
        gtk_widget_add_css_class(count, "stats-subject-count");
        if (!has_units)
            gtk_widget_add_css_class(count, "locked");
        gtk_box_append(GTK_BOX(head), count);
        stats_ui.subj[s].count = count;

        bar = gtk_progress_bar_new();
        gtk_widget_add_css_class(bar, "stats-subject-bar");
        gtk_widget_set_hexpand(bar, TRUE);
        gtk_box_append(GTK_BOX(subject), bar);
        stats_ui.subj[s].bar = bar;

        if (has_deutsch_units) {
            GtkWidget *units_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);

            gtk_widget_add_css_class(units_box, "stats-units");
            gtk_box_append(GTK_BOX(subject), units_box);
            stats_ui.subj[s].units_box = units_box;

            for (int j = 0; j < sub_unit_count[s]; j++) {
                int gi = sub_unit_start[s] + j;

                gtk_box_append(GTK_BOX(units_box),
                               make_unit_stats_row(&units[gi], j + 1, gi));
            }
        }
    }

    return outer;
}
