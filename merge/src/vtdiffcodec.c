#include "vtclib/vtdiffcodec.h"

#include <string.h>
// #include <stdlib.h>
#include <errno.h>

__BEGIN_DECLS

static void vt_uint32_to_buf(uint8_t* buf, uint32_t val){
    *buf++ = (val & 0x0ff);
    val >>= 8;
    *buf++ = (val & 0x0ff);
    val >>= 8;
    *buf++ = (val & 0x0ff);
    val >>= 8;
    *buf++ = (val & 0x0ff);
}

static void vt_buf_to_uint32(uint32_t* val_ptr, const uint8_t* buf){
    uint32_t val = 0;
    buf += 3;
    val += *buf--;
    val <<= 8;
    val += *buf--;
    val <<= 8;
    val += *buf--;
    val <<= 8;
    val += *buf;
    *val_ptr = val;
}

/**
 * do serializing
**/
int vt_diff_delta_file_header_to_buf(void* buf, const vt_diff_delta_file_header_t* header_ptr){
    uint8_t* ptr = (uint8_t*)buf;

    if((buf == (void*)NULL) || (header_ptr == (vt_diff_delta_file_header_t*)NULL)){
        errno = EINVAL;
        return -1;
    }

    memcpy(ptr, header_ptr->diff_mark, VT_DIFF_DELTA_MARK_LEN);
    ptr += VT_DIFF_DELTA_MARK_LEN;

    memcpy(ptr, header_ptr->cmpr_mark, VT_DIFF_COMPR_MARK_LEN);
    ptr += VT_DIFF_COMPR_MARK_LEN;

    vt_uint32_to_buf(ptr, header_ptr->meta_datum_count);
    ptr += 4;

    vt_uint32_to_buf(ptr, header_ptr->meta_data_size);
    ptr += 4;

    vt_uint32_to_buf(ptr, header_ptr->delta_data_size);
    ptr += 4;

    vt_uint32_to_buf(ptr, header_ptr->extra_data_size);
    ptr += 4;

    vt_uint32_to_buf(ptr, header_ptr->compressed_size);
    ptr += 4;

    vt_uint32_to_buf(ptr, header_ptr->file2_size);
    ptr += 4;

    vt_uint32_to_buf(ptr, header_ptr->file1_size);
    ptr += 4;

    memcpy(ptr, header_ptr->reserved, VT_DIFF_DELTA_FILE_HEADER_RESERVED_SIZE);
    ptr += VT_DIFF_DELTA_FILE_HEADER_RESERVED_SIZE;

    if((uint32_t)(ptr - (uint8_t*)buf) != VT_DIFF_DELTA_FILE_HEADER_SIZE){
        errno = EINVAL;
        return -1;
    }
    return 0;
}
/**
 * do deserializing
**/
int vt_diff_buf_to_delta_file_header(vt_diff_delta_file_header_t* header_ptr, const void* buf){
    const uint8_t* ptr = (const uint8_t*)buf;
    if((buf == (void*)NULL) || (header_ptr == (vt_diff_delta_file_header_t*)NULL)){
        errno = EINVAL;
        return -1;
    }

    memcpy(header_ptr->diff_mark, ptr, VT_DIFF_DELTA_MARK_LEN);
    ptr += VT_DIFF_DELTA_MARK_LEN;

    memcpy(header_ptr->cmpr_mark, ptr, VT_DIFF_COMPR_MARK_LEN);
    ptr += VT_DIFF_COMPR_MARK_LEN;

    vt_buf_to_uint32((uint32_t*)&(header_ptr->meta_datum_count), ptr);
    ptr += 4;

    vt_buf_to_uint32((uint32_t*)&(header_ptr->meta_data_size), ptr);
    ptr += 4;

    vt_buf_to_uint32((uint32_t*)&(header_ptr->delta_data_size), ptr);
    ptr += 4;

    vt_buf_to_uint32((uint32_t*)&(header_ptr->extra_data_size), ptr);
    ptr += 4;

    vt_buf_to_uint32((uint32_t*)&(header_ptr->compressed_size), ptr);
    ptr += 4;

    vt_buf_to_uint32((uint32_t*)&(header_ptr->file2_size), ptr);
    ptr += 4;

    vt_buf_to_uint32((uint32_t*)&(header_ptr->file1_size), ptr);
    ptr += 4;

    memcpy(header_ptr->reserved, ptr, VT_DIFF_DELTA_FILE_HEADER_RESERVED_SIZE);
    ptr += VT_DIFF_DELTA_FILE_HEADER_RESERVED_SIZE;

    if((uint32_t)(ptr - (uint8_t*)buf) != VT_DIFF_DELTA_FILE_HEADER_SIZE){
        errno = EINVAL;
        return -1;
    }
    return 0;
}

