#include "common.h"

#if defined(USE_AVX512)

export int64_t find_char(const struct moonbit_view_t haystack, const int32_t needle)
{
    const int32_t len = haystack.len;
    if (unlikely(!len))
        return INT64_C(0x100000000);

    const uint16_t *const start = (uint16_t *)haystack.buf + haystack.start;
    const uint16_t *const end = start + len;
    const uint16_t *ptr = start;

    if (likely(needle <= 0xFFFF))
    {
        const uint16_t tgt = (uint16_t)needle;
        const __m512i tgt_blk = _mm512_set1_epi16((int16_t)tgt);

        while (likely(ptr <= end - 32))
        {
            const __m512i src_blk = _mm512_loadu_si512((const __m512i *)ptr);
            const uint32_t res_mask = _mm512_cmpeq_epi16_mask(src_blk, tgt_blk);
            if (unlikely(res_mask))
                return (int32_t)(ptr - start) + ctz32(res_mask);
            ptr += 32;
        }

        if (likely(ptr < end))
        {
            uint16_t buf[32] = {0};
            memset(buf, !tgt, sizeof(uint16_t) * 32);
            memcpy(buf, ptr, sizeof(uint16_t) * (end - ptr));
            const __m512i src_blk = _mm512_loadu_si512((const __m512i *)buf);
            const uint32_t res_mask = _mm512_cmpeq_epi16_mask(src_blk, tgt_blk);
            if (unlikely(res_mask))
                return (int32_t)(ptr - start) + ctz32(res_mask);
        }
    }
    else
    {
        if (unlikely(len < 2))
            return INT64_C(0x100000000);

        const int32_t adjusted = needle - 0x10000;
        const uint16_t high_tgt = 0xD800 + (adjusted >> 10);
        const uint16_t low_tgt = 0xDC00 + (adjusted & 0x3FF);
        const __m512i high_tgt_blk = _mm512_set1_epi16((int16_t)high_tgt);
        const __m512i low_tgt_blk = _mm512_set1_epi16((int16_t)low_tgt);

        while (likely(ptr <= end - 33))
        {
            const __m512i high_src_blk = _mm512_loadu_si512((const __m512i *)ptr);
            const uint32_t high_res_mask = _mm512_cmpeq_epi16_mask(high_src_blk, high_tgt_blk);
            if (unlikely(high_res_mask))
            {
                const __m512i low_src_blk = _mm512_loadu_si512((const __m512i *)(ptr + 1));
                const uint32_t low_res_mask = _mm512_cmpeq_epi16_mask(low_src_blk, low_tgt_blk);
                const uint32_t res_mask = high_res_mask & low_res_mask;
                if (unlikely(res_mask))
                    return (int32_t)(ptr - start) + ctz32(res_mask);
            }
            ptr += 32;
        }

        if (likely(ptr < end - 1))
        {
            uint16_t buf[33] = {0};
            memcpy(buf, ptr, sizeof(uint16_t) * (end - ptr));
            const __m512i high_src_blk = _mm512_loadu_si512((const __m512i *)buf);
            const uint32_t high_res_mask = _mm512_cmpeq_epi16_mask(high_src_blk, high_tgt_blk);
            if (unlikely(high_res_mask))
            {
                const __m512i low_src_blk = _mm512_loadu_si512((const __m512i *)(buf + 1));
                const uint32_t low_res_mask = _mm512_cmpeq_epi16_mask(low_src_blk, low_tgt_blk);
                const uint32_t res_mask = high_res_mask & low_res_mask;
                if (unlikely(res_mask))
                    return (int32_t)(ptr - start) + ctz32(res_mask);
            }
        }
    }
    return INT64_C(0x100000000);
}

#elif defined(USE_AVX2)

