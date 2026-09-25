#include "maturita.h"

#include <stdlib.h>
#include <string.h>

#define SEARCH_MAX 24

typedef struct {
    char *title;
    char *subtitle;
    char *hay;
    char *target;
    gboolean locked;
    int rank;
    int score;
} SearchItem;

static GtkWidget *search_layer;
static GtkWidget *search_entry;
static GtkWidget *search_list;
static GtkWidget *search_empty;
static GPtrArray *search_items;

static gboolean subject_open(int i) {
    return i == 0 || i == NET_SUBJ || i == HW_SUBJ || i == CZ_SUBJ;
}

static const char *subject_page(int i) {
    if (i == 0)
        return "roadmap";
    if (i == NET_SUBJ)
        return "netyears";
    if (i == HW_SUBJ)
        return "hwyears";
    if (i == CZ_SUBJ)
        return "czechmap";
    return "subjects";
}

static void hay_add(GString *h, const char *text) {
    if (text && *text) {
        g_string_append(h, text);
        g_string_append_c(h, ' ');
    }
}

static void hay_add_key(GString *h, const char *key) {
    int i;

    hay_add(h, key);
    for (i = 0; tr_ui[i].key; i++) {
        if (g_strcmp0(tr_ui[i].key, key) == 0) {
            hay_add(h, tr_ui[i].cs);
            hay_add(h, tr_ui[i].en);
            return;
        }
    }
}

static SearchItem *item_new(const char *title, const char *subtitle,
                            const char *target, gboolean locked, int rank,
                            const char *extra) {
    SearchItem *it = g_new0(SearchItem, 1);
    GString *h = g_string_new(NULL);
    char *norm;

    it->title = g_strdup(title ? title : "");
    it->subtitle = g_strdup(subtitle ? subtitle : "");
    it->target = g_strdup(target ? target : "");
    it->locked = locked;
    it->rank = rank;
    hay_add(h, title);
    hay_add(h, subtitle);
    hay_add(h, extra);
    hay_add(h, target);
    norm = normalize_answer(h->str);
    it->hay = norm ? norm : g_strdup("");
    g_string_free(h, TRUE);
    return it;
}

static void item_free(gpointer p) {
    SearchItem *it = p;

    if (!it)
        return;
    g_free(it->title);
    g_free(it->subtitle);
    g_free(it->hay);
    g_free(it->target);
    g_free(it);
}

static void catalog_add(const char *title, const char *subtitle,
                        const char *target, gboolean locked, int rank,
                        const char *extra);

void search_catalog_add(const char *title, const char *subtitle,
                        const char *target, gboolean locked, int rank,
                        const char *extra) {
    catalog_add(title, subtitle, target, locked, rank, extra);
}

static void catalog_add(const char *title, const char *subtitle,
                        const char *target, gboolean locked, int rank,
                        const char *extra) {
    if (!title || !target)
        return;
    if (main_stack && !gtk_stack_get_child_by_name(main_stack, target)
        && strncmp(target, "cetbastub:", 10) != 0)
        return;
    g_ptr_array_add(search_items,
                    item_new(title, subtitle, target, locked, rank, extra));
}

static void catalog_add_key(const char *title_key, const char *sub_key,
                            const char *target, gboolean locked, int rank,
                            const char *extra) {
    GString *h = g_string_new(NULL);
    SearchItem *it;

    if (!title_key || !target)
        return;
    if (main_stack && !gtk_stack_get_child_by_name(main_stack, target))
        return;
    hay_add_key(h, title_key);
    if (sub_key)
        hay_add_key(h, sub_key);
    hay_add(h, extra);
    it = item_new(tr(title_key), sub_key ? tr(sub_key) : "",
                  target, locked, rank, h->str);
    g_string_free(h, TRUE);
    g_ptr_array_add(search_items, it);
}

