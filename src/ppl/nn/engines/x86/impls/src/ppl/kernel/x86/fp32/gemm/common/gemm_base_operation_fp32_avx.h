// Licensed to the Apache Software Foundation (ASF) under one
// or more contributor license agreements.  See the NOTICE file
// distributed with this work for additional information
// regarding copyright ownership.  The ASF licenses this file
// to you under the Apache License, Version 2.0 (the
// "License"); you may not use this file except in compliance
// with the License.  You may obtain a copy of the License at
//
//   http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing,
// software distributed under the License is distributed on an
// "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, either express or implied.  See the License for the
// specific language governing permissions and limitations
// under the License.

#ifndef __ST_PPL_KERNEL_X86_FP32_GEMM_COMMON_GEMM_BASE_OPERATION_FP32_AVX_H_
#define __ST_PPL_KERNEL_X86_FP32_GEMM_COMMON_GEMM_BASE_OPERATION_FP32_AVX_H_

#include <immintrin.h>

#include "ppl/kernel/x86/fp32/gemm.h"
#include "ppl/kernel/x86/common/avx_tools.h"
#include "ppl/kernel/x86/fp32/transpose/avx/transpose_fp32_avx.h"
#include "ppl/kernel/x86/fp32/transpose/sse/transpose_fp32_sse.h"

