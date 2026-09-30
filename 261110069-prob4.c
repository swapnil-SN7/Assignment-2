#include <immintrin.h>
#include <stdint.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define NSEC_SEC_MUL (1.0e9)

void gridloopsearch(
    double dd1, double dd2, double dd3, double dd4, double dd5, double dd6, double dd7, double dd8,
    double dd9, double dd10, double dd11, double dd12, double dd13, double dd14, double dd15,
    double dd16, double dd17, double dd18, double dd19, double dd20, double dd21, double dd22,
    double dd23, double dd24, double dd25, double dd26, double dd27, double dd28, double dd29,
    double dd30, double c11, double c12, double c13, double c14, double c15, double c16, double c17,
    double c18, double c19, double c110, double d1, double ey1, double c21, double c22, double c23,
    double c24, double c25, double c26, double c27, double c28, double c29, double c210, double d2,
    double ey2, double c31, double c32, double c33, double c34, double c35, double c36, double c37,
    double c38, double c39, double c310, double d3, double ey3, double c41, double c42, double c43,
    double c44, double c45, double c46, double c47, double c48, double c49, double c410, double d4,
    double ey4, double c51, double c52, double c53, double c54, double c55, double c56, double c57,
    double c58, double c59, double c510, double d5, double ey5, double c61, double c62, double c63,
    double c64, double c65, double c66, double c67, double c68, double c69, double c610, double d6,
    double ey6, double c71, double c72, double c73, double c74, double c75, double c76, double c77,
    double c78, double c79, double c710, double d7, double ey7, double c81, double c82, double c83,
    double c84, double c85, double c86, double c87, double c88, double c89, double c810, double d8,
    double ey8, double c91, double c92, double c93, double c94, double c95, double c96, double c97,
    double c98, double c99, double c910, double d9, double ey9, double c101, double c102,
    double c103, double c104, double c105, double c106, double c107, double c108, double c109,
    double c1010, double d10, double ey10, double kk);

struct timespec begin_grid, end_main;

// to store values of disp.txt
double a[120];

// to store values of grid.txt
double b[30];

int main() {
  int i, j;

  i = 0;
  FILE* fp = fopen("./disp.txt", "r");
  if (fp == NULL) {
    printf("Error: could not open file\n");
    return 1;
  }

  while (!feof(fp)) {
    if (!fscanf(fp, "%lf", &a[i])) {
      printf("Error: fscanf failed while reading disp.txt\n");
      exit(EXIT_FAILURE);
    }
    i++;
  }
  fclose(fp);

  // read grid file
  j = 0;
  FILE* fpq = fopen("./grid.txt", "r");
  if (fpq == NULL) {
    printf("Error: could not open file\n");
    return 1;
  }

  while (!feof(fpq)) {
    if (!fscanf(fpq, "%lf", &b[j])) {
      printf("Error: fscanf failed while reading grid.txt\n");
      exit(EXIT_FAILURE);
    }
    j++;
  }
  fclose(fpq);

  // grid value initialize
  // initialize value of kk;
  double kk = 0.3;

  clock_gettime(CLOCK_MONOTONIC_RAW, &begin_grid);
  gridloopsearch(b[0], b[1], b[2], b[3], b[4], b[5], b[6], b[7], b[8], b[9], b[10], b[11], b[12],
                 b[13], b[14], b[15], b[16], b[17], b[18], b[19], b[20], b[21], b[22], b[23], b[24],
                 b[25], b[26], b[27], b[28], b[29], a[0], a[1], a[2], a[3], a[4], a[5], a[6], a[7],
                 a[8], a[9], a[10], a[11], a[12], a[13], a[14], a[15], a[16], a[17], a[18], a[19],
                 a[20], a[21], a[22], a[23], a[24], a[25], a[26], a[27], a[28], a[29], a[30], a[31],
                 a[32], a[33], a[34], a[35], a[36], a[37], a[38], a[39], a[40], a[41], a[42], a[43],
                 a[44], a[45], a[46], a[47], a[48], a[49], a[50], a[51], a[52], a[53], a[54], a[55],
                 a[56], a[57], a[58], a[59], a[60], a[61], a[62], a[63], a[64], a[65], a[66], a[67],
                 a[68], a[69], a[70], a[71], a[72], a[73], a[74], a[75], a[76], a[77], a[78], a[79],
                 a[80], a[81], a[82], a[83], a[84], a[85], a[86], a[87], a[88], a[89], a[90], a[91],
                 a[92], a[93], a[94], a[95], a[96], a[97], a[98], a[99], a[100], a[101], a[102],
                 a[103], a[104], a[105], a[106], a[107], a[108], a[109], a[110], a[111], a[112],
                 a[113], a[114], a[115], a[116], a[117], a[118], a[119], kk);
  clock_gettime(CLOCK_MONOTONIC_RAW, &end_main);
  printf("Total time = %f seconds\n", (end_main.tv_nsec - begin_grid.tv_nsec) / NSEC_SEC_MUL +
                                          (end_main.tv_sec - begin_grid.tv_sec));

  return EXIT_SUCCESS;
}

