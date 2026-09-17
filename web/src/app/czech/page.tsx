import { notFound } from 'next/navigation';

import { getLessons, getPath } from '@/content';
import { PathScreen } from '@/components/PathScreen';

/**
 * The three Czech areas from `build_czech_page`: Literatura (still empty),
 * Mluvnice and Maturitní četba. Each node links on rather than opening a
 * lesson, so `hrefBase` is unused here.
 */
export default async function CzechPage() {
  const path = await getPath('czech');
  if (!path) notFound();

  const lessons = await getLessons('czech');
  const taskCounts = Object.fromEntries(lessons.map((l) => [l.id, l.tasks.length]));

  return (
    <PathScreen
      path={path}
      titleKey="Český jazyk a literatura"
      subtitleKey="czech_sub"
      backHref="/subjects"
      hrefBase="/czech"
      taskCounts={taskCounts}
    />
  );
}
