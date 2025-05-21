#include "common.h"

scalar_export int32_t scalar_has_prefix(const moonbit_view_t haystack, const moonbit_view_t needle)
{
    const int32_t haystack_len = haystack.len;
    const int32_t needle_len = needle.len;
    if (unlikely(haystack_len < needle_len))
        return false;

    const uint16_t *haystack_data = haystack.buf + haystack.start;
    const uint16_t *needle_data = needle.buf + needle.start;
    for (int32_t pos = 0; pos < needle_len; ++pos)
        if (*haystack_data++ != *needle_data++)
            return false;

    return true;
}