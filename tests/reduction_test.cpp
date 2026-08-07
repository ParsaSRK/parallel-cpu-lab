#include "reduction.hpp"

#include <gtest/gtest.h>

#include <cstdint>
#include <random>
#include <vector>
#include <stdexcept>

using i32 = std::int32_t;

namespace {
    const std::size_t N = 1001;

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

TEST(ParallelSumLocal, HandlesSingleThread) {
    const auto arr = make_random_array(N, 2026);

    EXPECT_EQ(sum_parallel_local(arr, 1), sum_sequential(arr));
}

TEST(ParallelSumLocal, HandlesMoreThreadsThanElements) {

    std::size_t size = N / 100;
    const auto arr = make_random_array(size, 2027);

    EXPECT_EQ(sum_parallel_local(arr, size + 5), sum_sequential(arr));
}

TEST(ParallelSumLocal, HandlesEmptyArray) {
    const std::vector<i32> arr{};

    EXPECT_EQ(sum_parallel_local(arr, 5), sum_sequential(arr));
}

TEST(ParallelSumLocal, TenThreads) {
    const auto arr = make_random_array(N, 2028);

    EXPECT_EQ(sum_parallel_local(arr, 10), sum_sequential(arr));
}

TEST(ParallelSumLocal, ZeroThreads) {
    const auto arr = make_random_array(N / 100, 2029);

    EXPECT_THROW(sum_parallel_local(arr, 0), std::invalid_argument);
}

TEST(ParallelSumMutex, TenThreads) {
    const auto arr = make_random_array(N, 2030);

    EXPECT_EQ(sum_parallel_mutex(arr, 10), sum_sequential(arr));
}


TEST(ParallelSumAtmoic, TenThreads) {
    const auto arr = make_random_array(N, 2031);

    EXPECT_EQ(sum_parallel_atomic(arr, 10), sum_sequential(arr));
}
