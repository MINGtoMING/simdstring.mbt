#include "common.h"

avx2_inline int32_t contains_bmp_needle_unrolled(const uint16_t *const haystack, const u16x16 bmp_needle_block)
{
    _mm_prefetch((const char *)(haystack + 320), _MM_HINT_T0);
    u16x16 mask_block = u16x16__eq(u16x16__load(haystack), bmp_needle_block);
    mask_block = u16x16__or(mask_block, u16x16__eq(u16x16__load(haystack + 16), bmp_needle_block));
    mask_block = u16x16__or(mask_block, u16x16__eq(u16x16__load(haystack + 32), bmp_needle_block));
    mask_block = u16x16__or(mask_block, u16x16__eq(u16x16__load(haystack + 48), bmp_needle_block));
    return u16x16__bitmask(mask_block) != 0;
}

avx2_inline int32_t contains_bmp_needle(const uint16_t *const haystack, const u16x16 bmp_needle_block)
{
    const u16x16 mask_block = u16x16__eq(u16x16__load(haystack), bmp_needle_block);
    return u16x16__bitmask(mask_block) != 0;
}

avx2_inline int32_t contains_surrogate_needle_unrolled(const uint16_t *const haystack, const u16x16 high_needle_block,
                                                       const u16x16 low_needle_block)
{
    const u16x16 high_mask_block1 = u16x16__eq(u16x16__load(haystack), high_needle_block);
    const u16x16 low_mask_block1 = u16x16__eq(u16x16__load(haystack + 1), low_needle_block);
    const u16x16 high_mask_block2 = u16x16__eq(u16x16__load(haystack + 16), high_needle_block);
    const u16x16 low_mask_block2 = u16x16__eq(u16x16__load(haystack + 17), low_needle_block);
    const u16x16 high_mask_block3 = u16x16__eq(u16x16__load(haystack + 32), high_needle_block);
    const u16x16 low_mask_block3 = u16x16__eq(u16x16__load(haystack + 33), low_needle_block);
    const u16x16 high_mask_block4 = u16x16__eq(u16x16__load(haystack + 48), high_needle_block);
    const u16x16 low_mask_block4 = u16x16__eq(u16x16__load(haystack + 49), low_needle_block);
    u16x16 mask_block = u16x16__and(high_mask_block1, low_mask_block1);
    mask_block = u16x16__or(mask_block, u16x16__and(high_mask_block2, low_mask_block2));
    mask_block = u16x16__or(mask_block, u16x16__and(high_mask_block3, low_mask_block3));
    mask_block = u16x16__or(mask_block, u16x16__and(high_mask_block4, low_mask_block4));
    return u16x16__bitmask(mask_block) != 0;
}

avx2_inline int32_t contains_surrogate_needle(const uint16_t *const haystack, const u16x16 high_needle_block,
                                              const u16x16 low_needle_block)
{
    const u16x16 high_mask_block = u16x16__eq(u16x16__load(haystack), high_needle_block);
    const u16x16 low_mask_block = u16x16__eq(u16x16__load(haystack + 1), low_needle_block);
    const u16x16 mask_block = u16x16__and(high_mask_block, low_mask_block);
    return u16x16__bitmask(mask_block) != 0;
}

avx2_export int32_t avx2_contains_char(const moonbit_view_t str, const int32_t cp)
{
    const int32_t len = str.len;
    if (unlikely(len == 0))
        return false;

    const uint16_t *const haystack = str.buf + str.start;
    int32_t pos = 0;

    if (likely(cp <= 0xFFFF))
    {
        const uint16_t bmp_needle = (uint16_t)cp;
        const u16x16 bmp_needle_block = u16x16__splat(bmp_needle);

        for (; pos <= len - 64; pos += 64)
            if (unlikely(contains_bmp_needle_unrolled(haystack + pos, bmp_needle_block)))
                return true;

        for (; pos <= len - 16; pos += 16)
            if (unlikely(contains_bmp_needle(haystack + pos, bmp_needle_block)))
                return true;

        for (; pos < len; ++pos)
            if (haystack[pos] == bmp_needle)
                return true;
    }
    else
    {
        if (unlikely(len < 2))
            return false;

        uint16_t const high_needle = 0xD800 + ((cp - 0x10000) >> 10);
        uint16_t const low_needle = 0xDC00 + ((cp - 0x10000) & 0x3FF);
        u16x16 const high_needle_block = u16x16__splat(high_needle);
        u16x16 const low_needle_block = u16x16__splat(low_needle);

        for (; pos <= len - 65; pos += 64)
            if (unlikely(contains_surrogate_needle_unrolled(haystack + pos, high_needle_block, low_needle_block)))
                return true;

        for (; pos <= len - 17; pos += 16)
            if (unlikely(contains_surrogate_needle(haystack + pos, high_needle_block, low_needle_block)))
                return true;

        for (; pos < len - 1; ++pos)
            if (unlikely(haystack[pos] == high_needle && haystack[pos + 1] == low_needle))
                return true;
    }

    return false;
}
