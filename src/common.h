#ifndef COMMON_H
#define COMMON_H

#if defined(__x86_64__) || defined(_M_X64)
    #if (defined(__AVX512F__) && defined(__AVX512BW__)) || defined(__AVX2__)
        #include <immintrin.h>
    #elif defined(__SSE2__)
        #include <emmintrin.h>
    #endif
#elif defined(__aarch64__) || defined(_M_ARM64)
    #if defined(__ARM_NEON)
        #include <arm_neon.h>
    #endif
#endif

#if defined(__GNUC__) || defined(__clang__)
    #define likely(cond) __builtin_expect((cond), 1)
    #define unlikely(cond) __builtin_expect((cond), 0)
    #define always_inline __attribute__((always_inline)) inline
    #define export __attribute__((visibility("default")))
#elif defined(_MSC_VER)
    #define likely(cond) (cond)
    #define unlikely(cond) (cond)
    #define always_inline __forceinline inline
    #define export __declspec(dllexport)
#else
    #define likely(cond) (cond)
    #define unlikely(cond) (cond)
    #define always_inline inline
    #define export
#endif

#include "moonbit.h"
#include <string.h>

#endif // COMMON_H
