// Compile:
// g++ -std=c++17 -O3 -mavx2 -msse4.1 -Wall -Wextra problem2.cpp -o problem2
// Run:
// ./problem2

#include <algorithm>
#include <cassert>
#include <chrono>
#include <climits>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <x86intrin.h>

using std::cout;
using std::endl;
using std::chrono::duration_cast;
using HR = std::chrono::high_resolution_clock;
using HRTimer = HR::time_point;
using std::chrono::microseconds;

#define N (1 << 16)
#define SSE_WIDTH_BITS (128)
#define ALIGN (32)

/** Helper methods for debugging */

void print_array(const int* array) {
    for (int i = 0; i < N; i++) {
        cout << array[i] << "\t";
    }
    cout << "\n";
}

void print128i_u32(__m128i var, int start) {
    alignas(ALIGN) uint32_t val[4];
    _mm_store_si128(reinterpret_cast<__m128i*>(val), var);
    cout << "Values [" << start << ":" << start + 3 << "]: "
         << val[0] << " " << val[1] << " "
         << val[2] << " " << val[3] << "\n";
}

void print128i_u64(__m128i var) {
    alignas(ALIGN) uint64_t val[2];
    _mm_store_si128(reinterpret_cast<__m128i*>(val), var);
    cout << "Values [0:1]: " << val[0] << " " << val[1] << "\n";
}

__attribute__((optimize("no-tree-vectorize"))) int
ref_version(int* __restrict__ source, int* __restrict__ dest) {
    source = static_cast<int*>(__builtin_assume_aligned(source, ALIGN));
    dest = static_cast<int*>(__builtin_assume_aligned(dest, ALIGN));

    int tmp = 0;
    for (int i = 0; i < N; i++) {
        // Store before adding to compute an exclusive prefix sum.
        dest[i] = tmp;
        tmp += source[i];
    }
    return tmp;
}

__attribute__((noinline))
int sse4_version(const int* __restrict__ source,
                 int* __restrict__ dest) {
    int running_sum = 0;

    for (int i = 0; i < N; i += 4) {
        // Load 4 integers.
        __m128i x =
            _mm_load_si128(reinterpret_cast<const __m128i*>(source + i));

        // Shift by 1 element and add.
        // [a, b, c, d] becomes [a, a+b, b+c, c+d].
        __m128i t = _mm_slli_si128(x, 4);
        x = _mm_add_epi32(x, t);

        // Shift by 2 elements and add.
        // x becomes [a, a+b, a+b+c, a+b+c+d].
        t = _mm_slli_si128(x, 8);
        x = _mm_add_epi32(x, t);

        // Convert the inclusive prefix sum to an exclusive prefix sum.
        __m128i exclusive = _mm_slli_si128(x, 4);

        // Add the sum of all previously processed blocks.
        __m128i offset = _mm_set1_epi32(running_sum);
        exclusive = _mm_add_epi32(exclusive, offset);

        _mm_store_si128(reinterpret_cast<__m128i*>(dest + i), exclusive);

        // The last inclusive prefix sum is the current block's total.
        running_sum += _mm_extract_epi32(x, 3);
    }

    return running_sum;
}

