'use client';

import { useApp } from '@/app/providers';

export type CheckState = 'idle' | 'ok' | 'wrong';

/** The instruction lines every exercise takes from its task. */
export interface TaskFrame {
  subtitle?: string;
  /** Worked example shown above the items. */
  example?: string;
  /** Extra guidance under the instruction, e.g. a word bank. */
  note?: string;
}

/**
 * Frame shared by every exercise: the instruction, the task body, a check
 * button and the pass/fail feedback line.
 */
export function TaskShell({
  subtitle,
  example,
  note,
  state,
  checkLabel,
  onCheck,
  onReveal,
  revealed,
  children,
}: TaskFrame & {
  state: CheckState;
  /** i18n key overriding the default "check" button label. */
  checkLabel?: string;
  onCheck: () => void;
  /** Provided by exercises that can show a model solution. */
  onReveal?: () => void;
  revealed?: boolean;
  children: React.ReactNode;
}) {
  const { t } = useApp();

  return (
    <div className="stack">
      {subtitle && <p className="page__sub">{subtitle}</p>}
      {note && <p className="ex__note">{note}</p>}
      {example && <p className="ex__example">{example}</p>}

      <div className="ex">{children}</div>

      <div className="actions">
        <button className="btn" onClick={onCheck}>
          {t(checkLabel ?? 'check')}
        </button>

        {onReveal && (
          <button className="btn btn--ghost" onClick={onReveal} disabled={revealed}>
            {t('net_solution')}
          </button>
        )}
      </div>

      {state !== 'idle' && (
        <p className={`feedback feedback--${state === 'ok' ? 'ok' : 'err'}`} role="status">
          {t(state === 'ok' ? 'feedback_ok' : 'feedback_retry_short')}
        </p>
      )}
    </div>
  );
}
