#ifndef _VTERRNO_H_
#define _VTERRNO_H_

#include "vtclib/vtdef.h"
#include <errno.h>


__BEGIN_DECLS

#define VT_ERRNO_oem_get_file_operators_is_null                     4001
#define VT_ERRNO_oem_get_max_tmp_file_id_size_is_null            4002
#define VT_ERRNO_oem_gen_tmp_file_id_is_null                       4003
#define VT_ERRNO_oem_copy_and_remove_file_is_null                   4004
#define VT_ERRNO_oem_get_max_tmp_file_id_size_return_zero        4005

#define VT_ERRNO_oem_get_partition_operators_is_null                4011
#define VT_ERRNO_oem_get_max_tmp_partition_name_length_is_null      4012
#define VT_ERRNO_oem_gen_tmp_partition_name_is_null                 4013
#define VT_ERRNO_oem_copy_and_remove_partition_is_null              4014
#define VT_ERRNO_oem_get_max_tmp_partition_name_length_return_zero  4015

extern const char* vt_strerror(const int errnum);

__END_DECLS

#endif // _VTERRNO_H_
