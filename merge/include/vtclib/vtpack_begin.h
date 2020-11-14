

#ifdef VT_PACKED
    #error It is not allowed to include "vtpack_begin.h" multiple times without ending with including "vtpack_end.h".
#else
    #ifdef _MSC_VER
        #define VT_PACKED
        #pragma pack(1)
    #else
        #define VT_PACKED __attribute__ ((packed))
    #endif
#endif
