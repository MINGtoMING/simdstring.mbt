#ifndef COMMON_H
#define COMMON_H

#include "moonbit.h"

typedef struct moonbit_view_t moonbit_view_t;

#define likely(x) __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)

#define true 1
#define false 0

#define scalar_export MOONBIT_EXPORT

#endif // COMMON_H
