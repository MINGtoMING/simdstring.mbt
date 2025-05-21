#include "common.h"

sse4_2_inline int32_t match_unrolled(const uint16_t *haystack, const uint16_t *needle)
{
    u16x8 mask_block = u16x8__eq(u16x8__load(haystack), u16x8__load(needle));
    mask_block = u16x8__and(mask_block, u16x8__eq(u16x8__load(haystack + 8), u16x8__load(needle + 8)));
    mask_block = u16x8__and(mask_block, u16x8__eq(u16x8__load(haystack + 16), u16x8__load(needle + 16)));
    mask_block = u16x8__and(mask_block, u16x8__eq(u16x8__load(haystack + 24), u16x8__load(needle + 24)));
    mask_block = u16x8__and(mask_block, u16x8__eq(u16x8__load(haystack + 32), u16x8__load(needle + 32)));
    mask_block = u16x8__and(mask_block, u16x8__eq(u16x8__load(haystack + 40), u16x8__load(needle + 40)));
    mask_block = u16x8__and(mask_block, u16x8__eq(u16x8__load(haystack + 48), u16x8__load(needle + 48)));
    mask_block = u16x8__and(mask_block, u16x8__eq(u16x8__load(haystack + 56), u16x8__load(needle + 56)));
    return u16x8__bitmask(mask_block) == 0xFFFF;
}

sse4_2_inline int32_t match(const uint16_t *haystack, const uint16_t *needle)
{
    const u16x8 mask_block = u16x8__eq(u16x8__load(haystack), u16x8__load(needle));
    return u16x8__bitmask(mask_block) == 0xFFFF;
}

sse4_2_export int32_t sse4_2_has_prefix(const moonbit_view_t haystack, const moonbit_view_t needle)
{
    const int32_t haystack_len = haystack.len;
    const int32_t needle_len = needle.len;
    if (unlikely(haystack_len < needle_len))
        return false;

    const uint16_t *haystack_data = haystack.buf + haystack.start;
    const uint16_t *needle_data = needle.buf + needle.start;
    int32_t pos = 0;

    for (; pos <= needle_len - 64; pos += 64)
        if (unlikely(!match_unrolled(haystack_data + pos, needle_data + pos)))
            return false;

    for (; pos <= needle_len - 8; pos += 8)
        if (unlikely(!match(haystack_data + pos, needle_data + pos)))
            return false;

    for (; pos < needle_len; ++pos)
        if (unlikely(haystack_data[pos] != needle_data[pos]))
            return false;

    return true;
}