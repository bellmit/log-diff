#ifndef DISABLE_VTDIFF02

#ifndef VTMERGE02_H_INCLUDED
#define VTMERGE02_H_INCLUDED

/**  2016.04 Dehua Tan **/

#include "vtclib/vtdef.h"

int vt_sjaonse_file_merge02(const char* filepath2,
														const char* filepath1,
														const char* delta_filepath,
														int need_temp_files);

#ifndef DISABLE_VTLZMA
int vt_sjaonse_file_lzma_merge02(const char* filepath2,
																 const char* filepath1,
																 const char* delta_filepath,
																 int need_temp_files);
#endif

#ifndef DISABLE_VTBZ2
int vt_sjaonse_file_bz2_merge02(const char* filepath2,
																const char* filepath1,
																const char* delta_filepath,
																int need_temp_files);
#endif

#endif // VTMERGE02_H_INCLUDED
																
#endif
