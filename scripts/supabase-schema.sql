-- Graduately progress sync schema for Supabase (Free tier)
--
-- Setup:
-- 1. Create a free project at https://supabase.com
-- 2. Open SQL Editor -> New query, paste this entire file and click Run.
-- 3. Under Authentication -> Providers -> Email, disable "Confirm email" if you
--    want students to sign up with a nickname (or email) and sync immediately
--    without email verification.
-- 4. Copy Project URL and anon public key from Project Settings -> API into
--    GitHub Secrets (SUPABASE_URL, SUPABASE_ANON_KEY) or directly into
--    Settings -> Account & Sync in the app.

create table if not exists public.user_progress (
    user_id uuid primary key references auth.users(id) on delete cascade,
    done text[] not null default '{}',
    updated_at timestamptz not null default now()
);

alter table public.user_progress enable row level security;

drop policy if exists "Users can read own progress" on public.user_progress;
create policy "Users can read own progress"
    on public.user_progress for select
    using (auth.uid() = user_id);

drop policy if exists "Users can insert own progress" on public.user_progress;
create policy "Users can insert own progress"
    on public.user_progress for insert
    with check (auth.uid() = user_id);

drop policy if exists "Users can update own progress" on public.user_progress;
create policy "Users can update own progress"
    on public.user_progress for update
    using (auth.uid() = user_id)
    with check (auth.uid() = user_id);

-- Atomic set-union sync: merges local completed IDs with the server row so
-- offline progress from different devices never overwrites each other.
create or replace function public.sync_progress(client_done text[] default '{}')
returns text[]
language plpgsql
security invoker
as $$
declare
    uid uuid := auth.uid();
    merged text[];
begin
    if uid is null then
        raise exception 'not authenticated';
    end if;

    insert into public.user_progress (user_id, done, updated_at)
    values (
        uid,
        coalesce((select array_agg(distinct x order by x) from unnest(coalesce(client_done, '{}')) as x where length(trim(x)) > 0), '{}'),
        now()
    )
    on conflict (user_id) do update
    set done = coalesce((
            select array_agg(distinct x order by x)
            from unnest(public.user_progress.done || coalesce(client_done, '{}')) as x
            where length(trim(x)) > 0
        ), '{}'),
        updated_at = now()
    returning done into merged;

    return coalesce(merged, '{}');
end;
$$;

grant execute on function public.sync_progress(text[]) to authenticated;
