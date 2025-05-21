#include "common.h"

avx2_inline int32_t match_unrolled(const uint16_t *haystack, const uint16_t *needle)
{
    u16x16 mask_block = u16x16__eq(u16x16__load(haystack), u16x16__load(needle));
    mask_block = u16x16__and(mask_block, u16x16__eq(u16x16__load(haystack + 16), u16x16__load(needle + 16)));
    mask_block = u16x16__and(mask_block, u16x16__eq(u16x16__load(haystack + 32), u16x16__load(needle + 32)));
    mask_block = u16x16__and(mask_block, u16x16__eq(u16x16__load(haystack + 48), u16x16__load(needle + 48)));
    return u16x16__bitmask(mask_block) == 0xFFFFFFFF;
}

avx2_inline int32_t match(const uint16_t *haystack, const uint16_t *needle)
{
    const u16x16 mask_block = u16x16__eq(u16x16__load(haystack), u16x16__load(needle));
    return u16x16__bitmask(mask_block) == 0xFFFFFFFF;
}

avx2_export int32_t avx2_has_suffix(const moonbit_view_t haystack, const moonbit_view_t needle)
{
    const int32_t haystack_len = haystack.len;
    const int32_t needle_len = needle.len;
    if (unlikely(haystack_len < needle_len))
        return false;

    const uint16_t *const haystack_data = (uint16_t *)haystack.buf + (haystack.start + haystack_len - needle_len);
    const uint16_t *const needle_data = (uint16_t *)needle.buf + needle.start;
    int32_t pos = 0;

    for (; pos <= needle_len - 64; pos += 64)
        if (unlikely(!match_unrolled(haystack_data + pos, needle_data + pos)))
            return false;

    for (; pos <= needle_len - 16; pos += 16)
        if (unlikely(!match(haystack_data + pos, needle_data + pos)))
            return false;

    for (; pos < needle_len; ++pos)
        if (unlikely(haystack_data[pos] != needle_data[pos]))
            return false;

    return true;
}