/**
 * get the header from the given delta filepath
 * return 0 on successful, -1 on failed.
**/
int vt_get_delta_file_header(vt_diff_delta_file_header_t* header_ptr, const void* delta_file_id, vt_read_file_t read_file){
    int errnum = 0;
    int retcode = 0;
    char header_buf[VT_DIFF_DELTA_FILE_HEADER_SIZE];

    if(header_ptr == (vt_diff_delta_file_header_t*)NULL){
        errnum = EINVAL;
        retcode = -1;
        goto done;
    }
    // if((header_buf = malloc(VT_DIFF_DELTA_FILE_HEADER_SIZE)) == (void*)NULL){
    //     errnum = errno;
    //     retcode = -1;
    //     goto done;
    // }
    if((size_t)read_file(header_buf, delta_file_id, -1, VT_DIFF_DELTA_FILE_HEADER_SIZE) != VT_DIFF_DELTA_FILE_HEADER_SIZE){
        errnum = (errno)? errno : EINVAL;
        retcode = -1;
        goto done;
    }
    if(vt_diff_buf_to_delta_file_header(header_ptr, header_buf) < 0){
        errnum = EINVAL;
        retcode = -1;
        goto done;
    }

    done:
    // if(header_buf != (char*)NULL){
    //     free(header_buf);
    //     header_buf = (char*)NULL;
    // }
    errno = errnum;
    return retcode;
}

/**
 * the size of diff_mark_buf is at least VT_DIFF_DELTA_MARK_LEN.
 * the size of cmpr_mark_buf is at least VT_DIFF_COMPR_MARK_LEN.
 * return 0 on successfu, -1 on failed.
**/
int vt_get_delta_file_marks(char* diff_mark_buf, char* cmpr_mark_buf, const void* delta_file_id, vt_read_file_t read_file){
    int errnum = 0;
    int retcode = 0;
    if((size_t)read_file(diff_mark_buf, delta_file_id, -1, VT_DIFF_DELTA_MARK_LEN) != VT_DIFF_DELTA_MARK_LEN){
        errnum = (errno)? errno : EINVAL;
        retcode = -1;
        goto done;
    }
    if((size_t)read_file(cmpr_mark_buf, delta_file_id, VT_DIFF_DELTA_MARK_LEN, VT_DIFF_COMPR_MARK_LEN) != VT_DIFF_COMPR_MARK_LEN){
        errnum = (errno)? errno : EINVAL;
        retcode = -1;
        goto done;
    }
    done:
    errno = errnum;
    return retcode;
}

/**
 * the size of mark_buf is at least VT_DIFF_DELTA_MARK_LEN.
 * return 0 on successfu, -1 on failed.
**/
int vt_get_delta_file_mark(char* mark_buf, const void* delta_file_id, vt_read_file_t read_file){
    int errnum = 0;
    int retcode = 0;
    if((size_t)read_file(mark_buf, delta_file_id, -1, VT_DIFF_DELTA_MARK_LEN) != VT_DIFF_DELTA_MARK_LEN){
        errnum = (errno)? errno : EINVAL;
        retcode = -1;
        goto done;
    }
    done:
    errno = errnum;
    return retcode;
}

