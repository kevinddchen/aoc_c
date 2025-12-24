#include <assert.h>
#include <errno.h>
#include <stdlib.h>

long util_atol(const char* str)
{
    errno = 0;  // reset
    char* str_end = {};
    const long num = strtol(str, &str_end, 0);
    assert(str_end != str);   // check conversion performned
    assert(errno != ERANGE);  // check out-of-range error
    return num;
}
