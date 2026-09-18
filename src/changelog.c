#include "maturita.h"

#include <string.h>

/* ------------------------------------------------------------------ */
/* What's new on the home page after an update                        */
/* ------------------------------------------------------------------ */

/* The packaged notes live in changelog.txt next to style.css. After a
 * published (or first) start whose commit is not the one the user last
 * dismissed, the welcome page shows the newest entries. Dismissing stores
 * APP_COMMIT in settings so the card stays gone until the next update. */

#define CHANGELOG_MAX_BLOCKS 3

typedef struct {
    char *date;
    GPtrArray *cs;
    GPtrArray *en;
} ChangelogBlock;

static struct {
    GPtrArray *blocks;
    char *seen;
    GtkWidget *card;
    GtkWidget *date;
    GtkWidget *items;
    gboolean loaded;
} cl;

static gboolean same_build(const char *a, const char *b) {
    size_t n;

    if (!a || !b || !a[0] || !b[0])
        return FALSE;
    if (g_ascii_strcasecmp(a, b) == 0)
        return TRUE;
    n = MIN(strlen(a), strlen(b));
    return n >= 7 && g_ascii_strncasecmp(a, b, n) == 0;
}

void changelog_set_seen(const char *sha) {
    g_free(cl.seen);
    cl.seen = (sha && sha[0]) ? g_strdup(sha) : NULL;
}

const char *changelog_seen(void) {
    return cl.seen ? cl.seen : "";
}

static void free_block(gpointer data) {
    ChangelogBlock *b = data;

    if (!b)
        return;
    g_free(b->date);
    if (b->cs)
        g_ptr_array_free(b->cs, TRUE);
    if (b->en)
        g_ptr_array_free(b->en, TRUE);
    g_free(b);
}

static ChangelogBlock *new_block(const char *date) {
    ChangelogBlock *b = g_new0(ChangelogBlock, 1);

    b->date = g_strdup(date ? date : "");
    b->cs = g_ptr_array_new_with_free_func(g_free);
    b->en = g_ptr_array_new_with_free_func(g_free);
    return b;
}

static void load_changelog(void) {
    gchar *text = NULL;
    gchar **lines;
    ChangelogBlock *cur = NULL;
    guint i;

    if (cl.loaded)
        return;
    cl.loaded = TRUE;
    cl.blocks = g_ptr_array_new_with_free_func(free_block);

    if (!g_file_get_contents(CHANGELOG_FILE, &text, NULL, NULL) || !text)
        return;

    lines = g_strsplit(text, "\n", -1);
    g_free(text);

    for (i = 0; lines[i]; i++) {
        char *line = g_strstrip(lines[i]);

        if (!line[0] || line[0] == '#')
            continue;
        if (g_str_has_prefix(line, "---")) {
            char *date = g_strstrip(line + 3);

            cur = new_block(date);
            g_ptr_array_add(cl.blocks, cur);
            continue;
        }
        if (!cur)
            continue;
        if (g_str_has_prefix(line, "cs:"))
            g_ptr_array_add(cur->cs, g_strdup(g_strstrip(line + 3)));
        else if (g_str_has_prefix(line, "en:"))
            g_ptr_array_add(cur->en, g_strdup(g_strstrip(line + 3)));
    }
    g_strfreev(lines);
}

static gboolean changelog_pending(void) {
    load_changelog();
    if (!cl.blocks || cl.blocks->len == 0)
        return FALSE;
    return !same_build(APP_COMMIT, cl.seen);
}

static GPtrArray *block_lines(const ChangelogBlock *b) {
    GPtrArray *src;

    if (!b)
        return NULL;
    src = (app_lang == LANG_EN) ? b->en : b->cs;
    if (src && src->len > 0)
        return src;
    return (b->cs && b->cs->len > 0) ? b->cs : b->en;
}

static void fill_items(void) {
    GtkWidget *child;
    guint i, shown = 0;

    if (!cl.items)
        return;

    while ((child = gtk_widget_get_first_child(cl.items)))
        gtk_box_remove(GTK_BOX(cl.items), child);

    if (!cl.blocks)
        return;

    for (i = 0; i < cl.blocks->len && shown < CHANGELOG_MAX_BLOCKS; i++) {
        const ChangelogBlock *b = g_ptr_array_index(cl.blocks, i);
        GPtrArray *lines = block_lines(b);
        guint j;

        if (!lines || lines->len == 0)
            continue;
        if (shown == 0 && cl.date)
            gtk_label_set_text(GTK_LABEL(cl.date), b->date ? b->date : "");
        if (shown > 0) {
            GtkWidget *gap = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);

            gtk_widget_set_margin_top(gap, 8);
            gtk_box_append(GTK_BOX(cl.items), gap);
        }
        for (j = 0; j < lines->len; j++) {
            char *text = g_strdup_printf("•  %s",
                                         (const char *)lines->pdata[j]);
            GtkWidget *row = gtk_label_new(text);

            gtk_widget_set_halign(row, GTK_ALIGN_START);
            gtk_label_set_wrap(GTK_LABEL(row), TRUE);
            gtk_label_set_xalign(GTK_LABEL(row), 0.0);
            gtk_label_set_max_width_chars(GTK_LABEL(row), 52);
            gtk_widget_add_css_class(row, "changelog-item");
            gtk_box_append(GTK_BOX(cl.items), row);
            g_free(text);
        }
        shown++;
    }
}

void changelog_apply_lang(void) {
    if (cl.card && gtk_widget_get_visible(cl.card))
        fill_items();
}

static void on_changelog_dismiss(GtkButton *btn, gpointer data) {
    (void)btn;
    (void)data;
    changelog_set_seen(APP_COMMIT);
    save_settings();
    if (cl.card)
        gtk_widget_set_visible(cl.card, FALSE);
}

GtkWidget *build_changelog_card(void) {
    GtkWidget *card;
    GtkWidget *head;
    GtkWidget *title;
    GtkWidget *date;
    GtkWidget *items;
    GtkWidget *dismiss;

    load_changelog();

    card = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_widget_add_css_class(card, "changelog-card");
    gtk_widget_set_hexpand(card, TRUE);
    gtk_widget_set_visible(card, changelog_pending());

    head = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 12);
    gtk_box_append(GTK_BOX(card), head);

    title = gtk_label_new(NULL);
    i18n_bind(title, "changelog_title", 0);
    gtk_widget_set_halign(title, GTK_ALIGN_START);
    gtk_widget_set_hexpand(title, TRUE);
    gtk_widget_add_css_class(title, "changelog-title");
    gtk_box_append(GTK_BOX(head), title);

    date = gtk_label_new("");
    gtk_widget_set_valign(date, GTK_ALIGN_CENTER);
    gtk_widget_add_css_class(date, "changelog-date");
    gtk_box_append(GTK_BOX(head), date);

    items = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
    gtk_box_append(GTK_BOX(card), items);

    dismiss = gtk_button_new();
    i18n_bind(dismiss, "changelog_dismiss", 1);
    gtk_widget_set_halign(dismiss, GTK_ALIGN_START);
    gtk_widget_add_css_class(dismiss, "changelog-dismiss");
    g_signal_connect(dismiss, "clicked", G_CALLBACK(on_changelog_dismiss), NULL);
    gtk_box_append(GTK_BOX(card), dismiss);

    cl.card = card;
    cl.date = date;
    cl.items = items;
    fill_items();
    return card;
}