namespace ppl { namespace kernel { namespace x86 {

template<gemm_m_type_t typeB, int64_t ldpacked_b, int64_t kN>
void gemm_pack_b_operation_fp32_avx(
    const float *B,
    const int64_t N,
    const int64_t K,
    const int64_t ldb,
    float *packedB)
{
    const int64_t N_REG_ELTS = 8;
    const bool is_constant_N = kN != 0;
    const int64_t lN = is_constant_N ? kN : N;
    const bool is_aligned_kN = is_constant_N && (kN % N_REG_ELTS == 0);
    if (typeB == gemm_m_type::TRANS) {
        const int64_t unroll_n = N_REG_ELTS;
        const int64_t unroll_k = 8;
        const float *src = B;
        float *dst = packedB;
        int64_t n = lN;
        while (n >= unroll_n) { // B(N, K) -> (K, N)
            n -= unroll_n;
            const float *l_src = src;
            float *l_dst = dst;
            int64_t k = K;
// ========================================================================== //
#define K_TRANS_B_STEP(K) do {\
    l_dst[K * ldpacked_b + 0] = l_src[K + ldb * 0];\
    l_dst[K * ldpacked_b + 1] = l_src[K + ldb * 1];\
    l_dst[K * ldpacked_b + 2] = l_src[K + ldb * 2];\
    l_dst[K * ldpacked_b + 3] = l_src[K + ldb * 3];\
    l_dst[K * ldpacked_b + 4] = l_src[K + ldb * 4];\
    l_dst[K * ldpacked_b + 5] = l_src[K + ldb * 5];\
    l_dst[K * ldpacked_b + 6] = l_src[K + ldb * 6];\
    l_dst[K * ldpacked_b + 7] = l_src[K + ldb * 7];\
} while (0)
// ========================================================================== //
            while (k >= unroll_k) {
                k -= unroll_k;
                transpose_8x8_fp32_avx(l_src, ldb, ldpacked_b, l_dst);
                l_src += unroll_k;
                l_dst += unroll_k * ldpacked_b;
            }
            if (k & 4) {
                K_TRANS_B_STEP(0);
                K_TRANS_B_STEP(1);
                K_TRANS_B_STEP(2);
                K_TRANS_B_STEP(3);
                l_src += 4;
                l_dst += 4 * ldpacked_b;
            }
            if (k & 2) {
                K_TRANS_B_STEP(0);
                K_TRANS_B_STEP(1);
                l_src += 2;
                l_dst += 2 * ldpacked_b;
            }
            if (k & 1) {
                K_TRANS_B_STEP(0);
            }
#undef K_TRANS_B_STEP
            src += unroll_n * ldb;
            dst += unroll_n;
        }
        if (n & 4) {
            const float *l_src = src;
            float *l_dst = dst;
            int64_t k = K;
// ========================================================================== //
#define K_TRANS_B_STEP(K) do {\
    l_dst[K * ldpacked_b + 0] = l_src[K + ldb * 0];\
    l_dst[K * ldpacked_b + 1] = l_src[K + ldb * 1];\
    l_dst[K * ldpacked_b + 2] = l_src[K + ldb * 2];\
    l_dst[K * ldpacked_b + 3] = l_src[K + ldb * 3];\
} while (0)
// ========================================================================== //
            while (k >= unroll_k) {
                k -= unroll_k;
                K_TRANS_B_STEP(0);
                K_TRANS_B_STEP(1);
                K_TRANS_B_STEP(2);
                K_TRANS_B_STEP(3);
                K_TRANS_B_STEP(4);
                K_TRANS_B_STEP(5);
                K_TRANS_B_STEP(6);
                K_TRANS_B_STEP(7);
                l_src += unroll_k;
                l_dst += unroll_k * ldpacked_b;
            }
            if (k & 4) {
                K_TRANS_B_STEP(0);
                K_TRANS_B_STEP(1);
                K_TRANS_B_STEP(2);
                K_TRANS_B_STEP(3);
                l_src += 4;
                l_dst += 4 * ldpacked_b;
            }
            if (k & 2) {
                K_TRANS_B_STEP(0);
                K_TRANS_B_STEP(1);
                l_src += 2;
                l_dst += 2 * ldpacked_b;
            }
            if (k & 1) {
                K_TRANS_B_STEP(0);
            }
#undef K_TRANS_B_STEP
            src += 4 * ldb;
            dst += 4;
        }
        if (n & 2) {
            const float *l_src = src;
            float *l_dst = dst;
            int64_t k = K;
// ========================================================================== //
#define K_TRANS_B_STEP(K) do {\
    l_dst[K * ldpacked_b + 0] = l_src[K + ldb * 0];\
    l_dst[K * ldpacked_b + 1] = l_src[K + ldb * 1];\
} while (0)
// ========================================================================== //
            while (k >= unroll_k) {
                k -= unroll_k;
                K_TRANS_B_STEP(0);
                K_TRANS_B_STEP(1);
                K_TRANS_B_STEP(2);
                K_TRANS_B_STEP(3);
                K_TRANS_B_STEP(4);
                K_TRANS_B_STEP(5);
                K_TRANS_B_STEP(6);
                K_TRANS_B_STEP(7);
                l_src += unroll_k;
                l_dst += unroll_k * ldpacked_b;
            }
            if (k & 4) {
                K_TRANS_B_STEP(0);
                K_TRANS_B_STEP(1);
                K_TRANS_B_STEP(2);
                K_TRANS_B_STEP(3);
                l_src += 4;
                l_dst += 4 * ldpacked_b;
            }
            if (k & 2) {
                K_TRANS_B_STEP(0);
                K_TRANS_B_STEP(1);
                l_src += 2;
                l_dst += 2 * ldpacked_b;
            }
            if (k & 1) {
                K_TRANS_B_STEP(0);
            }
#undef K_TRANS_B_STEP
            src += 2 * ldb;
            dst += 2;
        }
        if (n & 1) {
            const float *l_src = src;
            float *l_dst = dst;
            int64_t k = K;
// ========================================================================== //
#define K_TRANS_B_STEP(K) do {\
    l_dst[K * ldpacked_b + 0] = l_src[K + ldb * 0];\
} while (0)
// ========================================================================== //
            while (k >= unroll_k) {
                k -= unroll_k;
                K_TRANS_B_STEP(0);
                K_TRANS_B_STEP(1);
                K_TRANS_B_STEP(2);
                K_TRANS_B_STEP(3);
                K_TRANS_B_STEP(4);
                K_TRANS_B_STEP(5);
                K_TRANS_B_STEP(6);
                K_TRANS_B_STEP(7);
                l_src += unroll_k;
                l_dst += unroll_k * ldpacked_b;
            }
            if (k & 4) {
                K_TRANS_B_STEP(0);
                K_TRANS_B_STEP(1);
                K_TRANS_B_STEP(2);
                K_TRANS_B_STEP(3);
                l_src += 4;
                l_dst += 4 * ldpacked_b;
            }
            if (k & 2) {
                K_TRANS_B_STEP(0);
                K_TRANS_B_STEP(1);
                l_src += 2;
                l_dst += 2 * ldpacked_b;
            }
            if (k & 1) {
                K_TRANS_B_STEP(0);
            }
#undef K_TRANS_B_STEP
        }
    } else {
        const int64_t unroll_k = 4;
        const float *src = B;
        float *dst = packedB;
        int64_t k = K;
        if (is_aligned_kN && kN <= 8 * N_REG_ELTS) {
#define K_PACK_B_STEP(K) do{\
    if (kN > 0 * N_REG_ELTS) _mm256_storeu_ps(dst + K * ldpacked_b + 0 * N_REG_ELTS, _mm256_loadu_ps(src + K * ldb + 0 * N_REG_ELTS));\
    if (kN > 1 * N_REG_ELTS) _mm256_storeu_ps(dst + K * ldpacked_b + 1 * N_REG_ELTS, _mm256_loadu_ps(src + K * ldb + 1 * N_REG_ELTS));\
    if (kN > 2 * N_REG_ELTS) _mm256_storeu_ps(dst + K * ldpacked_b + 2 * N_REG_ELTS, _mm256_loadu_ps(src + K * ldb + 2 * N_REG_ELTS));\
    if (kN > 3 * N_REG_ELTS) _mm256_storeu_ps(dst + K * ldpacked_b + 3 * N_REG_ELTS, _mm256_loadu_ps(src + K * ldb + 3 * N_REG_ELTS));\
    if (kN > 4 * N_REG_ELTS) _mm256_storeu_ps(dst + K * ldpacked_b + 4 * N_REG_ELTS, _mm256_loadu_ps(src + K * ldb + 4 * N_REG_ELTS));\
    if (kN > 5 * N_REG_ELTS) _mm256_storeu_ps(dst + K * ldpacked_b + 5 * N_REG_ELTS, _mm256_loadu_ps(src + K * ldb + 5 * N_REG_ELTS));\
    if (kN > 6 * N_REG_ELTS) _mm256_storeu_ps(dst + K * ldpacked_b + 6 * N_REG_ELTS, _mm256_loadu_ps(src + K * ldb + 6 * N_REG_ELTS));\
    if (kN > 7 * N_REG_ELTS) _mm256_storeu_ps(dst + K * ldpacked_b + 7 * N_REG_ELTS, _mm256_loadu_ps(src + K * ldb + 7 * N_REG_ELTS));\
} while (0)
            while (k >= unroll_k) { // B(K, N) -> (K, N)
                k -= unroll_k;
                K_PACK_B_STEP(0);
                K_PACK_B_STEP(1);
                K_PACK_B_STEP(2);
                K_PACK_B_STEP(3);
                src += unroll_k * ldb;
                dst += unroll_k * ldpacked_b;
            }
            if (k & 2) {
                K_PACK_B_STEP(0);
                K_PACK_B_STEP(1);
                src += 2 * ldb;
                dst += 2 * ldpacked_b;
            }
            if (k & 1) {
                K_PACK_B_STEP(0);
            }
#undef K_PACK_B_STEP
        } else {
            while (k >= unroll_k) { // B(K, N) -> (K, N)
                k -= unroll_k;
                memcpy32_avx(dst + 0 * ldpacked_b, src + 0 * ldb, lN);
                memcpy32_avx(dst + 1 * ldpacked_b, src + 1 * ldb, lN);
                memcpy32_avx(dst + 2 * ldpacked_b, src + 2 * ldb, lN);
                memcpy32_avx(dst + 3 * ldpacked_b, src + 3 * ldb, lN);
                src += unroll_k * ldb;
                dst += unroll_k * ldpacked_b;
            }
            while (k > 0) {
                k -= 1;
                memcpy32_avx(dst, src, lN);
                src += ldb;
                dst += ldpacked_b;
            }
        }
    }
}

