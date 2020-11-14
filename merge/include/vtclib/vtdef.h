#ifndef VTDEF_H_INCLUDED
#define VTDEF_H_INCLUDED



#ifndef ARMKEIL_ENV
    #include <sys/cdefs.h>
    #include <sys/types.h>
#endif

#include <stddef.h>
#include <stdint.h>

#if defined(_WIN32)
    #define VT_FILEPATH_SEPERATOR_CHR       '\\'
    #define VT_FILEPATH_SEPERATOR_CSTR      "\\"
    #ifndef __WORDSIZE
        #if defined(_WIN64)
            #define __WORDSIZE              64
        #else
            #define __WORDSIZE              32
        #endif
    #endif
#else
    #define VT_FILEPATH_SEPERATOR_CHR       '/'
    #define VT_FILEPATH_SEPERATOR_CSTR      "/"
    #ifndef O_BINARY
        #define O_BINARY                    0
    #endif
    #ifndef O_TEXT
        #define O_TEXT                      0
    #endif
#endif

#ifdef ARMKEIL_ENV
    #ifdef  __cplusplus
        #define __BEGIN_DECLS  extern "C" {
        #define __END_DECLS    }
    #else
        #define __BEGIN_DECLS
        #define __END_DECLS
    #endif
    #if __sizeof_ptr == 8
        typedef int64_t ssize_t;
        typedef int64_t off_t;
    #else
        typedef int32_t ssize_t;
        typedef int32_t off_t;
    #endif
#endif

#define VT_UNUSED(x)    (void)(x)
#define VT_NULL_CSTR    ((char*)NULL)
#define VT_NULL_PTR     ((void*)NULL)

#include "vtclib/vtffn.h"

#endif // VTDEF_H_INCLUDED
