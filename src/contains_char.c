#include "common.h"

#if (defined(__x86_64__) || defined(_M_X64)) && (defined(__AVX512F__) && defined(__AVX512BW__))

export int32_t contains_char(const struct moonbit_view_t haystack, const uint32_t needle)
{
    const int32_t len = haystack.len;
    if (unlikely(len == 0))
        return 0;

    const uint16_t *ptr = (uint16_t *)haystack.buf + haystack.start;
    const uint16_t *end = ptr + len;

    if (likely(needle <= 0xFFFF))
    {
        const uint16_t tgt = (uint16_t)needle;
        const __m512i tgt_blk = _mm512_set1_epi16((int16_t)tgt);

        while (likely(ptr <= end - 32))
        {
            const __m512i src_blk = _mm512_loadu_si512((const __m512i *)ptr);
            if (unlikely(_mm512_cmpeq_epi16_mask(src_blk, tgt_blk)))
                return 1;
            ptr += 32;
        }

        if (likely(ptr < end))
        {
            uint16_t buf[32] = {0};
            memset(buf, !tgt, sizeof(uint16_t) * 32);
            memcpy(buf, ptr, sizeof(uint16_t) * (end - ptr));
            const __m512i src_blk = _mm512_loadu_si512((const __m512i *)buf);
            if (unlikely(_mm512_cmpeq_epi16_mask(src_blk, tgt_blk)))
                return 1;
        }
    }
    else
    {
        if (unlikely(len < 2))
            return 0;

        const uint16_t high_tgt = 0xD800 + ((needle - 0x10000) >> 10);
        const uint16_t low_tgt = 0xDC00 + ((needle - 0x10000) & 0x3FF);
        const __m512i high_tgt_blk = _mm512_set1_epi16((int16_t)high_tgt);
        const __m512i low_tgt_blk = _mm512_set1_epi16((int16_t)low_tgt);

        while (likely(ptr <= end - 33))
        {
            const __m512i high_src_blk = _mm512_loadu_si512((const __m512i *)ptr);
            const __mmask32 high_res_mask = _mm512_cmpeq_epi16_mask(high_src_blk, high_tgt_blk);
            if (unlikely(high_res_mask))
            {
                const __m512i low_src_blk = _mm512_loadu_si512((const __m512i *)(ptr + 1));
                const __mmask32 low_res_mask = _mm512_cmpeq_epi16_mask(low_src_blk, low_tgt_blk);
                if (unlikely(high_res_mask & low_res_mask))
                    return 1;
            }
            ptr += 32;
        }

        if (likely(ptr < end - 1))
        {
            uint16_t buf[33] = {0};
            memcpy(buf, ptr, sizeof(uint16_t) * (end - ptr));
            const __m512i high_src_blk = _mm512_loadu_si512((const __m512i *)buf);
            const __mmask32 high_res_mask = _mm512_cmpeq_epi16_mask(high_src_blk, high_tgt_blk);
            if (unlikely(high_res_mask))
            {
                const __m512i low_src_blk = _mm512_loadu_si512((const __m512i *)(buf + 1));
                const __mmask32 low_res_mask = _mm512_cmpeq_epi16_mask(low_src_blk, low_tgt_blk);
                if (unlikely(high_res_mask & low_res_mask))
                    return 1;
            }
        }
    }
    return 0;
}

#elif (defined(__x86_64__) || defined(_M_X64)) && defined(__AVX2__)

export int32_t contains_char(const struct moonbit_view_t haystack, const uint32_t needle)
{
    const int32_t len = haystack.len;
    if (unlikely(len == 0))
        return 0;

    const uint16_t *ptr = (uint16_t *)haystack.buf + haystack.start;
    const uint16_t *end = ptr + len;

    if (likely(needle <= 0xFFFF))
    {
        const uint16_t tgt = (uint16_t)needle;
        const __m256i tgt_blk = _mm256_set1_epi16((int16_t)tgt);

        while (likely(ptr <= end - 16))
        {
            const __m256i src_blk = _mm256_loadu_si256((const __m256i *)ptr);
            if (_mm256_movemask_epi8(_mm256_cmpeq_epi16(src_blk, tgt_blk)))
                return 1;
            ptr += 16;
        }

        if (unlikely(ptr < end))
        {
            uint16_t buf[16];
            memset(buf, !tgt, sizeof(uint16_t) * 16);
            memcpy(buf, ptr, sizeof(uint16_t) * (end - ptr));
            const __m256i src_blk = _mm256_loadu_si256((const __m256i *)buf);
            if (_mm256_movemask_epi8(_mm256_cmpeq_epi16(src_blk, tgt_blk)))
                return 1;
        }
    }
    else
    {
        if (unlikely(len < 2))
            return 0;
        const uint16_t high_tgt = 0xD800 + ((needle - 0x10000) >> 10);
        const uint16_t low_tgt = 0xDC00 + ((needle - 0x10000) & 0x3FF);
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
                if (unlikely(high_res_mask & low_res_mask))
                    return 1;
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
                if (unlikely(high_res_mask & low_res_mask))
                    return 1;
            }
        }
    }
    return 0;
}

#elif (defined(__x86_64__) || defined(_M_X64)) && defined(__SSE2__)

