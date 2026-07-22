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

TEST(ParallelSum, HandlesSingleThread) {
    const auto arr = make_random_array(N, 2026);

    EXPECT_EQ(sum_parallel(arr, 1), sum_sequential(arr));
}

TEST(ParallelSum, HandlesMoreThreadsThanElements) {

    std::size_t size = N / 100;
    const auto arr = make_random_array(size, 2027);

    EXPECT_EQ(sum_parallel(arr, size + 5), sum_sequential(arr));
}

TEST(ParallelSum, HandlesEmptyArray) {
    const std::vector<i32> arr{};

    EXPECT_EQ(sum_parallel(arr, 5), sum_sequential(arr));
}

TEST(ParallelSum, TenThreads) {
    const auto arr = make_random_array(N, 2028);

    EXPECT_EQ(sum_parallel(arr, 10), sum_sequential(arr));
}

TEST(ParallelSum, ZeroThreads) {
    const auto arr = make_random_array(N / 100, 2029);

    EXPECT_THROW(sum_parallel(arr, 0), std::invalid_argument);
}
