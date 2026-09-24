#include <algorithm>
#include <cassert>
#include <chrono>
#include <climits>
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

  cout << "Values [0:1]: "
       << val[0] << " " << val[1] << "\n";
}

/*
 * Sequential exclusive prefix sum.
 *
 * Input:  [a, b, c, d]
 * Output: [0, a, a+b, a+b+c]
 */
__attribute__((optimize("no-tree-vectorize"))) int
ref_version(int* __restrict__ source, int* __restrict__ dest) {
  source =
      static_cast<int*>(__builtin_assume_aligned(source, ALIGN));

  dest =
      static_cast<int*>(__builtin_assume_aligned(dest, ALIGN));

  int tmp = 0;

  for (int i = 0; i < N; i++) {
    dest[i] = tmp;
    tmp += source[i];
  }

  return tmp;
}

/*
 * SSE4 exclusive prefix sum.
 *
 * Four 32-bit integers are processed in each iteration.
 */
int sse4_version(const int* __restrict__ source,
                 int* __restrict__ dest) {
  source =
      static_cast<const int*>(
          __builtin_assume_aligned(source, ALIGN));

  dest =
      static_cast<int*>(
          __builtin_assume_aligned(dest, ALIGN));

  int carry = 0;
  const int stride =
      SSE_WIDTH_BITS / (sizeof(int) * CHAR_BIT);

  for (int i = 0; i < N; i += stride) {
    /*
     * Suppose:
     *
     * x = [a, b, c, d]
     */
    __m128i x = _mm_load_si128(
        reinterpret_cast<const __m128i*>(&source[i]));

    /*
     * Shift by one integer:
     *
     * [a, b, c, d]
     * [0, a, b, c]
     */
    __m128i shifted = _mm_slli_si128(x, 4);
    x = _mm_add_epi32(x, shifted);

    /*
     * x = [a, a+b, b+c, c+d]
     *
     * Shift by two integers:
     *
     * [0, 0, a, a+b]
     */
    shifted = _mm_slli_si128(x, 8);
    x = _mm_add_epi32(x, shifted);

    /*
     * x now contains the local inclusive prefix:
     *
     * [a, a+b, a+b+c, a+b+c+d]
     */

    __m128i carry_vector = _mm_set1_epi32(carry);

    __m128i inclusive =
        _mm_add_epi32(x, carry_vector);

    /*
     * Convert global inclusive prefix into exclusive prefix.
     *
     * inclusive = [g0, g1, g2, g3]
     *
     * shift     = [0, g0, g1, g2]
     *
     * Replace the first zero with the previous carry:
     *
     * exclusive = [carry, g0, g1, g2]
     */
    __m128i exclusive =
        _mm_slli_si128(inclusive, 4);

    exclusive =
        _mm_insert_epi32(exclusive, carry, 0);

    _mm_store_si128(
        reinterpret_cast<__m128i*>(&dest[i]),
        exclusive);

    /*
     * The last inclusive value is the sum of
     * everything processed so far.
     */
    carry = _mm_extract_epi32(inclusive, 3);
  }

  return carry;
}

/*
 * AVX2 exclusive prefix sum.
 *
 * Eight 32-bit integers are processed in each iteration.
 */
