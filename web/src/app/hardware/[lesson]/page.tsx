import { notFound } from 'next/navigation';

import { getLesson, getLessonIds } from '@/content';
import { LessonScreen } from '@/components/LessonScreen';

export async function generateStaticParams() {
  const ids = await getLessonIds('hardware');
  return ids.map((lesson) => ({ lesson }));
}

export default async function HardwareLessonPage({
  params,
}: {
  params: Promise<{ lesson: string }>;
}) {
  const { lesson: id } = await params;

  try {
    return <LessonScreen lesson={await getLesson('hardware', id)} backHref="/hardware" />;
  } catch {
    notFound();
  }
}
