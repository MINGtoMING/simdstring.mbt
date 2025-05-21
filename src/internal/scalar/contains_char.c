#include "common.h"

scalar_export int32_t scalar_contains_char(const moonbit_view_t str, const int32_t cp)
{
    const int32_t len = str.len;
    if (unlikely(len == 0))
        return false;

    const uint16_t *const haystack = str.buf + str.start;
    int32_t pos = 0;

    if (likely(cp <= 0xFFFF))
    {
        const uint16_t bmp_needle = (uint16_t)cp;
        for (; pos < len; ++pos)
            if (unlikely(haystack[pos] == bmp_needle))
                return true;
    }
    else
    {
        const uint16_t high_needle = 0xD800 + ((cp - 0x10000) >> 10);
        const uint16_t low_needle = 0xDC00 + ((cp - 0x10000) & 0x3FF);
        for (; pos < len - 1; ++pos)
            if (unlikely(haystack[pos] == high_needle && haystack[pos + 1] == low_needle))
                return true;
    }

    return false;
}