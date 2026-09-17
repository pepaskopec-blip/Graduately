import { notFound } from 'next/navigation';

import { getLesson, getLessonIds } from '@/content';
import { LessonScreen } from '@/components/LessonScreen';

export async function generateStaticParams() {
  const ids = await getLessonIds('deutsch');
  return ids.map((lesson) => ({ lesson }));
}

export default async function DeutschLessonPage({
  params,
}: {
  params: Promise<{ lesson: string }>;
}) {
  const { lesson: id } = await params;

  try {
    const lesson = await getLesson('deutsch', id);
    return (
      <LessonScreen
        lesson={lesson}
        backHref="/deutsch"
        vocabHref={lesson.vocab?.length ? `/deutsch/${id}/vocab` : undefined}
      />
    );
  } catch {
    notFound();
  }
}
