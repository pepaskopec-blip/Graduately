'use client';

import Link from 'next/link';

import type { Subject } from '@/content';
import { useApp } from '@/app/providers';
import { TopBar } from '@/components/TopBar';
import { LockIcon, SUBJECT_ICONS } from '@/components/icons';

/** Subject picker. Locked subjects are dimmed placeholders, as on the desktop. */
export function SubjectsScreen({ subjects }: { subjects: Subject[] }) {
  const { t } = useApp();

  return (
    <div className="shell">
      <TopBar title={t('subjects_title')} backHref="/" />

      <main className="page">
        <div className="page__head">
          <h1 className="page__title">{t('subjects_title')}</h1>
        </div>

        <div className="grid">
          {subjects.map((subject) => (
            <SubjectCard key={subject.id} subject={subject} label={t(subject.key)} />
          ))}
        </div>
      </main>
    </div>
  );
}

function SubjectCard({ subject, label }: { subject: Subject; label: string }) {
  const Icon = subject.icon ? SUBJECT_ICONS[subject.icon] : null;

  const inner = (
    <>
      <span className="row" style={{ color: 'var(--accent2)' }}>
        {Icon ? <Icon /> : <LockIcon size={24} />}
      </span>
      <h2 className="card__title">{label}</h2>
    </>
  );

  if (!subject.unlocked || !subject.route) {
    return (
      <div className="card card--locked" aria-disabled>
        {inner}
      </div>
    );
  }

  return (
    <Link className="card card--interactive" href={subject.route}>
      {inner}
    </Link>
  );
}
