

# 🧰  README

````md
# Memory Safety Lab (C + gdb + ASan) 🧨

This repo documents hands-on work reproducing and diagnosing classic memory-safety failures in C,
with a vulnerability-research lens: evidence, root cause, and fixes.

> [!NOTE]
> Watching crash videos is not skill. Reproducing, debugging, and explaining crashes is skill.

## Contents
- `src/basics.c` – stack vs heap basics (addresses + lifetime framing)
- `src/dangling_stack.c` – returning address of stack local → undefined behavior → crash
- `src/uaf.c` – heap use-after-free (best observed with AddressSanitizer)
- `src/crashes.c` – segmentation fault patterns for gdb practice
- `notes/analysis.md` – write-up: what happened, why, evidence, fix, security relevance

## Build
```bash
make
````

## Run

```bash
./bin/basics
./bin/dangling_stack
./bin/crashes 1
./bin/uaf_asan
```

## Debug with gdb

```bash
gdb --args ./bin/dangling_stack
```

Inside gdb:

```gdb
run
bt
p p
frame 0
list
```

## Sanitize (deterministic bug reports)

```bash
./bin/uaf_asan
```

## Disclaimer

> [!WARNING]
> These programs intentionally perform unsafe memory operations for learning purposes.
> Do not run them on systems where you care about stability.

````
