#ifndef VTSTRUTIL_H_INCLUDED
#define VTSTRUTIL_H_INCLUDED

/**  2016.02 Dehua Tan **/

#include "vtsys/vtdef.h"

__BEGIN_DECLS

/**
 * return -1 if str_buf_size is not enough.
 * return 0, no ${from} found in str
 * return num > 0, the ${num} of replacing times.
 *
 * only scan once, so no dead-loop within this function.
**/
int vt_str_replace(char* str, const size_t str_buf_size, const char* from, const char* to);

__END_DECLS


#endif // VTSTRUTIL_H_INCLUDED
