#ifndef COMMON_H
#define COMMON_H

#include "moonbit.h"
#include <immintrin.h>

typedef __m256i u16x16;
typedef struct moonbit_view_t moonbit_view_t;

#define likely(x) __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)

#define true 1
#define false 0

#define avx2_inline __attribute__((target("avx2"), always_inline)) inline
#define avx2_export MOONBIT_EXPORT __attribute__((target("avx2")))

avx2_inline u16x16 u16x16__load(const uint16_t *src)
{
    return _mm256_loadu_si256((const u16x16 *)src);
}

avx2_inline u16x16 u16x16__splat(const uint16_t val)
{
    return _mm256_set1_epi16((int16_t)val);
}

avx2_inline u16x16 u16x16__eq(const u16x16 lhs, const u16x16 rhs)
{
    return _mm256_cmpeq_epi16(lhs, rhs);
}

avx2_inline int32_t u16x16__bitmask(const u16x16 src)
{
    return _mm256_movemask_epi8(src);
}

avx2_inline u16x16 u16x16__and(const u16x16 lhs, const u16x16 rhs)
{
    return _mm256_and_si256(lhs, rhs);
}

avx2_inline u16x16 u16x16__or(const u16x16 lhs, const u16x16 rhs)
{
    return _mm256_or_si256(lhs, rhs);
}

#endif // COMMON_H
