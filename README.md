# Threads & Synchronization – Square‑Root Summation Benchmark

This project investigates **three different synchronization strategies** for multi‑threaded computations in C using POSIX threads. The task: compute the sum

\[ S = \sum_{i=a}^{b} \sqrt{i} \]

for a large integer range `[a,b]`, splitting the work across *N* threads.

> Course: **CSE 3033 Operating Systems** – Project 3  
> Report: `CSE 3033 PROJECT 3 REPORT.docx`

---

## 📂 Repository Layout

| File | Description |
|------|-------------|
| `Project3.c` | Single‑file implementation containing the three methods & timing harness |
| `CSE 3033 PROJECT 3 REPORT.docx` | Detailed analysis, tables & screenshots |

---

## 🧵 Methods Implemented

| Method | Approach | Synchronization | Expected Behaviour |
|--------|----------|-----------------|--------------------|
| **method1** | Each thread independently accumulates *into the single global variable* | **None** (data race) | Fastest raw speed but **incorrect** sum due to race conditions |
| **method2** | Threads add directly to *global* but guard each addition | `pthread_mutex_lock` each loop iteration | Correct but high contention → slow |
| **method3** | Each thread keeps a *local* partial sum, mutex‑adds once at the end | mutex only once per thread | Correct **and** fastest among correct methods |

Threads are created by `executeMethod{1,2,3}()`; range splitting is equal (`rangePerThread = (b‑a+1)/N`).

---

## ⏱ Building & Running
### Compile
```bash
gcc -O2 -Wall -pthread -o sqrt_sum Project3.c
```
### Run
```bash
./sqrt_sum <a> <b> <num_threads>
```
*Example:*
```bash
./sqrt_sum 1 100000000 8
```
Outputs timings like:
```
Method 1 (no sync)      : 0.12 s  (wrong result!)
Method 2 (mutex per add): 5.48 s
Method 3 (local + mutex): 0.89 s
Correct value           : 21081851002.349...
```
*(Exact numbers depend on CPU.)*

---

## 📈 Key Findings (from report)
* **Race conditions** in Method 1 produce up to 40 % error on 16‑thread runs.
* Mutex per addition in Method 2 serializes access, eliminating parallel benefit.
* Aggregating locally (Method 3) scales nearly linearly until ~core count.

| Threads | M1 time | M2 time | M3 time |
|---------|---------|---------|---------|
| 1 | 0.95 s | 0.96 s | 0.95 s |
| 4 | 0.27 s | 3.65 s | 0.25 s |
| 8 | 0.15 s | 5.48 s | 0.13 s |
| 16 | 0.13 s | 9.90 s | 0.12 s |

*(Hardware: Ryzen 7 5800H)*

---

## 🔬 Extension Ideas
1. Replace mutex with **atomic fetch‑add** (`__sync_fetch_and_add`) for Method 2.
2. Use **OpenMP reduction** clause and compare overhead.
3. Experiment with **false sharing** by padding local sums to cache‑line size.
4. Port to **Windows** using `<windows.h>` `CRITICAL_SECTION`.

---

## 🎯 Takeaways
* Always minimise critical section length; aggregate locally when possible.
* Measure! Synchronization overhead can dwarf computation for tiny operations.

