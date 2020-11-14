

#ifndef VT_PACKED
    #error It is not allowed to include "vtpack_end.h" multiple times without begining with including "vtpack_begin.h"
#else
    #undef VT_PACKED
    #ifdef _MSC_VER
        #pragma pack()
    #endif
#endif
