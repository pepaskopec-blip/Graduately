import type { Metadata, Viewport } from 'next';

import { getThemes, getTranslations } from '@/content';
import { AppProvider } from './providers';
import { ThemeScript } from './ThemeScript';
import './globals.css';

export const metadata: Metadata = {
  title: 'maturita.c',
  description:
    'Vzdělávací aplikace pro přípravu k maturitě — němčina, počítačové sítě, ' +
    'technické vybavení a český jazyk.',
};

export const viewport: Viewport = {
  width: 'device-width',
  initialScale: 1,
  themeColor: '#1e1e2e',
};

export default async function RootLayout({ children }: { children: React.ReactNode }) {
  const [themes, translations] = await Promise.all([getThemes(), getTranslations()]);

  return (
    <html lang="cs" data-mode="dark" suppressHydrationWarning>
      <head>
        <ThemeScript themes={themes} />
      </head>
      <body>
        <AppProvider themes={themes} translations={translations}>
          {children}
        </AppProvider>
      </body>
    </html>
  );
}
