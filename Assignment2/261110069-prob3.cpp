#include <algorithm>
#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <vector>
#include <x86intrin.h>

using namespace std;
using namespace chrono;

#define N (1 << 9)
#define TILE 64
#define ALIGN 32
#define RUNS 5

using MatmulFunction = void (*)(const uint32_t *, const uint32_t *,
                                uint32_t *, int);

__attribute__((noinline, optimize("no-tree-vectorize")))
void reference_version(const uint32_t *A, const uint32_t *B,
                       uint32_t *C, int n) {
  for (int i = 0; i < n; i++) {
    for (int k = 0; k < n; k++) {
      uint32_t a = A[i * n + k];
      for (int j = 0; j < n; j++) {
        C[i * n + j] += a * B[k * n + j];
      }
    }
  }
}

__attribute__((noinline))
void sse4_aligned(const uint32_t *A, const uint32_t *B,
                  uint32_t *C, int n) {
  A = static_cast<const uint32_t *>(__builtin_assume_aligned(A, ALIGN));
  B = static_cast<const uint32_t *>(__builtin_assume_aligned(B, ALIGN));
  C = static_cast<uint32_t *>(__builtin_assume_aligned(C, ALIGN));

  for (int ii = 0; ii < n; ii += TILE) {
    int i_end = min(ii + TILE, n);

    for (int kk = 0; kk < n; kk += TILE) {
      int k_end = min(kk + TILE, n);

      for (int jj = 0; jj < n; jj += TILE) {
        int j_end = min(jj + TILE, n);

        for (int i = ii; i < i_end; i++) {
          for (int k = kk; k < k_end; k++) {
            __m128i a = _mm_set1_epi32(static_cast<int>(A[i * n + k]));

            for (int j = jj; j < j_end; j += 4) {
              __m128i b = _mm_load_si128(
                  reinterpret_cast<const __m128i *>(&B[k * n + j]));
              __m128i c = _mm_load_si128(
                  reinterpret_cast<const __m128i *>(&C[i * n + j]));

              __m128i product = _mm_mullo_epi32(a, b);
              c = _mm_add_epi32(c, product);

              _mm_store_si128(
                  reinterpret_cast<__m128i *>(&C[i * n + j]), c);
            }
          }
        }
      }
    }
  }
}

__attribute__((noinline))
void sse4_unaligned(const uint32_t *A, const uint32_t *B,
                    uint32_t *C, int n) {
  for (int ii = 0; ii < n; ii += TILE) {
    int i_end = min(ii + TILE, n);

    for (int kk = 0; kk < n; kk += TILE) {
      int k_end = min(kk + TILE, n);

      for (int jj = 0; jj < n; jj += TILE) {
        int j_end = min(jj + TILE, n);

        for (int i = ii; i < i_end; i++) {
          for (int k = kk; k < k_end; k++) {
            __m128i a = _mm_set1_epi32(static_cast<int>(A[i * n + k]));

            for (int j = jj; j < j_end; j += 4) {
              __m128i b = _mm_loadu_si128(
                  reinterpret_cast<const __m128i *>(&B[k * n + j]));
              __m128i c = _mm_loadu_si128(
                  reinterpret_cast<const __m128i *>(&C[i * n + j]));

              __m128i product = _mm_mullo_epi32(a, b);
              c = _mm_add_epi32(c, product);

              _mm_storeu_si128(
                  reinterpret_cast<__m128i *>(&C[i * n + j]), c);
            }
          }
        }
      }
    }
  }
}

