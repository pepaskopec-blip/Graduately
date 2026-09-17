import type { NextConfig } from 'next';

const config: NextConfig = {
  // `content/` lives outside web/, so allow tracing files from the repo root.
  outputFileTracingRoot: __dirname + '/..',
  // Routes are built from content ids at runtime, so they cannot be statically typed.
  typedRoutes: false,
};

export default config;
