import { notFound } from 'next/navigation';

import { getLessons, getPath } from '@/content';
import { PathScreen } from '@/components/PathScreen';

export default async function NetworkYear1Page() {
  const path = await getPath('networks');
  if (!path) notFound();

  const lessons = await getLessons('networks');
  const taskCounts = Object.fromEntries(lessons.map((l) => [l.id, l.tasks.length]));

  return (
    <PathScreen
      path={path}
      titleKey="net_years_title"
      subtitleKey="net_sub"
      backHref="/networks"
      hrefBase="/networks/year-1"
      taskCounts={taskCounts}
    />
  );
}
