#ifndef VTDIFFCONTEXT_H_INCLUDED
#define VTDIFFCONTEXT_H_INCLUDED


#include "vtsys/vtdef.h"

__BEGIN_DECLS

/* The default value is VT_DIFF_DATA_GRAM_SIZE_8 */
extern uint32_t vt_diff_data_gram_size;
/* The default value is VT_DIFF_FNV_32_LS_BITS_FOR_DATA_GRAM_SIZE_8 */
extern uint32_t vt_diff_fnv_32_ls_bits;

/* init default value for vt_diff_data_gram_size & vt_diff_fnv_32_ls_bits */
extern void vt_init_diff_data_gram_size();
extern void vt_set_diff_data_gram_size(const uint32_t data_gram_size);

#if 0
extern void vt_set_diff_fnv_32_ls_bits(const uint32_t fnv_32_ls_bits);
#endif

extern uint32_t vt_hash_window_size;            //default: 8
extern uint32_t vt_continue_mismatch_limit;     //default: 4
extern uint32_t vt_max_mismatch_percentage;     //default: 35

extern void vt_set_hash_window_size(const uint32_t hash_window_size);
extern void vt_set_continue_mismatch_limit(const uint32_t continue_mismatch_limit);
extern void vt_set_max_mismatch_percentage(const uint32_t max_mismatch_percentage);

__END_DECLS

#endif