/**
 * except bk_sz_high_bits, how many extra bytes need to hold op_block_len is the return value
**/
uint32_t vt_diff_codec_get_op_block_len_extra_byte_num(const uint32_t op_block_len){
    return
        ((op_block_len <= VT_DIFF_OP_BK_SZ_04_MAX_NUM)? VT_DIFF_OP_BK_SZ_04 :
        ((op_block_len <= VT_DIFF_OP_BK_SZ_12_MAX_NUM)? VT_DIFF_OP_BK_SZ_12 :
        ((op_block_len <= VT_DIFF_OP_BK_SZ_20_MAX_NUM)? VT_DIFF_OP_BK_SZ_20 : VT_DIFF_OP_BK_SZ_28)));
}

/**
 * suppose first byte of buf is type of vt_diff_op_first_byte_t
**/
int vt_diff_codec_set_op(void* buf, const uint8_t op){
    switch(op){
        case VT_DIFF_OP_SND:
        case VT_DIFF_OP_XOR:
        case VT_DIFF_OP_ADD:
        case VT_DIFF_OP_PAD:
        //case VT_DIFF_OP_CPY:
            ((vt_diff_op_first_byte_t*)buf)->op = (op & 0x03);
            return 0;
        default:
            return -1;
    }
}

uint8_t vt_diff_codec_get_op(const void* buf){
    return (uint8_t)(((vt_diff_op_first_byte_t*)buf)->op);
}

/**
 * suppose first byte of buf is type of vt_diff_op_first_byte_t
 * return value is byte num taken in buf for block_len, including first byte
**/
uint32_t vt_diff_codec_put_op_block_len(void* buf, const uint32_t op_block_len){
    const uint32_t extra_byte_num = vt_diff_codec_get_op_block_len_extra_byte_num(op_block_len);
    uint32_t tmp_op_block_len = op_block_len;
    uint32_t i;
    for(i = extra_byte_num; i > 0; i --){
        *((uint8_t*)buf + i) = (uint8_t)(tmp_op_block_len & 0x0ff);
        tmp_op_block_len >>= 8;
    }
    ((vt_diff_op_first_byte_t*)buf)->bk_sz_high_bits = tmp_op_block_len & 0x0f;
    ((vt_diff_op_first_byte_t*)buf)->bk_sz_ex_byte_num = extra_byte_num;
    return (extra_byte_num + 1);
}

uint32_t vt_diff_codec_get_op_block_len(uint32_t* op_block_len_ptr, const void* buf){
    const uint32_t extra_byte_num = ((vt_diff_op_first_byte_t*)buf)->bk_sz_ex_byte_num;
    uint32_t i;
    *op_block_len_ptr = ((vt_diff_op_first_byte_t*)buf)->bk_sz_high_bits;
    for(i = 1; i <= extra_byte_num; i ++){
        *op_block_len_ptr <<= 8;
        *op_block_len_ptr += *((uint8_t*)buf + i);
    }
    return (extra_byte_num + 1);
}

/**
 * except off_high_bits, how many extra bytes need to hold offset is the return value
**/
uint32_t vt_diff_codec_get_offset64_extra_byte_num(const uint64_t offset){
    return
        ((VT_DIFF_OP_OFF_05_MAX_NUM >= offset) ? VT_DIFF_OP_OFF_05 :
        ((VT_DIFF_OP_OFF_13_MAX_NUM >= offset) ? VT_DIFF_OP_OFF_13 :
        ((VT_DIFF_OP_OFF_21_MAX_NUM >= offset) ? VT_DIFF_OP_OFF_21 :
        ((VT_DIFF_OP_OFF_29_MAX_NUM >= offset) ? VT_DIFF_OP_OFF_29 :
        ((VT_DIFF_OP_OFF_37_MAX_NUM >= offset) ? VT_DIFF_OP_OFF_37 :
        ((VT_DIFF_OP_OFF_45_MAX_NUM >= offset) ? VT_DIFF_OP_OFF_45 :
        ((VT_DIFF_OP_OFF_53_MAX_NUM >= offset) ? VT_DIFF_OP_OFF_53 : VT_DIFF_OP_OFF_61)))))));
}