template<gemm_m_type_t typeA>
void gemm_pack_a_m8_operation_fp32_avx(
    const float *A,
    const int64_t M,
    const int64_t K,
    const int64_t lda,
    float *packedA)
{
    if (typeA == gemm_m_type::TRANS) { // A: (K, M) -> (M/8, K, 8)
        const int64_t unroll_m = 8;
        const int64_t unroll_k = 4;
        int64_t k = K;
        const float *src = A;
        const int64_t m_body = round(M, unroll_m);
        const int64_t m_tail = M - m_body;
        float *dst_body = packedA;
        float *dst_tail = packedA + K * m_body;
        while (k >= unroll_k) {
            k -= unroll_k;
            if (m_body) {
                const float *l_src = src;
                float *l_dst = dst_body;
                int64_t m = m_body;
                while (m >= unroll_m) {
                    m -= unroll_m;
                    _mm256_storeu_ps(l_dst + 0 * unroll_m, _mm256_loadu_ps(l_src + 0 * lda));
                    _mm256_storeu_ps(l_dst + 1 * unroll_m, _mm256_loadu_ps(l_src + 1 * lda));
                    _mm256_storeu_ps(l_dst + 2 * unroll_m, _mm256_loadu_ps(l_src + 2 * lda));
                    _mm256_storeu_ps(l_dst + 3 * unroll_m, _mm256_loadu_ps(l_src + 3 * lda));
                    l_src += unroll_m;
                    l_dst += K * unroll_m;
                }
            }
#define M_TAIL_1(DST, SRC) do { *(DST) = *(SRC); } while (0)
#define M_TAIL_2(DST, SRC) do { *(int64_t*)(DST) = *(const int64_t*)(SRC); } while (0)
#define M_TAIL_3(DST, SRC) do { *(int64_t*)(DST) = *(const int64_t*)(SRC); *(DST + 2) = *(SRC + 2); } while (0)
#define M_TAIL_4(DST, SRC) do { _mm_storeu_ps(DST, _mm_loadu_ps(SRC)); } while (0)
#define M_TAIL_5(DST, SRC) do { _mm_storeu_ps(DST, _mm_loadu_ps(SRC)); *(DST + 4) = *(SRC + 4); } while (0)
#define M_TAIL_6(DST, SRC) do { _mm_storeu_ps(DST, _mm_loadu_ps(SRC)); *(int64_t*)(DST + 4) = *(const int64_t*)(SRC + 4); } while (0)
#define M_TAIL_7(DST, SRC) do { _mm_storeu_ps(DST, _mm_loadu_ps(SRC)); *(int64_t*)(DST + 4) = *(const int64_t*)(SRC + 4); *(DST + 6) = *(SRC + 6); } while (0)
            if (m_tail) {
                const float *l_src = src + m_body;
                float *l_dst = dst_tail;
                switch (m_tail) {
                    case 7: {
                        M_TAIL_7(l_dst + 0 * 7, l_src + 0 * lda);
                        M_TAIL_7(l_dst + 1 * 7, l_src + 1 * lda);
                        M_TAIL_7(l_dst + 2 * 7, l_src + 2 * lda);
                        M_TAIL_7(l_dst + 3 * 7, l_src + 3 * lda);
                        break;
                    }
                    case 6: {
                        M_TAIL_6(l_dst + 0 * 6, l_src + 0 * lda);
                        M_TAIL_6(l_dst + 1 * 6, l_src + 1 * lda);
                        M_TAIL_6(l_dst + 2 * 6, l_src + 2 * lda);
                        M_TAIL_6(l_dst + 3 * 6, l_src + 3 * lda);
                        break;
                    }
                    case 5: {
                        M_TAIL_5(l_dst + 0 * 5, l_src + 0 * lda);
                        M_TAIL_5(l_dst + 1 * 5, l_src + 1 * lda);
                        M_TAIL_5(l_dst + 2 * 5, l_src + 2 * lda);
                        M_TAIL_5(l_dst + 3 * 5, l_src + 3 * lda);
                        break;
                    }
                    case 4: {
                        M_TAIL_4(l_dst + 0 * 4, l_src + 0 * lda);
                        M_TAIL_4(l_dst + 1 * 4, l_src + 1 * lda);
                        M_TAIL_4(l_dst + 2 * 4, l_src + 2 * lda);
                        M_TAIL_4(l_dst + 3 * 4, l_src + 3 * lda);
                        break;
                    }
                    case 3: {
                        M_TAIL_3(l_dst + 0 * 3, l_src + 0 * lda);
                        M_TAIL_3(l_dst + 1 * 3, l_src + 1 * lda);
                        M_TAIL_3(l_dst + 2 * 3, l_src + 2 * lda);
                        M_TAIL_3(l_dst + 3 * 3, l_src + 3 * lda);
                        break;
                    }
                    case 2: {
                        M_TAIL_2(l_dst + 0 * 2, l_src + 0 * lda);
                        M_TAIL_2(l_dst + 1 * 2, l_src + 1 * lda);
                        M_TAIL_2(l_dst + 2 * 2, l_src + 2 * lda);
                        M_TAIL_2(l_dst + 3 * 2, l_src + 3 * lda);
                        break;
                    }
                    case 1: {
                        M_TAIL_1(l_dst + 0 * 1, l_src + 0 * lda);
                        M_TAIL_1(l_dst + 1 * 1, l_src + 1 * lda);
                        M_TAIL_1(l_dst + 2 * 1, l_src + 2 * lda);
                        M_TAIL_1(l_dst + 3 * 1, l_src + 3 * lda);
                        break;
                    }
                    default: break;
                }
            }
#undef M_TAIL_1
#undef M_TAIL_2
#undef M_TAIL_3
#undef M_TAIL_4
#undef M_TAIL_5
#undef M_TAIL_6
#undef M_TAIL_7
            src += unroll_k * lda;
            dst_body += unroll_k * unroll_m;
            dst_tail += unroll_k * m_tail;
        }
        while (k > 0) {
            k -= 1;
            if (m_body) {
                const float *l_src = src;
                float *l_dst = dst_body;
                int64_t m = m_body;
                while (m >= unroll_m * 2) {
                    m -= unroll_m * 2;
                    _mm256_storeu_ps(l_dst + 0 * K * unroll_m, _mm256_loadu_ps(l_src + 0 * unroll_m));
                    _mm256_storeu_ps(l_dst + 1 * K * unroll_m, _mm256_loadu_ps(l_src + 1 * unroll_m));
                    l_src += unroll_m * 2;
                    l_dst += K * unroll_m * 2;
                }
                if (m > 0) {
                    _mm256_storeu_ps(l_dst + 0 * K * unroll_m, _mm256_loadu_ps(l_src + 0 * unroll_m));
                }
            }
            if (m_tail) {
                const float *l_src = src + m_body;
                float *l_dst = dst_tail;
                if (m_tail & 4) {
                    _mm_storeu_ps(l_dst, _mm_loadu_ps(l_src));
                    l_src += 4;
                    l_dst += 4;
                }
                if (m_tail & 2) {
                    *(int64_t*)(l_dst) = *(int64_t*)(l_src);
                    l_src += 2;
                    l_dst += 2;
                }
                if (m_tail & 1) {
                    *l_dst = *l_src;
                }
            }
            src += lda;
            dst_body += unroll_m;
            dst_tail += m_tail;
        }
    } else { // A: (M, K) -> (M/8, K, 8)
        const int64_t unroll_m = 8;
        const int64_t unroll_k = 8;
        int64_t m = M;
        const float *src = A;
        float *dst = packedA;
        while (m >= unroll_m) {
            m -= unroll_m;
            int64_t k = K;
            const float *l_src = src;
            float *l_dst = dst;
            while (k >= unroll_k) {
                k -= unroll_k;
                transpose_8x8_fp32_avx(l_src, lda, unroll_m, l_dst);
                l_src += unroll_k;
                l_dst += unroll_k * unroll_m;
            }
// ========================================================================== //
#define K_TRANS_A_STEP(K) do {\
    l_dst[K * unroll_m + 0] = l_src[K + lda * 0];\
    l_dst[K * unroll_m + 1] = l_src[K + lda * 1];\
    l_dst[K * unroll_m + 2] = l_src[K + lda * 2];\
    l_dst[K * unroll_m + 3] = l_src[K + lda * 3];\
    l_dst[K * unroll_m + 4] = l_src[K + lda * 4];\
    l_dst[K * unroll_m + 5] = l_src[K + lda * 5];\
    l_dst[K * unroll_m + 6] = l_src[K + lda * 6];\
    l_dst[K * unroll_m + 7] = l_src[K + lda * 7];\
} while (0)
// ========================================================================== //
            if (k & 4) {
                K_TRANS_A_STEP(0);
                K_TRANS_A_STEP(1);
                K_TRANS_A_STEP(2);
                K_TRANS_A_STEP(3);
                l_src += 4;
                l_dst += 4 * unroll_m;
            }
            if (k & 2) {
                K_TRANS_A_STEP(0);
                K_TRANS_A_STEP(1);
                l_src += 2;
                l_dst += 2 * unroll_m;
            }
            if (k & 1) {
                K_TRANS_A_STEP(0);
            }
#undef K_TRANS_A_STEP
            src += unroll_m * lda;
            dst += unroll_m * K;
        }
        if (m > 0) {
            int64_t k = K;
// ========================================================================== //
#define K_TRANS_A_STEP(K) do {\
    const float *l_src = src;\
    float *l_dst = dst;\
    if (m & 4) {\
        l_dst[K * m + 0] = l_src[K + lda * 0];\
        l_dst[K * m + 1] = l_src[K + lda * 1];\
        l_dst[K * m + 2] = l_src[K + lda * 2];\
        l_dst[K * m + 3] = l_src[K + lda * 3];\
        l_src += 4 * lda;\
        l_dst += 4;\
    }\
    if (m & 2) {\
        l_dst[K * m + 0] = l_src[K + lda * 0];\
        l_dst[K * m + 1] = l_src[K + lda * 1];\
        l_src += 2 * lda;\
        l_dst += 2;\
    }\
    if (m & 1) {\
        l_dst[K * m + 0] = l_src[K + lda * 0];\
    }\
} while (0)
// ========================================================================== //
            while (k >= unroll_k) {
                k -= unroll_k;
                K_TRANS_A_STEP(0);
                K_TRANS_A_STEP(1);
                K_TRANS_A_STEP(2);
                K_TRANS_A_STEP(3);
                K_TRANS_A_STEP(4);
                K_TRANS_A_STEP(5);
                K_TRANS_A_STEP(6);
                K_TRANS_A_STEP(7);
                src += unroll_k;
                dst += unroll_k * m;
            }
            if (k & 4) {
                K_TRANS_A_STEP(0);
                K_TRANS_A_STEP(1);
                K_TRANS_A_STEP(2);
                K_TRANS_A_STEP(3);
                src += 4;
                dst += 4 * m;
            }
            if (k & 2) {
                K_TRANS_A_STEP(0);
                K_TRANS_A_STEP(1);
                src += 2;
                dst += 2 * m;
            }
            if (k & 1) {
                K_TRANS_A_STEP(0);
            }
#undef K_TRANS_A_STEP
        }
    }
}

