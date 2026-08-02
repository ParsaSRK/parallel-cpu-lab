# Parallel CPU Lab

A C++23 experimental repository for studying shared-memory CPU parallelism
through implementation, correctness testing, benchmarking, and performance
analysis.

The repository currently contains a parallel integer-reduction experiment
implemented with `std::thread`. Additional CPU-parallelism experiments will be
added over time.

## Current Experiment: Parallel Reduction

The first experiment compares sequential and multithreaded summation of a large
array of signed 32-bit integers.

The parallel implementation:

- Divides the input into balanced contiguous ranges
- Assigns one range to each worker thread
- Accumulates each range into a thread-local 64-bit sum
- Joins all worker threads
- Combines the partial sums into the final result

Using 64-bit partial sums prevents overflow caused by accumulating many 32-bit
input values.

## Benchmark Results

The benchmark processes:

- **10,000,000** signed 32-bit integers
- **40,000,000 bytes** of input data
- Thread counts from **1 to 32**
- **7 benchmark repetitions**
- Median measurements reported

### Selected Results

| Implementation | Workers | Median time | Effective throughput | Speedup |
|---|---:|---:|---:|---:|
| Sequential | 1 | 1.310 ms | 28.5 GiB/s | 1.00x |
| Parallel | 16 | 0.483 ms | 77.1 GiB/s | 2.71x |
| Parallel | 20 | 0.550 ms | 67.7 GiB/s | 2.38x |
| Parallel | 22 | 0.578 ms | 64.5 GiB/s | 2.27x |
| Parallel | 32 | 0.604 ms | 61.7 GiB/s | 2.17x |

The test system exposed **22 logical CPUs**.

The best measured result used **16 worker threads**, reducing median execution
time from **1.310 ms to 0.483 ms**.

This corresponds to:

- **2.71x speedup**
- **63.1% lower execution time**
- Effective throughput increasing from **28.5 GiB/s to 77.1 GiB/s**

Using 32 workers was approximately **25% slower** than the 16-worker optimum,
demonstrating the cost of oversubscription and additional thread-management
overhead.

## Scaling Observations

The benchmark showed three general regions:

1. **Low worker counts:** Fixed thread-creation and synchronization costs
   outweighed the benefit of parallel execution.
2. **Useful scaling region:** Performance improved substantially between
   approximately 4 and 16 workers.
3. **Scaling saturation:** Performance declined beyond 16 workers despite the
   machine exposing 22 logical CPUs.

The reduction is a low-computation, high-memory-traffic operation. Its scaling
is therefore influenced by more than available CPU cores, including:

- Worker-thread creation and joining
- Operating-system scheduling
- Cache behavior
- Memory-subsystem throughput
- Oversubscription
- CPU frequency changes

Hardware performance counters would be required to attribute the scaling limit
precisely.

## Benchmark Methodology

Input generation occurs before the timed benchmark loop.

Each parallel benchmark iteration includes:

- Worker-thread construction
- Range processing
- Worker-thread joining
- Final partial-sum reduction

As a result, the measurements represent complete one-shot operation latency,
not only steady-state computation performed by an existing thread pool.

The benchmark uses Google Benchmark's real-time measurement mode.

The recorded run reported that CPU frequency scaling was enabled. The results
should therefore be interpreted as measurements from the specified test
environment rather than universal performance guarantees.

## Correctness Testing

The GoogleTest suite compares the parallel implementation against the
sequential reference implementation.

Current test coverage includes:

- Single-worker execution
- Uneven input partitioning
- More workers than input elements
- Empty input
- Invalid zero-worker input

## Project Structure

```text
parallel-cpu-lab/
├── benchmarks/
│   └── reduction_bench.cpp
├── include/
│   └── reduction.hpp
├── src/
│   └── reduction.cpp
├── tests/
│   └── reduction_test.cpp
├── build_all.sh
├── test_all.sh
├── CMakeLists.txt
├── CMakePresets.json
├── vcpkg.json
└── README.md
```

## Requirements

- A C++23-compatible compiler
- CMake 3.20 or newer
- Ninja
- Git
- vcpkg
- Google Benchmark
- GoogleTest

Google Benchmark and GoogleTest are declared through `vcpkg.json` and are
installed through vcpkg manifest mode during CMake configuration.

## Setting Up vcpkg

Clone and bootstrap vcpkg:

```bash
git clone https://github.com/microsoft/vcpkg.git ~/vcpkg
cd ~/vcpkg
./bootstrap-vcpkg.sh -disableMetrics
```

Set `VCPKG_ROOT`:

```bash
export VCPKG_ROOT="$HOME/vcpkg"
export PATH="$VCPKG_ROOT:$PATH"
```

To make these variables persistent in Zsh:

```bash
cat >> ~/.zshrc <<'EOF'
export VCPKG_ROOT="$HOME/vcpkg"
export PATH="$VCPKG_ROOT:$PATH"
EOF
```

## Building

Clone the repository:

```bash
git clone https://github.com/ParsaSRK/parallel-cpu-lab.git
cd parallel-cpu-lab
```

Configure and build the release preset:

```bash
cmake --preset release
cmake --build --preset release
```

Available presets include:

- `debug`
- `release`
- `relwithdebinfo`
- `asan-ubsan`
- `tsan`

To build every configuration:

```bash
./build_all.sh
```

## Running Tests

Run the release tests:

```bash
ctest --preset release
```

Run the debug tests:

```bash
ctest --preset debug
```

Build and test every configured preset:

```bash
./test_all.sh
```

The sanitizer presets can also be run individually:

```bash
cmake --preset asan-ubsan
cmake --build --preset asan-ubsan
ctest --preset asan-ubsan
```

```bash
cmake --preset tsan
cmake --build --preset tsan
ctest --preset tsan
```

## Running the Benchmark

Build the release configuration:

```bash
cmake --preset release
cmake --build --preset release
```

Run the benchmark with seven repetitions and median aggregate reporting:

```bash
./build/release/reduction_bench \
    --benchmark_repetitions=7 \
    --benchmark_report_aggregates_only=true \
    --benchmark_counters_tabular=true
```

The benchmark currently evaluates the following worker counts:

```text
1, 2, 4, 8, 16, 20, 22, 24, 32
```

## Planned Experiments

Potential future additions include:

- False-sharing benchmarks
- Atomic operations versus mutex-based synchronization
- Parallel prefix sum
- Thread-pool implementations
- OpenMP comparisons
- Cache-line padding experiments
- Work partitioning strategies
- Parallel sorting
- Matrix and stencil computations
- Thread affinity and scheduling experiments

GPU parallelism is intentionally outside the scope of this repository and may
be explored separately.

## Purpose

This repository is intended to document both successful optimizations and the
conditions under which parallelism becomes counterproductive.

The focus is not only on producing parallel implementations, but also on:

- Verifying correctness
- Measuring real performance
- Identifying fixed overhead
- Studying scaling limits
- Explaining unexpected benchmark results
- Improving experimental methodology