static void catalog_rebuild(void) {
    int i, n;
    char key[32];
    char page[32];

    if (!search_items)
        search_items = g_ptr_array_new_with_free_func(item_free);
    else
        g_ptr_array_set_size(search_items, 0);

    catalog_add_key("subjects_title", "search_page", "subjects", FALSE, 0,
                    "predmety subjects mapa");
    catalog_add_key("stats", "search_page", "stats", FALSE, 0, "progress");
    catalog_add_key("search_home", "search_page", "welcome", FALSE, 0,
                    "uvod welcome uvodni");

    for (i = 0; i < NUM_SUBJECTS; i++) {
        gboolean open = subject_open(i);

        catalog_add_key(sub_keys[i], "search_subject",
                        open ? subject_page(i) : "subjects", !open, 0, NULL);
    }

    for (i = 0; i < NUM_UNITS; i++) {
        UnitCtx *u = &units[i];
        const char *target = (u->unlocked && u->page) ? u->page : "roadmap";

        catalog_add(u->title, tr("search_unit"), target, !u->unlocked, 1,
                    "deutsch nemcina german");
        if (!u->unlocked || !u->ex_tag)
            continue;
        if (u->has_branch && u->branch_target)
            catalog_add(u->branch_name, u->title, u->branch_target, FALSE, 2,
                        "vokabel slovicka vocabulary");
        for (n = 1; n <= u->n_ex; n++) {
            g_snprintf(page, sizeof(page), "%se%d", u->ex_tag, n);
            catalog_add(u->ex_names[n], u->title, page, FALSE, 3, NULL);
        }
    }

    catalog_add_key("net_years_title", "search_subject", "netyears", FALSE, 0,
                    "site network ip");
    catalog_add_key("net_year1", "search_lesson", "netmap", FALSE, 1, "rocnik");
    catalog_add_key("net_year2", "search_lesson", "netyears", TRUE, 1, "rocnik");
    catalog_add_key("net_year3", "search_lesson", "netyears", TRUE, 1, "rocnik");
    catalog_add_key("net_year4", "search_lesson", "netyears", TRUE, 1, "rocnik");
    for (i = 1; i <= NET_LESSONS; i++) {
        g_snprintf(key, sizeof(key), "net_unit%d", i);
        g_snprintf(page, sizeof(page), "netunit%d", i);
        catalog_add_key(key, "search_lesson", page, FALSE, 2, "site sit");
        g_snprintf(page, sizeof(page), "netex%d", i);
        catalog_add(tr(key), tr("search_exercise"), page, FALSE, 3, "kviz quiz");
    }

    catalog_add_key("hw_year1", "search_lesson", "hwmap", FALSE, 1, "rocnik");
    catalog_add_key("hw_year2", "search_lesson", "hwyears", TRUE, 1, "rocnik");
    catalog_add_key("hw_year3", "search_lesson", "hwyears", TRUE, 1, "rocnik");
    catalog_add_key("hw_year4", "search_lesson", "hwyears", TRUE, 1, "rocnik");
    for (i = 1; i <= HW_LESSONS; i++) {
        g_snprintf(key, sizeof(key), "hw_unit%d", i);
        g_snprintf(page, sizeof(page), "hwunit%d", i);
        catalog_add_key(key, "search_lesson", page, FALSE, 2, "hardware hw");
        g_snprintf(page, sizeof(page), "hwex%d", i);
        catalog_add(tr(key), tr("search_exercise"), page, FALSE, 3, "kviz quiz");
    }

    catalog_add_key("Český jazyk a literatura", "search_subject",
                    "czechmap", FALSE, 0, "cestina czech");
    catalog_add_key("Literatura", "search_lesson", "czechmap", TRUE, 1, NULL);
    catalog_add_key("Mluvnice", "search_lesson", "mluvnice", FALSE, 1,
                    "gramatika grammar");
    catalog_add_key("Maturitní četba", "search_book", "readinglist", FALSE, 1,
                    "knihy books");
    cetba_register_search();

    {
        static const char *mluv_names[] = {
            "i/y", "Velká písmena", "Slovní druhy", "Podst. jména", "Slovesa",
            "Slovotvorba", "Větné členy", "Vedlejší věty", "Synonyma",
            "Antonyma", "s/z", "Najdi chybu", "Tvary", "Test A–D",
            "Přímá řeč", "Rozdíly", "Interpunkce", "Obrazná pojmen.",
            "Slohové útvary", "Opakování",
        };

        for (i = 0; i < (int)G_N_ELEMENTS(mluv_names); i++) {
            g_snprintf(page, sizeof(page), "mluve%d", i + 1);
            catalog_add(mluv_names[i], tr("Mluvnice"), page, FALSE, 3,
                        "mluvnice gramatika");
        }
    }
}

