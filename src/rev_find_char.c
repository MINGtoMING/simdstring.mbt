#include "common.h"

#if defined(USE_AVX512)

export int64_t rev_find_char(const struct moonbit_view_t haystack, const int32_t needle)
{
    const int32_t len = haystack.len;
    if (unlikely(!len))
        return INT64_C(0x100000000);

    const uint16_t *const start = (uint16_t *)haystack.buf + haystack.start;
    const uint16_t *const end = start + len;
    const uint16_t *ptr = end;

    if (likely(needle <= 0xFFFF))
    {
        const uint16_t tgt = (uint16_t)needle;
        const __m512i tgt_blk = _mm512_set1_epi16((int16_t)tgt);

        while (likely(ptr >= start + 32))
        {
            ptr -= 32;
            const __m512i src_blk = _mm512_loadu_si512((const __m512i *)ptr);
            const uint32_t res_mask = _mm512_cmpeq_epi16_mask(src_blk, tgt_blk);
            if (unlikely(res_mask))
                return (int32_t)(ptr - start) + ctz32(res_mask);
        }

        if (likely(ptr > start))
        {
            uint16_t buf[32] = {0};
            memset(buf, !tgt, sizeof(uint16_t) * 32);
            memcpy(buf, start, sizeof(uint16_t) * (ptr - start));
            const __m512i src_blk = _mm512_loadu_si512((const __m512i *)buf);
            const uint32_t res_mask = _mm512_cmpeq_epi16_mask(src_blk, tgt_blk);
            if (unlikely(res_mask))
                return ctz32(res_mask);
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

        --ptr;
        while (likely(ptr >= start + 32))
        {
            ptr -= 32;
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
        }

        if (likely(ptr > start))
        {
            uint16_t buf[33] = {0};
            memcpy(buf, start, sizeof(uint16_t) * (ptr - start + 1));
            const __m512i high_src_blk = _mm512_loadu_si512((const __m512i *)buf);
            const uint32_t high_res_mask = _mm512_cmpeq_epi16_mask(high_src_blk, high_tgt_blk);
            if (unlikely(high_res_mask))
            {
                const __m512i low_src_blk = _mm512_loadu_si512((const __m512i *)(buf + 1));
                const uint32_t low_res_mask = _mm512_cmpeq_epi16_mask(low_src_blk, low_tgt_blk);
                const uint32_t res_mask = high_res_mask & low_res_mask;
                if (unlikely(res_mask))
                    return ctz32(res_mask);
            }
        }
    }
    return INT64_C(0x100000000);
}

#elif defined(USE_AVX2)

export int64_t rev_find_char(const struct moonbit_view_t haystack, const int32_t needle)
{
    const int32_t len = haystack.len;
    if (unlikely(!len))
        return INT64_C(0x100000000);

    const uint16_t *const start = (uint16_t *)haystack.buf + haystack.start;
    const uint16_t *const end = start + len;
    const uint16_t *ptr = end;

    if (likely(needle <= 0xFFFF))
    {
        const uint16_t tgt = (uint16_t)needle;
        const __m256i tgt_blk = _mm256_set1_epi16((int16_t)tgt);

        while (likely(ptr >= start + 16))
        {
            ptr -= 16;
            const __m256i src_blk = _mm256_loadu_si256((const __m256i *)ptr);
            const uint32_t res_mask = _mm256_movemask_epi8(_mm256_cmpeq_epi16(src_blk, tgt_blk));
            if (unlikely(res_mask))
                return (int32_t)(ptr - start) + (ctz32(res_mask) >> 1);
        }

        if (unlikely(ptr > start))
        {
            uint16_t buf[16];
            memset(buf, !tgt, sizeof(uint16_t) * 16);
            memcpy(buf, start, sizeof(uint16_t) * (ptr - start));
            const __m256i src_blk = _mm256_loadu_si256((const __m256i *)buf);
            const uint32_t res_mask = _mm256_movemask_epi8(_mm256_cmpeq_epi16(src_blk, tgt_blk));
            if (unlikely(res_mask))
                return ctz32(res_mask) >> 1;
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

        --ptr;
        while (likely(ptr >= start + 16))
        {
            ptr -= 16;
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
        }

        if (unlikely(ptr > start))
        {
            uint16_t buf[17] = {0};
            memcpy(buf, start, sizeof(uint16_t) * (ptr - start + 1));
            const __m256i high_src_blk = _mm256_loadu_si256((const __m256i *)buf);
            const uint32_t high_res_mask = _mm256_movemask_epi8(_mm256_cmpeq_epi16(high_src_blk, high_tgt_blk));
            if (unlikely(high_res_mask))
            {
                const __m256i low_src_blk = _mm256_loadu_si256((const __m256i *)(buf + 1));
                const uint32_t low_res_mask = _mm256_movemask_epi8(_mm256_cmpeq_epi16(low_src_blk, low_tgt_blk));
                const uint32_t res_mask = high_res_mask & low_res_mask;
                if (unlikely(res_mask))
                    return ctz32(res_mask) >> 1;
            }
        }
    }
    return INT64_C(0x100000000);
}

#elif defined(USE_SSE2)

export int64_t rev_find_char(const struct moonbit_view_t haystack, const int32_t needle)
{
    const int32_t len = haystack.len;
    if (unlikely(!len))
        return INT64_C(0x100000000);

    const uint16_t *const start = (uint16_t *)haystack.buf + haystack.start;
    const uint16_t *const end = start + len;
    const uint16_t *ptr = end;

    if (likely(needle <= 0xFFFF))
    {
        const uint16_t tgt = (uint16_t)needle;
        const __m128i tgt_blk = _mm_set1_epi16((int16_t)tgt);

        while (likely(ptr >= start + 8))
        {
            ptr -= 8;
            const __m128i src_blk = _mm_loadu_si128((const __m128i *)ptr);
            const uint32_t res_mask = _mm_movemask_epi8(_mm_cmpeq_epi16(src_blk, tgt_blk));
            if (unlikely(res_mask))
                return (int32_t)(ptr - start) + (ctz32(res_mask) >> 1);
        }

        if (unlikely(ptr > start))
        {
            uint16_t buf[8];
            memset(buf, !tgt, sizeof(uint16_t) * 8);
            memcpy(buf, start, sizeof(uint16_t) * (ptr - start));
            const __m128i src_blk = _mm_loadu_si128((const __m128i *)buf);
            const uint32_t res_mask = _mm_movemask_epi8(_mm_cmpeq_epi16(src_blk, tgt_blk));
            if (unlikely(res_mask))
                return ctz32(res_mask) >> 1;
        }
    }
    else
    {
        if (unlikely(len < 2))
            return INT64_C(0x100000000);

        const uint16_t high_tgt = 0xD800 + ((needle - 0x10000) >> 10);
        const uint16_t low_tgt = 0xDC00 + ((needle - 0x10000) & 0x3FF);
        const __m128i high_tgt_blk = _mm_set1_epi16((int16_t)high_tgt);
        const __m128i low_tgt_blk = _mm_set1_epi16((int16_t)low_tgt);

        --ptr;
        while (likely(ptr >= start + 8))
        {
            ptr -= 8;
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
        }

        if (unlikely(ptr > start))
        {
            uint16_t buf[9] = {0};
            memcpy(buf, start, sizeof(uint16_t) * (ptr - start + 1));
            const __m128i high_src_blk = _mm_loadu_si128((const __m128i *)buf);
            const uint32_t high_res_mask = _mm_movemask_epi8(_mm_cmpeq_epi16(high_src_blk, high_tgt_blk));
            if (unlikely(high_res_mask))
            {
                const __m128i low_src_blk = _mm_loadu_si128((const __m128i *)(buf + 1));
                const uint32_t low_res_mask = _mm_movemask_epi8(_mm_cmpeq_epi16(low_src_blk, low_tgt_blk));
                const uint32_t res_mask = high_res_mask & low_res_mask;
                if (unlikely(res_mask))
                    return ctz32(res_mask) >> 1;
            }
        }
    }
    return INT64_C(0x100000000);
}

#elif defined(USE_NEON)

always_inline uint64_t to_bitmask(const uint16x8_t words)
{
    return (uint64_t)vshrn_n_u16(words, 8) & UINT64_C(0x8080808080808080);
}

export int64_t rev_find_char(const struct moonbit_view_t haystack, const int32_t needle)
{
    const int32_t len = haystack.len;
    if (unlikely(!len))
        return INT64_C(0x100000000);

    const uint16_t *const start = (uint16_t *)haystack.buf + haystack.start;
    const uint16_t *const end = start + len;
    const uint16_t *ptr = end;

    if (likely(needle <= 0xFFFF))
    {
        const uint16_t tgt = (uint16_t)needle;
        const uint16x8_t tgt_blk = vdupq_n_u16(tgt);

        while (likely(ptr >= start + 8))
        {
            ptr -= 8;
            const uint16x8_t src_blk = vld1q_u16(ptr);
            const uint16x8_t res_blk = vceqq_u16(src_blk, tgt_blk);
            if (unlikely(vmaxvq_u16(res_blk)))
                return (int32_t)(ptr - start) + (ctz64(to_bitmask(res_blk)) >> 3);
        }

        if (unlikely(ptr > start))
        {
            uint16_t buf[8];
            memset(buf, !tgt, sizeof(uint16_t) * 8);
            memcpy(buf, start, sizeof(uint16_t) * (ptr - start));
            const uint16x8_t src_blk = vld1q_u16(buf);
            const uint16x8_t res_blk = vceqq_u16(src_blk, tgt_blk);
            if (unlikely(vmaxvq_u16(res_blk)))
                return ctz64(to_bitmask(res_blk)) >> 3;
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

        --ptr;
        while (likely(ptr >= start + 8))
        {
            ptr -= 8;
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
        }

        if (unlikely(ptr > start))
        {
            uint16_t buf[9] = {0};
            memcpy(buf, start, sizeof(uint16_t) * (ptr - start + 1));
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
                        return ctz64(to_bitmask(res_blk)) >> 3;
                }
            }
        }
    }
    return INT64_C(0x100000000);
}

#elif defined(USE_RVV)

vuint16m8_t __riscv_vle16_v_u16m8(const uint16_t *rs1, size_t vl);

vbool2_t __riscv_vmseq_vx_u16m8_b2(vuint16m8_t vs2, uint16_t rs1, size_t vl);

long __riscv_vfirst_m_b2(vbool2_t vs2, size_t vl);

vbool2_t __riscv_vmand_mm_b2(vbool2_t vs2, vbool2_t vs1, size_t vl);

int64_t rev_find_char(const struct moonbit_view_t haystack, const int32_t needle)
{
    const int32_t len = haystack.len;
    if (unlikely(!len))
        return INT64_C(0x100000000);

    const uint16_t *const start = (uint16_t *)haystack.buf + haystack.start;
    const uint16_t *const end = start + len;
    const uint16_t *ptr = end;

    if (likely(needle <= 0xFFFF))
    {
        const uint16_t tgt = (uint16_t)needle;
        while (likely(ptr > start))
        {
            const size_t vec_len = __riscv_vsetvl_e16m8(ptr - start);
            ptr -= vec_len;
            const vuint16m8_t src_blk = __riscv_vle16_v_u16m8(ptr, vec_len);
            const vbool2_t res_blk = __riscv_vmseq_vx_u16m8_b2(src_blk, tgt, vec_len);
            const int64_t res_offset = __riscv_vfirst_m_b2(res_blk, vec_len);
            if (unlikely(res_offset >= 0))
                return (int32_t)(ptr - start) + res_offset;
        }
    }
    else
    {
        if (unlikely(len < 2))
            return INT64_C(0x100000000);

        const int32_t adjusted = needle - 0x10000;
        const uint16_t high_tgt = 0xD800 + (adjusted >> 10);
        const uint16_t low_tgt = 0xDC00 + (adjusted & 0x3FF);
        --ptr;
        while (likely(ptr > start))
        {
            const size_t vec_len = __riscv_vsetvl_e16m8(ptr - start);
            ptr -= vec_len;
            const vuint16m8_t high_src_blk = __riscv_vle16_v_u16m8(ptr, vec_len);
            const vbool2_t high_res_mask = __riscv_vmseq_vx_u16m8_b2(high_src_blk, high_tgt, vec_len);
            if (unlikely(__riscv_vfirst_m_b2(high_res_mask, vec_len) >= 0))
            {
                const vuint16m8_t low_src_blk = __riscv_vle16_v_u16m8(ptr + 1, vec_len);
                const vbool2_t low_res_mask = __riscv_vmseq_vx_u16m8_b2(low_src_blk, low_tgt, vec_len);
                const vbool2_t res_mask = __riscv_vmand_mm_b2(high_res_mask, low_res_mask, vec_len);
                const int64_t res_offset = __riscv_vfirst_m_b2(res_mask, vec_len);
                if (unlikely(res_offset >= 0))
                    return (int32_t)(ptr - start) + res_offset;
            }
        }
    }

    return INT64_C(0x100000000);
}

#else

always_inline uint64_t to_bitmask(const uint64_t words)
{
    return (~words - UINT64_C(0x0001000100010001)) & words & UINT64_C(0x8000800080008000);
}

always_inline uint64_t splat(const uint16_t word)
{
    return (uint64_t)word * UINT64_C(0x0001000100010001);
}

export int64_t rev_find_char(const struct moonbit_view_t haystack, const int32_t needle)
{
    const int32_t len = haystack.len;
    if (unlikely(!len))
        return INT64_C(0x100000000);

    const uint16_t *const start = (uint16_t *)haystack.buf + haystack.start;
    const uint16_t *const end = start + len;
    const uint16_t *ptr = end;

    if (likely(needle <= 0xFFFF))
    {
        const uint16_t tgt = (uint16_t)needle;
        const uint64_t tgt_blk = splat(tgt);

        while (likely(ptr >= start + 4))
        {
            ptr -= 4;
            uint64_t src_blk;
            memcpy(&src_blk, ptr, sizeof(uint64_t));
            const uint64_t res_mask = to_bitmask(~(src_blk ^ tgt_blk));
            if (unlikely(res_mask))
                return (int32_t)(ptr - start) + (ctz64(res_mask) >> 4);
        }

        while (unlikely(ptr > start))
        {
            --ptr;
            if (unlikely(*ptr == tgt))
                return (int32_t)(ptr - start);
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

        --ptr;
        while (likely(ptr >= start + 4))
        {
            ptr -= 4;
            uint64_t high_src_blk, low_src_blk;
            memcpy(&high_src_blk, ptr, sizeof(uint64_t));
            memcpy(&low_src_blk, ptr + 1, sizeof(uint64_t));
            const uint64_t res_mask = to_bitmask(~((high_src_blk ^ high_tgt_blk) | (low_src_blk ^ low_tgt_blk)));
            if (unlikely(res_mask))
                return (int32_t)(ptr - start) + (ctz64(res_mask) >> 4);
        }

        while (unlikely(ptr > start))
        {
            --ptr;
            if (unlikely(*ptr == high_tgt && *(ptr + 1) == low_tgt))
                return (int32_t)(ptr - start);
        }
    }

    return INT64_C(0x100000000);
}

#endif
