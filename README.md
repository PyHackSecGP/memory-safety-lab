# memory-safety-lab

# Memory Safety Lab Notes (Vuln Research Perspective)

## Goals
- Reproduce memory-safety failures in C
- Capture evidence (output + gdb backtrace)
- Explain root cause and fix
- Connect to real vulnerability classes (UAF, OOB, stack lifetime bugs)

---

## 1) Stack vs Heap: `src/basics.c`

### What I did
Compiled and ran:
```bash
make basics
./bin/basics
What happened
It printed 2 addresses:

stack local variable address (&x)

heap allocation address (p returned by malloc)

Example from my run:

yaml
Copy code
stack x addr: 0x7ffcc703c25c value=42
heap p addr: 0x5578c08c86b0 value=1337
Why it matters (security)
Most memory corruption bugs come from:

using memory after lifetime ends (stack/heap)

writing out of bounds

confusing ownership responsibilities

Stack lifetime is automatic; heap lifetime is manual and error-prone.

2) Dangling Stack Pointer: src/dangling_stack.c
What I did
Compiled and ran:

bash
Copy code
make dangling_stack
./bin/dangling_stack
What happened
Compiler warned:

returning address of local variable

Program output:

showed &x inside the function

p printed as (nil) in main

segfault when dereferencing *p

Evidence (gdb)
Run:

bash
Copy code
gdb --args ./bin/dangling_stack
(gdb) run
(gdb) bt
(gdb) p p
(gdb) frame 0
(gdb) list
Observed crash:

SIGSEGV at the line that dereferenced *p

p == 0x0 (NULL) at crash time

Root cause
The function returns &x where x is a local (stack) variable.
When the function returns, the stack frame is gone. Using that address is undefined behavior.
Undefined behavior means the return value can become NULL, garbage, or “appear to work”.

Fix
Do not return pointers to stack locals.
Correct patterns:

return a value (not pointer)

allocate on heap and document ownership (malloc + caller free)

or pass output pointer into function

Security relevance
This is the same conceptual category as use-after-free: using memory past lifetime.
Lifetime bugs become primitives for:

information disclosure (reading stale memory)

control-flow corruption (if attacker can influence reused memory)

exploitation depends on mitigations (ASLR, NX, stack canaries, hardened allocators)

3) Heap Use-After-Free (UAF): src/uaf.c
What I did
Build with sanitizers:

bash
Copy code
make uaf_asan
./bin/uaf_asan
Expected result
AddressSanitizer should report heap-use-after-free with:

allocation site

free site

invalid access site

Fix
After free(p):

never read or write p

set p = NULL

establish clear ownership rules and lifetimes

yaml
Copy code

---

# 6) Add a README that sells this as real vuln-research practice

## `README.md`
```md
# Memory Safety Lab (C + gdb + ASan) 🧨

This repo documents hands-on work reproducing and diagnosing classic memory-safety failures in C,
with a vulnerability-research lens: evidence, root cause, and fixes.

## Why this exists
Watching crash videos is not skill. Reproducing, debugging, and explaining crashes is skill.

## Contents
- `src/basics.c` – stack vs heap basics (addresses + lifetime framing)
- `src/dangling_stack.c` – returning address of stack local → undefined behavior → crash
- `src/uaf.c` – heap use-after-free (best observed with AddressSanitizer)
- `src/crashes.c` – segmentation fault patterns for gdb practice
- `notes/analysis.md` – write-up: what happened, why, evidence, fix, security relevance

## Build
```bash
make
Run
bash
Copy code
./bin/basics
./bin/dangling_stack
./bin/crashes 1
./bin/uaf_asan
Debug with gdb
bash
Copy code
gdb --args ./bin/dangling_stack
(gdb) run
(gdb) bt
(gdb) p p
(gdb) frame 0
(gdb) list
Sanitize (deterministic bug reports)
bash
Copy code
./bin/uaf_asan
Disclaimer
These programs intentionally perform unsafe memory operations for learning purposes.
Do not run on systems where you care about stability.

yaml
Copy code

---

# 7) Add a “how to capture gdb logs” file (optional but looks serious)

## `gdb_logs/HOWTO.md`
```md
# Capturing gdb logs

Inside gdb:
```gdb
set pagination off
set logging file gdb_dangling_stack.txt
set logging on
run
bt
p p
frame 0
list
set logging off
This writes a reproducible debugging transcript to gdb_dangling_stack.txt.

yaml
Copy code

---

# 8) Initialize git + commit
```bash
git init
git add .
git commit -m "Initial memory safety lab: stack vs heap, dangling stack ptr crash, tooling"