static int item_score(const SearchItem *it, const char *needle) {
    const char *p;

    if (!needle || !*needle)
        return 40 - it->rank;
    if (!it->hay || !it->hay[0])
        return -1;
    if (g_strcmp0(it->hay, needle) == 0)
        return 200 - it->rank;
    if (g_str_has_prefix(it->hay, needle))
        return 160 - it->rank;
    p = strstr(it->hay, needle);
    if (!p)
        return -1;
    if (p > it->hay && p[-1] == ' ')
        return 120 - it->rank;
    return 80 - it->rank;
}

static int cmp_item(gconstpointer a, gconstpointer b) {
    const SearchItem *ia = *(SearchItem * const *)a;
    const SearchItem *ib = *(SearchItem * const *)b;

    if (ia->score != ib->score)
        return ib->score - ia->score;
    return g_strcmp0(ia->title, ib->title);
}

static void search_go(const char *target) {
    if (!target || !main_stack)
        return;
    if (strncmp(target, "cetbastub:", 10) == 0) {
        int idx = atoi(target + 10);
        search_close();
        cetba_open_stub(idx);
        return;
    }
    if (!gtk_stack_get_child_by_name(main_stack, target))
        return;
    search_close();
    gtk_stack_set_visible_child_name(main_stack, target);
}

static void on_row_activated(GtkListBox *box, GtkListBoxRow *row,
                             gpointer data) {
    const char *target;

    (void)box;
    (void)data;
    if (!row)
        return;
    target = g_object_get_data(G_OBJECT(row), "target");
    search_go(target);
}

static void search_fill(void) {
    const char *raw;
    char *needle;
    GPtrArray *hits;
    guint i;

    if (!search_list)
        return;
    catalog_rebuild();
    raw = gtk_editable_get_text(GTK_EDITABLE(search_entry));
    needle = normalize_answer(raw ? raw : "");
    hits = g_ptr_array_new();

    for (i = 0; i < search_items->len; i++) {
        SearchItem *it = g_ptr_array_index(search_items, i);
        int score;

        if (!needle[0] && it->rank > 1)
            continue;
        score = item_score(it, needle);
        if (score < 0)
            continue;
        it->score = score;
        g_ptr_array_add(hits, it);
    }
    g_ptr_array_sort(hits, cmp_item);
    if (hits->len > SEARCH_MAX)
        g_ptr_array_set_size(hits, SEARCH_MAX);

    {
        GtkWidget *c = gtk_widget_get_first_child(search_list);

        while (c) {
            GtkWidget *next = gtk_widget_get_next_sibling(c);

            gtk_list_box_remove(GTK_LIST_BOX(search_list), c);
            c = next;
        }
    }
    for (i = 0; i < hits->len; i++) {
        SearchItem *it = g_ptr_array_index(hits, i);
        GtkWidget *row;
        GtkWidget *box;
        GtkWidget *title;
        GtkWidget *sub;
        GString *sub_text;

        row = gtk_list_box_row_new();
        gtk_widget_add_css_class(row, "search-hit");
        if (it->locked)
            gtk_widget_add_css_class(row, "search-hit-locked");
        g_object_set_data_full(G_OBJECT(row), "target",
                               g_strdup(it->target), g_free);

        box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 2);
        gtk_widget_set_margin_start(box, 12);
        gtk_widget_set_margin_end(box, 12);
        gtk_widget_set_margin_top(box, 8);
        gtk_widget_set_margin_bottom(box, 8);
        gtk_list_box_row_set_child(GTK_LIST_BOX_ROW(row), box);

        title = gtk_label_new(it->title);
        gtk_widget_set_halign(title, GTK_ALIGN_START);
        gtk_label_set_xalign(GTK_LABEL(title), 0.0);
        gtk_label_set_ellipsize(GTK_LABEL(title), PANGO_ELLIPSIZE_END);
        gtk_widget_add_css_class(title, "search-hit-title");
        gtk_box_append(GTK_BOX(box), title);

        sub_text = g_string_new(it->subtitle);
        if (it->locked) {
            if (sub_text->len)
                g_string_append(sub_text, " · ");
            g_string_append(sub_text, tr("stats_locked"));
        }
        sub = gtk_label_new(sub_text->str);
        g_string_free(sub_text, TRUE);
        gtk_widget_set_halign(sub, GTK_ALIGN_START);
        gtk_label_set_xalign(GTK_LABEL(sub), 0.0);
        gtk_widget_add_css_class(sub, "search-hit-sub");
        gtk_box_append(GTK_BOX(box), sub);

        gtk_list_box_append(GTK_LIST_BOX(search_list), row);
    }

    gtk_widget_set_visible(search_empty, hits->len == 0);
    gtk_widget_set_visible(search_list, hits->len > 0);
    if (hits->len > 0)
        gtk_list_box_select_row(GTK_LIST_BOX(search_list),
                                gtk_list_box_get_row_at_index(
                                    GTK_LIST_BOX(search_list), 0));
    if (needle[0])
        gtk_label_set_text(GTK_LABEL(search_empty), tr("search_empty"));
    else
        gtk_label_set_text(GTK_LABEL(search_empty), tr("search_hint"));

    g_ptr_array_unref(hits);
    g_free(needle);
}

