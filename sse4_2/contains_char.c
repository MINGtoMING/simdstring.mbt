#include "common.h"

sse4_2_inline int32_t contains_bmp_needle_unrolled(const uint16_t *const haystack, const u16x8 bmp_needle_block)
{
    u16x8 mask_block = u16x8__eq(u16x8__load(haystack), bmp_needle_block);
    mask_block = u16x8__or(mask_block, u16x8__eq(u16x8__load(haystack + 8), bmp_needle_block));
    mask_block = u16x8__or(mask_block, u16x8__eq(u16x8__load(haystack + 16), bmp_needle_block));
    mask_block = u16x8__or(mask_block, u16x8__eq(u16x8__load(haystack + 24), bmp_needle_block));
    mask_block = u16x8__or(mask_block, u16x8__eq(u16x8__load(haystack + 32), bmp_needle_block));
    mask_block = u16x8__or(mask_block, u16x8__eq(u16x8__load(haystack + 40), bmp_needle_block));
    mask_block = u16x8__or(mask_block, u16x8__eq(u16x8__load(haystack + 48), bmp_needle_block));
    mask_block = u16x8__or(mask_block, u16x8__eq(u16x8__load(haystack + 56), bmp_needle_block));
    return u16x8__bitmask(mask_block) != 0;
}

sse4_2_inline int32_t contains_bmp_needle(const uint16_t *const haystack, const u16x8 bmp_needle_block)
{
    const u16x8 mask_block = u16x8__eq(u16x8__load(haystack), bmp_needle_block);
    return u16x8__bitmask(mask_block) != 0;
}

sse4_2_inline int32_t contains_surrogate_needle_unrolled(const uint16_t *const haystack, const u16x8 high_needle_block,
                                                         const u16x8 low_needle_block)
{
    const u16x8 high_mask_block1 = u16x8__eq(u16x8__load(haystack), high_needle_block);
    const u16x8 low_mask_block1 = u16x8__eq(u16x8__load(haystack + 1), low_needle_block);
    const u16x8 high_mask_block2 = u16x8__eq(u16x8__load(haystack + 8), high_needle_block);
    const u16x8 low_mask_block2 = u16x8__eq(u16x8__load(haystack + 9), low_needle_block);
    const u16x8 high_mask_block3 = u16x8__eq(u16x8__load(haystack + 16), high_needle_block);
    const u16x8 low_mask_block3 = u16x8__eq(u16x8__load(haystack + 17), low_needle_block);
    const u16x8 high_mask_block4 = u16x8__eq(u16x8__load(haystack + 24), high_needle_block);
    const u16x8 low_mask_block4 = u16x8__eq(u16x8__load(haystack + 25), low_needle_block);
    const u16x8 high_mask_block5 = u16x8__eq(u16x8__load(haystack + 32), high_needle_block);
    const u16x8 low_mask_block5 = u16x8__eq(u16x8__load(haystack + 33), low_needle_block);
    const u16x8 high_mask_block6 = u16x8__eq(u16x8__load(haystack + 40), high_needle_block);
    const u16x8 low_mask_block6 = u16x8__eq(u16x8__load(haystack + 41), low_needle_block);
    const u16x8 high_mask_block7 = u16x8__eq(u16x8__load(haystack + 48), high_needle_block);
    const u16x8 low_mask_block7 = u16x8__eq(u16x8__load(haystack + 49), low_needle_block);
    const u16x8 high_mask_block8 = u16x8__eq(u16x8__load(haystack + 56), high_needle_block);
    const u16x8 low_mask_block8 = u16x8__eq(u16x8__load(haystack + 57), low_needle_block);

    u16x8 mask_block = u16x8__and(high_mask_block1, low_mask_block1);
    mask_block = u16x8__or(mask_block, u16x8__and(high_mask_block2, low_mask_block2));
    mask_block = u16x8__or(mask_block, u16x8__and(high_mask_block3, low_mask_block3));
    mask_block = u16x8__or(mask_block, u16x8__and(high_mask_block4, low_mask_block4));
    mask_block = u16x8__or(mask_block, u16x8__and(high_mask_block5, low_mask_block5));
    mask_block = u16x8__or(mask_block, u16x8__and(high_mask_block6, low_mask_block6));
    mask_block = u16x8__or(mask_block, u16x8__and(high_mask_block7, low_mask_block7));
    mask_block = u16x8__or(mask_block, u16x8__and(high_mask_block8, low_mask_block8));

    return u16x8__bitmask(mask_block) != 0;
}

sse4_2_inline int32_t contains_surrogate_needle(const uint16_t *const haystack, const u16x8 high_needle_block,
                                                const u16x8 low_needle_block)
{
    const u16x8 high_mask_block = u16x8__eq(u16x8__load(haystack), high_needle_block);
    const u16x8 low_mask_block = u16x8__eq(u16x8__load(haystack + 1), low_needle_block);
    const u16x8 mask_block = u16x8__and(high_mask_block, low_mask_block);
    return u16x8__bitmask(mask_block) != 0;
}

sse4_2_export int32_t sse4_2_contains_char(const moonbit_view_t str, const int32_t cp)
{
    const int32_t len = str.len;
    if (unlikely(len == 0))
        return false;

    const uint16_t *const haystack = str.buf + str.start;
    int32_t pos = 0;

    if (likely(cp <= 0xFFFF))
    {
        const uint16_t bmp_needle = (uint16_t)cp;
        const u16x8 bmp_needle_block = u16x8__splat(bmp_needle);

        for (; pos <= len - 64; pos += 64)
            if (unlikely(contains_bmp_needle_unrolled(haystack + pos, bmp_needle_block)))
                return true;

        for (; pos <= len - 8; pos += 8)
            if (unlikely(contains_bmp_needle(haystack + pos, bmp_needle_block)))
                return true;

        for (; pos < len; ++pos)
            if (unlikely(haystack[pos] == bmp_needle))
                return true;
    }
    else
    {
        if (unlikely(len < 2))
            return false;

        const uint16_t high_needle = 0xD800 + ((cp - 0x10000) >> 10);
        const uint16_t low_needle = 0xDC00 + ((cp - 0x10000) & 0x3FF);
        const u16x8 high_needle_block = u16x8__splat(high_needle);
        const u16x8 low_needle_block = u16x8__splat(low_needle);

        for (; pos <= len - 65; pos += 64)
            if (unlikely(contains_surrogate_needle_unrolled(haystack + pos, high_needle_block, low_needle_block)))
                return true;

        for (; pos <= len - 9; pos += 8)
            if (unlikely(contains_surrogate_needle(haystack + pos, high_needle_block, low_needle_block)))
                return true;

        for (; pos < len - 1; ++pos)
            if (unlikely(haystack[pos] == high_needle && haystack[pos + 1] == low_needle))
                return true;
    }

    return false;
}