int avx2_version(const int* source, int* dest) {
  source =
      static_cast<const int*>(
          __builtin_assume_aligned(source, ALIGN));

  dest =
      static_cast<int*>(
          __builtin_assume_aligned(dest, ALIGN));

  __m256i zero = _mm256_setzero_si256();

  /*
   * These indices convert:
   *
   * [g0, g1, g2, g3, g4, g5, g6, g7]
   *
   * into:
   *
   * [g0, g0, g1, g2, g3, g4, g5, g6]
   */
  __m256i indices =
      _mm256_setr_epi32(0, 0, 1, 2, 3, 4, 5, 6);

  int carry = 0;

  for (int i = 0; i < N; i += 8) {
    /*
     * x = [a, b, c, d | e, f, g, h]
     */
    __m256i x = _mm256_load_si256(
        reinterpret_cast<const __m256i*>(&source[i]));

    /*
     * The byte-shift works independently inside
     * the two 128-bit lanes.
     *
     * shifted = [0, a, b, c | 0, e, f, g]
     */
    __m256i shifted =
        _mm256_slli_si256(x, 4);

    x = _mm256_add_epi32(x, shifted);

    /*
     * shifted = [0, 0, a, a+b | 0, 0, e, e+f]
     */
    shifted = _mm256_slli_si256(x, 8);
    x = _mm256_add_epi32(x, shifted);

    /*
     * x now contains:
     *
     * [a, a+b, a+b+c, a+b+c+d |
     *  e, e+f, e+f+g, e+f+g+h]
     */

    /*
     * Copy the lower 128-bit lane into both lanes.
     */
    __m256i lower_lane =
        _mm256_permute2x128_si256(x, x, 0x00);

    /*
     * Broadcast the final value of the lower lane.
     *
     * lower_sum = [L, L, L, L | L, L, L, L]
     *
     * where L = a+b+c+d.
     */
    __m256i lower_sum =
        _mm256_shuffle_epi32(
            lower_lane,
            _MM_SHUFFLE(3, 3, 3, 3));

    /*
     * Keep L only in the upper lane.
     *
     * correction = [0, 0, 0, 0 | L, L, L, L]
     */
    __m256i correction =
        _mm256_blend_epi32(
            zero, lower_sum, 0xF0);

    /*
     * Connect the two 128-bit lane-prefix sums.
     */
    x = _mm256_add_epi32(x, correction);

    /*
     * Add the sum of all earlier AVX2 blocks.
     */
    __m256i carry_vector =
        _mm256_set1_epi32(carry);

    __m256i inclusive =
        _mm256_add_epi32(x, carry_vector);

    /*
     * Shift global inclusive results by one element.
     */
    __m256i exclusive =
        _mm256_permutevar8x32_epi32(
            inclusive, indices);

    /*
     * Put the previous block's carry in element zero.
     */
    exclusive =
        _mm256_blend_epi32(
            exclusive, carry_vector, 0x01);

    _mm256_store_si256(
        reinterpret_cast<__m256i*>(&dest[i]),
        exclusive);

    /*
     * Final inclusive value becomes the carry
     * for the next block.
     */
    carry =
        _mm256_extract_epi32(inclusive, 7);
  }

  return carry;
}

__attribute__((optimize("no-tree-vectorize"))) int main() {
  int* array =
      static_cast<int*>(
          aligned_alloc(ALIGN, N * sizeof(int)));

  std::fill(array, array + N, 1);

  int* ref_res =
      static_cast<int*>(
          aligned_alloc(ALIGN, N * sizeof(int)));

  std::fill(ref_res, ref_res + N, 0);

  HRTimer start = HR::now();
  int val_ser = ref_version(array, ref_res);
  HRTimer end = HR::now();

  auto duration =
      duration_cast<microseconds>(end - start).count();

  cout << "Serial version: "
       << val_ser
       << " time: "
       << duration
       << endl;

  int* sse_res =
      static_cast<int*>(
          aligned_alloc(ALIGN, N * sizeof(int)));

  std::fill(sse_res, sse_res + N, 0);

  start = HR::now();
  int val_sse = sse4_version(array, sse_res);
  end = HR::now();

  duration =
      duration_cast<microseconds>(end - start).count();

  assert(val_ser == val_sse ||
         printf("SSE result is wrong!\n"));

  /*
   * The returned total alone is not enough to verify
   * every prefix, so compare the complete arrays too.
   */
  assert(std::equal(ref_res, ref_res + N, sse_res) ||
         printf("SSE output array is wrong!\n"));

  cout << "SSE version: "
       << val_sse
       << " time: "
       << duration
       << endl;

  int* avx2_res =
      static_cast<int*>(
          aligned_alloc(ALIGN, N * sizeof(int)));

  std::fill(avx2_res, avx2_res + N, 0);

  start = HR::now();
  int val_avx2 = avx2_version(array, avx2_res);
  end = HR::now();

  duration =
      duration_cast<microseconds>(end - start).count();

  assert(val_ser == val_avx2 ||
         printf("AVX2 result is wrong!\n"));

  assert(std::equal(ref_res, ref_res + N, avx2_res) ||
         printf("AVX2 output array is wrong!\n"));

  cout << "AVX2 version: "
       << val_avx2
       << " time: "
       << duration
       << endl;

  free(array);
  free(ref_res);
  free(sse_res);
  free(avx2_res);

  return EXIT_SUCCESS;
}