static void on_search_changed(GtkEditable *editable, gpointer data) {
    (void)editable;
    (void)data;
    search_fill();
}

static void search_activate_selected(void) {
    GtkListBoxRow *row;

    if (!search_list)
        return;
    row = gtk_list_box_get_selected_row(GTK_LIST_BOX(search_list));
    if (!row)
        row = gtk_list_box_get_row_at_index(GTK_LIST_BOX(search_list), 0);
    if (row)
        search_go(g_object_get_data(G_OBJECT(row), "target"));
}

static gboolean on_entry_key(GtkEventControllerKey *controller, guint keyval,
                             guint keycode, GdkModifierType state,
                             gpointer data) {
    (void)controller;
    (void)keycode;
    (void)state;
    (void)data;

    if (keyval == GDK_KEY_Escape) {
        search_close();
        return TRUE;
    }
    if (keyval == GDK_KEY_Return || keyval == GDK_KEY_KP_Enter) {
        search_activate_selected();
        return TRUE;
    }
    if (keyval == GDK_KEY_Down || keyval == GDK_KEY_Tab) {
        GtkListBoxRow *row = gtk_list_box_get_row_at_index(
            GTK_LIST_BOX(search_list), 0);

        if (row)
            gtk_widget_grab_focus(GTK_WIDGET(row));
        return TRUE;
    }
    return FALSE;
}

static void on_scrim_clicked(GtkGestureClick *g, gint n_press, gdouble x,
                             gdouble y, gpointer data) {
    (void)g;
    (void)n_press;
    (void)x;
    (void)y;
    (void)data;
    search_close();
}

gboolean search_is_open(void) {
    return search_layer && gtk_widget_get_visible(search_layer);
}

void search_close(void) {
    if (search_layer)
        gtk_widget_set_visible(search_layer, FALSE);
}

void search_open(void) {
    if (!search_layer)
        return;
    gtk_widget_set_visible(search_layer, TRUE);
    if (search_entry) {
        gtk_editable_set_text(GTK_EDITABLE(search_entry), "");
        gtk_widget_grab_focus(search_entry);
    }
    search_fill();
}

void search_apply_lang(void) {
    if (!search_layer)
        return;
    if (search_is_open())
        search_fill();
}

static void on_search_clicked(GtkButton *button, gpointer data) {
    (void)button;
    (void)data;
    if (search_is_open())
        search_close();
    else
        search_open();
}

