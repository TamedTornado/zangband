# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

This is the **C reference codebase** for Zangband, a roguelike game. We are porting it to TypeScript.

**Important**: The TypeScript project lives at `../zangbandts`. See `../zangbandts/zangband-ts.md` for the full project plan.

## Development Rules

- **Test-first**: Write tests before implementation (TDD)
- **GitHub**: Use GitHub for version control and collaboration

## TypeScript Port Tech Stack

| Layer | Technology |
|-------|------------|
| Language | TypeScript 5.x (strict mode) |
| Display | rot.js |
| Build | Vite |
| Testing | Vitest |
| Data | JSON with schemas |

## C Reference: Key Files for Data Extraction

### Structured Data Files (`lib/edit/`)
- `r_info.txt` - Monster definitions
- `k_info.txt` - Item (object kind) definitions
- `a_info.txt` - Artifact definitions
- `e_info.txt` - Ego item definitions
- `f_info.txt` - Terrain/feature definitions
- `v_info.txt` - Vault definitions

### Source Code
- `src/spells1.c`, `spells2.c`, `spells3.c` - Spell effects and formulas
- `src/monster1.c`, `monster2.c` - Monster logic
- `src/object1.c`, `object2.c` - Item logic
- `src/melee1.c`, `melee2.c` - Combat formulas
- `src/generate.c`, `rooms.c` - Dungeon generation
- `src/mutation.c` - Mutation system
- `src/wild1.c`, `wild2.c`, `wild3.c` - Wilderness

## Building the C Reference

```bash
meson setup build
meson compile -C build
```
