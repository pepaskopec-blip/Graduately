import { notFound } from 'next/navigation';

import { getLesson } from '@/content';
import { LessonScreen } from '@/components/LessonScreen';

export default async function MluvnicePage() {
  try {
    return <LessonScreen lesson={await getLesson('czech', 'mluvnice')} backHref="/czech" />;
  } catch {
    notFound();
  }
}