export int64_t find_char(const struct moonbit_view_t haystack, const int32_t needle)
{
    const int32_t len = haystack.len;
    if (unlikely(!len))
        return INT64_C(0x100000000);

    const uint16_t *const start = (uint16_t *)haystack.buf + haystack.start;
    const uint16_t *const end = start + len;
    const uint16_t *ptr = start;

    if (likely(needle <= 0xFFFF))
    {
        const uint16_t tgt = (uint16_t)needle;
        const __m256i tgt_blk = _mm256_set1_epi16((int16_t)tgt);

        while (likely(ptr <= end - 16))
        {
            const __m256i src_blk = _mm256_loadu_si256((const __m256i *)ptr);
            const uint32_t res_mask = _mm256_movemask_epi8(_mm256_cmpeq_epi16(src_blk, tgt_blk));
            if (unlikely(res_mask))
                return (int32_t)(ptr - start) + (ctz32(res_mask) >> 1);
            ptr += 16;
        }

        if (unlikely(ptr < end))
        {
            uint16_t buf[16];
            memset(buf, !tgt, sizeof(uint16_t) * 16);
            memcpy(buf, ptr, sizeof(uint16_t) * (end - ptr));
            const __m256i src_blk = _mm256_loadu_si256((const __m256i *)buf);
            const uint32_t res_mask = _mm256_movemask_epi8(_mm256_cmpeq_epi16(src_blk, tgt_blk));
            if (unlikely(res_mask))
                return (int32_t)(ptr - start) + (ctz32(res_mask) >> 1);
        }
    }
    else
    {
        if (unlikely(len < 2))
            return INT64_C(0x100000000);

        const int32_t adjusted = needle - 0x10000;
        const uint16_t high_tgt = 0xD800 + (adjusted >> 10);
        const uint16_t low_tgt = 0xDC00 + (adjusted & 0x3FF);
        const __m256i high_tgt_blk = _mm256_set1_epi16((int16_t)high_tgt);
        const __m256i low_tgt_blk = _mm256_set1_epi16((int16_t)low_tgt);

        while (likely(ptr <= end - 17))
        {
            const __m256i high_src_blk = _mm256_loadu_si256((const __m256i *)ptr);
            const uint32_t high_res_mask = _mm256_movemask_epi8(_mm256_cmpeq_epi16(high_src_blk, high_tgt_blk));
            if (unlikely(high_res_mask))
            {
                const __m256i low_src_blk = _mm256_loadu_si256((const __m256i *)(ptr + 1));
                const uint32_t low_res_mask = _mm256_movemask_epi8(_mm256_cmpeq_epi16(low_src_blk, low_tgt_blk));
                const uint32_t res_mask = high_res_mask & low_res_mask;
                if (unlikely(res_mask))
                    return (int32_t)(ptr - start) + (ctz32(res_mask) >> 1);
            }
            ptr += 16;
        }

        if (unlikely(ptr < end - 1))
        {
            uint16_t buf[17] = {0};
            memcpy(buf, ptr, sizeof(uint16_t) * (end - ptr));
            const __m256i high_src_blk = _mm256_loadu_si256((const __m256i *)buf);
            const uint32_t high_res_mask = _mm256_movemask_epi8(_mm256_cmpeq_epi16(high_src_blk, high_tgt_blk));
            if (unlikely(high_res_mask))
            {
                const __m256i low_src_blk = _mm256_loadu_si256((const __m256i *)(buf + 1));
                const uint32_t low_res_mask = _mm256_movemask_epi8(_mm256_cmpeq_epi16(low_src_blk, low_tgt_blk));
                const uint32_t res_mask = high_res_mask & low_res_mask;
                if (unlikely(res_mask))
                    return (int32_t)(ptr - start) + (ctz32(res_mask) >> 1);
            }
        }
    }
    return INT64_C(0x100000000);
}

#elif defined(USE_SSE2)

