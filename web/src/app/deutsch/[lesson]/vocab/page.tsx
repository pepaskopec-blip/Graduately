import { notFound } from 'next/navigation';

import { getLesson, getLessonIds } from '@/content';
import { VocabTrainer } from './VocabTrainer';

export async function generateStaticParams() {
  const ids = await getLessonIds('deutsch');
  return ids.map((lesson) => ({ lesson }));
}

export default async function VocabPage({ params }: { params: Promise<{ lesson: string }> }) {
  const { lesson: id } = await params;

  try {
    const lesson = await getLesson('deutsch', id);
    if (!lesson.vocab?.length) notFound();
    return <VocabTrainer sections={lesson.vocab} backHref={`/deutsch/${id}`} />;
  } catch {
    notFound();
  }
}
