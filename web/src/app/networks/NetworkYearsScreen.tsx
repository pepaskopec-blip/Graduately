'use client';

import Link from 'next/link';

import { useApp } from '@/app/providers';
import { TopBar } from '@/components/TopBar';
import { LockIcon } from '@/components/icons';

/** Only year 1 has content, matching `build_net_years_page` in the desktop app. */
const YEARS = [1, 2, 3, 4];

export function NetworkYearsScreen() {
  const { t } = useApp();

  return (
    <div className="shell">
      <TopBar title={t('net_years_title')} backHref="/subjects" />

      <main className="page">
        <div className="page__head">
          <h1 className="page__title">{t('net_years_title')}</h1>
          <p className="page__sub">{t('net_years_sub')}</p>
        </div>

        <div className="grid">
          {YEARS.map((year) => {
            const label = t(`net_year${year}`);
            const locked = year !== 1;

            if (locked) {
              return (
                <div className="card card--locked" key={year} aria-disabled>
                  <span className="row" style={{ color: 'var(--overlay)' }}>
                    <LockIcon size={22} />
                  </span>
                  <h2 className="card__title">{label}</h2>
                  <p className="card__body">{t('net_year_locked_sub')}</p>
                </div>
              );
            }

            return (
              <Link className="card card--interactive" href={`/networks/year-${year}`} key={year}>
                <span className="card__kicker">{year}</span>
                <h2 className="card__title">{label}</h2>
              </Link>
            );
          })}
        </div>
      </main>
    </div>
  );
}
