#ifndef COMMON_H
#define COMMON_H

#include "moonbit.h"
#include <string.h>

#if defined(__x86_64__) || defined(_M_X64)
#if (defined(__AVX512F__) && defined(__AVX512BW__))
#include <immintrin.h>
#define USE_AVX512
#elif defined(__AVX2__)
#include <immintrin.h>
#define USE_AVX2
#elif defined(__SSE2__)
#include <emmintrin.h>
#define USE_SSE2
#endif
#elif defined(__aarch64__) || defined(_M_ARM64)
#if defined(__ARM_NEON)
#include <arm_neon.h>
#define USE_NEON
#endif
#elif defined(__riscv) && __riscv_xlen == 64
#if defined(__riscv_v)
#include <riscv_vector.h>
#define USE_RVV
#endif
#endif

#if defined(__GNUC__) || defined(__clang__)
#define likely(cond) __builtin_expect((cond), 1)
#define unlikely(cond) __builtin_expect((cond), 0)
#define always_inline __attribute__((always_inline)) inline
#define export __attribute__((visibility("default")))
#define ctz32 __builtin_ctz
#define ctz64 __builtin_ctzll
#elif defined(_MSC_VER)
#define likely(cond) (cond)
#define unlikely(cond) (cond)
#define always_inline __forceinline inline
#define export __declspec(dllexport)
#include <intrin.h>
always_inline int32_t ctz32(const uint32_t bitmask)
{
    uint64_t count;
    return _BitScanForward(&count, (uint64_t)bitmask);
}
always_inline int32_t ctz64(const uint64_t bitmask)
{
    uint64_t count;
    return _BitScanForward(&count, bitmask);
}

#else
#define likely(cond) (cond)
#define unlikely(cond) (cond)
#define always_inline inline
#define export
always_inline int32_t ctz32(uint32_t bitmask)
{
    int32_t count = 0;
    while ((bitmask & 1) == 0)
    {
        bitmask >>= 1;
        ++count;
    }
    return count;
}
always_inline int32_t ctz64(uint64_t bitmask)
{
    int32_t count = 0;
    while ((bitmask & 1) == 0)
    {
        bitmask >>= 1;
        ++count;
    }
    return count;
}
#endif

#endif // COMMON_H
