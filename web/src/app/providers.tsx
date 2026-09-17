'use client';

import {
  createContext,
  useCallback,
  useContext,
  useEffect,
  useMemo,
  useState,
  type ReactNode,
} from 'react';

import type { Lang, Theme, ThemePalette } from '../../../content/schema';
import type { TranslationTable } from '@/content';

/**
 * Client-side app settings: palette, dark/light mode and UI language.
 *
 * These mirror `progress/settings.conf` in the desktop app and persist to
 * localStorage so a reload keeps the student's choices.
 */

const STORAGE_KEY = 'maturita.settings';

interface Settings {
  themeId: string;
  mode: 'dark' | 'light';
  lang: Lang;
}

const DEFAULTS: Settings = { themeId: 'catppuccin', mode: 'dark', lang: 'cs' };

interface AppContextValue extends Settings {
  themes: Theme[];
  palette: ThemePalette;
  setThemeId: (id: string) => void;
  setMode: (mode: 'dark' | 'light') => void;
  setLang: (lang: Lang) => void;
  /** Translate a key, falling back to Czech and then to the key itself. */
  t: (key: string) => string;
  /** Like `t`, but returns undefined for unknown keys instead of the key. */
  tOpt: (key: string | null | undefined) => string | undefined;
}

const AppContext = createContext<AppContextValue | null>(null);

function readStored(): Settings {
  if (typeof window === 'undefined') return DEFAULTS;
  try {
    const raw = window.localStorage.getItem(STORAGE_KEY);
    if (!raw) return DEFAULTS;
    return { ...DEFAULTS, ...(JSON.parse(raw) as Partial<Settings>) };
  } catch {
    return DEFAULTS;
  }
}

/** Turn a palette into the CSS custom properties the stylesheet consumes. */
function paletteVars(palette: ThemePalette): Record<string, string> {
  const vars: Record<string, string> = {};
  for (const [key, value] of Object.entries(palette)) {
    // camelCase -> kebab-case, e.g. lockedBg -> --locked-bg
    const name = key.replace(/[A-Z]/g, (c) => `-${c.toLowerCase()}`);
    vars[`--${name}`] = value;
  }
  return vars;
}

export function AppProvider({
  themes,
  translations,
  children,
}: {
  themes: Theme[];
  translations: TranslationTable;
  children: ReactNode;
}) {
  const [settings, setSettings] = useState<Settings>(DEFAULTS);
  const [hydrated, setHydrated] = useState(false);

  // Read persisted settings after mount so server and client markup agree.
  useEffect(() => {
    setSettings(readStored());
    setHydrated(true);
  }, []);

  useEffect(() => {
    if (!hydrated) return;
    window.localStorage.setItem(STORAGE_KEY, JSON.stringify(settings));
  }, [settings, hydrated]);

  const theme = useMemo(
    () => themes.find((x) => x.id === settings.themeId) ?? themes[0],
    [themes, settings.themeId]
  );
  const palette = settings.mode === 'light' ? theme.light : theme.dark;

  // Apply the palette to <html> so it also covers the page background.
  useEffect(() => {
    const root = document.documentElement;
    for (const [name, value] of Object.entries(paletteVars(palette))) {
      root.style.setProperty(name, value);
    }
    root.dataset.mode = settings.mode;
    root.style.colorScheme = settings.mode;
  }, [palette, settings.mode]);

  useEffect(() => {
    document.documentElement.lang = settings.lang;
  }, [settings.lang]);

  const t = useCallback(
    (key: string) => {
      if (!key) return '';
      if (settings.lang === 'en') {
        return translations.en[key] ?? translations.cs[key] ?? key;
      }
      return translations.cs[key] ?? key;
    },
    [translations, settings.lang]
  );

  const tOpt = useCallback(
    (key: string | null | undefined) => {
      if (!key) return undefined;
      const table = settings.lang === 'en' ? translations.en : translations.cs;
      return table[key] ?? (settings.lang === 'en' ? translations.cs[key] : undefined);
    },
    [translations, settings.lang]
  );

  const value: AppContextValue = {
    ...settings,
    themes,
    palette,
    setThemeId: (themeId) => setSettings((s) => ({ ...s, themeId })),
    setMode: (mode) => setSettings((s) => ({ ...s, mode })),
    setLang: (lang) => setSettings((s) => ({ ...s, lang })),
    t,
    tOpt,
  };

  return <AppContext.Provider value={value}>{children}</AppContext.Provider>;
}

export function useApp(): AppContextValue {
  const ctx = useContext(AppContext);
  if (!ctx) throw new Error('useApp must be used inside <AppProvider>');
  return ctx;
}
