import type { Theme } from '../../../content/schema';

/**
 * Applies the stored palette before the first paint.
 *
 * Without this the page renders with the dark fallback from `globals.css` and
 * only recolours once <AppProvider>'s effect runs, which shows as a dark flash
 * for anyone using a light theme.
 */
export function ThemeScript({ themes }: { themes: Theme[] }) {
  const palettes = Object.fromEntries(
    themes.map((theme) => [theme.id, { dark: theme.dark, light: theme.light }])
  );

  const source = `
(function () {
  try {
    var palettes = ${JSON.stringify(palettes)};
    var stored = JSON.parse(localStorage.getItem('maturita.settings') || '{}');
    var theme = palettes[stored.themeId] || palettes['catppuccin'];
    var mode = stored.mode === 'light' ? 'light' : 'dark';
    var palette = theme[mode];
    var root = document.documentElement;

    for (var key in palette) {
      root.style.setProperty('--' + key.replace(/[A-Z]/g, function (c) {
        return '-' + c.toLowerCase();
      }), palette[key]);
    }

    root.dataset.mode = mode;
    root.style.colorScheme = mode;
    if (stored.lang === 'en' || stored.lang === 'cs') root.lang = stored.lang;
  } catch (e) {}
})();`;

  return <script dangerouslySetInnerHTML={{ __html: source }} />;
}
