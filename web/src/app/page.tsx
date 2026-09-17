'use client';

import Link from 'next/link';

import { useApp } from './providers';

/** Welcome screen: branded greeting plus a continue button, as on the desktop. */
export default function WelcomePage() {
  const { t } = useApp();

  return (
    <main className="welcome">
      <h1 className="welcome__title">maturita.c</h1>
      <p className="welcome__body">{t('welcome_body')}</p>
      <Link className="btn btn--lg" href="/subjects">
        {t('continue')}
      </Link>
    </main>
  );
}