export int64_t find_char(const struct moonbit_view_t haystack, const int32_t needle)
{
    const int32_t len = haystack.len;
    if (unlikely(!len))
        return INT64_C(0x100000000);

    const uint16_t *const start = (uint16_t *)haystack.buf + haystack.start;
    const uint16_t *const end = start + len;
    const uint16_t *ptr = start;

    if (likely(needle <= 0xFFFF))
    {
        const uint16_t tgt = (uint16_t)needle;
        const __m128i tgt_blk = _mm_set1_epi16((int16_t)tgt);

        while (likely(ptr <= end - 8))
        {
            const __m128i src_blk = _mm_loadu_si128((const __m128i *)ptr);
            const uint32_t res_mask = _mm_movemask_epi8(_mm_cmpeq_epi16(src_blk, tgt_blk));
            if (unlikely(res_mask))
                return (int32_t)(ptr - start) + (ctz32(res_mask) >> 1);
            ptr += 8;
        }

        if (unlikely(ptr < end))
        {
            uint16_t buf[8];
            memset(buf, !tgt, sizeof(uint16_t) * 8);
            memcpy(buf, ptr, sizeof(uint16_t) * (end - ptr));
            const __m128i src_blk = _mm_loadu_si128((const __m128i *)buf);
            const uint32_t res_mask = _mm_movemask_epi8(_mm_cmpeq_epi16(src_blk, tgt_blk));
            if (unlikely(res_mask))
                return (int32_t)(ptr - start) + (ctz32(res_mask) >> 1);
        }
    }
    else
    {
        if (unlikely(len < 2))
            return INT64_C(0x100000000);
        const int32_t adjusted = needle - 0x10000;
        const uint16_t high_tgt = 0xD800 + (adjusted >> 10);
        const uint16_t low_tgt = 0xDC00 + (adjusted & 0x3FF);
        const __m128i high_tgt_blk = _mm_set1_epi16((int16_t)high_tgt);
        const __m128i low_tgt_blk = _mm_set1_epi16((int16_t)low_tgt);

        while (likely(ptr <= end - 9))
        {
            const __m128i high_src_blk = _mm_loadu_si128((const __m128i *)ptr);
            const uint32_t high_res_mask = _mm_movemask_epi8(_mm_cmpeq_epi16(high_src_blk, high_tgt_blk));
            if (unlikely(high_res_mask))
            {
                const __m128i low_src_blk = _mm_loadu_si128((const __m128i *)(ptr + 1));
                const uint32_t low_res_mask = _mm_movemask_epi8(_mm_cmpeq_epi16(low_src_blk, low_tgt_blk));
                const uint32_t res_mask = high_res_mask & low_res_mask;
                if (unlikely(res_mask))
                    return (int32_t)(ptr - start) + (ctz32(res_mask) >> 1);
            }
            ptr += 8;
        }

        if (unlikely(ptr < end - 1))
        {
            uint16_t buf[9] = {0};
            memcpy(buf, ptr, sizeof(uint16_t) * (end - ptr));
            const __m128i high_src_blk = _mm_loadu_si128((const __m128i *)buf);
            const uint32_t high_res_mask = _mm_movemask_epi8(_mm_cmpeq_epi16(high_src_blk, high_tgt_blk));
            if (unlikely(high_res_mask))
            {
                const __m128i low_src_blk = _mm_loadu_si128((const __m128i *)(buf + 1));
                const uint32_t low_res_mask = _mm_movemask_epi8(_mm_cmpeq_epi16(low_src_blk, low_tgt_blk));
                const uint32_t res_mask = high_res_mask & low_res_mask;
                if (unlikely(res_mask))
                    return (int32_t)(ptr - start) + (ctz32(res_mask) >> 1);
            }
        }
    }
    return INT64_C(0x100000000);
}

#elif defined(USE_NEON)

always_inline uint64_t to_bitmask(const uint16x8_t words)
{
    return (uint64_t)vshrn_n_u16(words, 8) & 0x8080808080808080ull;
}

export int64_t find_char(const struct moonbit_view_t haystack, const int32_t needle)
{
    const int32_t len = haystack.len;
    if (unlikely(!len))
        return INT64_C(0x100000000);

    const uint16_t *const start = (uint16_t *)haystack.buf + haystack.start;
    const uint16_t *const end = start + len;
    const uint16_t *ptr = start;

    if (likely(needle <= 0xFFFF))
    {
        const uint16_t tgt = (uint16_t)needle;
        const uint16x8_t tgt_blk = vdupq_n_u16(tgt);

        while (likely(ptr <= end - 8))
        {
            const uint16x8_t src_blk = vld1q_u16(ptr);
            const uint16x8_t res_blk = vceqq_u16(src_blk, tgt_blk);
            if (unlikely(vmaxvq_u16(res_blk)))
                return (int32_t)(ptr - start) + (ctz64(to_bitmask(res_blk)) >> 3);
            ptr += 8;
        }

        if (unlikely(ptr < end))
        {
            uint16_t buf[8];
            memset(buf, !tgt, sizeof(uint16_t) * 8);
            memcpy(buf, ptr, sizeof(uint16_t) * (end - ptr));
            const uint16x8_t src_blk = vld1q_u16(buf);
            const uint16x8_t res_blk = vceqq_u16(src_blk, tgt_blk);
            if (unlikely(vmaxvq_u16(res_blk)))
                return (int32_t)(ptr - start) + (ctz64(to_bitmask(res_blk)) >> 3);
        }
    }
    else
    {
        if (unlikely(len < 2))
            return INT64_C(0x100000000);

        const int32_t adjusted = needle - 0x10000;
        const uint16_t high_tgt = 0xD800 + (adjusted >> 10);
        const uint16_t low_tgt = 0xDC00 + (adjusted & 0x3FF);
        const uint16x8_t high_tgt_blk = vdupq_n_u16(high_tgt);
        const uint16x8_t low_tgt_blk = vdupq_n_u16(low_tgt);

        while (likely(ptr <= end - 9))
        {
            const uint16x8_t high_src_blk = vld1q_u16(ptr);
            const uint16x8_t high_res_blk = vceqq_u16(high_src_blk, high_tgt_blk);
            if (unlikely(vmaxvq_u16(high_res_blk)))
            {
                const uint16x8_t low_src_blk = vld1q_u16(ptr + 1);
                const uint16x8_t low_res_blk = vceqq_u16(low_src_blk, low_tgt_blk);
                if (unlikely(vmaxvq_u16(low_res_blk)))
                {
                    const uint16x8_t res_blk = vandq_u16(high_res_blk, low_res_blk);
                    if (unlikely(vmaxvq_u16(res_blk)))
                        return (int32_t)(ptr - start) + (ctz64(to_bitmask(res_blk)) >> 3);
                }
            }
            ptr += 8;
        }

        if (unlikely(ptr < end - 1))
        {
            uint16_t buf[9] = {0};
            memcpy(buf, ptr, sizeof(uint16_t) * (end - ptr));
            const uint16x8_t high_src_blk = vld1q_u16(buf);
            const uint16x8_t high_res_blk = vceqq_u16(high_src_blk, high_tgt_blk);
            if (unlikely(vmaxvq_u16(high_res_blk)))
            {
                const uint16x8_t low_src_blk = vld1q_u16(buf + 1);
                const uint16x8_t low_res_blk = vceqq_u16(low_src_blk, low_tgt_blk);
                if (unlikely(vmaxvq_u16(low_res_blk)))
                {
                    const uint16x8_t res_blk = vandq_u16(high_res_blk, low_res_blk);
                    if (unlikely(vmaxvq_u16(res_blk)))
                        return (int32_t)(ptr - start) + (ctz64(to_bitmask(res_blk)) >> 3);
                }
            }
        }
    }
    return INT64_C(0x100000000);
}

