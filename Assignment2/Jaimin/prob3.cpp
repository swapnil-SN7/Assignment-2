// Compile:
// g++ -std=c++17 -O3 -mavx2 -msse4.1 -Wall -Wextra problem3.cpp -o problem3
//
// Run:
// ./problem3

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <iostream>

#include <x86intrin.h>

using namespace std;

using HR = chrono::high_resolution_clock;
using HRTimer = HR::time_point;

constexpr int N = 128;
constexpr int TILE = 32;
constexpr int ALIGN = 32;

static_assert(N > 0 && N % 8 == 0,
              "N must be a positive multiple of 8.");

static_assert(TILE > 0 && TILE % 8 == 0,
              "TILE must be a positive multiple of 8.");

// Sequential matrix multiplication for correctness checking.
// Each function assumes that C is initialized to zero.
__attribute__((optimize("no-tree-vectorize")))
void ref_version(const uint32_t* A,
                 const uint32_t* B,
                 uint32_t* C) {
    for (int i = 0; i < N; ++i) {
        for (int k = 0; k < N; ++k) {
            for (int j = 0; j < N; ++j) {
                C[i * N + j] += A[i * N + k] * B[k * N + j];
            }
        }
    }
}

// Tiled SSE4 multiplication using aligned loads and stores.
void sse4_aligned(const uint32_t* A,
                  const uint32_t* B,
                  uint32_t* C) {
    // TODO:
    // 1. Add tile loops: ii, kk, jj.
    // 2. Compute tile limits using min(start + TILE, N).
    // 3. Add inner loops: i, k, j.
    // 4. Increment j by 4.
    // 5. Broadcast A[i * N + k] using _mm_set1_epi32.
    // 6. Load B and C using _mm_load_si128.
    // 7. Multiply using _mm_mullo_epi32.
    // 8. Add using _mm_add_epi32.
    // 9. Store C using _mm_store_si128.

    for (int ii = 0; ii < N; ii += TILE) {
    for (int kk = 0; kk < N; kk += TILE) {
        for (int jj = 0; jj < N; jj += TILE) {
            int i_end = min(ii + TILE, N);
            int k_end = min(kk + TILE, N);
            int j_end = min(jj + TILE, N);

            for (int i = ii; i < i_end; ++i) {  
                for (int k = kk; k < k_end; ++k) {
                    // Broadcast A[i * N + k] here.
                    __m128i a = _mm_set1_epi32(A[i * N + k]);

                    for (int j = jj; j < j_end; j += 4) {
                        // Load B and C, multiply, add, and store.
                        __m128i b = _mm_load_si128(reinterpret_cast<const __m128i*>(B + k * N + j));

                        __m128i c = _mm_load_si128(reinterpret_cast<const __m128i*>(C + i * N + j));

                        // Multiply and accumulate four products.
                        c = _mm_add_epi32(c, _mm_mullo_epi32(a, b));

                        _mm_store_si128(reinterpret_cast<__m128i*>(C + i * N + j), c);
                    }
                }
            }
        }
    }
}

}

// Tiled SSE4 multiplication using unaligned loads and stores.
void sse4_unaligned(const uint32_t* A,
                    const uint32_t* B,
                    uint32_t* C) {
    // TODO:
    // Use the same tiled algorithm as sse4_aligned.
    // Replace the vector memory operations with:
    // _mm_loadu_si128 and _mm_storeu_si128.

    for (int ii = 0; ii < N; ii += TILE) {
    for (int kk = 0; kk < N; kk += TILE) {
        for (int jj = 0; jj < N; jj += TILE) {
            int i_end = min(ii + TILE, N);
            int k_end = min(kk + TILE, N);
            int j_end = min(jj + TILE, N);

            for (int i = ii; i < i_end; ++i) {  
                for (int k = kk; k < k_end; ++k) {
                    // Broadcast A[i * N + k] here.
                    __m128i a = _mm_set1_epi32(A[i * N + k]);

                    for (int j = jj; j < j_end; j += 4) {
                        // Load B and C, multiply, add, and store.
                        __m128i b = _mm_loadu_si128(reinterpret_cast<const __m128i*>(B + k * N + j));

                        __m128i c = _mm_loadu_si128(reinterpret_cast<const __m128i*>(C + i * N + j));

                        // Multiply and accumulate four products.
                        c = _mm_add_epi32(c, _mm_mullo_epi32(a, b));

                        _mm_storeu_si128(reinterpret_cast<__m128i*>(C + i * N + j), c);
                    }
                }
            }
        }
    }
}
}

