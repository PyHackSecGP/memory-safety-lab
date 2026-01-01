# Memory Safety Lab: What Happened (My POV)

## 1) Setup and Goal
I wanted to *see* how basic C memory mistakes turn into real crashes, then prove the cause with debugging tools (like a vuln researcher, not a guesser).

---

## 2) Stack vs Heap (baseline sanity check)
I wrote and ran a simple program (`basics.c`) that prints:
- the address of a **stack** variable (`&x`)
- the address of a **heap** allocation (`p` from `malloc`)

What I saw:
- The stack address looked like `0x7fff...` (high memory)
- The heap address looked like `0x55...` (different region)

**What that means in plain terms:**
- **Stack memory** is temporary. It exists only while the function is running.
- **Heap memory** is rented manually. It exists until I explicitly `free()` it.

> [!NOTE]
> This isn’t about “address patterns.” The real difference is **lifetime control**: stack is automatic, heap is manual.

---

## 3) The First Real Bug: Returning a Stack Address (dangling pointer)
I wrote `dangling_stack.c`, where a function returns `&x` (a local variable).

GCC warned me:
- **“function returns address of local variable”**

I ran it anyway (because learning):
- Inside the function, `&x` printed a real address.
- In `main`, the pointer printed as `(nil)` (NULL).
- Then it **segfaulted** when the program tried to read `*p`.

> [!WARNING]
> Returning a pointer to a local variable is **undefined behavior**. Once the function returns, that stack memory is “gone.” The program is no longer reliable.

---

## 4) Proving the Cause with gdb (no guessing)
I opened it in gdb and ran it until it crashed.

Evidence gdb gave me:
- `SIGSEGV` occurred in `main` at the line dereferencing `*p`
- `p` was `(nil)` at crash time

**Plain-English root cause:**
- I created a pointer to temporary memory.
- After the function ended, the pointer became invalid (in my run, it became NULL).
- Dereferencing NULL caused the OS to kill the program.

> [!IMPORTANT]
> Crash = symptom. Root cause = **using memory outside its valid lifetime**.

---

## 5) Next Vulnerability Class: Heap Use-After-Free (UAF)
I compiled `uaf.c` with sanitizers (`-fsanitize=address,undefined`).
GCC warned:
- **pointer ‘p’ used after ‘free’**

**What this means:**
- I freed heap memory, then kept using the old pointer.
- Next step is to run `./uaf_asan` to get a full AddressSanitizer report showing:
  - where memory was allocated
  - where it was freed
  - where it was illegally used

> [!CAUTION]
> UAFs matter because they can become **info leaks** or **memory corruption**, depending on allocator reuse and attacker control.
