#ifndef VTMERGE01UTIL_H_INCLUDED
#define VTMERGE01UTIL_H_INCLUDED

/**  2015.07 Dehua Tan **/

#include "vtclib/vtdiffcodec.h"

#define VT_DIFF_VER01_SIZE          VT_DIFF_DELTA_MARK_LEN
#define VT_DIFF_VER01_CSTR          VT_DIFF_DELTA_MARK_V01

#ifndef VT_DIFF_OP_CPY
#define VT_DIFF_OP_CPY              ((uint8_t)0x00)
#endif
#ifndef VT_DIFF_OP_ADD
#define VT_DIFF_OP_ADD              ((uint8_t)0x01)
#endif
#ifndef VT_DIFF_OP_PAD
#define VT_DIFF_OP_PAD              ((uint8_t)0x02)
#endif
#define VT_DIFF_OP_CP1              VT_DIFF_OP_CPY
#define VT_DIFF_OP_CP2              VT_DIFF_OP_XOR

#define VT_DIFF_LEN_BITS_12         ((uint8_t)0x00)
#define VT_DIFF_LEN_BITS_20         ((uint8_t)0x01)
#define VT_DIFF_LEN_BITS_28         ((uint8_t)0x02)
#define VT_DIFF_LEN_BITS_36         ((uint8_t)0x03)

#define VT_DIFF_MAX_INT36           (uint64_t)((1ULL << 36) - 1)
#define VT_DIFF_MAX_INT28           (uint32_t)((1ULL << 28) - 1)
#define VT_DIFF_MAX_INT20           (uint32_t)((1ULL << 20) - 1)
#define VT_DIFF_MAX_INT12           (uint16_t)((1ULL << 12) - 1)

#define VT_DIFF_DATA_GRAM_SIZE_32   8
#define VT_DIFF_MAX_PREFIX_BUF_SIZE 10

__BEGIN_DECLS

#include <stdio.h>

    #include "vtclib/vtpack_begin.h"

    typedef struct _vt_diff01_hv_pos_pair_t{
        uint32_t hv;
        size_t pos;
    }   VT_PACKED
    vt_diff01_hv_pos_pair_t;

    typedef struct _vt_diff01_pos1_len_pos2_t{
        size_t pos1;
        uint32_t len;
        size_t pos2;
    }   VT_PACKED
    vt_diff01_pos1_len_pos2_t;

    typedef struct _vt_diff01_first_op_byte_t{
        unsigned int len_bits_type:2;
        unsigned int op:2;
        unsigned int len_high_bits:4;
    }   VT_PACKED
    vt_diff01_first_op_byte_t;

    typedef struct _vt_diff01_pad_pos_len_t{
        size_t pos;
        uint32_t len;
    }   VT_PACKED
    vt_diff01_pad_pos_len_t;

    #include "vtclib/vtpack_end.h"

    extern vt_diff01_first_op_byte_t vt_diff01_to_first_op_byte(const uint8_t byte);
    extern uint8_t vt_diff01_from_first_op_byte(const vt_diff01_first_op_byte_t first_op_byte);
    extern uint8_t vt_diff01_get_prefix_buf_size_on_first_op_byte(const vt_diff01_first_op_byte_t first_op_byte);
    extern uint8_t vt_diff01_get_prefix_buf_size_on_first_byte(const uint8_t byte);
    extern uint8_t vt_diff01_to_op_prefix(char* op_prefix_buf, const uint8_t op, const size_t len, const size_t pos);
    extern uint8_t vt_diff01_from_op_prefix(uint8_t* op_ptr, size_t* len_ptr, size_t* pos_ptr, const char* op_prefix_buf);

__END_DECLS

#endif // VTMERGE01UTIL_H_INCLUDED
