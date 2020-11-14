#ifndef VTDIFFCODEC_H_INCLUDED
#define VTDIFFCODEC_H_INCLUDED

/**  2016.02 Dehua Tan **/

#include "vtsys/vtdef.h"

#define VT_DIFF_DELTA_MARK_LEN      8
#define VT_DIFF_COMPR_MARK_LEN      8
#define VT_DIFF_DELTA_MARK_V01      "VTDIFF01"
#define VT_DIFF_DELTA_MARK_V02      "VTDIFF02"
#define VT_DIFF_COMPR_MARK_LZMA     "vtlzma"
#define VT_DIFF_COMPR_MARK_BZ2      "vtbz2"

/**
 * file based differ operations:
 *  SND: Sub aNd aDd (sub and add), differing by sub, mergering by add
 *  XOR: differing and mergering both by xor
**/
#define VT_DIFF_OP_SND              ((uint8_t)0x00)
#define VT_DIFF_OP_XOR              ((uint8_t)0x03)

/**
 * block based differ operations
**/
#define VT_DIFF_OP_CPY              VT_DIFF_OP_SND
#define VT_DIFF_OP_ADD              ((uint8_t)0x01)
#define VT_DIFF_OP_PAD              ((uint8_t)0x02)

#define VT_DIFF_OP_BK_SZ_04         ((uint8_t)0x00)
#define VT_DIFF_OP_BK_SZ_12         ((uint8_t)0x01)
#define VT_DIFF_OP_BK_SZ_20         ((uint8_t)0x02)
#define VT_DIFF_OP_BK_SZ_28         ((uint8_t)0x03)
#define VT_DIFF_OP_BK_SZ_04_MAX_NUM ((uint32_t)((1UL << 4) - 1))
#define VT_DIFF_OP_BK_SZ_12_MAX_NUM ((uint32_t)((1UL << 12) - 1))
#define VT_DIFF_OP_BK_SZ_20_MAX_NUM ((uint32_t)((1UL << 20) - 1))
#define VT_DIFF_OP_BK_SZ_28_MAX_NUM ((uint32_t)((1UL << 28) - 1))
/**
 * The maximum block size (2^28, 256MB) for operation xor, add, pad, copy
**/
#define VT_DIFF_OP_BK_SZ_MAX_NUM    VT_DIFF_OP_BK_SZ_28_MAX_NUM

#define VT_DIFF_OP_OFF_05           ((uint8_t)0x00)
#define VT_DIFF_OP_OFF_13           ((uint8_t)0x01)
#define VT_DIFF_OP_OFF_21           ((uint8_t)0x02)
#define VT_DIFF_OP_OFF_29           ((uint8_t)0x03)
#define VT_DIFF_OP_OFF_37           ((uint8_t)0x04)
#define VT_DIFF_OP_OFF_45           ((uint8_t)0x05)
#define VT_DIFF_OP_OFF_53           ((uint8_t)0x06)
#define VT_DIFF_OP_OFF_61           ((uint8_t)0x07)
#define VT_DIFF_OP_OFF_05_MAX_NUM   ((uint32_t)((1ULL << 5) - 1))
#define VT_DIFF_OP_OFF_13_MAX_NUM   ((uint32_t)((1ULL << 13) - 1))
#define VT_DIFF_OP_OFF_21_MAX_NUM   ((uint32_t)((1ULL << 21) - 1))
#define VT_DIFF_OP_OFF_29_MAX_NUM   ((uint32_t)((1ULL << 29) - 1))
#define VT_DIFF_OP_OFF_37_MAX_NUM   ((uint64_t)((1ULL << 37) - 1))
#define VT_DIFF_OP_OFF_45_MAX_NUM   ((uint64_t)((1ULL << 45) - 1))
#define VT_DIFF_OP_OFF_53_MAX_NUM   ((uint64_t)((1ULL << 53) - 1))
#define VT_DIFF_OP_OFF_61_MAX_NUM   ((uint64_t)((1ULL << 61) - 1))
/**
 * The maximum offset (2^61, no file larger than this in real world) for operation xor, copy
**/
#define VT_DIFF_OP_OFF32_MAX_NUM      ((uint32_t)0x07fffffffUL)
#define VT_DIFF_OP_OFF64_MAX_NUM      VT_DIFF_OP_OFF_61_MAX_NUM

__BEGIN_DECLS

/**
 * vt_diff_op_first_byte_t op_first_byte
 * vt_diff_off_first_byte_t off_first_byte
 *
 * ADD operation in delta file:
 *  op_first_byte [left_block_size_bytes] {bytes_for_adding}
 *  --------ADD block_size--------------- ----block bytes---
 *
 * PAD operation in delta file:
 *  op_first_byte [left_block_size_bytes] {byte_for_pading}
 *  --------PAD times_to_pad------------- ----the_byte---
 *
 * COPY operation in delta file:
 *  op_first_byte [left_block_size_bytes] vt_diff_off_first_byte_t [left_offset_size_bytes]
 *  --------COPY block_size--------------
 *                                        ---------------SOURCE FILE OFFSET----------------
 * XOR operation in delta file:
 *  op_first_byte [left_block_size_bytes] vt_diff_off_first_byte_t [left_offset_size_bytes] vt_diff_off_first_byte_t [left_offset_size_bytes]
 *  -------XOR block_size----------------                                                   ------------TARGET FILE OFFSET-------------------
 *                                        ---------------SOURCE FILE OFFSET----------------
**/