// Tiled AVX2 multiplication using aligned loads and stores.
void avx2_aligned(const uint32_t* A,
                  const uint32_t* B,
                  uint32_t* C) {
    // TODO:
    // Use the same tiled loop structure.
    // Increment j by 8.
    // Broadcast using _mm256_set1_epi32.
    // Load using _mm256_load_si256.
    // Multiply using _mm256_mullo_epi32.
    // Add using _mm256_add_epi32.
    // Store using _mm256_store_si256.

    for (int ii = 0; ii < N; ii += TILE) {
    for (int kk = 0; kk < N; kk += TILE) {
        for (int jj = 0; jj < N; jj += TILE) {
            int i_end = min(ii + TILE, N);
            int k_end = min(kk + TILE, N);
            int j_end = min(jj + TILE, N);

            for (int i = ii; i < i_end; ++i) {  
                for (int k = kk; k < k_end; ++k) {
                    // Broadcast A[i * N + k] here.
                    __m256i a = _mm256_set1_epi32(A[i * N + k]);

                    for (int j = jj; j < j_end; j += 8) {
                        // Load B and C, multiply, add, and store.
                        __m256i b = _mm256_load_si256(reinterpret_cast<const __m256i*>(B + k * N + j));

                        __m256i c = _mm256_load_si256(reinterpret_cast<const __m256i*>(C + i * N + j));

                        // Multiply and accumulate four products.
                        c = _mm256_add_epi32(c, _mm256_mullo_epi32(a, b));

                        _mm256_store_si256(reinterpret_cast<__m256i*>(C + i * N + j), c);
                    }
                }
            }
        }
    }
}
}

// Tiled AVX2 multiplication using unaligned loads and stores.
void avx2_unaligned(const uint32_t* A,
                    const uint32_t* B,
                    uint32_t* C) {
    // TODO:
    // Use the same tiled algorithm as avx2_aligned.
    // Replace the vector memory operations with:
    // _mm256_loadu_si256 and _mm256_storeu_si256.

    for (int ii = 0; ii < N; ii += TILE) {
    for (int kk = 0; kk < N; kk += TILE) {
        for (int jj = 0; jj < N; jj += TILE) {
            int i_end = min(ii + TILE, N);
            int k_end = min(kk + TILE, N);
            int j_end = min(jj + TILE, N);

            for (int i = ii; i < i_end; ++i) {  
                for (int k = kk; k < k_end; ++k) {
                    // Broadcast A[i * N + k] here.
                    __m256i a = _mm256_set1_epi32(A[i * N + k]);

                    for (int j = jj; j < j_end; j += 8) {
                        // Load B and C, multiply, add, and store.
                        __m256i b = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(B + k * N + j));

                        __m256i c = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(C + i * N + j));

                        // Multiply and accumulate four products.
                        c = _mm256_add_epi32(c, _mm256_mullo_epi32(a, b));

                        _mm256_storeu_si256(reinterpret_cast<__m256i*>(C + i * N + j), c);
                    }
                }
            }
        }
    }
}
}

// Compare every output element with the sequential reference.
bool check_result(const uint32_t* expected,
                  const uint32_t* actual) {
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            if (expected[i * N + j] != actual[i * N + j]) {
                cout << "Mismatch at (" << i << ", " << j << ")\n";
                cout << "Expected: " << expected[i * N + j] << '\n';
                cout << "Actual:   " << actual[i * N + j] << '\n';
                return false;
            }
        }
    }

    return true;
}