__attribute__((noinline))
int avx2_version(const int* source, int* dest) {
    int running_sum = 0;

    for (int i = 0; i < N; i += 8) {
        // Load 8 integers.
        __m256i x =
            _mm256_load_si256(reinterpret_cast<const __m256i*>(source + i));

        // These shifts operate independently within each 128-bit lane.
        // They compute a separate inclusive prefix sum within each lane.

        // Shift by 1 element within each lane and add.
        // [a, b, c, d | e, f, g, h]
        // becomes [a, a+b, b+c, c+d | e, e+f, f+g, g+h].
        __m256i t = _mm256_slli_si256(x, 4);
        x = _mm256_add_epi32(x, t);

        // Shift by 2 elements within each lane and add.
        // x becomes:
        // [a, a+b, a+b+c, a+b+c+d | e, e+f, e+f+g, e+f+g+h].
        t = _mm256_slli_si256(x, 8);
        x = _mm256_add_epi32(x, t);

        // Extract the lower and upper 128-bit lanes.
        __m128i lower = _mm256_castsi256_si128(x);
        __m128i upper = _mm256_extracti128_si256(x, 1);

        // Convert each lane's inclusive prefix sum to an exclusive sum.
        __m128i exclusive_lower = _mm_slli_si128(lower, 4);
        __m128i exclusive_upper = _mm_slli_si128(upper, 4);

        // The last element of lower contains the lower lane's total.
        int lower_sum = _mm_extract_epi32(lower, 3);
        __m128i lower_offset = _mm_set1_epi32(lower_sum);

        // Account for the first 4 elements in the upper lane's sums.
        upper = _mm_add_epi32(upper, lower_offset);
        exclusive_upper = _mm_add_epi32(exclusive_upper, lower_offset);

        // Combine both exclusive prefix sums into one 256-bit vector.
        __m256i result = _mm256_castsi128_si256(exclusive_lower);
        result = _mm256_inserti128_si256(result, exclusive_upper, 1);

        // Add the total of all previously processed blocks.
        __m256i global_offset = _mm256_set1_epi32(running_sum);
        result = _mm256_add_epi32(result, global_offset);

        _mm256_store_si256(reinterpret_cast<__m256i*>(dest + i), result);

        // The last element of upper contains the entire block's total.
        running_sum += _mm_extract_epi32(upper, 3);
    }

    return running_sum;
}

__attribute__((optimize("no-tree-vectorize"))) int main() {
    // Allocate and initialize the input array.
    int* array =
        static_cast<int*>(aligned_alloc(ALIGN, N * sizeof(int)));
    std::fill(array, array + N, 1);

    // Allocate and initialize the sequential output.
    int* ref_res =
        static_cast<int*>(aligned_alloc(ALIGN, N * sizeof(int)));
    std::fill(ref_res, ref_res + N, 0);

    // Measure the sequential execution time.
    HRTimer start = HR::now();
    int val_ser = ref_version(array, ref_res);
    HRTimer end = HR::now();

    auto duration = duration_cast<microseconds>(end - start).count();
    auto serial_time = duration;

    cout << "Serial version: " << val_ser
         << " time: " << duration << " us" << endl;

    // Allocate and initialize the SSE4 output.
    int* sse_res =
        static_cast<int*>(aligned_alloc(ALIGN, N * sizeof(int)));
    std::fill(sse_res, sse_res + N, 0);

    // Measure the SSE4 execution time.
    start = HR::now();
    int val_sse = sse4_version(array, sse_res);
    end = HR::now();

    duration = duration_cast<microseconds>(end - start).count();
    auto sse_time = duration;

    // Verify the SSE4 total and every output element.
    assert(val_ser == val_sse && "SSE total is wrong!");

    for (int i = 0; i < N; i++) {
        assert(ref_res[i] == sse_res[i] && "SSE result is wrong!");
    }

    cout << "SSE version: " << val_sse
         << " time: " << duration << " us" << endl;

    // Release the SSE4 output after verification.
    free(sse_res);

    // Allocate and initialize the AVX2 output.
    int* avx2_res =
        static_cast<int*>(aligned_alloc(ALIGN, N * sizeof(int)));
    std::fill(avx2_res, avx2_res + N, 0);

    // Measure the AVX2 execution time.
    start = HR::now();
    int val_avx2 = avx2_version(array, avx2_res);
    end = HR::now();

    duration = duration_cast<microseconds>(end - start).count();
    auto avx2_time = duration;

    // Verify the AVX2 total and every output element.
    assert(val_ser == val_avx2 && "AVX2 total is wrong!");

    for (int i = 0; i < N; i++) {
        assert(ref_res[i] == avx2_res[i] && "AVX2 result is wrong!");
    }

    cout << "AVX2 version: " << val_avx2
         << " time: " << duration << " us" << endl;

    // Compute speedups, avoiding division by zero.
    if (sse_time > 0) {
        cout << "SSE4 speedup over serial: "
             << static_cast<double>(serial_time) / sse_time
             << "x" << endl;
    }

    if (avx2_time > 0) {
        cout << "AVX2 speedup over serial: "
             << static_cast<double>(serial_time) / avx2_time
             << "x" << endl;
    }

    // Release the remaining arrays.
    free(avx2_res);
    free(ref_res);
    free(array);

    return EXIT_SUCCESS;
}