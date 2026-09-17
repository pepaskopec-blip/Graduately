import { notFound } from 'next/navigation';

import { getLesson, getPath } from '@/content';
import { LessonScreen } from '@/components/LessonScreen';

export async function generateStaticParams() {
  const path = await getPath('czech', 'reading');
  return (path?.units ?? [])
    .filter((unit) => unit.lesson !== null)
    .map((unit) => ({ book: unit.lesson as string }));
}

export default async function BookPage({ params }: { params: Promise<{ book: string }> }) {
  const { book } = await params;

  try {
    return <LessonScreen lesson={await getLesson('czech', book)} backHref="/czech/reading" />;
  } catch {
    notFound();
  }
}
