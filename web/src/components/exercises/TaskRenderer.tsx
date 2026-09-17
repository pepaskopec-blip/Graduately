'use client';

import type { Task } from '@/content';
import { AssemblyTask } from './AssemblyTask';
import { AssignTask } from './AssignTask';
import { ChoiceTask } from './ChoiceTask';
import { DialogueTask } from './DialogueTask';
import { FieldsTask } from './FieldsTask';
import { FillTask } from './FillTask';
import { FreeTask } from './FreeTask';
import { HangmanTask } from './HangmanTask';
import { ProfileTask } from './ProfileTask';
import { RevealTask } from './RevealTask';
import { SubnetTask } from './SubnetTask';
import { TableTask } from './TableTask';
import { TypedTask } from './TypedTask';

/**
 * Picks the component for a task based on the `kind` of its items.
 *
 * Every task in the content holds items of a single kind, so the first item
 * decides the renderer.
 */
export function TaskRenderer({
  task,
  subtitle,
  onSolved,
}: {
  task: Task;
  subtitle?: string;
  onSolved: () => void;
}) {
  const first = task.items[0];
  if (!first) return null;

  const shared = {
    subtitle,
    example: task.example,
    note: task.note,
    onSolved,
  };

  switch (first.kind) {
    case 'choice':
      return <ChoiceTask items={task.items.filter(is('choice'))} {...shared} />;

    case 'fill':
      return (
        <FillTask
          items={task.items.filter(is('fill'))}
          pool={task.pool}
          altPool={task.altPool}
          {...shared}
        />
      );

    case 'dialogue':
      return (
        <DialogueTask items={task.items.filter(is('dialogue'))} pool={task.pool} {...shared} />
      );

    case 'assembly':
      return <AssemblyTask items={task.items.filter(is('assembly'))} {...shared} />;

    case 'assign':
      // Group sorting is always a single exercise per task.
      return <AssignTask exercise={first} {...shared} />;

    case 'free':
      return <FreeTask items={task.items.filter(is('free'))} {...shared} />;

    case 'typed':
      return <TypedTask items={task.items.filter(is('typed'))} {...shared} />;

    case 'fields':
      return <FieldsTask items={task.items.filter(is('fields'))} {...shared} />;

    case 'reveal':
      return <RevealTask items={task.items.filter(is('reveal'))} {...shared} />;

    case 'hangman':
      return <HangmanTask items={task.items.filter(is('hangman'))} {...shared} />;

    case 'profile':
      return <ProfileTask items={task.items.filter(is('profile'))} {...shared} />;

    case 'table':
      return <TableTask exercise={first} {...shared} />;

    case 'subnet':
      return <SubnetTask items={task.items.filter(is('subnet'))} {...shared} />;
  }
}

/** Narrowing filter, so each component receives only its own item type. */
function is<K extends Task['items'][number]['kind']>(kind: K) {
  return (item: Task['items'][number]): item is Extract<Task['items'][number], { kind: K }> =>
    item.kind === kind;
}
