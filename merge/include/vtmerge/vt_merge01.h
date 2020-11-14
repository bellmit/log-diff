#ifndef VTMERGE01_H_INCLUDED
#define VTMERGE01_H_INCLUDED

/**  2015.07 Dehua Tan **/

#include "vtclib/vtdef.h"


__BEGIN_DECLS

    int vt_sjaonse_file_merge01(const char* filepath2,
																const char* filepath1,
																const char* delta_filepath);

    #ifndef DISABLE_VTLZMA
    int vt_sjaonse_file_lzma_merge01(const char* filepath2,
																		 const char* filepath1,
																		 const char* delta_filepath);
    #endif

    #ifndef DISABLE_VTBZ2
    int vt_sjaonse_file_bz2_merge01(const char* filepath2,
																		const char* filepath1,
																		const char* delta_filepath);
    #endif

__END_DECLS

#endif // VTDIFF_H_INCLUDED
