import { getLessons, getSubjects } from '@/content';
import { StatsScreen, type SubjectStats } from './StatsScreen';

export default async function StatsPage() {
  const subjects = await getSubjects();

  const tracked: SubjectStats[] = await Promise.all(
    subjects
      .filter((s) => s.unlocked)
      .map(async (subject) => {
        const lessons = await getLessons(subject.id);
        return {
          id: subject.id,
          key: subject.key,
          lessons: lessons.map((l) => ({
            id: l.id,
            number: l.number,
            titleKey: l.titleKey ?? null,
            title: l.title?.cs ?? null,
            taskCount: l.tasks.length,
          })),
        };
      })
  );

  const locked = subjects.filter((s) => !s.unlocked).map((s) => ({ id: s.id, key: s.key }));

  return <StatsScreen tracked={tracked} locked={locked} />;
}