GtkWidget *build_search_button(void) {
    GtkWidget *btn;
    GtkWidget *icon;

    btn = gtk_button_new();
    gtk_widget_add_css_class(btn, "settings-btn");
    gtk_widget_add_css_class(btn, "flat");
    i18n_bind(btn, "search", 2);
    gtk_widget_set_valign(btn, GTK_ALIGN_CENTER);
    gtk_widget_set_focus_on_click(btn, FALSE);

    icon = gtk_drawing_area_new();
    gtk_widget_set_size_request(icon, 18, 18);
    gtk_widget_set_can_target(icon, FALSE);
    gtk_drawing_area_set_draw_func(GTK_DRAWING_AREA(icon),
                                   draw_search_icon, NULL, NULL);
    gtk_button_set_child(GTK_BUTTON(btn), icon);
    g_signal_connect_swapped(btn, "state-flags-changed",
                             G_CALLBACK(gtk_widget_queue_draw), icon);
    g_signal_connect(btn, "clicked", G_CALLBACK(on_search_clicked), NULL);
    return btn;
}

void search_attach(GtkOverlay *overlay) {
    GtkWidget *scrim;
    GtkWidget *card;
    GtkWidget *scroll;
    GtkEventController *keys;
    GtkGesture *click;

    search_layer = gtk_overlay_new();
    gtk_widget_add_css_class(search_layer, "search-layer");
    gtk_widget_set_hexpand(search_layer, TRUE);
    gtk_widget_set_vexpand(search_layer, TRUE);
    gtk_widget_set_visible(search_layer, FALSE);

    scrim = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_add_css_class(scrim, "search-scrim");
    gtk_widget_set_hexpand(scrim, TRUE);
    gtk_widget_set_vexpand(scrim, TRUE);
    gtk_overlay_set_child(GTK_OVERLAY(search_layer), scrim);
    click = gtk_gesture_click_new();
    g_signal_connect(click, "pressed", G_CALLBACK(on_scrim_clicked), NULL);
    gtk_widget_add_controller(scrim, GTK_EVENT_CONTROLLER(click));

    card = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_widget_add_css_class(card, "search-card");
    gtk_widget_set_halign(card, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(card, GTK_ALIGN_START);
    gtk_widget_set_margin_top(card, 72);
    gtk_widget_set_size_request(card, 560, -1);
    gtk_overlay_add_overlay(GTK_OVERLAY(search_layer), card);

    search_entry = gtk_entry_new();
    gtk_widget_add_css_class(search_entry, "search-entry");
    i18n_bind(search_entry, "search_placeholder", 3);
    gtk_box_append(GTK_BOX(card), search_entry);
    g_signal_connect(search_entry, "changed",
                     G_CALLBACK(on_search_changed), NULL);
    keys = gtk_event_controller_key_new();
    g_signal_connect(keys, "key-pressed", G_CALLBACK(on_entry_key), NULL);
    gtk_widget_add_controller(search_entry, keys);

    search_empty = gtk_label_new(NULL);
    i18n_bind(search_empty, "search_hint", 0);
    gtk_widget_add_css_class(search_empty, "search-empty");
    gtk_box_append(GTK_BOX(card), search_empty);

    scroll = gtk_scrolled_window_new();
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll),
                                   GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
    gtk_scrolled_window_set_max_content_height(GTK_SCROLLED_WINDOW(scroll),
                                               420);
    gtk_scrolled_window_set_propagate_natural_height(
        GTK_SCROLLED_WINDOW(scroll), TRUE);
    gtk_box_append(GTK_BOX(card), scroll);

    search_list = gtk_list_box_new();
    gtk_widget_add_css_class(search_list, "search-results");
    gtk_list_box_set_selection_mode(GTK_LIST_BOX(search_list),
                                    GTK_SELECTION_SINGLE);
    gtk_list_box_set_activate_on_single_click(GTK_LIST_BOX(search_list), TRUE);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), search_list);
    g_signal_connect(search_list, "row-activated",
                     G_CALLBACK(on_row_activated), NULL);

    gtk_overlay_add_overlay(overlay, search_layer);
}
