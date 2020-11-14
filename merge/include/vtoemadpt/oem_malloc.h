#ifndef _VTARMSRC_MYMALLOC_H_
#define _VTARMSRC_MYMALLOC_H_

#include "vtclib/vtdef.h"

__BEGIN_DECLS

int   oem_memInit(void);
void  oem_free(void* ptr);
void* oem_malloc(const size_t size);
void* oem_realloc(void* ptr, const size_t size);

__END_DECLS

#endif // _ARMCLIB_MYMALLOC_H_
