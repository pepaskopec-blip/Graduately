'use client';

import { useApp } from '@/app/providers';
import { TopBar } from '@/components/TopBar';

/** Mode, palette and language, mirroring the desktop settings panel. */
export default function SettingsPage() {
  const { t, themes, themeId, mode, lang, setThemeId, setMode, setLang } = useApp();

  return (
    <div className="shell">
      <TopBar title={t('settings_title')} backHref="/subjects" showTools={false} />

      <main className="page">
        <div className="page__head">
          <h1 className="page__title">{t('settings_title')}</h1>
        </div>

        <div className="settings">
          <section className="settings__group">
            <span className="settings__label">{t('mode')}</span>
            <div className="segmented" role="group" aria-label={t('mode')}>
              <button aria-pressed={mode === 'dark'} onClick={() => setMode('dark')}>
                {t('mode_dark')}
              </button>
              <button aria-pressed={mode === 'light'} onClick={() => setMode('light')}>
                {t('mode_light')}
              </button>
            </div>
          </section>

          <section className="settings__group">
            <span className="settings__label">{t('theme')}</span>
            <div className="swatches">
              {themes.map((theme) => {
                const palette = mode === 'light' ? theme.light : theme.dark;
                return (
                  <button
                    className="swatch"
                    key={theme.id}
                    aria-pressed={theme.id === themeId}
                    onClick={() => setThemeId(theme.id)}
                  >
                    <span className="swatch__dots">
                      {/* Keyed by position: some palettes reuse the same hex twice. */}
                      {[palette.accent, palette.accent3, palette.success, palette.warning].map(
                        (color, i) => (
                          <span className="swatch__dot" key={i} style={{ background: color }} />
                        )
                      )}
                    </span>
                    <span className="swatch__name">{theme.name}</span>
                  </button>
                );
              })}
            </div>
          </section>

          <section className="settings__group">
            <span className="settings__label">{t('language')}</span>
            <div className="segmented" role="group" aria-label={t('language')}>
              <button aria-pressed={lang === 'cs'} onClick={() => setLang('cs')}>
                Čeština
              </button>
              <button aria-pressed={lang === 'en'} onClick={() => setLang('en')}>
                English
              </button>
            </div>
          </section>
        </div>
      </main>
    </div>
  );
}
