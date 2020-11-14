#ifndef VTDIFF01UTIL_H_INCLUDED
#define VTDIFF01UTIL_H_INCLUDED


#include "vtdiff/vtmerge01util.h"
#include <stdio.h>

__BEGIN_DECLS

    #include "vtsys/vtpack_begin.h"
    typedef struct _vt_diff01_file_hash_info_t{
        FILE* fp;
        char* filepath;
        size_t file_size;
        size_t total_map_count;
        size_t valid_map_count;
        vt_diff01_hv_pos_pair_t* hv2pos_map;
    }   VT_PACKED
    vt_diff01_file_hash_info_t;
    #include "vtsys/vtpack_end.h"

    extern void vt_diff01_free_file_hash_info_ptr(vt_diff01_file_hash_info_t* ptr);
    extern int vt_diff01_create_file1_hash_info(vt_diff01_file_hash_info_t** file_hash_info_ptr_ptr, const char* filepath);

    extern int vt_diff01_gen_stage(
        FILE* staging_file_fp, const char* staging_file_filepath,
        vt_diff01_file_hash_info_t* file1_info_ptr,
        FILE* file2_fp, const char* file2_filepath
    );

    /**
     * This API is for test purpose.
     * return non-zero means correct, return 0 means NOT correct.
    **/
    extern int vt_diff01_is_stage_out_correct(
        FILE* staging_file_fp, const char* staging_file_filepath,
        FILE* file1_fp, const char* file1_filepath,
        FILE* file2_fp, const char* file2_filepath
    );

    extern int vt_diff01_gen_delta(
        const size_t file1_size,
        FILE* delta_file_fp, const char* delta_file_filepath,
        FILE* staging_file_fp, const char* staging_file_filepath,
        FILE* file2_fp, const char* file2_filepath
    );

__END_DECLS

#endif // VTDIFFUTIL_H_INCLUDED
