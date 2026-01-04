# Virtual Memory Simulator (TLB Simulator)

Check out this Translation Lookaside Buffer simulation! 

This program uses LRU replacement.

## Build

```bash
gcc -o tlb_sim tlb_sim.c
```

## Usage

```bash
./tlb_sim <trace_file> [tlb_size]
```

Trace file format is one access per line. Write a hex address followed by size in bytes.

```
0A32 4
4F10 8
```

Output shows TLB hits, misses, and segfaults.
