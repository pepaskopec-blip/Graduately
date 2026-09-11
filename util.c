#include "maturita.h"

/* ------------------------------------------------------------------ */
/* Utilities                                                          */
/* ------------------------------------------------------------------ */

char *normalize_answer(const char *input) {
    GString *out = g_string_new(NULL);
    const gchar *down = g_utf8_strdown(input, -1);
    const gchar *p = down;
    gboolean prev_space = FALSE;

    while (*p) {
        gunichar ch = g_utf8_get_char(p);
        p = g_utf8_next_char(p);
        if (g_unichar_isspace(ch)) {
            if (out->len > 0 && !prev_space)
                g_string_append_c(out, ' ');
            prev_space = TRUE;
            continue;
        }
        prev_space = FALSE;
        switch (ch) {
            case 0x00e4: g_string_append(out, "ae"); break; /* ä */
            case 0x00f6: g_string_append(out, "oe"); break; /* ö */
            case 0x00fc: g_string_append(out, "ue"); break; /* ü */
            case 0x00df: g_string_append(out, "ss"); break; /* ß */
            default:     g_string_append_unichar(out, ch); break;
        }
    }

    if (out->len > 0 && out->str[out->len - 1] == ' ')
        g_string_truncate(out, out->len - 1);

    g_free((gpointer)down);
    return g_string_free(out, FALSE);
}

void shuffle_indices(int *arr, int n) {
    for (int i = n - 1; i > 0; i--) {
        int j = g_random_int_range(0, i + 1);
        int tmp = arr[i];
        arr[i] = arr[j];
        arr[j] = tmp;
    }
}

void flow_clear(GtkFlowBox *fb) {
    GtkWidget *c = gtk_widget_get_first_child(GTK_WIDGET(fb));
    while (c) {
        GtkWidget *next = gtk_widget_get_next_sibling(c);
        gtk_flow_box_remove(fb, c);
        c = next;
    }
}

void answer_mark(GtkWidget *widget, gboolean ok) {
    if (ok) {
        gtk_widget_remove_css_class(widget, "answer-wrong");
        gtk_widget_add_css_class(widget, "answer-ok");
    } else {
        gtk_widget_remove_css_class(widget, "answer-ok");
        gtk_widget_add_css_class(widget, "answer-wrong");
    }
}

void set_feedback(GtkWidget *label, gboolean ok, const char *text) {
    gtk_widget_remove_css_class(label, "feedback-ok");
    gtk_widget_remove_css_class(label, "feedback-err");
    gtk_widget_add_css_class(label, ok ? "feedback-ok" : "feedback-err");
    gtk_label_set_text(GTK_LABEL(label), text);
}

/* True if `s` contains any German special letter (ä, ö, ü or ß). */
gboolean text_has_german_umlaut(const char *s) {
    const char *p = s ? s : "";

    while (*p) {
        gunichar c = g_utf8_get_char(p);

        if (c == 0x00e4 || c == 0x00f6 || c == 0x00fc || c == 0x00df)
            return TRUE;
        p = g_utf8_next_char(p);
    }
    return FALSE;
}

/* Small bilingual note shown under the exercise title whenever the answer
 * the student has to type may contain a German special letter. */
void add_umlaut_note(GtkWidget *body) {
    GtkWidget *n = gtk_label_new(NULL);

    i18n_bind(n, "hint_umlauts", 0);
    gtk_widget_set_halign(n, GTK_ALIGN_START);
    gtk_label_set_wrap(GTK_LABEL(n), TRUE);
    gtk_widget_set_margin_bottom(n, 2);
    gtk_widget_add_css_class(n, "hint");
    gtk_box_append(GTK_BOX(body), n);
}
