#ifndef COMMON_H
#define COMMON_H

#include "moonbit.h"
#include <smmintrin.h>

typedef __m128i u16x8;
typedef struct moonbit_view_t moonbit_view_t;

#define likely(x) __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)

#define true 1
#define false 0

#define sse4_2_inline __attribute__((target("sse4.2"), always_inline)) inline
#define sse4_2_export MOONBIT_EXPORT __attribute__((target("sse4.2")))

sse4_2_inline u16x8 u16x8__load(const uint16_t *src)
{
    return _mm_loadu_si128((const u16x8 *)src);
}

sse4_2_inline u16x8 u16x8__splat(const uint16_t word)
{
    return _mm_set1_epi16((int16_t)word);
}

sse4_2_inline u16x8 u16x8__eq(const u16x8 lhs, const u16x8 rhs)
{
    return _mm_cmpeq_epi16(lhs, rhs);
}

sse4_2_inline int32_t u16x8__bitmask(const u16x8 mask)
{
    return _mm_movemask_epi8(mask);
}

sse4_2_inline u16x8 u16x8__and(const u16x8 lhs, const u16x8 rhs)
{
    return _mm_and_si128(lhs, rhs);
}

sse4_2_inline u16x8 u16x8__or(const u16x8 lhs, const u16x8 rhs)
{
    return _mm_or_si128(lhs, rhs);
}

#endif // COMMON_H