__attribute__((noinline))
void avx2_aligned(const uint32_t *A, const uint32_t *B,
                  uint32_t *C, int n) {
  A = static_cast<const uint32_t *>(__builtin_assume_aligned(A, ALIGN));
  B = static_cast<const uint32_t *>(__builtin_assume_aligned(B, ALIGN));
  C = static_cast<uint32_t *>(__builtin_assume_aligned(C, ALIGN));

  for (int ii = 0; ii < n; ii += TILE) {
    int i_end = min(ii + TILE, n);

    for (int kk = 0; kk < n; kk += TILE) {
      int k_end = min(kk + TILE, n);

      for (int jj = 0; jj < n; jj += TILE) {
        int j_end = min(jj + TILE, n);

        for (int i = ii; i < i_end; i++) {
          for (int k = kk; k < k_end; k++) {
            __m256i a = _mm256_set1_epi32(static_cast<int>(A[i * n + k]));

            for (int j = jj; j < j_end; j += 8) {
              __m256i b = _mm256_load_si256(
                  reinterpret_cast<const __m256i *>(&B[k * n + j]));
              __m256i c = _mm256_load_si256(
                  reinterpret_cast<const __m256i *>(&C[i * n + j]));

              __m256i product = _mm256_mullo_epi32(a, b);
              c = _mm256_add_epi32(c, product);

              _mm256_store_si256(
                  reinterpret_cast<__m256i *>(&C[i * n + j]), c);
            }
          }
        }
      }
    }
  }
}

__attribute__((noinline))
void avx2_unaligned(const uint32_t *A, const uint32_t *B,
                    uint32_t *C, int n) {
  for (int ii = 0; ii < n; ii += TILE) {
    int i_end = min(ii + TILE, n);

    for (int kk = 0; kk < n; kk += TILE) {
      int k_end = min(kk + TILE, n);

      for (int jj = 0; jj < n; jj += TILE) {
        int j_end = min(jj + TILE, n);

        for (int i = ii; i < i_end; i++) {
          for (int k = kk; k < k_end; k++) {
            __m256i a = _mm256_set1_epi32(static_cast<int>(A[i * n + k]));

            for (int j = jj; j < j_end; j += 8) {
              __m256i b = _mm256_loadu_si256(
                  reinterpret_cast<const __m256i *>(&B[k * n + j]));
              __m256i c = _mm256_loadu_si256(
                  reinterpret_cast<const __m256i *>(&C[i * n + j]));

              __m256i product = _mm256_mullo_epi32(a, b);
              c = _mm256_add_epi32(c, product);

              _mm256_storeu_si256(
                  reinterpret_cast<__m256i *>(&C[i * n + j]), c);
            }
          }
        }
      }
    }
  }
}

uint32_t *allocate_aligned(size_t count) {
  void *ptr = aligned_alloc(ALIGN, count * sizeof(uint32_t));
  if (ptr == nullptr) {
    cerr << "Memory allocation failed\n";
    exit(EXIT_FAILURE);
  }
  return static_cast<uint32_t *>(ptr);
}

struct UnalignedBuffer {
  uint32_t *raw;
  uint32_t *data;
};

UnalignedBuffer allocate_unaligned(size_t count) {
  uint32_t *raw = allocate_aligned(count + 8);
  return {raw, raw + 1};
}

void check_result(const char *name, const uint32_t *expected,
                  const uint32_t *actual, size_t count) {
  for (size_t i = 0; i < count; i++) {
    if (expected[i] != actual[i]) {
      cerr << name << " is wrong at element " << i
           << ": expected " << expected[i]
           << ", found " << actual[i] << '\n';
      exit(EXIT_FAILURE);
    }
  }
}

long long benchmark(MatmulFunction function, const uint32_t *A,
                    const uint32_t *B, uint32_t *C, int n) {
  vector<long long> times;
  size_t count = static_cast<size_t>(n) * n;

  for (int run = 0; run < RUNS; run++) {
    fill(C, C + count, 0);

    auto start = steady_clock::now();
    function(A, B, C, n);
    auto finish = steady_clock::now();

    times.push_back(
        duration_cast<microseconds>(finish - start).count());
  }

  sort(times.begin(), times.end());
  return times[RUNS / 2];
}

