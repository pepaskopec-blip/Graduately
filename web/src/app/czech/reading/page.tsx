import { notFound } from 'next/navigation';

import { getLessons, getPath } from '@/content';
import { PathScreen } from '@/components/PathScreen';

/** The reading list: one node per book, both unlocked. */
export default async function ReadingListPage() {
  const path = await getPath('czech', 'reading');
  if (!path) notFound();

  const lessons = await getLessons('czech');
  const taskCounts = Object.fromEntries(lessons.map((l) => [l.id, l.tasks.length]));

  return (
    <PathScreen
      path={path}
      title="Maturitní četba"
      subtitle="Vyberte knihu a projděte si přehled, kvíz a řazení děje."
      backHref="/czech"
      hrefBase="/czech/reading"
      taskCounts={taskCounts}
    />
  );
}
