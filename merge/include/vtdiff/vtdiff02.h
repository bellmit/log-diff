#ifndef VTDIFF02_H_INCLUDED
#define VTDIFF02_H_INCLUDED

#include "vtsys/vtdiffcodec.h"
#include "vtsys/vtrollhash.h"

__BEGIN_DECLS

#include "vtsys/vtpack_begin.h"
/////////////////////////////
typedef struct _vt_diff_hash_table_tmp_item_t{
    uint32_t hash_value;
    uint32_t offset;
}   VT_PACKED
vt_diff_hash_table_tmp_item_t;

typedef struct _vt_diff_hash_table_item_t{
    uint32_t hash_value;
    uint32_t offset_count;
    uint32_t offset_array_offset;
}   VT_PACKED
vt_diff_hash_table_item_t;
/////////////////////////////
#include "vtsys/vtpack_end.h"


typedef struct _vt_diff_hash_table_t{
    vt_diff_hash_table_item_t* item_array;
    uint32_t item_allocated;
    uint32_t item_count;
    //
    uint32_t* offset_array;
    uint32_t offset_allocated;
    uint32_t offset_count;
    //
    uint32_t max_item_offset_count;
}   vt_diff_hash_table_t;

int vt_diff_hash_table_uint32_compare(const void* i1_ptr, const void* i2_ptr);
int vt_diff_hash_table_tmp_item_compare(const void* item1_ptr, const void* item2_ptr);
int vt_diff_hash_table_item_compare(const void* item1_ptr, const void* item2_ptr);

int vt_diff_create_hash_table_tmp_items(
    vt_diff_hash_table_tmp_item_t** tmp_items_ptr,
    uint32_t* tmp_item_count_ptr,
    const uint8_t* src,
    const uint32_t src_len,
    const vt_rollhash32_context_t* ctx_ptr
);

int vt_diff_create_src_hash_table(
    vt_diff_hash_table_t** src_hash_table_ptr_ptr,
    const uint8_t* src,
    const uint32_t src_len,
    const vt_rollhash32_context_t* ctx_ptr
);

int vt_diff_free_src_hash_table(vt_diff_hash_table_t* src_hash_table_ptr);

typedef struct _vt_diff_near_identical_block_searcher_context_t{
    vt_rollhash32_context_t rollhash_ctx;
    uint32_t continue_mismatch_limit;   //v in algorithm
    uint32_t max_mismatch_percentage;   //p in algorithm
    int max_identical_first_in_match;
    int jmp_identical_block_in_match;
}   vt_diff_near_identical_block_searcher_context_t;

typedef struct _vt_diff_near_identical_block_searcher_param_t{
    uint8_t* target_addr;
    uint32_t target_len;
    //
    uint8_t* src_addr;
    uint32_t src_len;
    vt_diff_hash_table_t* src_hash_table_ptr;
    //
}   vt_diff_near_identical_block_searcher_param_t;

typedef struct _vt_diff_near_identical_block_item_t{
    uint32_t block_len;
    uint32_t src_offset;
    uint32_t target_offset;
}   vt_diff_near_identical_block_item_t;

typedef struct _vt_diff_near_identical_block_searcher_result_t{
    vt_diff_near_identical_block_item_t* block_items;
    uint32_t block_item_count;
    uint32_t block_item_allocated;
}   vt_diff_near_identical_block_searcher_result_t;

int vt_diff_set_near_identical_block_searcher_context(
    vt_diff_near_identical_block_searcher_context_t* ctx_ptr,
    const uint32_t continue_mismatch_limit, //v in algorithm
    const uint32_t max_mismatch_percentage, //p in algorithm
    const int max_identical_first_in_match,
    const int jmp_identical_block_in_match
);

/**
 *  1> return non-zero, something wrong, check errno for details.
 *  2> return 0, successful
 *     2.1> if (result_ptr == NULL || result_ptr->block_len == 0 || result_ptr->block_len < ctx_ptr->min_match_window_size),
 *          no near_identical_block fount for param_ptr->target_hash_off.
 *     2.2> if (result_ptr->block_len > 0 && result_ptr->block_len >= ctx_ptr->min_match_window_size),
 *          there is near_identical_block fount for param_ptr->target_hash_off.
**/
int vt_diff_search_near_identical_blocks(
    vt_diff_near_identical_block_searcher_result_t** result_ptr_ptr,
    const vt_diff_near_identical_block_searcher_param_t* param_ptr,
    const vt_diff_near_identical_block_searcher_context_t* ctx_ptr
);

void vt_diff_free_near_identical_block_searcher_result(vt_diff_near_identical_block_searcher_result_t* result_ptr);

/**
 * generating
 *      meta_data_filepath and delta_data_filepath
 * on
 *      filepath1, filepath2, hash_window_size, max_mismatch_percentage
 *
 * 1> hash_window_size must not 0
 * 2> max_mismatch_percentage should be reasonable and not larger than 100
 * 3> other params must not be null
 *
 * return value < 0,  something wrong, check error for details.
 * return value == 0, successful,
 *                    if *meta_data_size_ptr == 0 && *delta_data_size_ptr == 0,
 *                    it means filepath1 is identical to filepath2.
 *                    and file meta_data_filepath and delta_data_filepath will not be generated.
 **/
int vt_file_diff02_impl(
    uint32_t* meta_data_size_ptr,
    uint32_t* delta_data_size_ptr,
    uint32_t* extra_data_size_ptr,
    const char* meta_data_filepath,
    const char* delta_data_filepath,
    const char* extra_data_filepath,
    const char* filepath1,
    const char* filepath2,
    const uint32_t hash_window_size,
    const uint32_t continue_mismatch_limit, //v in algorithm
    const uint32_t max_mismatch_percentage,  //p in algorithm
    const uint8_t diff_op,
    const int max_identical_first_in_match,
    const int jmp_identical_block_in_match
);

/**
 * generating
 *      filepath2
 * on
 *      filepath1, meta_data_filepath, delta_data_filepath
**/
int vt_file_merge02_impl(
    const char* filepath2,
    const char* filepath1,
    const char* meta_data_filepath,
    const char* delta_data_filepath,
    const char* extra_data_filepath
);

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
int vt_file_diff02(
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

/**
 * generating
 *      filepath2
 * on
 *      filepath1, delta_filepath
**/
int vt_file_merge02(
    const char* filepath2,
    const char* filepath1,
    const char* delta_filepath,
    const int need_keep_interim_temp_files
);

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
int vt_file_lzma_diff02(
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

/**
 * generating
 *      filepath2
 * on
 *      filepath1, delta_filepath
**/
int vt_file_lzma_merge02(
    const char* filepath2,
    const char* filepath1,
    const char* delta_filepath,
    const int need_keep_interim_temp_files
);

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
int vt_file_bz2_diff02(
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

/**
 * generating
 *      filepath2
 * on
 *      filepath1, delta_filepath
**/
int vt_file_bz2_merge02(
    const char* filepath2,
    const char* filepath1,
    const char* delta_filepath,
    const int need_keep_interim_temp_files
);

__END_DECLS

#endif // VTDIFF01_H_INCLUDED
