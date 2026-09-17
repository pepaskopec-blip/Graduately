'use client';

import Link from 'next/link';
import { useRouter } from 'next/navigation';

import { useApp } from '@/app/providers';
import { BackIcon, ChartIcon, GearIcon } from './icons';

/**
 * Header shared by every screen: a circular back button, the page title and the
 * statistics / settings shortcuts, mirroring the desktop app's top bar.
 */
export function TopBar({
  title,
  backHref,
  showTools = true,
}: {
  title: string;
  /** Explicit destination; falls back to browser history when omitted. */
  backHref?: string;
  showTools?: boolean;
}) {
  const router = useRouter();
  const { t } = useApp();

  return (
    <header className="topbar">
      {backHref ? (
        <Link className="icon-btn" href={backHref} aria-label={t('back')}>
          <BackIcon />
        </Link>
      ) : (
        <button className="icon-btn" onClick={() => router.back()} aria-label={t('back')}>
          <BackIcon />
        </button>
      )}

      <span className="topbar__title">{title}</span>

      {showTools && (
        <>
          <Link className="icon-btn" href="/stats" aria-label={t('stats_title')}>
            <ChartIcon />
          </Link>
          <Link className="icon-btn" href="/settings" aria-label={t('settings_title')}>
            <GearIcon />
          </Link>
        </>
      )}
    </header>
  );
}
