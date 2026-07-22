#pragma once

#include <cstdint>
#include <span>

using i32 = std::int32_t;
using i64 = std::int64_t;

i64 sum_sequential(std::span<const i32> arr);
i64 sum_parallel(std::span<const i32> arr, std::size_t thread_count);