export int32_t contains_char(const struct moonbit_view_t haystack, const uint32_t needle)
{
    const int32_t len = haystack.len;
    if (unlikely(len == 0))
        return 0;

    const uint16_t *ptr = (uint16_t *)haystack.buf + haystack.start;
    const uint16_t *end = ptr + len;

    if (likely(needle <= 0xFFFF))
    {
        const uint16_t tgt = (uint16_t)needle;
        const __m128i tgt_blk = _mm_set1_epi16((int16_t)tgt);

        while (likely(ptr <= end - 8))
        {
            const __m128i src_blk = _mm_loadu_si128((const __m128i *)ptr);
            if (_mm_movemask_epi8(_mm_cmpeq_epi16(src_blk, tgt_blk)))
                return 1;
            ptr += 8;
        }

        if (unlikely(ptr < end))
        {
            uint16_t buf[8];
            memset(buf, !tgt, sizeof(uint16_t) * 8);
            memcpy(buf, ptr, sizeof(uint16_t) * (end - ptr));
            const __m128i src_blk = _mm_loadu_si128((const __m128i *)buf);
            if (_mm_movemask_epi8(_mm_cmpeq_epi16(src_blk, tgt_blk)))
                return 1;
        }
    }
    else
    {
        if (unlikely(len < 2))
            return 0;
        const uint16_t high_tgt = 0xD800 + ((needle - 0x10000) >> 10);
        const uint16_t low_tgt = 0xDC00 + ((needle - 0x10000) & 0x3FF);
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
                if (unlikely(high_res_mask & low_res_mask))
                    return 1;
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
                if (unlikely(high_res_mask & low_res_mask))
                    return 1;
            }
        }
    }
    return 0;
}

#elif (defined(__aarch64__) || defined(_M_ARM64)) && defined(__ARM_NEON)

export int32_t contains_char(const struct moonbit_view_t haystack, const uint32_t needle)
{
    const int32_t len = haystack.len;
    if (unlikely(len == 0))
        return 0;

    const uint16_t *ptr = (uint16_t *)haystack.buf + haystack.start;
    const uint16_t *end = ptr + len;

    if (likely(needle <= 0xFFFF))
    {
        const uint16_t tgt = (uint16_t)needle;
        const uint16x8_t tgt_blk = vdupq_n_u16(tgt);

        while (likely(ptr <= end - 8))
        {
            const uint16x8_t src_blk = vld1q_u16(ptr);
            if (unlikely(vmaxvq_u16(vceqq_u16(src_blk, tgt_blk))))
                return 1;
            ptr += 8;
        }

        if (unlikely(ptr < end))
        {
            uint16_t buf[8];
            memset(buf, !tgt, sizeof(uint16_t) * 8);
            memcpy(buf, ptr, sizeof(uint16_t) * (end - ptr));
            const uint16x8_t src_blk = vld1q_u16(buf);
            if (unlikely(vmaxvq_u16(vceqq_u16(src_blk, tgt_blk))))
                return 1;
        }
    }
    else
    {
        if (unlikely(len < 2))
            return 0;

        const uint16_t high_tgt = 0xD800 + ((needle - 0x10000) >> 10);
        const uint16_t low_tgt = 0xDC00 + ((needle - 0x10000) & 0x3FF);
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
                    if (unlikely(vmaxvq_u16(vandq_u16(high_res_blk, low_res_blk))))
                        return 1;
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
                    if (unlikely(vmaxvq_u16(vandq_u16(high_res_blk, low_res_blk))))
                        return 1;
            }
        }
    }
    return 0;
}

#else

always_inline int32_t has_zero_word(const uint64_t quadword)
{
    return ((quadword - 0x0001000100010001ULL) & ~(quadword) & 0x8000800080008000ULL) != 0;
}

always_inline uint64_t splat_word(const uint16_t word)
{
    return (uint64_t)word * 0x0001000100010001ULL;
}

export int32_t contains_char(const struct moonbit_view_t haystack, const uint32_t needle)
{
    const int32_t len = haystack.len;
    if (unlikely(len == 0))
        return 0;

    const uint16_t *ptr = (uint16_t *)haystack.buf + haystack.start;
    const uint16_t *end = ptr + len;

    if (likely(needle <= 0xFFFF))
    {
        const uint16_t tgt = (uint16_t)needle;
        const uint64_t tgt_blk = splat_word(tgt);

        while (likely(ptr <= end - 4))
        {
            uint64_t src_blk;
            memcpy(&src_blk, ptr, sizeof(uint64_t));
            if (unlikely(has_zero_word(src_blk ^ tgt_blk)))
                return 1;
            ptr += 4;
        }

        while (unlikely(ptr < end))
        {
            if (unlikely(ptr[0] == tgt))
                return 1;
            ++ptr;
        }
    }
    else
    {
        if (unlikely(len < 2))
            return 0;

        const uint16_t high_tgt = 0xD800 + ((needle - 0x10000) >> 10);
        const uint16_t low_tgt = 0xDC00 + ((needle - 0x10000) & 0x3FF);
        const uint64_t high_tgt_blk = splat_word(high_tgt);
        const uint64_t low_tgt_blk = splat_word(low_tgt);

        while (likely(ptr <= end - 5))
        {
            uint64_t high_src_blk, low_src_blk;
            memcpy(&high_src_blk, ptr, sizeof(uint64_t));
            memcpy(&low_src_blk, ptr + 1, sizeof(uint64_t));
            if (unlikely(has_zero_word((high_src_blk ^ high_tgt_blk) | (low_src_blk ^ low_tgt_blk))))
                return 1;
            ptr += 4;
        }

        while (unlikely(ptr < end - 1))
        {
            if (unlikely(ptr[0] == high_tgt && ptr[1] == low_tgt))
                return 1;
            ++ptr;
        }
    }

    return 0;
}

#endif
