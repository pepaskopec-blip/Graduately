import { getSubjects } from '@/content';
import { SubjectsScreen } from './SubjectsScreen';

export default async function SubjectsPage() {
  return <SubjectsScreen subjects={await getSubjects()} />;
}