template<gemm_m_type_t typeA>
void gemm_pack_a_m4_operation_fp32_avx(
    const float *A,
    const int64_t M,
    const int64_t K,
    const int64_t lda,
    float *packedA)
{
    if (typeA == gemm_m_type::TRANS) { // A: (K, M) -> (M/4, K, 4)
        const int64_t unroll_m = 4;
        const int64_t unroll_k = 4;
        int64_t k = K;
        const float *src = A;
        const int64_t m_body = round(M, unroll_m);
        const int64_t m_tail = M - m_body;
        float *dst_body = packedA;
        float *dst_tail = packedA + K * m_body;
        while (k >= unroll_k) {
            k -= unroll_k;
            if (m_body) {
                const float *l_src = src;
                float *l_dst = dst_body;
                int64_t m = m_body;
                while (m >= unroll_m) {
                    m -= unroll_m;
                    _mm_storeu_ps(l_dst + 0 * unroll_m, _mm_loadu_ps(l_src + 0 * lda));
                    _mm_storeu_ps(l_dst + 1 * unroll_m, _mm_loadu_ps(l_src + 1 * lda));
                    _mm_storeu_ps(l_dst + 2 * unroll_m, _mm_loadu_ps(l_src + 2 * lda));
                    _mm_storeu_ps(l_dst + 3 * unroll_m, _mm_loadu_ps(l_src + 3 * lda));
                    l_src += unroll_m;
                    l_dst += K * unroll_m;
                }
            }
#define M_TAIL_1(DST, SRC) do { *(DST) = *(SRC); } while (0)
#define M_TAIL_2(DST, SRC) do { *(int64_t*)(DST) = *(const int64_t*)(SRC); } while (0)
#define M_TAIL_3(DST, SRC) do { *(int64_t*)(DST) = *(const int64_t*)(SRC); *(DST + 2) = *(SRC + 2); } while (0)
            if (m_tail) {
                const float *l_src = src + m_body;
                float *l_dst = dst_tail;
                switch (m_tail) {
                    case 3: {
                        M_TAIL_3(l_dst + 0 * 3, l_src + 0 * lda);
                        M_TAIL_3(l_dst + 1 * 3, l_src + 1 * lda);
                        M_TAIL_3(l_dst + 2 * 3, l_src + 2 * lda);
                        M_TAIL_3(l_dst + 3 * 3, l_src + 3 * lda);
                        break;
                    }
                    case 2: {
                        M_TAIL_2(l_dst + 0 * 2, l_src + 0 * lda);
                        M_TAIL_2(l_dst + 1 * 2, l_src + 1 * lda);
                        M_TAIL_2(l_dst + 2 * 2, l_src + 2 * lda);
                        M_TAIL_2(l_dst + 3 * 2, l_src + 3 * lda);
                        break;
                    }
                    case 1: {
                        M_TAIL_1(l_dst + 0 * 1, l_src + 0 * lda);
                        M_TAIL_1(l_dst + 1 * 1, l_src + 1 * lda);
                        M_TAIL_1(l_dst + 2 * 1, l_src + 2 * lda);
                        M_TAIL_1(l_dst + 3 * 1, l_src + 3 * lda);
                        break;
                    }
                    default: break;
                }
            }
#undef M_TAIL_1
#undef M_TAIL_2
#undef M_TAIL_3
            src += unroll_k * lda;
            dst_body += unroll_k * unroll_m;
            dst_tail += unroll_k * m_tail;
        }
        while (k > 0) {
            k -= 1;
            if (m_body) {
                const float *l_src = src;
                float *l_dst = dst_body;
                int64_t m = m_body;
                while (m >= unroll_m * 2) {
                    m -= unroll_m * 2;
                    _mm_storeu_ps(l_dst + 0 * K * unroll_m, _mm_loadu_ps(l_src + 0 * unroll_m));
                    _mm_storeu_ps(l_dst + 1 * K * unroll_m, _mm_loadu_ps(l_src + 1 * unroll_m));
                    l_src += unroll_m * 2;
                    l_dst += K * unroll_m * 2;
                }
                if (m > 0) {
                    _mm_storeu_ps(l_dst + 0 * K * unroll_m, _mm_loadu_ps(l_src + 0 * unroll_m));
                }
            }
            if (m_tail) {
                const float *l_src = src + m_body;
                float *l_dst = dst_tail;
                if (m_tail & 2) {
                    *(int64_t*)(l_dst) = *(int64_t*)(l_src);
                    l_src += 2;
                    l_dst += 2;
                }
                if (m_tail & 1) {
                    *l_dst = *l_src;
                }
            }
            src += lda;
            dst_body += unroll_m;
            dst_tail += m_tail;
        }
    } else { // A: (M, K) -> (M/4, K, 4)
        const int64_t unroll_m = 4;
        const int64_t unroll_k = 4;
        int64_t m = M;
        const float *src = A;
        float *dst = packedA;
        while (m >= unroll_m) {
            m -= unroll_m;
            int64_t k = K;
            const float *l_src = src;
            float *l_dst = dst;
            while (k >= unroll_k * 2) {
                k -= unroll_k * 2;
                transpose_4x4_fp32_sse(l_src + 0 * unroll_k, lda, unroll_m, l_dst + 0 * unroll_k * unroll_m);
                transpose_4x4_fp32_sse(l_src + 1 * unroll_k, lda, unroll_m, l_dst + 1 * unroll_k * unroll_m);
                l_src += unroll_k * 2;
                l_dst += unroll_k * 2 * unroll_m;
            }
            if (k & 4) {
                transpose_4x4_fp32_sse(l_src + 0 * unroll_k, lda, unroll_m, l_dst + 0 * unroll_k * unroll_m);
                l_src += unroll_k;
                l_dst += unroll_k * unroll_m;
            }
// ========================================================================== //
#define K_TRANS_A_STEP(K) do {\
    l_dst[K * unroll_m + 0] = l_src[K + lda * 0];\
    l_dst[K * unroll_m + 1] = l_src[K + lda * 1];\
    l_dst[K * unroll_m + 2] = l_src[K + lda * 2];\
    l_dst[K * unroll_m + 3] = l_src[K + lda * 3];\
} while (0)
// ========================================================================== //
            if (k & 2) {
                K_TRANS_A_STEP(0);
                K_TRANS_A_STEP(1);
                l_src += 2;
                l_dst += 2 * unroll_m;
            }
            if (k & 1) {
                K_TRANS_A_STEP(0);
            }
#undef K_TRANS_A_STEP
            src += unroll_m * lda;
            dst += unroll_m * K;
        }
        if (m > 0) {
            int64_t k = K;
// ========================================================================== //
#define K_TRANS_A_STEP(K) do {\
    const float *l_src = src;\
    float *l_dst = dst;\
    if (m & 2) {\
        l_dst[K * m + 0] = l_src[K + lda * 0];\
        l_dst[K * m + 1] = l_src[K + lda * 1];\
        l_src += 2 * lda;\
        l_dst += 2;\
    }\
    if (m & 1) {\
        l_dst[K * m + 0] = l_src[K + lda * 0];\
    }\
} while (0)
// ========================================================================== //
            while (k >= unroll_k * 2) {
                k -= unroll_k * 2;
                K_TRANS_A_STEP(0);
                K_TRANS_A_STEP(1);
                K_TRANS_A_STEP(2);
                K_TRANS_A_STEP(3);
                K_TRANS_A_STEP(4);
                K_TRANS_A_STEP(5);
                K_TRANS_A_STEP(6);
                K_TRANS_A_STEP(7);
                src += unroll_k * 2;
                dst += unroll_k * 2 * m;
            }
            if (k & 4) {
                K_TRANS_A_STEP(0);
                K_TRANS_A_STEP(1);
                K_TRANS_A_STEP(2);
                K_TRANS_A_STEP(3);
                src += unroll_k;
                dst += unroll_k * m;
            }
            if (k & 2) {
                K_TRANS_A_STEP(0);
                K_TRANS_A_STEP(1);
                src += 2;
                dst += 2 * m;
            }
            if (k & 1) {
                K_TRANS_A_STEP(0);
            }
#undef K_TRANS_A_STEP
        }
    }
}