uint32_t vt_diff_codec_get_offset32_extra_byte_num(const uint32_t offset){
    return
        ((VT_DIFF_OP_OFF_05_MAX_NUM >= offset) ? VT_DIFF_OP_OFF_05 :
        ((VT_DIFF_OP_OFF_13_MAX_NUM >= offset) ? VT_DIFF_OP_OFF_13 :
        ((VT_DIFF_OP_OFF_21_MAX_NUM >= offset) ? VT_DIFF_OP_OFF_21 :
        ((VT_DIFF_OP_OFF_29_MAX_NUM >= offset) ? VT_DIFF_OP_OFF_29 : VT_DIFF_OP_OFF_37))));
}

/**
 * suppose first byte of buf is type of vt_diff_off_first_byte_t
 * return value is byte num take in buf for offset, including first byte
**/
uint32_t vt_diff_codec_put_offset64(void* buf, const uint64_t offset){
    const uint32_t extra_byte_num = vt_diff_codec_get_offset64_extra_byte_num(offset);
    uint64_t tmp_offset = offset;
    uint32_t i;
    for(i = extra_byte_num; i > 0; i --){
        *((uint8_t*)buf + i) = (uint8_t)(tmp_offset & 0x0ff);
        tmp_offset >>= 8;
    }
    ((vt_diff_off_first_byte_t*)buf)->off_high_bits = tmp_offset & 0x01f;
    ((vt_diff_off_first_byte_t*)buf)->off_ex_byte_num = extra_byte_num;
    return (extra_byte_num + 1);
}

uint32_t vt_diff_codec_get_offset64(uint64_t* offset_ptr, const void* buf){
    const uint32_t extra_byte_num = ((vt_diff_off_first_byte_t*)buf)->off_ex_byte_num;
    uint32_t i;
    *offset_ptr = ((vt_diff_off_first_byte_t*)buf)->off_high_bits;
    for(i = 1; i <= extra_byte_num; i ++){
        *offset_ptr <<= 8;
        *offset_ptr += *((uint8_t*)buf + i);
    }
    return (extra_byte_num + 1);
}

uint32_t vt_diff_codec_put_offset32(void* buf, const uint32_t offset){
    const uint32_t extra_byte_num = vt_diff_codec_get_offset32_extra_byte_num(offset);
    uint32_t tmp_offset = offset;
    uint32_t i;
    for(i = extra_byte_num; i > 0; i --){
        *((uint8_t*)buf + i) = (uint8_t)(tmp_offset & 0x0ff);
        tmp_offset >>= 8;
    }
    ((vt_diff_off_first_byte_t*)buf)->off_high_bits = tmp_offset & 0x01f;
    ((vt_diff_off_first_byte_t*)buf)->off_ex_byte_num = extra_byte_num;
    return (extra_byte_num + 1);
}

uint32_t vt_diff_codec_get_offset32(uint32_t* offset_ptr, const void* buf){
    const uint32_t extra_byte_num = ((vt_diff_off_first_byte_t*)buf)->off_ex_byte_num;
    uint32_t i;
    *offset_ptr = ((vt_diff_off_first_byte_t*)buf)->off_high_bits;
    for(i = 1; i <= extra_byte_num; i ++){
        *offset_ptr <<= 8;
        *offset_ptr += *((uint8_t*)buf + i);
    }
    return (extra_byte_num + 1);
}

__END_DECLS
