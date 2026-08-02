# Parallel Reduction and Benchmarking Lab

A C++23 project exploring multithreaded reduction, synchronization overhead,
thread-count scaling, and memory-bandwidth limits using `std::thread`,
Google Benchmark, and GoogleTest.

## Results

Benchmarks processed 10,000,000 signed 32-bit integers (approximately 40 MB)
on a machine exposing 22 logical CPUs.

| Implementation | Median time | Effective throughput | Speedup |
|---|---:|---:|---:|
| Sequential | 1.31 ms | 28.5 GiB/s | 1.00x |
| Parallel, 8 threads | ... | ... | ... |
| Parallel, 16 threads | 0.483 ms | 77.1 GiB/s | 2.71x |
| Parallel, 20 threads | 0.550 ms | ... | 2.38x |
| Parallel, 22 threads | 0.578 ms | ... | 2.27x |
| Parallel, 32 threads | 0.604 ms | ... | 2.17x |

The best result occurred at 16 worker threads. Increasing the worker count to
32 made the reduction approximately 25% slower than the 16-thread optimum,
demonstrating oversubscription and thread-management overhead.

Results are hardware-specific and include thread creation and joining during
each benchmark iteration.

## Implementation

- Sequential integer reduction
- Balanced contiguous range partitioning
- Per-thread local accumulation
- Final reduction after joining worker threads
- 64-bit accumulation for 32-bit input values

## Testing

GoogleTest coverage includes:

- Single-thread execution
- Uneven partitions
- More requested workers than input elements
- Empty input
- Invalid zero-worker input

## Build

### Requirements

- C++23 compiler
- CMake 3.20+
- Ninja
- vcpkg
- GoogleTest
- Google Benchmark

```bash
cmake --preset release
cmake --build --preset release
