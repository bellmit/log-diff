#ifndef VTDIFF_H_INCLUDED
#define VTDIFF_H_INCLUDED


#include "vtsys/vtdef.h"
#include "vtdiff/vtdiff01.h"

__BEGIN_DECLS

    /**
     * generating
     *      delta_filepath
     * on
     *      filepath1, filepath2, hash_window_size, max_mismatch_percentage
     *
     * 1> hash_window_size must not 0
     * 2> max_mismatch_percentage should be reasonable and not larger than 100
     * 3> other params must not be null
     *
     * return value < 0,  something wrong, check error for details.
     * return value == 0, successful,
     *                    if delta_filepath not exist, it means filepath1 is identical to filepath2.
     **/
    extern int vt_file_diff02(
        const char* delta_filepath,
        const char* filepath1,
        const char* filepath2,
        const uint32_t hash_window_size,
        const uint32_t continue_mismatch_limit, //v in algorithm
        const uint32_t max_mismatch_percentage,  //p in algorithm
        const uint8_t diff_op,
        const int max_identical_first_in_match,
        const int jmp_identical_block_in_match,
        const int need_keep_interim_temp_files
    );
    //
    extern int vt_file_lzma_diff02(
        const char* delta_filepath,
        const char* filepath1,
        const char* filepath2,
        const uint32_t hash_window_size,
        const uint32_t continue_mismatch_limit, //v in algorithm
        const uint32_t max_mismatch_percentage, //p in algorithm
        const uint8_t diff_op,
        const int max_identical_first_in_match,
        const int jmp_identical_block_in_match,
        const int need_keep_interim_temp_files
    );
    //
    extern int vt_file_bz2_diff02(
        const char* delta_filepath,
        const char* filepath1,
        const char* filepath2,
        const uint32_t hash_window_size,
        const uint32_t continue_mismatch_limit, //v in algorithm
        const uint32_t max_mismatch_percentage, //p in algorithm
        const uint8_t diff_op,
        const int max_identical_first_in_match,
        const int jmp_identical_block_in_match,
        const int need_keep_interim_temp_files
    );
    //
    /**
     * generating
     *      filepath2
     * on
     *      filepath1, delta_filepath
    **/
    extern int vt_file_merge02(
        const char* filepath2,
        const char* filepath1,
        const char* delta_filepath,
        const int need_keep_interim_temp_files
    );
    //
    int vt_file_lzma_merge02(
        const char* filepath2,
        const char* filepath1,
        const char* delta_filepath,
        const int need_keep_interim_temp_files
    );
    //
    int vt_file_bz2_merge02(
        const char* filepath2,
        const char* filepath1,
        const char* delta_filepath,
        const int need_keep_interim_temp_files
    );

__END_DECLS

#endif // VTDIFF_H_INCLUDED
