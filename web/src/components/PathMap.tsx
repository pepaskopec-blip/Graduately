'use client';

import Link from 'next/link';
import { useEffect, useState } from 'react';

import { CheckIcon, LockIcon } from './icons';

export interface PathNode {
  number: number;
  label: string;
  /** Destination for nodes that navigate; omit when using `onSelect`. */
  href?: string | null;
  locked: boolean;
  done: boolean;
}

/**
 * Serpentine learning path.
 *
 * The desktop version measures the window and folds the rail into rows; here
 * the node count per row is derived from the viewport width and rows alternate
 * direction in CSS, so the shape survives a resize without measuring anything.
 */
export function PathMap({
  nodes,
  finish,
  onSelect,
}: {
  nodes: PathNode[];
  finish?: boolean;
  /** Used instead of `href` when a node selects something on the same page. */
  onSelect?: (node: PathNode) => void;
}) {
  const perRow = useNodesPerRow();
  const rows: PathNode[][] = [];

  for (let i = 0; i < nodes.length; i += perRow) {
    rows.push(nodes.slice(i, i + perRow));
  }

  return (
    <div className="path">
      {rows.map((row, rowIndex) => (
        <div className="path__row" key={rowIndex}>
          {row.map((node, i) => (
            <Fragmentish key={node.number}>
              {i > 0 && <Rail done={row[i - 1].done} />}
              <PathNodeButton node={node} onSelect={onSelect} />
            </Fragmentish>
          ))}

          {/* Keep the last row left-aligned instead of stretching its nodes. */}
          {row.length < perRow && <span className="spacer" />}
        </div>
      ))}

      {finish && (
        <div className="path__row">
          <span className="node node--finish" aria-label="Finish">
            <span className="node__number">★</span>
          </span>
          <span className="spacer" />
        </div>
      )}
    </div>
  );
}

const Rail = ({ done }: { done: boolean }) => (
  <span className={`path__rail${done ? ' path__rail--done' : ''}`} aria-hidden />
);

/** Avoids importing Fragment just to group a rail with its node. */
function Fragmentish({ children }: { children: React.ReactNode }) {
  return <>{children}</>;
}

function PathNodeButton({
  node,
  onSelect,
}: {
  node: PathNode;
  onSelect?: (node: PathNode) => void;
}) {
  const state = node.locked ? 'locked' : node.done ? 'done' : 'current';
  const className = `node node--${state}`;

  const inner = (
    <>
      {node.done ? (
        <CheckIcon />
      ) : node.locked ? (
        <LockIcon />
      ) : (
        <span className="node__number">{node.number}</span>
      )}
      <span className="node__label">{node.label}</span>
    </>
  );

  if (node.locked) {
    return (
      <span className={className} aria-disabled title={node.label}>
        {inner}
      </span>
    );
  }

  if (onSelect) {
    return (
      <button className={className} title={node.label} onClick={() => onSelect(node)}>
        {inner}
      </button>
    );
  }

  if (!node.href) {
    return (
      <span className={className} aria-disabled title={node.label}>
        {inner}
      </span>
    );
  }

  return (
    <Link className={className} href={node.href} title={node.label}>
      {inner}
    </Link>
  );
}

/** Node count per serpentine row, chosen to keep labels legible on phones. */
function useNodesPerRow(): number {
  const [perRow, setPerRow] = useState(4);

  useEffect(() => {
    const update = () => {
      const w = window.innerWidth;
      if (w < 420) setPerRow(2);
      else if (w < 620) setPerRow(3);
      else if (w < 860) setPerRow(4);
      else setPerRow(5);
    };

    update();
    window.addEventListener('resize', update);
    return () => window.removeEventListener('resize', update);
  }, []);

  return perRow;
}