template<gemm_m_type_t typesum, gemm_v_type_t typebias>
void gemm_fp32_apply_betas_avx(
    const float *bias,
    const float *sum,
    const int64_t M,
    const int64_t N,
    const int64_t ldc,
    const int64_t ldsum,
    const float beta,
    const float beta_bias,
    const float beta_sum,
    float *C)
{
    const int64_t VEC_REG_ELTS = 8;
    const int64_t unroll_n = VEC_REG_ELTS * 2;
    const bool has_beta = beta != 0.0f;
 
    __m256 ymm_bias, ymm_beta, ymm_beta_bias, ymm_beta_sum;
    if (typebias == gemm_v_type::SCALAR) ymm_bias = _mm256_set1_ps(bias[0] * beta_bias);
    else if (typebias == gemm_v_type::ROW_VEC) ymm_beta_bias = _mm256_set1_ps(beta_bias);
    ymm_beta = _mm256_set1_ps(beta);
    ymm_beta_sum = _mm256_set1_ps(beta_sum);

    for (int64_t m = 0; m < M; ++m) {
        const float *l_bias = nullptr;
        const float *l_sum = nullptr;
        if (typebias == gemm_v_type::COL_VEC) ymm_bias = _mm256_set1_ps(bias[m] * beta_bias);
        if (typebias == gemm_v_type::ROW_VEC) l_bias = bias;
        if (typesum == gemm_m_type::NOTRANS) l_sum = sum + m * ldsum;
        float *l_c = C + m * ldc;
        int64_t n = N;
        while (n >= unroll_n) {
            __m256 ymm0 = _mm256_setzero_ps();
            __m256 ymm1 = _mm256_setzero_ps();
            if (typebias != gemm_v_type::EMPTY) {
                if (typebias == gemm_v_type::ROW_VEC) {
                    ymm0 = _mm256_mul_ps(_mm256_loadu_ps(l_bias + 0 * VEC_REG_ELTS), ymm_beta_bias);
                    ymm1 = _mm256_mul_ps(_mm256_loadu_ps(l_bias + 1 * VEC_REG_ELTS), ymm_beta_bias);
                } else {
                    ymm0 = ymm_bias;
                    ymm1 = ymm_bias;
                }
            }
            if (typesum == gemm_m_type::NOTRANS) {
                if (typebias == gemm_v_type::EMPTY) {
                    ymm0 = _mm256_mul_ps(_mm256_loadu_ps(l_sum + 0 * VEC_REG_ELTS), ymm_beta_sum);
                    ymm1 = _mm256_mul_ps(_mm256_loadu_ps(l_sum + 1 * VEC_REG_ELTS), ymm_beta_sum);
                } else {
                    ymm0 = _mm256_add_ps(_mm256_mul_ps(_mm256_loadu_ps(l_sum + 0 * VEC_REG_ELTS), ymm_beta_sum), ymm0);
                    ymm1 = _mm256_add_ps(_mm256_mul_ps(_mm256_loadu_ps(l_sum + 1 * VEC_REG_ELTS), ymm_beta_sum), ymm1);
                }
                _mm_prefetch((const char*)(l_sum + 4 * ldsum), _MM_HINT_T0);
            }
            if (has_beta) {
                ymm0 = _mm256_add_ps(_mm256_mul_ps(_mm256_loadu_ps(l_c + 0 * VEC_REG_ELTS), ymm_beta), ymm0);
                ymm1 = _mm256_add_ps(_mm256_mul_ps(_mm256_loadu_ps(l_c + 1 * VEC_REG_ELTS), ymm_beta), ymm1);
            }
            _mm256_storeu_ps(l_c + 0 * VEC_REG_ELTS, ymm0);
            _mm256_storeu_ps(l_c + 1 * VEC_REG_ELTS, ymm1);
            _mm_prefetch((const char*)(l_c + 4 * ldc), _MM_HINT_T0);
            if (typebias == gemm_v_type::ROW_VEC) l_bias += unroll_n;
            if (typesum == gemm_m_type::NOTRANS) l_sum += unroll_n;
            l_c += unroll_n;
            n -= unroll_n;
        }
        while (n > 0) {
            float y = 0.0f;
            if (typebias != gemm_v_type::EMPTY) {
                if (typebias == gemm_v_type::SCALAR) {
                    y = bias[0] * beta_bias;
                }
                if (typebias == gemm_v_type::ROW_VEC) {
                    y = l_bias[0] * beta_bias;
                }
                if (typebias == gemm_v_type::COL_VEC) {
                    y = bias[m] * beta_bias;
                }
            }
            if (typesum == gemm_m_type::NOTRANS) {
                if (typebias == gemm_v_type::EMPTY) {
                    y = l_sum[0] * beta_sum;
                } else {
                    y += l_sum[0] * beta_sum;
                }
            }
            if (has_beta) y += beta * l_c[0];
            l_c[0] = y;
            if (typebias == gemm_v_type::ROW_VEC) l_bias += 1;
            if (typesum == gemm_m_type::NOTRANS) l_sum += 1;
            l_c += 1;
            n -= 1;
        }
    }
}

}}};

#endif
