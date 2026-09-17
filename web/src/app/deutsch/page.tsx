import { notFound } from 'next/navigation';

import { getLessons, getPath } from '@/content';
import { PathScreen } from '@/components/PathScreen';

export default async function DeutschPathPage() {
  const path = await getPath('deutsch');
  if (!path) notFound();

  const lessons = await getLessons('deutsch');
  const taskCounts = Object.fromEntries(lessons.map((l) => [l.id, l.tasks.length]));

  return (
    <PathScreen
      path={path}
      titleKey="roadmap_title"
      subtitleKey="roadmap_sub"
      backHref="/subjects"
      hrefBase="/deutsch"
      taskCounts={taskCounts}
      finish
    />
  );
}
