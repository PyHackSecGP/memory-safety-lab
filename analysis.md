# 🧠💥 `memory-safety-lab`

## Memory Safety Lab Notes (Vuln Research Perspective)

> [!TIP]
> **Goal:** stop “watching crash videos” and start **reproducing + proving + explaining** crashes like an actual vuln researcher.

---

## 🎯 Goals

* **Reproduce** memory-safety failures in C
* **Capture evidence** (program output + **gdb backtrace**)
* **Explain** root cause and fix (not vibes)
* **Map** to real vulnerability classes (**UAF**, **OOB**, **stack lifetime bugs**)

---

## 📌 1) Stack vs Heap: `src/basics.c`

### ✅ What I did

Compiled and ran:

```bash
make basics
./bin/basics
```

### 🧾 What happened

It printed **two addresses**:

* **Stack** local variable address (`&x`)
* **Heap** allocation address (`p` returned by `malloc`)

Example from my run:

```yaml
stack x addr: 0x7ffcc703c25c value=42
heap  p addr: 0x5578c08c86b0 value=1337
```

### 🔥 Why it matters (security)

Most memory corruption bugs come from:

* **Using memory after lifetime ends** (stack/heap)
* **Writing out of bounds**
* **Ownership confusion** (who frees what, when)

**Stack lifetime = automatic.**
**Heap lifetime = manual and error-prone.**
Humans + manual lifetime management = predictable disaster.

---

## ☠️ 2) Dangling Stack Pointer: `src/dangling_stack.c`

> [!WARNING]
> This is **undefined behavior**. That means it can “work,” crash, return NULL, or summon demons depending on compiler mood.

### ✅ What I did

Compiled and ran:

```bash
make dangling_stack
./bin/dangling_stack
```

### 🧾 What happened

Compiler warned about:

* **returning address of local variable**

Program output:

* showed `&x` inside the function
* `p` printed as `(nil)` in `main`
* **segfault** when dereferencing `*p`

---

### 🧪 Evidence (gdb)

Run:

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

Observed crash:

* **SIGSEGV** at the line dereferencing `*p`
* `p == 0x0` (**NULL**) at crash time

---

### 🧠 Root cause

The function returns `&x` where `x` is a **local stack variable**.
When the function returns, its stack frame is **gone**. Using that address is **undefined behavior**.

Undefined behavior means the return value can become:

* `NULL`
* garbage
* “apparently fine” (the most dangerous outcome)

---

### ✅ Fix

**Never return pointers to stack locals.** Use one of these correct patterns:

#### Option A: Return a value (not a pointer)

```c
int f(void) { 
  int x = 123;
  return x; 
}
```

#### Option B: Allocate on heap + document ownership

```c
int *f(void) {
  int *p = malloc(sizeof(*p));
  *p = 123;
  return p; // caller must free
}
```

#### Option C: Caller provides output pointer

```c
void f(int *out) {
  *out = 123;
}
```

---

### 🧨 Security relevance

This is conceptually the same family as **use-after-free**: **using memory past lifetime**.
Lifetime bugs can become primitives for:

* **Info disclosure** (reading stale memory)
* **Control-flow corruption** (if attacker can influence reused memory)

Exploitability depends on mitigations like **ASLR**, **NX**, stack canaries, and allocator hardening.

---

## 🧟 3) Heap Use-After-Free (UAF): `src/uaf.c`

### ✅ What I did

Built with sanitizers:

```bash
make uaf_asan
./bin/uaf_asan
```

### ✅ Expected result (ASan)

AddressSanitizer should report **heap-use-after-free** with:

* allocation site
* free site
* invalid access site

> [!IMPORTANT]
> If ASan isn’t screaming, you either didn’t trigger the bug, didn’t compile correctly, or the universe is lying.

---

### ✅ Fix

After `free(p)`:

* **never read or write** `p`
* set `p = NULL`
* enforce **clear ownership + lifetimes**

Example pattern:

```c
free(p);
p = NULL;
```

---

---
