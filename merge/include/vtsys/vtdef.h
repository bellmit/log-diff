#ifndef VTDEF_H_INCLUDED
#define VTDEF_H_INCLUDED

/**  2015.07 Dehua Tan **/

    #include <sys/cdefs.h>
    #include <sys/types.h>
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

    #define VT_UNUSED(x)    (void)(x)
    #define VT_NULL_CSTR    ((char*)NULL)
    #define VT_NULL_PTR     ((void*)NULL)

    #include "vtsys/vtffn.h"

#endif // VTDEF_H_INCLUDED
