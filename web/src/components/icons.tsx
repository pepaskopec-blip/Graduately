/** Inline SVG counterparts to the Cairo-drawn icons in `desktop/icons.c`. */

const base = {
  width: 20,
  height: 20,
  viewBox: '0 0 24 24',
  fill: 'none',
  stroke: 'currentColor',
  strokeWidth: 2,
  strokeLinecap: 'round' as const,
  strokeLinejoin: 'round' as const,
  'aria-hidden': true,
};

export const BackIcon = () => (
  <svg {...base}>
    <path d="M15 18l-6-6 6-6" />
  </svg>
);

export const LockIcon = ({ size = 18 }: { size?: number }) => (
  <svg {...base} width={size} height={size}>
    <rect x="4" y="10" width="16" height="11" rx="2.5" />
    <path d="M8 10V7a4 4 0 0 1 8 0v3" />
  </svg>
);

export const CheckIcon = ({ size = 22 }: { size?: number }) => (
  <svg {...base} width={size} height={size} strokeWidth={2.6}>
    <path d="M4.5 12.5l5 5L19.5 7" />
  </svg>
);

export const GearIcon = () => (
  <svg {...base}>
    <circle cx="12" cy="12" r="3.2" />
    <path d="M12 2.6v2.6M12 18.8v2.6M4.2 12H1.6M22.4 12h-2.6M6.5 6.5L4.7 4.7M19.3 19.3l-1.8-1.8M17.5 6.5l1.8-1.8M4.7 19.3l1.8-1.8" />
  </svg>
);

export const ChartIcon = () => (
  <svg {...base}>
    <path d="M4 20V10M10 20V4M16 20v-7M22 20H2" />
  </svg>
);

export const WifiIcon = ({ size = 26 }: { size?: number }) => (
  <svg {...base} width={size} height={size}>
    <path d="M2.5 8.5a15 15 0 0 1 19 0M6 12.5a10 10 0 0 1 12 0M9.5 16.5a5 5 0 0 1 5 0" />
    <circle cx="12" cy="20" r="1" fill="currentColor" stroke="none" />
  </svg>
);

export const ChipIcon = ({ size = 26 }: { size?: number }) => (
  <svg {...base} width={size} height={size}>
    <rect x="7" y="7" width="10" height="10" rx="1.5" />
    <path d="M10 3v4M14 3v4M10 17v4M14 17v4M3 10h4M3 14h4M17 10h4M17 14h4" />
  </svg>
);

export const BookIcon = ({ size = 26 }: { size?: number }) => (
  <svg {...base} width={size} height={size}>
    <path d="M4 4.5A1.5 1.5 0 0 1 5.5 3H10a2 2 0 0 1 2 2v15a2 2 0 0 0-2-2H4z" />
    <path d="M20 4.5A1.5 1.5 0 0 0 18.5 3H14a2 2 0 0 0-2 2v15a2 2 0 0 1 2-2h6z" />
  </svg>
);

/** German flag as three stacked bands, matching the desktop subject bubble. */
export const FlagDeIcon = ({ size = 30 }: { size?: number }) => (
  <svg width={size} height={size} viewBox="0 0 24 24" aria-hidden>
    <clipPath id="flag-de-clip">
      <circle cx="12" cy="12" r="11" />
    </clipPath>
    <g clipPath="url(#flag-de-clip)">
      <rect x="0" y="1" width="24" height="7.34" fill="#000" />
      <rect x="0" y="8.34" width="24" height="7.33" fill="#dd0000" />
      <rect x="0" y="15.67" width="24" height="7.33" fill="#ffce00" />
    </g>
    <circle cx="12" cy="12" r="11" fill="none" stroke="currentColor" strokeWidth="1.4" />
  </svg>
);

export const SUBJECT_ICONS = {
  'flag-de': FlagDeIcon,
  wifi: WifiIcon,
  chip: ChipIcon,
  book: BookIcon,
} as const;