#else

always_inline uint64_t to_bitmask(const uint64_t words)
{
    return (~words - 0x0001000100010001ULL) & words & 0x8000800080008000ULL;
}

always_inline uint64_t splat(const uint16_t word)
{
    return (uint64_t)word * 0x0001000100010001ULL;
}

export int64_t find_char(const struct moonbit_view_t haystack, const int32_t needle)
{
    const int32_t len = haystack.len;
    if (unlikely(!len))
        return INT64_C(0x100000000);

    const uint16_t *const start = (uint16_t *)haystack.buf + haystack.start;
    const uint16_t *const end = start + len;
    const uint16_t *ptr = start;

    if (likely(needle <= 0xFFFF))
    {
        const uint16_t tgt = (uint16_t)needle;
        const uint64_t tgt_blk = splat(tgt);

        while (likely(ptr <= end - 4))
        {
            uint64_t src_blk;
            memcpy(&src_blk, ptr, sizeof(uint64_t));
            const uint64_t res_mask = to_bitmask(~(src_blk ^ tgt_blk));
            if (unlikely(res_mask))
                return (int32_t)(ptr - start) + (ctz64(res_mask) >> 4);
            ptr += 4;
        }

        while (unlikely(ptr < end))
        {
            if (unlikely(*ptr == tgt))
                return (int32_t)(ptr - start);
            ++ptr;
        }
    }
    else
    {
        if (unlikely(len < 2))
            return INT64_C(0x100000000);

        const int32_t adjusted = needle - 0x10000;
        const uint16_t high_tgt = 0xD800 + (adjusted >> 10);
        const uint16_t low_tgt = 0xDC00 + (adjusted & 0x3FF);
        const uint64_t high_tgt_blk = splat(high_tgt);
        const uint64_t low_tgt_blk = splat(low_tgt);

        while (likely(ptr <= end - 5))
        {
            uint64_t high_src_blk, low_src_blk;
            memcpy(&high_src_blk, ptr, sizeof(uint64_t));
            memcpy(&low_src_blk, ptr + 1, sizeof(uint64_t));
            const uint64_t res_mask = to_bitmask(~((high_src_blk ^ high_tgt_blk) | (low_src_blk ^ low_tgt_blk)));
            if (unlikely(res_mask))
                return (int32_t)(ptr - start) + (ctz64(res_mask) >> 4);
            ptr += 4;
        }

        while (unlikely(ptr < end - 1))
        {
            if (unlikely(*ptr == high_tgt && *(ptr + 1) == low_tgt))
                return (int32_t)(ptr - start);
            ++ptr;
        }
    }

    return INT64_C(0x100000000);
}

#endif