#include "vtsys/vtpack_begin.h"
/**
 *  if(sizeof(vt_diff_delta_file_header_t) != VT_DIFF_DELTA_FILE_HEADER_SIZE){
 *      logError("FIXME!");
 *      exit(-1);
 *  }
 *  If cmpr_mark are all zero, no compression algorithm used.
 *  The argument option "--${cmpr_mark} compression_so_filename" is used for telling the application
 *  which share object to be dynamically loaded in a loosely coupled way.
**/
#define VT_DIFF_DELTA_FILE_HEADER_SIZE          64
#define VT_DIFF_DELTA_FILE_HEADER_RESERVED_SIZE 20
typedef struct _vt_diff_delta_file_header_t{
    char        diff_mark[VT_DIFF_DELTA_MARK_LEN];  //on this to select different merger algorithm on merger side.
    char        cmpr_mark[VT_DIFF_COMPR_MARK_LEN];  //on this to select different decompresson algorithm on merge side.
    uint32_t    meta_datum_count;                   //meta data item count, meta datum count
    uint32_t    meta_data_size;                     //meta data size, total meta datum size
    uint32_t    delta_data_size;                    //delta data size
    uint32_t    extra_data_size;                    //extra data size
    uint32_t    compressed_size;                    //The compressed data size for meta data + delta data + extra data
    uint32_t    file2_size;                         //added on 2016.05.09
    uint32_t    file1_size;                         //added on 2016.05.30
    char        reserved[VT_DIFF_DELTA_FILE_HEADER_RESERVED_SIZE];
}   VT_PACKED
vt_diff_delta_file_header_t;

typedef struct _vt_diff_op_first_byte_t{
    unsigned int op:2;                  /** e.g.: VT_DIFF_OP_CPY **/
    unsigned int bk_sz_ex_byte_num:2;   /** e.g.: VT_DIFF_OP_BK_SZ_04 **/
    unsigned int bk_sz_high_bits:4;
}   VT_PACKED
vt_diff_op_first_byte_t;

typedef struct _vt_diff_off_first_byte_t{
    unsigned int off_ex_byte_num:3;            /** e.g.: VT_DIFF_OP_OFF_05 **/
    unsigned int off_high_bits:5;
}   VT_PACKED
vt_diff_off_first_byte_t;

#include "vtsys/vtpack_end.h"

/**
 * file_id,
 *      for standard OS, file_id is filepath, there is implementation in vtfile.h
 *      for Barebone system, it dependents on, it need implementation by app developers.
**/
typedef ssize_t (*vt_read_file_t)(void* buf, const void* file_id, const ssize_t offset, const size_t size);

/**
 * do serializing
**/
int vt_diff_delta_file_header_to_buf(void* buf, const vt_diff_delta_file_header_t* header_ptr);
/**
 * do deserializing
**/
int vt_diff_buf_to_delta_file_header(vt_diff_delta_file_header_t* header_ptr, const void* buf);

/**
 * get the header from the given delta filepath
 * return 0 on successful, -1 on failed.
**/
int vt_get_delta_file_header(vt_diff_delta_file_header_t* header_ptr, const void* delta_file_id, vt_read_file_t read_file);

/**
 * the size of diff_mark_buf is at least VT_DIFF_DELTA_MARK_LEN.
 * the size of diff_mark_buf is at least VT_DIFF_COMPR_MARK_LEN.
 * return 0 on successfu, -1 on failed.
**/
int vt_get_delta_file_marks(char* diff_mark_buf, char* cmpr_mark_buf, const void* delta_file_id, vt_read_file_t read_file);

/**
 * the size of mark_buf is at least VT_DIFF_DELTA_MARK_LEN.
 * return 0 on successfu, -1 on failed.
**/
int vt_get_delta_file_mark(char* mark_buf, const void* delta_file_id, vt_read_file_t read_file);

/**
 * except bk_sz_high_bits, how many extra bytes need to hold op_block_len is the return value
**/
uint32_t vt_diff_codec_get_op_block_len_extra_byte_num(const uint32_t op_block_len);

/**
 * suppose first byte of buf is type of vt_diff_op_first_byte_t
**/
int vt_diff_codec_set_op(void* buf, const uint8_t op);
uint8_t vt_diff_codec_get_op(const void* buf);

/**
 * suppose first byte of buf is type of vt_diff_op_first_byte_t
 * return value is byte num taken in buf for block_len, including first
**/
uint32_t vt_diff_codec_put_op_block_len(void* buf, const uint32_t op_block_len);
uint32_t vt_diff_codec_get_op_block_len(uint32_t* op_block_len_ptr, const void* buf);

/**
 * except off_high_bits, how many extra bytes need to hold offset is the return value
**/
uint32_t vt_diff_codec_get_offset64_extra_byte_num(const uint64_t offset);
uint32_t vt_diff_codec_get_offset32_extra_byte_num(const uint32_t offset);
/**
 * suppose first byte of buf is type of vt_diff_off_first_byte_t
 * return value is byte num take in buf for offset, including first byte
**/
uint32_t vt_diff_codec_put_offset64(void* buf, const uint64_t offset);
uint32_t vt_diff_codec_get_offset64(uint64_t* offset_ptr, const void* buf);
uint32_t vt_diff_codec_put_offset32(void* buf, const uint32_t offset);
uint32_t vt_diff_codec_get_offset32(uint32_t* offset_ptr, const void* buf);

__END_DECLS

#endif // VTDIFFDEF_H_INCLUDED
