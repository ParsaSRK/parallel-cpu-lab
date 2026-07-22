#include "reduction.hpp"

#include <span>
#include <cstdint>
#include <stdexcept>
#include <vector>
#include <thread>
#include <cassert>

using i32 = std::int32_t;
using i64 = std::int64_t;

i64 sum_sequential(std::span<const i32> arr) {
    i64 result{};
    for(const auto x: arr) result += x;
    return result;
}

i64 sum_parallel(std::span<const i32> arr, std::size_t thread_count) {
    if (thread_count == 0) {
        throw std::invalid_argument("thread_count must be greater than zero");
    }

    std::vector<i64> partial_sums(thread_count);

    std::vector<std::thread> threads;
    threads.reserve(thread_count);

    const std::size_t n = arr.size();

    for(std::size_t t{}; t < thread_count; ++t) {
        const std::size_t begin = t * n / thread_count;
        const std::size_t end = (t+1) * n / thread_count;

        threads.emplace_back(
            [&arr, &partial_sums, t, begin, end] {
                i64 local_sum = 0;
                for(std::size_t i = begin; i < end; ++i) {
                    local_sum += arr[i];
                }
                partial_sums[t] = local_sum;
            }
        );
    }

    for(std::thread& t: threads) {
        t.join();
    }

    i64 result{};
    
    for(const i64 partial_sum: partial_sums) {
        result += partial_sum;
    }

    return result;
}