int main() {
  static_assert(N % 8 == 0, "N must be a multiple of eight");
  static_assert(TILE % 8 == 0, "TILE must be a multiple of eight");

  const size_t count = static_cast<size_t>(N) * N;

  uint32_t *A = allocate_aligned(count);
  uint32_t *B = allocate_aligned(count);
  uint32_t *reference = allocate_aligned(count);
  uint32_t *sse_aligned_result = allocate_aligned(count);
  uint32_t *avx_aligned_result = allocate_aligned(count);

  UnalignedBuffer A_unaligned = allocate_unaligned(count);
  UnalignedBuffer B_unaligned = allocate_unaligned(count);
  UnalignedBuffer sse_unaligned_result = allocate_unaligned(count);
  UnalignedBuffer avx_unaligned_result = allocate_unaligned(count);

  for (size_t i = 0; i < count; i++) {
    A[i] = static_cast<uint32_t>((i * 17 + 3) % 7 + 1);
    B[i] = static_cast<uint32_t>((i * 13 + 5) % 7 + 1);
  }

  copy(A, A + count, A_unaligned.data);
  copy(B, B + count, B_unaligned.data);

  fill(reference, reference + count, 0);
  reference_version(A, B, reference, N);

  fill(sse_aligned_result, sse_aligned_result + count, 0);
  sse4_aligned(A, B, sse_aligned_result, N);
  check_result("SSE4 aligned", reference, sse_aligned_result, count);

  fill(sse_unaligned_result.data, sse_unaligned_result.data + count, 0);
  sse4_unaligned(A_unaligned.data, B_unaligned.data,
                 sse_unaligned_result.data, N);
  check_result("SSE4 unaligned", reference,
               sse_unaligned_result.data, count);

  fill(avx_aligned_result, avx_aligned_result + count, 0);
  avx2_aligned(A, B, avx_aligned_result, N);
  check_result("AVX2 aligned", reference, avx_aligned_result, count);

  fill(avx_unaligned_result.data, avx_unaligned_result.data + count, 0);
  avx2_unaligned(A_unaligned.data, B_unaligned.data,
                 avx_unaligned_result.data, N);
  check_result("AVX2 unaligned", reference,
               avx_unaligned_result.data, count);

  cout << "Correctness: all four SIMD versions match the reference\n\n";

  long long reference_time = benchmark(reference_version, A, B, reference, N);
  long long sse_aligned_time = benchmark(
      sse4_aligned, A, B, sse_aligned_result, N);
  long long sse_unaligned_time = benchmark(
      sse4_unaligned, A_unaligned.data, B_unaligned.data,
      sse_unaligned_result.data, N);
  long long avx_aligned_time = benchmark(
      avx2_aligned, A, B, avx_aligned_result, N);
  long long avx_unaligned_time = benchmark(
      avx2_unaligned, A_unaligned.data, B_unaligned.data,
      avx_unaligned_result.data, N);

  cout << fixed << setprecision(2);
  cout << "N = " << N << ", tile = " << TILE
       << ", median of " << RUNS << " runs\n\n";

  cout << "Reference:      " << reference_time << " us\n";
  cout << "SSE4 aligned:   " << sse_aligned_time << " us, speedup "
       << static_cast<double>(reference_time) / sse_aligned_time << "x\n";
  cout << "SSE4 unaligned: " << sse_unaligned_time << " us, speedup "
       << static_cast<double>(reference_time) / sse_unaligned_time << "x\n";
  cout << "AVX2 aligned:   " << avx_aligned_time << " us, speedup "
       << static_cast<double>(reference_time) / avx_aligned_time << "x\n";
  cout << "AVX2 unaligned: " << avx_unaligned_time << " us, speedup "
       << static_cast<double>(reference_time) / avx_unaligned_time << "x\n";

  free(A);
  free(B);
  free(reference);
  free(sse_aligned_result);
  free(avx_aligned_result);
  free(A_unaligned.raw);
  free(B_unaligned.raw);
  free(sse_unaligned_result.raw);
  free(avx_unaligned_result.raw);

  return EXIT_SUCCESS;
}
