#include "reduction.hpp"

#include <benchmark/benchmark.h>

#include <cstdint>
#include <vector>
#include <random>

using i32 = std::int32_t;
using i64 = std::int64_t;

namespace {
    std::vector<i32> make_random_array(std::size_t size, std::uint32_t seed) {
        std::mt19937 gen(seed);
        std::uniform_int_distribution<i32> dist(-1000, 1000);

        std::vector<i32> arr(size);

        for (i32& x : arr) {
            x = dist(gen);
        }

        return arr;
    }
} // namespace

void BM_SequentialSum(benchmark::State& state) {
    const auto n = static_cast<std::size_t>(state.range(0));
    
    const auto arr = make_random_array(n, 2026);

    for(auto _ : state) {
        i64 result = sum_sequential(arr);
        benchmark::DoNotOptimize(result);
    }
    state.SetItemsProcessed(
        state.iterations() *
        static_cast<std::int64_t>(n)
    );
    state.SetBytesProcessed(
        state.iterations() *
        static_cast<std::int64_t>(n * sizeof(i32))
    );
}

void BM_ParallelSumLocal(benchmark::State& state) {
    const auto n = static_cast<std::size_t>(state.range(0));
    const auto t = static_cast<std::size_t>(state.range(1));
    
    const auto arr = make_random_array(n, 2026);

    for(auto _ : state) {
        i64 result = sum_parallel_local(arr, t);
        benchmark::DoNotOptimize(result);
    }
    state.SetItemsProcessed(
        state.iterations() *
        static_cast<i64>(n)
    );
    state.SetBytesProcessed(
        state.iterations() *
        static_cast<i64>(n * sizeof(i32))
    );
}

void BM_ParallelSumMutex(benchmark::State& state) {
    const auto n = static_cast<std::size_t>(state.range(0));
    const auto t = static_cast<std::size_t>(state.range(1));

    const auto arr = make_random_array(n, 2027);

    for(auto _ : state) {
        i64 result = sum_parallel_mutex(arr, t);
        benchmark::DoNotOptimize(result);
    }

    state.SetItemsProcessed(
        state.iterations() *
        static_cast<i64>(n)
    );
    state.SetBytesProcessed(
        state.iterations() *
        static_cast<i64>(n * sizeof(i32))
    );
}

void BM_ParallelSumAtomic(benchmark::State& state) {
    const auto n = static_cast<std::size_t>(state.range(0));
    const auto t = static_cast<std::size_t>(state.range(1));

    const auto arr = make_random_array(n, 2027);

    for(auto _ : state) {
        i64 result = sum_parallel_atomic(arr, t);
        benchmark::DoNotOptimize(result);
    }

    state.SetItemsProcessed(
        state.iterations() *
        static_cast<i64>(n)
    );
    state.SetBytesProcessed(
        state.iterations() *
        static_cast<i64>(n * sizeof(i32))
    );
}

BENCHMARK(BM_SequentialSum)
    ->Arg(10'000'000)
    ->ArgName("size")
    ->Unit(benchmark::kMillisecond)
    ->UseRealTime();

BENCHMARK(BM_ParallelSumLocal)
    ->ArgsProduct({{10'000'000}, {1, 2, 8, 16, 24, 32}})
    ->ArgNames({"size", "threads"})
    ->Unit(benchmark::kMillisecond)
    ->UseRealTime();

BENCHMARK(BM_ParallelSumMutex)
    ->ArgsProduct({{10'000'000}, {1, 2, 8, 16, 24, 32}})
    ->ArgNames({"size", "threads"})
    ->Unit(benchmark::kMillisecond)
    ->UseRealTime();

BENCHMARK(BM_ParallelSumAtomic)
    ->ArgsProduct({{10'000'000}, {1, 2, 8, 16, 24, 32}})
    ->ArgNames({"size", "threads"})
    ->Unit(benchmark::kMillisecond)
    ->UseRealTime();