// grid search function with loop variables

void gridloopsearch(
    double dd1, double dd2, double dd3, double dd4, double dd5, double dd6, double dd7, double dd8,
    double dd9, double dd10, double dd11, double dd12, double dd13, double dd14, double dd15,
    double dd16, double dd17, double dd18, double dd19, double dd20, double dd21, double dd22,
    double dd23, double dd24, double dd25, double dd26, double dd27, double dd28, double dd29,
    double dd30, double c11, double c12, double c13, double c14, double c15, double c16, double c17,
    double c18, double c19, double c110, double d1, double ey1, double c21, double c22, double c23,
    double c24, double c25, double c26, double c27, double c28, double c29, double c210, double d2,
    double ey2, double c31, double c32, double c33, double c34, double c35, double c36, double c37,
    double c38, double c39, double c310, double d3, double ey3, double c41, double c42, double c43,
    double c44, double c45, double c46, double c47, double c48, double c49, double c410, double d4,
    double ey4, double c51, double c52, double c53, double c54, double c55, double c56, double c57,
    double c58, double c59, double c510, double d5, double ey5, double c61, double c62, double c63,
    double c64, double c65, double c66, double c67, double c68, double c69, double c610, double d6,
    double ey6, double c71, double c72, double c73, double c74, double c75, double c76, double c77,
    double c78, double c79, double c710, double d7, double ey7, double c81, double c82, double c83,
    double c84, double c85, double c86, double c87, double c88, double c89, double c810, double d8,
    double ey8, double c91, double c92, double c93, double c94, double c95, double c96, double c97,
    double c98, double c99, double c910, double d9, double ey9, double c101, double c102,
    double c103, double c104, double c105, double c106, double c107, double c108, double c109,
    double c1010, double d10, double ey10, double kk) {
  // results values
  double x1, x2, x3, x4, x5, x6, x7, x8, x9, x10;

  // constraint values
  double q1, q2, q3, q4, q5, q6, q7, q8, q9, q10;

  // results points
  long pnts = 0;

  // re-calculated limits
  double e1, e2, e3, e4, e5, e6, e7, e8, e9, e10;

  // opening the "results-v0.txt" for writing he results in append mode
  FILE* fptr = fopen("./results-v0.txt", "w");
  if (fptr == NULL) {
    printf("Error in creating file !");
    exit(1);
  }

  // initialization of re calculated limits, xi's.
  e1 = kk * ey1;
  e2 = kk * ey2;
  e3 = kk * ey3;
  e4 = kk * ey4;
  e5 = kk * ey5;
  e6 = kk * ey6;
  e7 = kk * ey7;
  e8 = kk * ey8;
  e9 = kk * ey9;
  e10 = kk * ey10;

  x1 = dd1;
  x2 = dd4;
  x3 = dd7;
  x4 = dd10;
  x5 = dd13;
  x6 = dd16;
  x7 = dd19;
  x8 = dd22;
  x9 = dd25;
  x10 = dd28;

  // for loop upper values
  int s1, s2, s3, s4, s5, s6, s7, s8, s9, s10;
  s1 = floor((dd2 - dd1) / dd3);
  s2 = floor((dd5 - dd4) / dd6);
  s3 = floor((dd8 - dd7) / dd9);
  s4 = floor((dd11 - dd10) / dd12);
  s5 = floor((dd14 - dd13) / dd15);
  s6 = floor((dd17 - dd16) / dd18);
  s7 = floor((dd20 - dd19) / dd21);
  s8 = floor((dd23 - dd22) / dd24);
  s9 = floor((dd26 - dd25) / dd27);
  s10 = floor((dd29 - dd28) / dd30);

  // Optimization: keep accepted points in memory and write them after the search.
  size_t capacity = 1024;
  double (*points)[10] = malloc(capacity * sizeof(*points));
  if (points == NULL) {
    printf("Memory allocation failed\n");
    fclose(fptr);
    exit(EXIT_FAILURE);
  }

  // Optimization: bounds for pruning branches that cannot satisfy a constraint.
  // These are bounds on products, not a rearrangement of the input arrays.
  double last1 = dd1 + (s1 - 1.0) * dd3;
  double last2 = dd4 + (s2 - 1.0) * dd6;
  double last3 = dd7 + (s3 - 1.0) * dd9;
  double last4 = dd10 + (s4 - 1.0) * dd12;
  double last5 = dd13 + (s5 - 1.0) * dd15;
  double last6 = dd16 + (s6 - 1.0) * dd18;
  double last7 = dd19 + (s7 - 1.0) * dd21;
  double last8 = dd22 + (s8 - 1.0) * dd24;
  double last9 = dd25 + (s9 - 1.0) * dd27;
  double last10 = dd28 + (s10 - 1.0) * dd30;

  double lower[10][10] = {
    {c11 * dd1, c12 * dd4, c13 * dd7, c14 * dd10, c15 * dd13,
     c16 * dd16, c17 * dd19, c18 * dd22, c19 * dd25, c110 * dd28},
    {c21 * dd1, c22 * dd4, c23 * dd7, c24 * dd10, c25 * dd13,
     c26 * dd16, c27 * dd19, c28 * dd22, c29 * dd25, c210 * dd28},
    {c31 * dd1, c32 * dd4, c33 * dd7, c34 * dd10, c35 * dd13,
     c36 * dd16, c37 * dd19, c38 * dd22, c39 * dd25, c310 * dd28},
    {c41 * dd1, c42 * dd4, c43 * dd7, c44 * dd10, c45 * dd13,
     c46 * dd16, c47 * dd19, c48 * dd22, c49 * dd25, c410 * dd28},
    {c51 * dd1, c52 * dd4, c53 * dd7, c54 * dd10, c55 * dd13,
     c56 * dd16, c57 * dd19, c58 * dd22, c59 * dd25, c510 * dd28},
    {c61 * dd1, c62 * dd4, c63 * dd7, c64 * dd10, c65 * dd13,
     c66 * dd16, c67 * dd19, c68 * dd22, c69 * dd25, c610 * dd28},
    {c71 * dd1, c72 * dd4, c73 * dd7, c74 * dd10, c75 * dd13,
     c76 * dd16, c77 * dd19, c78 * dd22, c79 * dd25, c710 * dd28},
    {c81 * dd1, c82 * dd4, c83 * dd7, c84 * dd10, c85 * dd13,
     c86 * dd16, c87 * dd19, c88 * dd22, c89 * dd25, c810 * dd28},
    {c91 * dd1, c92 * dd4, c93 * dd7, c94 * dd10, c95 * dd13,
     c96 * dd16, c97 * dd19, c98 * dd22, c99 * dd25, c910 * dd28},
    {c101 * dd1, c102 * dd4, c103 * dd7, c104 * dd10, c105 * dd13,
     c106 * dd16, c107 * dd19, c108 * dd22, c109 * dd25, c1010 * dd28}
  };

  double upper[10][10] = {
    {c11 * last1, c12 * last2, c13 * last3, c14 * last4, c15 * last5,
     c16 * last6, c17 * last7, c18 * last8, c19 * last9, c110 * last10},
    {c21 * last1, c22 * last2, c23 * last3, c24 * last4, c25 * last5,
     c26 * last6, c27 * last7, c28 * last8, c29 * last9, c210 * last10},
    {c31 * last1, c32 * last2, c33 * last3, c34 * last4, c35 * last5,
     c36 * last6, c37 * last7, c38 * last8, c39 * last9, c310 * last10},
    {c41 * last1, c42 * last2, c43 * last3, c44 * last4, c45 * last5,
     c46 * last6, c47 * last7, c48 * last8, c49 * last9, c410 * last10},
    {c51 * last1, c52 * last2, c53 * last3, c54 * last4, c55 * last5,
     c56 * last6, c57 * last7, c58 * last8, c59 * last9, c510 * last10},
    {c61 * last1, c62 * last2, c63 * last3, c64 * last4, c65 * last5,
     c66 * last6, c67 * last7, c68 * last8, c69 * last9, c610 * last10},
    {c71 * last1, c72 * last2, c73 * last3, c74 * last4, c75 * last5,
     c76 * last6, c77 * last7, c78 * last8, c79 * last9, c710 * last10},
    {c81 * last1, c82 * last2, c83 * last3, c84 * last4, c85 * last5,
     c86 * last6, c87 * last7, c88 * last8, c89 * last9, c810 * last10},
    {c91 * last1, c92 * last2, c93 * last3, c94 * last4, c95 * last5,
     c96 * last6, c97 * last7, c98 * last8, c99 * last9, c910 * last10},
    {c101 * last1, c102 * last2, c103 * last3, c104 * last4, c105 * last5,
     c106 * last6, c107 * last7, c108 * last8, c109 * last9, c1010 * last10}
  };

  for (int row = 0; row < 10; ++row) {
    for (int col = 0; col < 10; ++col) {
      if (lower[row][col] > upper[row][col]) {
        double temp = lower[row][col];
        lower[row][col] = upper[row][col];
        upper[row][col] = temp;
      }
    }
  }

  const double target[10] = {d1, d2, d3, d4, d5, d6, d7, d8, d9, d10};
  const double limit[10] = {e1, e2, e3, e4, e5, e6, e7, e8, e9, e10};
  const __m256d start10 = _mm256_set1_pd(dd28);
  const __m256d step10 = _mm256_set1_pd(dd30);
  const __m256d sign_bit = _mm256_set1_pd(-0.0);

  // grid search starts
  for (int r1 = 0; r1 < s1; ++r1) {
    x1 = dd1 + r1 * dd3;

    // Optimization: build each partial sum at the loop where it changes.
    double p1[10] = {
      c11 * x1, c21 * x1,
      c31 * x1, c41 * x1,
      c51 * x1, c61 * x1,
      c71 * x1, c81 * x1,
      c91 * x1, c101 * x1
    };

    // Try the smallest/largest remaining products, adding in the original
    // order. If even this range misses a constraint, skip the inner loops.
    int possible = 1;
    for (int row = 0; row < 10 && possible; ++row) {
      double low = p1[row];
      double high = p1[row];
      for (int col = 1; col < 10; ++col) {
        low += lower[row][col];
        high += upper[row][col];
      }
      if (low - target[row] > limit[row] || high - target[row] < -limit[row])
        possible = 0;
    }
    if (!possible) continue;

    for (int r2 = 0; r2 < s2; ++r2) {
      x2 = dd4 + r2 * dd6;

      double p2[10] = {
        p1[0] + c12 * x2, p1[1] + c22 * x2,
        p1[2] + c32 * x2, p1[3] + c42 * x2,
        p1[4] + c52 * x2, p1[5] + c62 * x2,
        p1[6] + c72 * x2, p1[7] + c82 * x2,
        p1[8] + c92 * x2, p1[9] + c102 * x2
      };

      int possible = 1;
      for (int row = 0; row < 10 && possible; ++row) {
        double low = p2[row];
        double high = p2[row];
        for (int col = 2; col < 10; ++col) {
          low += lower[row][col];
          high += upper[row][col];
        }
        if (low - target[row] > limit[row] || high - target[row] < -limit[row])
          possible = 0;
      }
      if (!possible) continue;

      for (int r3 = 0; r3 < s3; ++r3) {
        x3 = dd7 + r3 * dd9;

        double p3[10] = {
          p2[0] + c13 * x3, p2[1] + c23 * x3,
          p2[2] + c33 * x3, p2[3] + c43 * x3,
          p2[4] + c53 * x3, p2[5] + c63 * x3,
          p2[6] + c73 * x3, p2[7] + c83 * x3,
          p2[8] + c93 * x3, p2[9] + c103 * x3
        };

        int possible = 1;
        for (int row = 0; row < 10 && possible; ++row) {
          double low = p3[row];
          double high = p3[row];
          for (int col = 3; col < 10; ++col) {
            low += lower[row][col];
            high += upper[row][col];
          }
          if (low - target[row] > limit[row] || high - target[row] < -limit[row])
            possible = 0;
        }
        if (!possible) continue;

        for (int r4 = 0; r4 < s4; ++r4) {
          x4 = dd10 + r4 * dd12;

          double p4[10] = {
            p3[0] + c14 * x4, p3[1] + c24 * x4,
            p3[2] + c34 * x4, p3[3] + c44 * x4,
            p3[4] + c54 * x4, p3[5] + c64 * x4,
            p3[6] + c74 * x4, p3[7] + c84 * x4,
            p3[8] + c94 * x4, p3[9] + c104 * x4
          };

          int possible = 1;
          for (int row = 0; row < 10 && possible; ++row) {
            double low = p4[row];
            double high = p4[row];
            for (int col = 4; col < 10; ++col) {
              low += lower[row][col];
              high += upper[row][col];
            }
            if (low - target[row] > limit[row] || high - target[row] < -limit[row])
              possible = 0;
          }
          if (!possible) continue;

          for (int r5 = 0; r5 < s5; ++r5) {
            x5 = dd13 + r5 * dd15;

            double p5[10] = {
              p4[0] + c15 * x5, p4[1] + c25 * x5,
              p4[2] + c35 * x5, p4[3] + c45 * x5,
              p4[4] + c55 * x5, p4[5] + c65 * x5,
              p4[6] + c75 * x5, p4[7] + c85 * x5,
              p4[8] + c95 * x5, p4[9] + c105 * x5
            };

            int possible = 1;
            for (int row = 0; row < 10 && possible; ++row) {
              double low = p5[row];
              double high = p5[row];
              for (int col = 5; col < 10; ++col) {
                low += lower[row][col];
                high += upper[row][col];
              }
              if (low - target[row] > limit[row] || high - target[row] < -limit[row])
                possible = 0;
            }
            if (!possible) continue;

            for (int r6 = 0; r6 < s6; ++r6) {
              x6 = dd16 + r6 * dd18;

              double p6[10] = {
                p5[0] + c16 * x6, p5[1] + c26 * x6,
                p5[2] + c36 * x6, p5[3] + c46 * x6,
                p5[4] + c56 * x6, p5[5] + c66 * x6,
                p5[6] + c76 * x6, p5[7] + c86 * x6,
                p5[8] + c96 * x6, p5[9] + c106 * x6
              };

              int possible = 1;
              for (int row = 0; row < 10 && possible; ++row) {
                double low = p6[row];
                double high = p6[row];
                for (int col = 6; col < 10; ++col) {
                  low += lower[row][col];
                  high += upper[row][col];
                }
                if (low - target[row] > limit[row] || high - target[row] < -limit[row])
                  possible = 0;
              }
              if (!possible) continue;

              for (int r7 = 0; r7 < s7; ++r7) {
                x7 = dd19 + r7 * dd21;

                double p7[10] = {
                  p6[0] + c17 * x7, p6[1] + c27 * x7,
                  p6[2] + c37 * x7, p6[3] + c47 * x7,
                  p6[4] + c57 * x7, p6[5] + c67 * x7,
                  p6[6] + c77 * x7, p6[7] + c87 * x7,
                  p6[8] + c97 * x7, p6[9] + c107 * x7
                };

                int possible = 1;
                for (int row = 0; row < 10 && possible; ++row) {
                  double low = p7[row];
                  double high = p7[row];
                  for (int col = 7; col < 10; ++col) {
                    low += lower[row][col];
                    high += upper[row][col];
                  }
                  if (low - target[row] > limit[row] || high - target[row] < -limit[row])
                    possible = 0;
                }
                if (!possible) continue;

                for (int r8 = 0; r8 < s8; ++r8) {
                  x8 = dd22 + r8 * dd24;

                  double p8[10] = {
                    p7[0] + c18 * x8, p7[1] + c28 * x8,
                    p7[2] + c38 * x8, p7[3] + c48 * x8,
                    p7[4] + c58 * x8, p7[5] + c68 * x8,
                    p7[6] + c78 * x8, p7[7] + c88 * x8,
                    p7[8] + c98 * x8, p7[9] + c108 * x8
                  };

                  int possible = 1;
                  for (int row = 0; row < 10 && possible; ++row) {
                    double low = p8[row];
                    double high = p8[row];
                    for (int col = 8; col < 10; ++col) {
                      low += lower[row][col];
                      high += upper[row][col];
                    }
                    if (low - target[row] > limit[row] || high - target[row] < -limit[row])
                      possible = 0;
                  }
                  if (!possible) continue;

                  for (int r9 = 0; r9 < s9; ++r9) {
                    x9 = dd25 + r9 * dd27;

                    double p9[10] = {
                      p8[0] + c19 * x9, p8[1] + c29 * x9,
                      p8[2] + c39 * x9, p8[3] + c49 * x9,
                      p8[4] + c59 * x9, p8[5] + c69 * x9,
                      p8[6] + c79 * x9, p8[7] + c89 * x9,
                      p8[8] + c99 * x9, p8[9] + c109 * x9
                    };

                    int possible = 1;
                    for (int row = 0; row < 10 && possible; ++row) {
                      double low = p9[row];
                      double high = p9[row];
                      for (int col = 9; col < 10; ++col) {
                        low += lower[row][col];
                        high += upper[row][col];
                      }
                      if (low - target[row] > limit[row] || high - target[row] < -limit[row])
                        possible = 0;
                    }
                    if (!possible) continue;

                    // Optimization: check four x10 candidates with AVX, then handle the rest.
                    int r10 = 0;
                    for (; r10 <= s10 - 4; r10 += 4) {
                      __m256d index = _mm256_setr_pd(r10, r10 + 1, r10 + 2, r10 + 3);
                      __m256d values = _mm256_add_pd(start10, _mm256_mul_pd(index, step10));
                      __m256d error;
                      int mask = 15;

                      error = _mm256_add_pd(_mm256_set1_pd(p9[0]),
                                            _mm256_mul_pd(_mm256_set1_pd(c110), values));
                      error = _mm256_sub_pd(error, _mm256_set1_pd(d1));
                      error = _mm256_andnot_pd(sign_bit, error);
                      mask &= _mm256_movemask_pd(
                          _mm256_cmp_pd(error, _mm256_set1_pd(e1), _CMP_LE_OQ));
                      if (mask == 0) continue;

                      error = _mm256_add_pd(_mm256_set1_pd(p9[1]),
                                            _mm256_mul_pd(_mm256_set1_pd(c210), values));
                      error = _mm256_sub_pd(error, _mm256_set1_pd(d2));
                      error = _mm256_andnot_pd(sign_bit, error);
                      mask &= _mm256_movemask_pd(
                          _mm256_cmp_pd(error, _mm256_set1_pd(e2), _CMP_LE_OQ));
                      if (mask == 0) continue;

                      error = _mm256_add_pd(_mm256_set1_pd(p9[2]),
                                            _mm256_mul_pd(_mm256_set1_pd(c310), values));
                      error = _mm256_sub_pd(error, _mm256_set1_pd(d3));
                      error = _mm256_andnot_pd(sign_bit, error);
                      mask &= _mm256_movemask_pd(
                          _mm256_cmp_pd(error, _mm256_set1_pd(e3), _CMP_LE_OQ));
                      if (mask == 0) continue;

                      error = _mm256_add_pd(_mm256_set1_pd(p9[3]),
                                            _mm256_mul_pd(_mm256_set1_pd(c410), values));
                      error = _mm256_sub_pd(error, _mm256_set1_pd(d4));
                      error = _mm256_andnot_pd(sign_bit, error);
                      mask &= _mm256_movemask_pd(
                          _mm256_cmp_pd(error, _mm256_set1_pd(e4), _CMP_LE_OQ));
                      if (mask == 0) continue;

                      error = _mm256_add_pd(_mm256_set1_pd(p9[4]),
                                            _mm256_mul_pd(_mm256_set1_pd(c510), values));
                      error = _mm256_sub_pd(error, _mm256_set1_pd(d5));
                      error = _mm256_andnot_pd(sign_bit, error);
                      mask &= _mm256_movemask_pd(
                          _mm256_cmp_pd(error, _mm256_set1_pd(e5), _CMP_LE_OQ));
                      if (mask == 0) continue;

                      error = _mm256_add_pd(_mm256_set1_pd(p9[5]),
                                            _mm256_mul_pd(_mm256_set1_pd(c610), values));
                      error = _mm256_sub_pd(error, _mm256_set1_pd(d6));
                      error = _mm256_andnot_pd(sign_bit, error);
                      mask &= _mm256_movemask_pd(
                          _mm256_cmp_pd(error, _mm256_set1_pd(e6), _CMP_LE_OQ));
                      if (mask == 0) continue;

                      error = _mm256_add_pd(_mm256_set1_pd(p9[6]),
                                            _mm256_mul_pd(_mm256_set1_pd(c710), values));
                      error = _mm256_sub_pd(error, _mm256_set1_pd(d7));
                      error = _mm256_andnot_pd(sign_bit, error);
                      mask &= _mm256_movemask_pd(
                          _mm256_cmp_pd(error, _mm256_set1_pd(e7), _CMP_LE_OQ));
                      if (mask == 0) continue;

                      error = _mm256_add_pd(_mm256_set1_pd(p9[7]),
                                            _mm256_mul_pd(_mm256_set1_pd(c810), values));
                      error = _mm256_sub_pd(error, _mm256_set1_pd(d8));
                      error = _mm256_andnot_pd(sign_bit, error);
                      mask &= _mm256_movemask_pd(
                          _mm256_cmp_pd(error, _mm256_set1_pd(e8), _CMP_LE_OQ));
                      if (mask == 0) continue;

                      error = _mm256_add_pd(_mm256_set1_pd(p9[8]),
                                            _mm256_mul_pd(_mm256_set1_pd(c910), values));
                      error = _mm256_sub_pd(error, _mm256_set1_pd(d9));
                      error = _mm256_andnot_pd(sign_bit, error);
                      mask &= _mm256_movemask_pd(
                          _mm256_cmp_pd(error, _mm256_set1_pd(e9), _CMP_LE_OQ));
                      if (mask == 0) continue;

                      error = _mm256_add_pd(_mm256_set1_pd(p9[9]),
                                            _mm256_mul_pd(_mm256_set1_pd(c1010), values));
                      error = _mm256_sub_pd(error, _mm256_set1_pd(d10));
                      error = _mm256_andnot_pd(sign_bit, error);
                      mask &= _mm256_movemask_pd(
                          _mm256_cmp_pd(error, _mm256_set1_pd(e10), _CMP_LE_OQ));
                      if (mask == 0) continue;

                      double last_values[4];
                      _mm256_storeu_pd(last_values, values);
                      for (int lane = 0; lane < 4; ++lane) {
                        if (!(mask & (1 << lane))) continue;
                        x10 = last_values[lane];
                        if ((size_t)pnts == capacity) {
                          if (capacity > SIZE_MAX / (2 * sizeof(*points))) {
                            printf("Too many result points to store in memory\n");
                            free(points);
                            fclose(fptr);
                            exit(EXIT_FAILURE);
                          }
                          size_t new_capacity = capacity * 2;
                          double (*next)[10] = realloc(points, new_capacity * sizeof(*points));
                          if (next == NULL) {
                            printf("Memory allocation failed\n");
                            free(points);
                            fclose(fptr);
                            exit(EXIT_FAILURE);
                          }
                          points = next;
                          capacity = new_capacity;
                        }
                        points[pnts][0] = x1;
                        points[pnts][1] = x2;
                        points[pnts][2] = x3;
                        points[pnts][3] = x4;
                        points[pnts][4] = x5;
                        points[pnts][5] = x6;
                        points[pnts][6] = x7;
                        points[pnts][7] = x8;
                        points[pnts][8] = x9;
                        points[pnts][9] = x10;
                        pnts = pnts + 1;
                      }
                    }

                    // Remaining candidates use the same scalar constraint checks.
                    for (; r10 < s10; ++r10) {
                      x10 = dd28 + r10 * dd30;

                      q1 = fabs(p9[0] + c110 * x10 - d1);
                      if (!(q1 <= e1)) continue;

                      q2 = fabs(p9[1] + c210 * x10 - d2);
                      if (!(q2 <= e2)) continue;

                      q3 = fabs(p9[2] + c310 * x10 - d3);
                      if (!(q3 <= e3)) continue;

                      q4 = fabs(p9[3] + c410 * x10 - d4);
                      if (!(q4 <= e4)) continue;

                      q5 = fabs(p9[4] + c510 * x10 - d5);
                      if (!(q5 <= e5)) continue;

                      q6 = fabs(p9[5] + c610 * x10 - d6);
                      if (!(q6 <= e6)) continue;

                      q7 = fabs(p9[6] + c710 * x10 - d7);
                      if (!(q7 <= e7)) continue;

                      q8 = fabs(p9[7] + c810 * x10 - d8);
                      if (!(q8 <= e8)) continue;

                      q9 = fabs(p9[8] + c910 * x10 - d9);
                      if (!(q9 <= e9)) continue;

                      q10 = fabs(p9[9] + c1010 * x10 - d10);
                      if (!(q10 <= e10)) continue;

                      if ((size_t)pnts == capacity) {
                        if (capacity > SIZE_MAX / (2 * sizeof(*points))) {
                          printf("Too many result points to store in memory\n");
                          free(points);
                          fclose(fptr);
                          exit(EXIT_FAILURE);
                        }
                        size_t new_capacity = capacity * 2;
                        double (*next)[10] = realloc(points, new_capacity * sizeof(*points));
                        if (next == NULL) {
                          printf("Memory allocation failed\n");
                          free(points);
                          fclose(fptr);
                          exit(EXIT_FAILURE);
                        }
                        points = next;
                        capacity = new_capacity;
                      }
                      points[pnts][0] = x1;
                      points[pnts][1] = x2;
                      points[pnts][2] = x3;
                      points[pnts][3] = x4;
                      points[pnts][4] = x5;
                      points[pnts][5] = x6;
                      points[pnts][6] = x7;
                      points[pnts][7] = x8;
                      points[pnts][8] = x9;
                      points[pnts][9] = x10;
                      pnts = pnts + 1;
                    }
                  }
                }
              }
            }
          }
        }
      }
    }
  }

  // Optimization: one fprintf per point, after all search loops have finished.
  for (long p = 0; p < pnts; ++p) {
    if (fprintf(fptr, "%lf\t%lf\t%lf\t%lf\t%lf\t%lf\t%lf\t%lf\t%lf\t%lf\n",
                points[p][0], points[p][1], points[p][2], points[p][3], points[p][4],
                points[p][5], points[p][6], points[p][7], points[p][8], points[p][9]) < 0) {
      printf("Error while writing results\n");
      free(points);
      fclose(fptr);
      exit(EXIT_FAILURE);
    }
  }
  free(points);

  fclose(fptr);
  printf("result pnts: %ld\n", pnts);

  // end function gridloopsearch
}