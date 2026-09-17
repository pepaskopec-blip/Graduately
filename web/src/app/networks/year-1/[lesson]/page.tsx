import { notFound } from 'next/navigation';

import { getLesson, getLessonIds } from '@/content';
import { LessonScreen } from '@/components/LessonScreen';

export async function generateStaticParams() {
  const ids = await getLessonIds('networks');
  return ids.map((lesson) => ({ lesson }));
}

export default async function NetworkLessonPage({
  params,
}: {
  params: Promise<{ lesson: string }>;
}) {
  const { lesson: id } = await params;

  try {
    return <LessonScreen lesson={await getLesson('networks', id)} backHref="/networks/year-1" />;
  } catch {
    notFound();
  }
}
