import { notFound } from 'next/navigation';

import { getLessons, getPath } from '@/content';
import { PathScreen } from '@/components/PathScreen';

export default async function HardwarePage() {
  const path = await getPath('hardware');
  if (!path) notFound();

  const lessons = await getLessons('hardware');
  const taskCounts = Object.fromEntries(lessons.map((l) => [l.id, l.tasks.length]));

  return (
    <PathScreen
      path={path}
      // The desktop app keys subject titles by their Czech text.
      titleKey="Technické vybavení"
      subtitleKey="hw_sub"
      backHref="/subjects"
      hrefBase="/hardware"
      taskCounts={taskCounts}
    />
  );
}