int main() {
    const size_t elements = static_cast<size_t>(N) * N;
    const size_t bytes = elements * sizeof(uint32_t);

    // Allocate aligned matrices.
    uint32_t* A =
        static_cast<uint32_t*>(aligned_alloc(ALIGN, bytes));

    uint32_t* B =
        static_cast<uint32_t*>(aligned_alloc(ALIGN, bytes));

    uint32_t* C =
        static_cast<uint32_t*>(aligned_alloc(ALIGN, bytes));

    uint32_t* ref_C =
        static_cast<uint32_t*>(aligned_alloc(ALIGN, bytes));

    // Allocate extra space for the unaligned matrices.
    uint32_t* raw_A =
        static_cast<uint32_t*>(aligned_alloc(ALIGN, bytes + ALIGN));

    uint32_t* raw_B =
        static_cast<uint32_t*>(aligned_alloc(ALIGN, bytes + ALIGN));

    uint32_t* raw_C =
        static_cast<uint32_t*>(aligned_alloc(ALIGN, bytes + ALIGN));

    // Check whether any allocation failed.
    if (A == nullptr || B == nullptr || C == nullptr ||
        ref_C == nullptr || raw_A == nullptr ||
        raw_B == nullptr || raw_C == nullptr) {
        cout << "Memory allocation failed.\n";

        free(A);
        free(B);
        free(C);
        free(ref_C);
        free(raw_A);
        free(raw_B);
        free(raw_C);

        return EXIT_FAILURE;
    }

    // Shift by one integer to make the SIMD accesses unaligned.
    uint32_t* unaligned_A = raw_A + 1;
    uint32_t* unaligned_B = raw_B + 1;
    uint32_t* unaligned_C = raw_C + 1;

    // Initialize identical inputs for aligned and unaligned versions.
    for (size_t i = 0; i < elements; ++i) {
        A[i] = i % 14 + 1;
        B[i] = i % 7 + 1;

        unaligned_A[i] = A[i];
        unaligned_B[i] = B[i];

        ref_C[i] = 0;
        C[i] = 0;
        unaligned_C[i] = 0;
    }

    cout << "N = " << N << ", TILE = " << TILE << "\n\n";

    // Measure the sequential version.
    HRTimer start = HR::now();
    ref_version(A, B, ref_C);
    HRTimer end = HR::now();

    double serial_time =
        chrono::duration<double, micro>(end - start).count();

    cout << "Sequential time: " << serial_time << " us\n\n";

    // Measure the SSE4 aligned version.
    start = HR::now();
    sse4_aligned(A, B, C);
    end = HR::now();

    double sse_aligned_time =
        chrono::duration<double, micro>(end - start).count();

    cout << "SSE4 aligned:\n";
    bool sse_aligned_correct = check_result(ref_C, C);

    if (sse_aligned_correct) {
        cout << "Correctness: passed\n";
        cout << "Time: " << sse_aligned_time << " us\n";
    } else {
        cout << "Correctness: failed\n";
    }

    // Measure the SSE4 unaligned version.
    start = HR::now();
    sse4_unaligned(unaligned_A, unaligned_B, unaligned_C);
    end = HR::now();

    double sse_unaligned_time =
        chrono::duration<double, micro>(end - start).count();

    cout << "\nSSE4 unaligned:\n";
    bool sse_unaligned_correct = check_result(ref_C, unaligned_C);

    if (sse_unaligned_correct) {
        cout << "Correctness: passed\n";
        cout << "Time: " << sse_unaligned_time << " us\n";
    } else {
        cout << "Correctness: failed\n";
    }

    // Clear the aligned output before reusing it.
    for (size_t i = 0; i < elements; ++i) {
        C[i] = 0;
    }

    // Measure the AVX2 aligned version.
    start = HR::now();
    avx2_aligned(A, B, C);
    end = HR::now();

    double avx_aligned_time =
        chrono::duration<double, micro>(end - start).count();

    cout << "\nAVX2 aligned:\n";
    bool avx_aligned_correct = check_result(ref_C, C);

    if (avx_aligned_correct) {
        cout << "Correctness: passed\n";
        cout << "Time: " << avx_aligned_time << " us\n";
    } else {
        cout << "Correctness: failed\n";
    }

    // Clear the unaligned output before reusing it.
    for (size_t i = 0; i < elements; ++i) {
        unaligned_C[i] = 0;
    }

    // Measure the AVX2 unaligned version.
    start = HR::now();
    avx2_unaligned(unaligned_A, unaligned_B, unaligned_C);
    end = HR::now();

    double avx_unaligned_time =
        chrono::duration<double, micro>(end - start).count();

    cout << "\nAVX2 unaligned:\n";
    bool avx_unaligned_correct = check_result(ref_C, unaligned_C);

    if (avx_unaligned_correct) {
        cout << "Correctness: passed\n";
        cout << "Time: " << avx_unaligned_time << " us\n";
    } else {
        cout << "Correctness: failed\n";
    }

    // Print speedups only for correct implementations.
    cout << "\nSpeedups over sequential:\n";

    if (sse_aligned_correct && sse_aligned_time > 0) {
        cout << "SSE4 aligned: "
             << serial_time / sse_aligned_time << "x\n";
    }

    if (sse_unaligned_correct && sse_unaligned_time > 0) {
        cout << "SSE4 unaligned: "
             << serial_time / sse_unaligned_time << "x\n";
    }

    if (avx_aligned_correct && avx_aligned_time > 0) {
        cout << "AVX2 aligned: "
             << serial_time / avx_aligned_time << "x\n";
    }

    if (avx_unaligned_correct && avx_unaligned_time > 0) {
        cout << "AVX2 unaligned: "
             << serial_time / avx_unaligned_time << "x\n";
    }

    // Free the original allocation addresses.
    free(A);
    free(B);
    free(C);
    free(ref_C);
    free(raw_A);
    free(raw_B);
    free(raw_C);

    if (!sse_aligned_correct || !sse_unaligned_correct ||
        !avx_aligned_correct || !avx_unaligned_correct) {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}