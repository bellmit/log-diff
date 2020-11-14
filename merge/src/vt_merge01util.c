/**  2016.04 Dehua Tan **/

#ifndef DISABLE_VTDIFF01

#include "vtclib/vtlog.h"
#include "vtmerge/vt_merge01util.h"

__BEGIN_DECLS

    vt_diff01_first_op_byte_t vt_diff01_to_first_op_byte(const uint8_t byte){
        vt_diff01_first_op_byte_t first_op_byte;
        first_op_byte.len_bits_type = (byte >>6);
        first_op_byte.op = ((byte >> 4) & 0x03);
        first_op_byte.len_high_bits = (byte & 0x0f);
        return first_op_byte;
    }

    uint8_t vt_diff01_from_first_op_byte(const vt_diff01_first_op_byte_t first_op_byte){
        return (((uint8_t)(first_op_byte.len_bits_type) << 6) | ((uint8_t)(first_op_byte.op) << 4) | (uint8_t)(first_op_byte.len_high_bits));
    }

    uint8_t vt_diff01_get_prefix_buf_size_on_first_op_byte(const vt_diff01_first_op_byte_t first_op_byte){
        uint8_t buf_size = 0;
        switch(first_op_byte.len_bits_type){
            case VT_DIFF_LEN_BITS_12:
                buf_size = 2;
                break;
            case VT_DIFF_LEN_BITS_20:
                buf_size = 3;
                break;
            case VT_DIFF_LEN_BITS_28:
                buf_size = 4;
                break;
            case VT_DIFF_LEN_BITS_36:
                buf_size = 5;
                break;
            default:
                logError("The first_byte.len_bits_type(%u) is not supported", (unsigned int)first_op_byte.len_bits_type);
                return 0;
        }
        switch(first_op_byte.op){
            case VT_DIFF_OP_CP1:
            case VT_DIFF_OP_CP2:
                buf_size *= 2;
                break;
            case VT_DIFF_OP_ADD:
            case VT_DIFF_OP_PAD:
                break;
            default:
                logError("The first_byte.op(%u) is not supported", (unsigned int)first_op_byte.op);
                return 0;
        }
        return buf_size;
    }

    uint8_t vt_diff01_get_prefix_buf_size_on_first_byte(const uint8_t byte){
        vt_diff01_first_op_byte_t first_op_byte;
        first_op_byte.len_bits_type = (byte >>6);
        first_op_byte.op = ((byte >> 4) & 0x03);
        first_op_byte.len_high_bits = (byte & 0x0f);
        return vt_diff01_get_prefix_buf_size_on_first_op_byte(first_op_byte);
    }


    uint8_t vt_diff01_to_op_prefix(char* op_prefix_buf, const uint8_t op, const size_t len, const size_t pos){
        const size_t pos_or_len = (len > pos)? len : pos;
        char* cur_op_prefix_buf = op_prefix_buf;
        vt_diff01_first_op_byte_t first_op_byte = {0};
        first_op_byte.op = op;
        if(pos_or_len > VT_DIFF_MAX_INT36){
            /** pos_or_len is too large **/
            logError("pos_or_len(%lu) is too large!", pos_or_len);
            return 0;
        }
        else if(pos_or_len > VT_DIFF_MAX_INT28){
            first_op_byte.len_bits_type = VT_DIFF_LEN_BITS_36;
            first_op_byte.len_high_bits = ((uint64_t)len >> 32) & 0x0f;
            *cur_op_prefix_buf++ = vt_diff01_from_first_op_byte(first_op_byte);
            *cur_op_prefix_buf++ = (len >> 24) & 0x0ff;
            *cur_op_prefix_buf++ = (len >> 16) & 0x0ff;
            *cur_op_prefix_buf++ = (len >> 8) & 0x0ff;
            *cur_op_prefix_buf++ = len & 0x0ff;
        }
        else if(pos_or_len > VT_DIFF_MAX_INT20){
            first_op_byte.len_bits_type = VT_DIFF_LEN_BITS_28;
            first_op_byte.len_high_bits = (len >> 24) & 0x0f;
            *cur_op_prefix_buf++ = vt_diff01_from_first_op_byte(first_op_byte);
            *cur_op_prefix_buf++ = (len >> 16) & 0x0ff;
            *cur_op_prefix_buf++ = (len >> 8) & 0x0ff;
            *cur_op_prefix_buf++ = len & 0x0ff;
        }
        else if(pos_or_len > VT_DIFF_MAX_INT12){
            first_op_byte.len_bits_type = VT_DIFF_LEN_BITS_20;
            first_op_byte.len_high_bits = (len >> 16) & 0x0f;
            *cur_op_prefix_buf++ = vt_diff01_from_first_op_byte(first_op_byte);
            *cur_op_prefix_buf++ = (len >> 8) & 0x0ff;
            *cur_op_prefix_buf++ = len & 0x0ff;
        }
        else{
            first_op_byte.len_bits_type = VT_DIFF_LEN_BITS_12;
            first_op_byte.len_high_bits = (len >> 8) & 0x0f;
            *cur_op_prefix_buf++ = vt_diff01_from_first_op_byte(first_op_byte);
            *cur_op_prefix_buf++ = len & 0x0ff;
        }

        switch(op){
            case VT_DIFF_OP_CP1:
            case VT_DIFF_OP_CP2:
                switch(first_op_byte.len_bits_type){
                    case VT_DIFF_LEN_BITS_12:
                        *cur_op_prefix_buf++ = (pos >> 8) & 0x0ff;
                        *cur_op_prefix_buf++ = pos & 0x0ff;
                        break;
                    case VT_DIFF_LEN_BITS_20:
                        *cur_op_prefix_buf++ = (pos >> 16) & 0x0ff;
                        *cur_op_prefix_buf++ = (pos >> 8) & 0x0ff;
                        *cur_op_prefix_buf++ = pos & 0x0ff;
                        break;
                    case VT_DIFF_LEN_BITS_28:
                        *cur_op_prefix_buf++ = (pos >> 24) & 0x0ff;
                        *cur_op_prefix_buf++ = (pos >> 16) & 0x0ff;
                        *cur_op_prefix_buf++ = (pos >> 8) & 0x0ff;
                        *cur_op_prefix_buf++ = pos & 0x0ff;
                        break;
                    case VT_DIFF_LEN_BITS_36:
                        *cur_op_prefix_buf++ = ((uint64_t)pos >> 32) & 0x0ff;
                        *cur_op_prefix_buf++ = (pos >> 24) & 0x0ff;
                        *cur_op_prefix_buf++ = (pos >> 16) & 0x0ff;
                        *cur_op_prefix_buf++ = (pos >> 8) & 0x0ff;
                        *cur_op_prefix_buf++ = pos & 0x0ff;
                        break;
                    default:
                        logError("The first_byte.len_bits_type(%u) is not supported", (unsigned int)first_op_byte.len_bits_type);
                        return 0;
                }
                break;
            case VT_DIFF_OP_ADD:
            case VT_DIFF_OP_PAD:
                break;
            default:
                logError("The first_byte.op(%u) is not supported", (unsigned int)first_op_byte.op);
                return 0;
        }

        return (cur_op_prefix_buf - op_prefix_buf);
    }

    uint8_t vt_diff01_from_op_prefix(uint8_t* op_ptr, size_t* len_ptr, size_t* pos_ptr, const char* op_prefix_buf){
        const char* cur_op_prefix_buf = op_prefix_buf;
        size_t i;
        vt_diff01_first_op_byte_t first_byte = vt_diff01_to_first_op_byte(*cur_op_prefix_buf++);
        *len_ptr = first_byte.len_high_bits;
        switch(first_byte.len_bits_type){
            case VT_DIFF_LEN_BITS_12:
                for(i = 0; i < 1; i ++){
                    *len_ptr <<= 8;
                    *len_ptr |= *(uint8_t*)cur_op_prefix_buf++;
                }
                break;
            case VT_DIFF_LEN_BITS_20:
                for(i = 0; i < 2; i ++){
                    *len_ptr <<= 8;
                    *len_ptr |= *(uint8_t*)cur_op_prefix_buf++;
                }
                break;
            case VT_DIFF_LEN_BITS_28:
                for(i = 0; i < 3; i ++){
                    *len_ptr <<= 8;
                    *len_ptr |= *(uint8_t*)cur_op_prefix_buf++;
                }
                break;
            case VT_DIFF_LEN_BITS_36:
                for(i = 0; i < 4; i ++){
                    *len_ptr <<= 8;
                    *len_ptr |= *(uint8_t*)cur_op_prefix_buf++;
                }
                break;
            default:
                logError("The first_byte.len_bits_type(%u) is not supported", (unsigned int)first_byte.len_bits_type);
                return 0;
        }

        *op_ptr = first_byte.op;
        switch (*op_ptr){
            case VT_DIFF_OP_CP1:
            case VT_DIFF_OP_CP2:
                *pos_ptr = 0;
                switch(first_byte.len_bits_type){
                    case VT_DIFF_LEN_BITS_12:
                        for(i = 0; i < 2; i ++){
                            *pos_ptr <<= 8;
                            *pos_ptr |= *(uint8_t*)cur_op_prefix_buf++;
                        }
                        break;
                    case VT_DIFF_LEN_BITS_20:
                        for(i = 0; i < 3; i ++){
                            *pos_ptr <<= 8;
                            *pos_ptr |= *(uint8_t*)cur_op_prefix_buf++;
                        }
                        break;
                    case VT_DIFF_LEN_BITS_28:
                        for(i = 0; i < 4; i ++){
                            *pos_ptr <<= 8;
                            *pos_ptr |= *(uint8_t*)cur_op_prefix_buf++;
                        }
                        break;
                    case VT_DIFF_LEN_BITS_36:
                        for(i = 0; i < 5; i ++){
                            *pos_ptr <<= 8;
                            *pos_ptr |= *(uint8_t*)cur_op_prefix_buf++;
                        }
                        break;
                    default:
                        logError("The first_byte.len_bits_type(%u) is not supported", (unsigned int)first_byte.len_bits_type);
                        return 0;
                }
                break;
            case VT_DIFF_OP_ADD:
            case VT_DIFF_OP_PAD:
                /** no pos parameter **/
                break;
            default:
                logError("The first_byte.op(%u) is not supported", (unsigned int)first_byte.op);
                return 0;
        }
        return (cur_op_prefix_buf - op_prefix_buf);
    }


__END_DECLS

#endif // DISABLE_VTDIFF01
