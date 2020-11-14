#ifndef VTMERGE01UTIL_H_INCLUDED
#define VTMERGE01UTIL_H_INCLUDED


#include "vtsys/vtdiffcodec.h"

/**
 * mid staging delta file is a serial of following operations:
 *      COPY(POS1, LEN, POS2):
 *          Format: (POS1<8 bytes>) + (LEN<4 bytes>) + (POS2<8 bytes>)
 *           space: total bytes for each item is 20 bytes.
 *       operation: copy bytes start from POS1 with length LEN in file2 into POS2 in file2.
**/

/**
 *
 * Delta file is a serial of following operations:
 *      ADD(LEN, DAT): appending data "DAT", its length is "LEN" to file2.
 *      PAD(LEN, BYT): padding the byte "BYT" with times "LEN" to file2.
 *      CP1(LEN, POS): appending data from file1, its pos is "POS", its length is "LEN" to file2.
 *      CP2(LEN, POS): appending existing data from file2, its pos is "POS", its length is "LEN" to file2.
 *
 * For saving delta file space, the first byte(chr) in operator will be encoded as:
 *      1>  (chr & 0xc0) will be one of: VT_DIFF_OP_ADD, VT_DIFF_OP_CP1, VT_DIFF_OP_PAD, VT_DIFF_OP_CP2
 *      2>  (chr & 0x30) will be one of: VT_DIFF_LEN_BITS_12, VT_DIFF_LEN_BITS_20, VT_DIFF_LEN_BITS_28, VT_DIFF_LEN_BITS_36
 *      3>  (chr & 0x0f) will be the hight 4 bits for LEN, the additional bytes for len depends on (chr & 0x30).
 *      4>  The first byte format in operator
 *           (OP)   (LEN-BITS) (LEN's high 4 bits)
 *          c    c    b    b    l    l    l    l
 *      5>  For operations CP1 and CP2, the bytes for POS will be bytes of ((LEN's bytes)/8 + 1).
 *              2 bytes for VT_DIFF_LEN_BITS_12,
 *              3 bytes for VT_DIFF_LEN_BITS_20,
 *              4 bytes for VT_DIFF_LEN_BITS_28,
 *              5 bytes for VT_DIFF_LEN_BITS_36,
 *          So the supported max file size will be at least 1PB that can be represented by this coding format.
**/

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
    /**
     * return 0, something wrong,
     * otherwise the return value will be the byte count that op_prefix_buf taken.
     * the maximum op_prefix_buf size should be 10 to be allocated.
    **/
    extern uint8_t vt_diff01_to_op_prefix(char* op_prefix_buf, const uint8_t op, const size_t len, const size_t pos);
    extern uint8_t vt_diff01_from_op_prefix(uint8_t* op_ptr, size_t* len_ptr, size_t* pos_ptr, const char* op_prefix_buf);

__END_DECLS

#endif // VTDIFFUTIL_H_INCLUDED
