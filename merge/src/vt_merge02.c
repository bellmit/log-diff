
#ifndef DISABLE_VTDIFF02

#include "vtclib/vtmem.h"
#include "vtclib/vttime.h"
#include "vtclib/vtlog.h"
#include "vtclib/vtfile.h"
#include "vtclib/vtdiffcodec.h"
#include "vtclib/vtlzma.h"
#include "vtclib/vtbz2.h"
#include "vtmerge/oem_merge02.h"
#include "vtoemadpt/oem_flashfile.h"

#include <sys/types.h>
#include <sys/stat.h>
#if defined(_WIN32)
#include "vtclib/mman.h"
#else
#include <sys/mman.h>
#endif
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

#define VT_DIFF_FILEPATH_BUF_SIZE                   (16 * 1024)
#define VT_DIFF_HASH_TABLE_ITEM_RALLOC_INCREAMENT   (32 * 1024)
#define VT_DIFF_DELTADATUM_BUF_SIZE                 32

__BEGIN_DECLS

static int vt_file_merge02_impl(
    const char* filepath2,
    const char* filepath1,
    const char* meta_data_filepath,
    const char* delta_data_filepath,
    const char* extra_data_filepath
){
    int errnum = 0;
    int retcode = 0;

    vt_file_mmap_t* meta_data_file_mmap_ptr = (vt_file_mmap_t*)NULL;
    vt_diff_delta_file_header_t* meta_data_header_ptr = (vt_diff_delta_file_header_t*)NULL;
    uint32_t meta_data_offset;
    uint32_t meta_datum_count;
    uint32_t meta_data_length;
    uint32_t delta_data_length;
    uint32_t extra_data_length;
    vt_file_mmap_t* delta_data_file_mmap_ptr = (vt_file_mmap_t*)NULL;
    vt_file_mmap_t* extra_data_file_mmap_ptr = (vt_file_mmap_t*)NULL;
    vt_file_mmap_t* file1_mmap_ptr = (vt_file_mmap_t*)NULL;
    vt_file_mmap_t* file2_mmap_ptr = (vt_file_mmap_t*)NULL;
    uint32_t delta_data_offset;
    uint8_t* delta_data_file_mmap_addr;
    uint32_t extra_data_offset;
    uint32_t last_processed_file2_offset;
    uint8_t* file2_mmap_addr;
    uint32_t i, j;
    uint32_t block_len;
    uint32_t file1_offset;
    uint32_t file2_offset;
    uint8_t* ptr;
    uint8_t diff_op;
    uint32_t block_len_size;
    uint32_t file1_offset_size;
    uint32_t file2_offset_size;
    uint8_t* file1_mmap_addr;
    uint32_t extra_datum_len;

    if(sizeof(vt_diff_delta_file_header_t) != VT_DIFF_DELTA_FILE_HEADER_SIZE){
        logError("FIXME!");
        errnum = EINVAL;
        retcode = -1;
        goto done;
    }
    if(meta_data_filepath == (char*)NULL){
        logError("meta_data_filepath == (char*)NULL");
        errnum = EINVAL;
        retcode = -1;
        goto done;
    }
    if(delta_data_filepath == (char*)NULL){
        logError("delta_data_filepath == (char*)NULL");
        errnum = EINVAL;
        retcode = -1;
        goto done;
    }
    if(extra_data_filepath == (char*)NULL){
        logError("extra_data_filepath == (char*)NULL");
        errnum = EINVAL;
        retcode = -1;
        goto done;
    }
    if(filepath1 == (char*)NULL){
        logError("filepath1 == (char*)NULL");
        errnum = EINVAL;
        retcode = -1;
        goto done;
    }
    if(filepath2 == (char*)NULL){
        logError("filepath2 == (char*)NULL");
        errnum = EINVAL;
        retcode = -1;
        goto done;
    }
    ////
    meta_data_header_ptr = (vt_diff_delta_file_header_t*)malloc(sizeof(vt_diff_delta_file_header_t));
    if(meta_data_header_ptr == (vt_diff_delta_file_header_t*)NULL){
        errnum = errno;
        logError("Not enough virtual memory.");
        retcode = -1;
        goto done;
    }
    meta_data_file_mmap_ptr = vt_open_file_mmap(meta_data_filepath, O_RDONLY | O_BINARY, PROT_READ, MAP_PRIVATE);
    if(meta_data_file_mmap_ptr == (vt_file_mmap_t*)NULL){
        errnum = errno;
        logError("Failed at vt_open_file_mmap(%s)", delta_data_filepath);
        retcode = -1;
        goto done;
    }
    if(meta_data_file_mmap_ptr->addr == (void*)NULL || meta_data_file_mmap_ptr->size == 0){
        logError("File(%s) is empty", delta_data_filepath);
        errnum = EINVAL;
        retcode = -1;
        goto done;
    }
    //
    if(meta_data_file_mmap_ptr->size < VT_DIFF_DELTA_FILE_HEADER_SIZE){
        logError("The delta metadata is tamped.");
        errnum = EINVAL;
        retcode = -1;
        goto done;
    }
    vt_diff_buf_to_delta_file_header(meta_data_header_ptr, meta_data_file_mmap_ptr->addr);
    if(memcmp(meta_data_header_ptr->diff_mark, VT_DIFF_DELTA_MARK_V02, strlen(VT_DIFF_DELTA_MARK_V02)) != 0){
        logError("The delta metadata is tamped.");
        errnum = EINVAL;
        retcode = -1;
        goto done;
    }
    meta_data_offset = VT_DIFF_DELTA_FILE_HEADER_SIZE;
    meta_datum_count = meta_data_header_ptr->meta_datum_count;
    meta_data_length = meta_data_header_ptr->meta_data_size;
    delta_data_length = meta_data_header_ptr->delta_data_size;
    extra_data_length = meta_data_header_ptr->extra_data_size;

    logDebug("The meta_datum_count: %lu", (unsigned long)meta_datum_count);
    logDebug("The meta_data_length: %lu", (unsigned long)meta_data_length);
    logDebug("The delta_data_length: %lu", (unsigned long)delta_data_length);
    logDebug("The extra_data_length: %lu", (unsigned long)extra_data_length);
    if((VT_DIFF_DELTA_FILE_HEADER_SIZE + meta_data_length) != (meta_data_file_mmap_ptr->size)){
        logError(
            "(VT_DIFF_DELTA_FILE_HEADER_SIZE:%lu + meta_data_length:%lu):%lu != (meta_data_file_mmap_ptr->size):%lu",
            (unsigned long)VT_DIFF_DELTA_FILE_HEADER_SIZE,
            (unsigned long)meta_data_length,
            (unsigned long)(VT_DIFF_DELTA_FILE_HEADER_SIZE + meta_data_length),
            (unsigned long)(meta_data_file_mmap_ptr->size)
        );
        logError("The delta metadata is tamped.");
        errnum = EINVAL;
        retcode = -1;
        goto done;
    }

    ////
    delta_data_file_mmap_ptr = vt_open_file_mmap(delta_data_filepath, O_RDONLY | O_BINARY, PROT_READ, MAP_PRIVATE);
    if(delta_data_file_mmap_ptr == (vt_file_mmap_t*)NULL){
        errnum = errno;
        logError("Failed at vt_open_file_mmap(%s): %s", delta_data_filepath, strerror(errnum));
        retcode = -1;
        goto done;
    }
    if(delta_data_file_mmap_ptr->addr == (void*)NULL && delta_data_file_mmap_ptr->size > 0){
        logError("FIXME!");
        errnum = EINVAL;
        retcode = -1;
        goto done;
    }
    if(delta_data_file_mmap_ptr->size == 0){
        logInfo("File(%s) is empty", delta_data_filepath);
    }
    ////
    if(delta_data_length != delta_data_file_mmap_ptr->size){
        errnum = EINVAL;
        logError("Delta data file (%s) is tamped.", delta_data_filepath);
        retcode = -1;
        goto done;
    }

    ////
    extra_data_file_mmap_ptr = vt_open_file_mmap(extra_data_filepath, O_RDONLY | O_BINARY, PROT_READ, MAP_PRIVATE);
    if(extra_data_file_mmap_ptr == (vt_file_mmap_t*)NULL){
        errnum = errno;
        logError("Failed at vt_open_file_mmap(%s)", extra_data_filepath);
        retcode = -1;
        goto done;
    }
    if(extra_data_file_mmap_ptr->addr == (void*)NULL && extra_data_file_mmap_ptr->size > 0){
        logError("FIXME!");
        errnum = EINVAL;
        retcode = -1;
        goto done;
    }
    if(extra_data_file_mmap_ptr->size == 0){
        logInfo("File(%s) is empty", extra_data_filepath);
    }
    ////
    if(extra_data_length != extra_data_file_mmap_ptr->size){
        errnum = errno;
        logError("Extra data file (%s) is tamped.", extra_data_filepath);
        retcode = -1;
        goto done;
    }

    //// memory map for filepath1
    file1_mmap_ptr = vt_open_file_mmap(filepath1, O_RDONLY | O_BINARY, PROT_READ, MAP_PRIVATE);
    if(file1_mmap_ptr == (vt_file_mmap_t*)NULL){
        errnum = errno;
        logError("Failed at vt_open_file_mmap(%s)", filepath1);
        retcode = -1;
        goto done;
    }
    if(file1_mmap_ptr->addr == (void*)NULL && file1_mmap_ptr->size > 0){
        logError("FIXME!");
        errnum = EINVAL;
        retcode = -1;
        goto done;
    }
    if(file1_mmap_ptr->size == 0){
        logInfo("File(%s) is empty", filepath1);
    }
    ////

    //// copy delta_data_filepath to filepath2
    if(vt_remove_if_existed(filepath2) < 0){
        errnum = errno;
        logError("Failed at remove: %s", filepath2);
        retcode = -1;
        goto done;
    }
    if(vt_fput_file(
        filepath2,
        "wb", //O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH,
        -1,
        delta_data_file_mmap_ptr->addr,
        delta_data_file_mmap_ptr->size
    ) < 0){
        errnum = errno;
        logError("Failed at copy file:%s to file:%s", delta_data_filepath, filepath2);
        retcode = -1;
        goto done;
    }
    if(vt_fput_file(
        filepath2,
        "ab", //O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH,
        -1,
        extra_data_file_mmap_ptr->addr,
        extra_data_file_mmap_ptr->size
    ) < 0){
        errnum = errno;
        logError("Failed at copy file:%s to file:%s", delta_data_filepath, filepath2);
        retcode = -1;
        goto done;
    }
    if(delta_data_file_mmap_ptr->size + extra_data_file_mmap_ptr->size == 0){
        //filepath2 must be an empty file.
        goto done;
    }
    //// memory map filepath2
    file2_mmap_ptr = vt_open_file_mmap(filepath2, O_RDWR | O_BINARY, PROT_READ | PROT_WRITE, MAP_SHARED);
    if(file2_mmap_ptr == (vt_file_mmap_t*)NULL){
        errnum = errno;
        logError("Failed at vt_open_file_mmap(%s)", filepath2);
        retcode = -1;
        goto done;
    }
    if(file2_mmap_ptr->addr == (void*)NULL || file2_mmap_ptr->size == 0){
        logError("File(%s) is empty", filepath2);
        errnum = EINVAL;
        retcode = -1;
        goto done;
    }
    if(delta_data_file_mmap_ptr->size + extra_data_file_mmap_ptr->size != file2_mmap_ptr->size){
        logError("FIXME! For file(%s), delta_data_file_mmap_ptr->size:%lu + extra_data_file_mmap_ptr->size:%lu != file2_mmap_ptr->size:%lu",
            filepath2,
            (unsigned long)(delta_data_file_mmap_ptr->size),
            (unsigned long)(extra_data_file_mmap_ptr->size),
            (unsigned long)(file2_mmap_ptr->size)
        );
        errnum = EINVAL;
        retcode = -1;
        goto done;
    }

    ////
    delta_data_offset = 0;
    delta_data_file_mmap_addr = (uint8_t*)(delta_data_file_mmap_ptr->addr);
    extra_data_offset = 0;
    last_processed_file2_offset = (uint32_t)(-1);
    file2_mmap_addr = (uint8_t*)(file2_mmap_ptr->addr);

    for(i = 0; i < meta_datum_count; i ++){
        if(meta_data_offset >= meta_data_file_mmap_ptr->size){
            logError("Delta file is tamped.");
            errnum = EINVAL;
            retcode = -1;
            goto done;
        }

        block_len = 0;
        file1_offset = 0;
        file2_offset = 0;
        ptr = (uint8_t*)(meta_data_file_mmap_ptr->addr) + meta_data_offset;
        diff_op = vt_diff_codec_get_op(ptr);
        block_len_size = vt_diff_codec_get_op_block_len(&block_len, ptr);
        ptr += block_len_size;
        file1_offset_size = vt_diff_codec_get_offset32(&file1_offset, ptr);
        ptr += file1_offset_size;
        file2_offset_size = vt_diff_codec_get_offset32(&file2_offset, ptr);
        ptr += file2_offset_size;

        if(block_len == 0){
            logError("Delta file is tamped.");
            errnum = EINVAL;
            retcode = -1;
            goto done;
        }
        if(file1_offset + block_len > file1_mmap_ptr->size){
            logError("Delta file is tamped.");
            errnum = EINVAL;
            retcode = -1;
            goto done;
        }
        if(file2_offset + block_len > file2_mmap_ptr->size){
            logError("Delta file is tamped.");
            errnum = EINVAL;
            retcode = -1;
            goto done;
        }

        extra_datum_len = 0;
        if(last_processed_file2_offset + 1 < file2_offset){
            extra_datum_len = file2_offset - (last_processed_file2_offset + 1);
            if(extra_data_offset + extra_datum_len > extra_data_file_mmap_ptr->size){
                logError("Delta file is tamped.");
                errnum = EINVAL;
                retcode = -1;
                goto done;
            }
            memcpy(file2_mmap_addr, (uint8_t*)(extra_data_file_mmap_ptr->addr) +  extra_data_offset, extra_datum_len);
            file2_mmap_addr += extra_datum_len;
            extra_data_offset += extra_datum_len;
            last_processed_file2_offset += extra_datum_len;
        }

        file1_mmap_addr = (uint8_t*)(file1_mmap_ptr->addr) + file1_offset;
        for(j = 0; j < block_len; j ++){
            switch(diff_op){
                case VT_DIFF_OP_SND:
                    *file2_mmap_addr++ = (*delta_data_file_mmap_addr++) + (*file1_mmap_addr++);
                    break;
                case VT_DIFF_OP_XOR:
                    *file2_mmap_addr++ = (*delta_data_file_mmap_addr++) ^ (*file1_mmap_addr++);
                    break;
                default:
                    logError("Delta file is tamped. diff_op(%x) is invalid!", (unsigned int)diff_op);
                    errnum = EINVAL;
                    retcode = -1;
                    goto done;
            }
            //*file2_mmap_addr++ = (*delta_data_file_mmap_addr++) ^ (*file1_mmap_addr++);
            //*file2_mmap_addr++ = (*delta_data_file_mmap_addr++) + (*file1_mmap_addr++);
        }
        delta_data_offset += block_len;
        last_processed_file2_offset += block_len;

        if(i == meta_datum_count - 1){
            extra_datum_len = 0;
            if(last_processed_file2_offset + 1 < file2_mmap_ptr->size){
                extra_datum_len = file2_mmap_ptr->size - (last_processed_file2_offset + 1);
                if(extra_data_offset + extra_datum_len > extra_data_file_mmap_ptr->size){
                    logError("Delta file is tamped.");
                    errnum = EINVAL;
                    retcode = -1;
                    goto done;
                }
                memcpy(file2_mmap_addr, (uint8_t*)(extra_data_file_mmap_ptr->addr) +  extra_data_offset, extra_datum_len);
                file2_mmap_addr += extra_datum_len;
                extra_data_offset += extra_datum_len;
                last_processed_file2_offset += extra_datum_len;
            }
        }

        meta_data_offset += (block_len_size + file1_offset_size + file2_offset_size);
    }

    done:
    if(meta_data_header_ptr == (vt_diff_delta_file_header_t*)NULL){
        free(meta_data_header_ptr);
        meta_data_header_ptr = (vt_diff_delta_file_header_t*)NULL;
    }
    if(meta_data_file_mmap_ptr != (vt_file_mmap_t*)NULL){
        vt_close_file_mmap(meta_data_file_mmap_ptr);
        meta_data_file_mmap_ptr = (vt_file_mmap_t*)NULL;
    }
    if(delta_data_file_mmap_ptr != (vt_file_mmap_t*)NULL){
        vt_close_file_mmap(delta_data_file_mmap_ptr);
        delta_data_file_mmap_ptr = (vt_file_mmap_t*)NULL;
    }
    if(extra_data_file_mmap_ptr != (vt_file_mmap_t*)NULL){
        vt_close_file_mmap(extra_data_file_mmap_ptr);
        extra_data_file_mmap_ptr = (vt_file_mmap_t*)NULL;
    }
    if(file1_mmap_ptr != (vt_file_mmap_t*)NULL){
        vt_close_file_mmap(file1_mmap_ptr);
        file1_mmap_ptr = (vt_file_mmap_t*)NULL;
    }
    if(file2_mmap_ptr != (vt_file_mmap_t*)NULL){
        vt_close_file_mmap(file2_mmap_ptr);
        file2_mmap_ptr = (vt_file_mmap_t*)NULL;
    }
    errno = errnum;
    return retcode;
}

int vt_sjaonse_file_merge02(const char* filepath2,
										const char* filepath1,
										const char* delta_filepath,
										int need_temp_files){
    int errnum = 0;
    int retcode = 0;

    vt_file_mmap_t* delta_file_mmap_ptr = (vt_file_mmap_t*)NULL;
    vt_diff_delta_file_header_t* meta_data_header_ptr = (vt_diff_delta_file_header_t*)NULL;
    //uint32_t meta_datum_count;
    uint32_t meta_data_length;
    uint32_t delta_data_length;
    uint32_t extra_data_length;
    uint32_t delta_file_size;
    char* meta_data_filepath = (char*)NULL;
    char* delta_data_filepath = (char*)NULL;
    char* extra_data_filepath = (char*)NULL;

    meta_data_header_ptr = (vt_diff_delta_file_header_t*)malloc(sizeof(vt_diff_delta_file_header_t));
    if(meta_data_header_ptr == (vt_diff_delta_file_header_t*)NULL){
        errnum = errno;
        logError("Not enough virtual memory");
        retcode = -1;
        goto done;
    }
    delta_file_mmap_ptr = vt_open_file_mmap(delta_filepath, O_RDONLY | O_BINARY, PROT_READ, MAP_PRIVATE);
    if(delta_file_mmap_ptr == (vt_file_mmap_t*)NULL){
        errnum = errno;
        logError("Failed at vt_open_file_mmap(%s)", delta_filepath);
        retcode = -1;
        goto done;
    }
    if(delta_file_mmap_ptr->addr == (void*)NULL || delta_file_mmap_ptr->size == 0){
        logError("File(%s) is empty", delta_filepath);
        errnum = EINVAL;
        retcode = -1;
        goto done;
    }
    if(delta_file_mmap_ptr->size < VT_DIFF_DELTA_FILE_HEADER_SIZE){
        logError("The delta metadata is tamped.");
        errnum = EINVAL;
        retcode = -1;
        goto done;
    }
    vt_diff_buf_to_delta_file_header(meta_data_header_ptr, delta_file_mmap_ptr->addr);
    if(memcmp(meta_data_header_ptr->diff_mark, VT_DIFF_DELTA_MARK_V02, strlen(VT_DIFF_DELTA_MARK_V02)) != 0){
        logError("The delta metadata is tamped.");
        errnum = EINVAL;
        retcode = -1;
        goto done;
    }

    //meta_datum_count = meta_data_header_ptr->meta_datum_count;
    meta_data_length = meta_data_header_ptr->meta_data_size;
    delta_data_length = meta_data_header_ptr->delta_data_size;
    extra_data_length = meta_data_header_ptr->extra_data_size;

    //logDebug("The meta_datum_count: %lu", (unsigned long)meta_datum_count);
    logDebug("The meta_data_length: %lu", (unsigned long)meta_data_length);
    logDebug("The delta_data_length: %lu", (unsigned long)delta_data_length);
    logDebug("The extra_data_length: %lu", (unsigned long)extra_data_length);
    delta_file_size = VT_DIFF_DELTA_FILE_HEADER_SIZE + meta_data_length + delta_data_length + extra_data_length;
    if(delta_file_mmap_ptr->size != delta_file_size){
        logError("delta_file_mmap_ptr->size:%lu != delta_file_size:%lu, delta_filepath:%s is tamped.",
            (unsigned long)(delta_file_mmap_ptr->size),
            (unsigned long)delta_file_size,
            delta_filepath
        );
        errnum = EINVAL;
        retcode = -1;
        goto done;
    }


    if((meta_data_filepath = (char*)malloc(VT_DIFF_FILEPATH_BUF_SIZE)) == (char*)NULL){
        errnum = errno;
        retcode = -1;
        goto done;
    }
    if((delta_data_filepath = (char*)malloc(VT_DIFF_FILEPATH_BUF_SIZE)) == (char*)NULL){
        errnum = errno;
        retcode = -1;
        goto done;
    }
    if((extra_data_filepath = (char*)malloc(VT_DIFF_FILEPATH_BUF_SIZE)) == (char*)NULL){
        errnum = errno;
        retcode = -1;
        goto done;
    }
    memset(meta_data_filepath, 0, VT_DIFF_FILEPATH_BUF_SIZE);
    memset(delta_data_filepath, 0, VT_DIFF_FILEPATH_BUF_SIZE);
    memset(extra_data_filepath, 0, VT_DIFF_FILEPATH_BUF_SIZE);
    if(logDebugEnabled() && need_keep_interim_temp_files){
        strcpy(meta_data_filepath, delta_filepath);
        strcpy(delta_data_filepath, delta_filepath);
        strcpy(extra_data_filepath, delta_filepath);
    }
    else{
        vt_gen_tmp_filepath(meta_data_filepath, VT_DIFF_FILEPATH_BUF_SIZE);
        vt_gen_tmp_filepath(delta_data_filepath, VT_DIFF_FILEPATH_BUF_SIZE);
        vt_gen_tmp_filepath(extra_data_filepath, VT_DIFF_FILEPATH_BUF_SIZE);
    }
    strcat(meta_data_filepath, ".md");
    strcat(delta_data_filepath, ".dd");
    strcat(extra_data_filepath, ".ed");

    if(need_keep_interim_temp_files){
        logNotice("Don't forget removing meta_data_filepath: '%s'", meta_data_filepath);
        logNotice("Don't forget removing delta_data_filepath: '%s'", delta_data_filepath);
        logNotice("Don't forget removing extra_data_filepath: '%s'", extra_data_filepath);
    }
    else{
        logDebug("meta_data_filepath: '%s'", meta_data_filepath);
        logDebug("delta_data_filepath: '%s'", delta_data_filepath);
        logDebug("extra_data_filepath: '%s'", extra_data_filepath);
    }

    if(vt_remove_if_existed(meta_data_filepath) < 0){
        errnum = errno;
        logError("Failed at remove file: %s", meta_data_filepath);
        retcode = -1;
        goto done;
    }
    if(vt_fput_file(
        meta_data_filepath,
        "wb", //O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH,
        -1,
        delta_file_mmap_ptr->addr,
        VT_DIFF_DELTA_FILE_HEADER_SIZE + meta_data_length
    ) < 0){
        errnum = errno;
        logError("Failed at copy file:%s to file:%s", delta_filepath, meta_data_filepath);
        retcode = -1;
        goto done;
    }

    if(vt_remove_if_existed(delta_data_filepath) < 0){
        errnum = errno;
        logError("Failed at remove file: %s", delta_data_filepath);
        retcode = -1;
        goto done;
    }
    if(vt_fput_file(
        delta_data_filepath,
        "wb", //O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH,
        -1,
        (uint8_t*)(delta_file_mmap_ptr->addr) + (VT_DIFF_DELTA_FILE_HEADER_SIZE + meta_data_length),
        delta_data_length
    ) < 0){
        errnum = errno;
        logError("Failed at copy file:%s to file:%s", delta_filepath, delta_data_filepath);
        retcode = -1;
        goto done;
    }

    if(vt_remove_if_existed(extra_data_filepath) < 0){
        errnum = errno;
        logError("Failed at remove file: %s", extra_data_filepath);
        retcode = -1;
        goto done;
    }
    if(vt_fput_file(
        extra_data_filepath,
        "wb", //O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH,
        -1,
        (uint8_t*)(delta_file_mmap_ptr->addr) + (VT_DIFF_DELTA_FILE_HEADER_SIZE + meta_data_length + delta_data_length),
        extra_data_length
    ) < 0){
        errnum = errno;
        logError("Failed at copy file:%s to file:%s", delta_filepath, extra_data_filepath);
        retcode = -1;
        goto done;
    }

    if(vt_file_merge02_impl(
        filepath2,
        filepath1,
        meta_data_filepath,
        delta_data_filepath,
        extra_data_filepath
    ) < 0){
        errnum = errno;
        logError("Failed in vt_file_merge02_impl, errno(%d), error(%s)", errnum, strerror(errnum));
        retcode = -1;
        goto done;
    }

    done:
    if(meta_data_header_ptr != (vt_diff_delta_file_header_t*)NULL){
        free(meta_data_header_ptr);
        meta_data_header_ptr = (vt_diff_delta_file_header_t*)NULL;
    }
    if(delta_file_mmap_ptr != (vt_file_mmap_t*)NULL){
        vt_close_file_mmap(delta_file_mmap_ptr);
        delta_file_mmap_ptr = (vt_file_mmap_t*)NULL;
    }
    if(!need_keep_interim_temp_files){
        if(meta_data_filepath != (char*)NULL){
            remove(meta_data_filepath);
        }
        if(delta_data_filepath != (char*)NULL){
            remove(delta_data_filepath);
        }
        if(extra_data_filepath != (char*)NULL){
            remove(extra_data_filepath);
        }
    }
    if(meta_data_filepath != (char*)NULL){
        free(meta_data_filepath);
        meta_data_filepath = (char*)NULL;
    }
    if(delta_data_filepath != (char*)NULL){
        free(delta_data_filepath);
        delta_data_filepath = (char*)NULL;
    }
    if(extra_data_filepath != (char*)NULL){
        free(extra_data_filepath);
        extra_data_filepath = (char*)NULL;
    }
    errno = errnum;
    return retcode;
}

#ifndef DISABLE_VTLZMA

int vt_sjaonse_file_lzma_merge02(const char* filepath2,
												 const char* filepath1,
												 const char* delta_filepath,
												 int need_temp_files)
{
    int errnum = 0;
    int retcode = 0;

    vt_diff_delta_file_header_t* meta_data_header_ptr = (vt_diff_delta_file_header_t*)NULL;
    uint8_t* meta_data_header_buf = (uint8_t*)NULL;
    FILE* delta_fp  = (FILE*)NULL;
    char* uncompressed_delta_filepath = (char*)NULL;
    FILE* uncompressed_delta_fp = (FILE*)NULL;
    uint32_t supposed_delta_filesize;
    uint32_t supposed_uncompressed_delta_filesize;
    vtlzma_ios_t ios;
    off_t delta_filesize;
    off_t uncompressed_delta_filesize;

    if(sizeof(vt_diff_delta_file_header_t) != VT_DIFF_DELTA_FILE_HEADER_SIZE){
        logError("FIXME!");
        errnum = EINVAL;
        retcode = -1;
        goto done;
    }
    //
    if((meta_data_header_ptr = (vt_diff_delta_file_header_t*)malloc(sizeof(vt_diff_delta_file_header_t))) == (vt_diff_delta_file_header_t*)NULL){
        errnum = errno;
        logError("Not enough virtual memory.");
        retcode = -1;
        goto done;
    }
    if((meta_data_header_buf = (uint8_t*)malloc(VT_DIFF_DELTA_FILE_HEADER_SIZE)) == (uint8_t*)NULL){
        errnum = errno;
        logError("Not enough virtual memory.");
        retcode = -1;
        goto done;
    }

    memset(meta_data_header_ptr, 0, VT_DIFF_DELTA_FILE_HEADER_SIZE);
    memset(meta_data_header_buf, 0, VT_DIFF_DELTA_FILE_HEADER_SIZE);
    //
   //BEGIN of get the header
    delta_fp = oem_fopen(delta_filepath, "rb");
    if(delta_fp == (FILE*)NULL){
        errnum = errno;
        logError("Failed at open file:%s, errno:%d, error:%s", delta_filepath, errnum, strerror(errnum));
        retcode = -1;
        goto done;
    }
    if(fread(meta_data_header_buf, 1, VT_DIFF_DELTA_FILE_HEADER_SIZE, delta_fp) != VT_DIFF_DELTA_FILE_HEADER_SIZE){
        errnum = errno;
        logError("Failed at read file:%s, errno:%d, error:%s", delta_filepath, errnum, strerror(errnum));
        retcode = -1;
        goto done;
    }
    vt_diff_buf_to_delta_file_header(meta_data_header_ptr, meta_data_header_buf);
    fclose(delta_fp);
    delta_fp = (FILE*)NULL;
    //END of get header
    //
    if(strncasecmp(VT_DIFF_COMPR_MARK_LZMA, meta_data_header_ptr->cmpr_mark, VT_DIFF_COMPR_MARK_LEN) != 0){
        logError("File %s is tamped or it is not compressed.", delta_filepath);
        errnum = EINVAL;
        retcode = -1;
        goto done;
    }
    //
    if((uncompressed_delta_filepath = (char*)malloc(VT_DIFF_FILEPATH_BUF_SIZE)) == (char*)NULL){
        errnum = errno;
        logError("Not enough virtual memory.");
        retcode = -1;
        goto done;
    }
    memset(uncompressed_delta_filepath, 0, VT_DIFF_FILEPATH_BUF_SIZE);
    if(logDebugEnabled() && need_keep_interim_temp_files){
        strcpy(uncompressed_delta_filepath, delta_filepath);
    }
    else{
        vt_gen_tmp_filepath(uncompressed_delta_filepath, VT_DIFF_FILEPATH_BUF_SIZE);
    }
    strcat(uncompressed_delta_filepath, ".ud");
    if(need_keep_interim_temp_files){
        logNotice("Don't forget removing uncompressed_delta_filepath: '%s'", uncompressed_delta_filepath);
    }
    else{
        logDebug("uncompressed_delta_filepath: '%s'", uncompressed_delta_filepath);
    }

    // work out the uncompressed_delta_filepath
    if(vt_remove_if_existed(uncompressed_delta_filepath) < 0){
        errnum = errno;
        logError("Failed at remove: %s", uncompressed_delta_filepath);
        retcode = -1;
        goto done;
    }
    uncompressed_delta_fp = oem_fopen(uncompressed_delta_filepath, "wb");
    if(uncompressed_delta_fp == (FILE*)NULL){
        errnum = errno;
        logError("Failed at open file:%s, errno:%d, error:%s", uncompressed_delta_filepath, errnum, strerror(errnum));
        retcode = -1;
        goto done;
    }

    delta_fp = oem_fopen(delta_filepath, "rb");
    if(delta_fp == (FILE*)NULL){
        errnum = errno;
        logError("Failed at open file:%s, errno:%d, error:%s", delta_filepath, errnum, strerror(errnum));
        retcode = -1;
        goto done;
    }

    supposed_delta_filesize = VT_DIFF_DELTA_FILE_HEADER_SIZE + meta_data_header_ptr->compressed_size;
    supposed_uncompressed_delta_filesize = VT_DIFF_DELTA_FILE_HEADER_SIZE + meta_data_header_ptr->meta_data_size + meta_data_header_ptr->delta_data_size + meta_data_header_ptr->extra_data_size;

    meta_data_header_ptr->compressed_size = 0;
    memset(meta_data_header_ptr->cmpr_mark, 0, VT_DIFF_COMPR_MARK_LEN);
    vt_diff_delta_file_header_to_buf(meta_data_header_buf, meta_data_header_ptr);

    if(fwrite(meta_data_header_buf, 1, VT_DIFF_DELTA_FILE_HEADER_SIZE, uncompressed_delta_fp) != VT_DIFF_DELTA_FILE_HEADER_SIZE){
        errnum = errno;
        logError("Failed to write file:%s, errno:%d, error:%s",uncompressed_delta_filepath, errnum, strerror(errnum));
        retcode = -1;
        goto done;
    }
    if(fseeko(delta_fp, VT_DIFF_DELTA_FILE_HEADER_SIZE, SEEK_SET) < 0){
        errnum = errno;
        logError("Failed to fseeko file:%s, errno:%d, error:%s", delta_filepath, errnum, strerror(errnum));
        retcode = -1;
        goto done;
    }

    memset(&ios, 0, sizeof(vtlzma_ios_t));
    if(get_vtlzma_ios_on_file_impl(&ios) < 0){
        logError("Failed at get_vtlzma_ios_on_file_impl");
        errnum = EINVAL;
        retcode = -1;
        goto done;
    }
    if(vtlzma_decompress(&ios, 2, delta_fp, uncompressed_delta_fp) < 0){
        logError("Failed at vtlzma_decompress");
        errnum = EINVAL;
        retcode = -1;
        goto done;
    }
    delta_filesize = ftello(delta_fp);
    if(delta_filesize < VT_DIFF_DELTA_FILE_HEADER_SIZE){
        errnum = EINVAL;
        logError("Failed at ftello file:%s, errno:%d, error:%s", delta_filepath, errnum, strerror(errnum));
        retcode = -1;
        goto done;
    }
    if(supposed_delta_filesize != (uint32_t)delta_filesize){
        logError("supposed_delta_filesize:%lu != delta_filesize:%lu",
            (unsigned long)supposed_delta_filesize,
            (unsigned long)delta_filesize
        );
        errnum = EINVAL;
        retcode = -1;
        goto done;
    }

    uncompressed_delta_filesize = ftello(uncompressed_delta_fp);
    if(uncompressed_delta_filesize < VT_DIFF_DELTA_FILE_HEADER_SIZE){
        errnum = EINVAL;
        logError("Failed at ftello file:%s, errno:%d, error:%s", uncompressed_delta_filepath, errnum, strerror(errnum));
        retcode = -1;
        goto done;
    }
    if(supposed_uncompressed_delta_filesize != uncompressed_delta_filesize){
        logError("supposed_uncompressed_delta_filesize:%lu != uncompressed_delta_filesize:%lu",
            (unsigned long)supposed_uncompressed_delta_filesize,
            (unsigned long)uncompressed_delta_filesize
        );
        errnum = EINVAL;
        retcode = -1;
        goto done;
    }
    //
    if(uncompressed_delta_fp != (FILE*)NULL){
        //flush and close it
        fflush(uncompressed_delta_fp);
        fclose(uncompressed_delta_fp);
        uncompressed_delta_fp = (FILE*)NULL;
    }
    if(delta_fp != (FILE*)NULL){
        //close it.
        fclose(delta_fp);
        delta_fp = (FILE*)NULL;
    }
    //
    if(vt_file_merge02(filepath2, filepath1, uncompressed_delta_filepath, need_keep_interim_temp_files) < 0){
        logError("Failed in vt_file_merge02");
        errnum = EINVAL;
        retcode = -1;
        goto done;
    }

    done:
    if(uncompressed_delta_fp != (FILE*)NULL){
        fclose(uncompressed_delta_fp);
        uncompressed_delta_fp = (FILE*)NULL;
    }
    if(delta_fp != (FILE*)NULL){
        fclose(delta_fp);
        delta_fp = (FILE*)NULL;
    }
    //
    if(!need_keep_interim_temp_files){
        if(uncompressed_delta_filepath != (char*)NULL){
            remove(uncompressed_delta_filepath);
        }
    }
    if(uncompressed_delta_filepath != (char*)NULL){
        free(uncompressed_delta_filepath);
        uncompressed_delta_filepath = (char*)NULL;
    }
    if(meta_data_header_buf != (uint8_t*)NULL){
        free(meta_data_header_buf);
        meta_data_header_buf = (uint8_t*)NULL;
    }
    if(meta_data_header_ptr != (vt_diff_delta_file_header_t*)NULL){
        free(meta_data_header_ptr);
        meta_data_header_ptr = (vt_diff_delta_file_header_t*)NULL;
    }

    errno = errnum;
    return retcode;
}

#endif // DISABLE_VTLZMA

#ifndef DISABLE_VTBZ2

int vt_sjaonse_file_bz2_merge02(const char* filepath2,
												const char* filepath1,
												const char* delta_filepath,
												int need_temp_files)
{
    int errnum = 0;
    int retcode = 0;

    vt_diff_delta_file_header_t* meta_data_header_ptr = (vt_diff_delta_file_header_t*)NULL;
    uint8_t* meta_data_header_buf = (uint8_t*)NULL;
    FILE* delta_fp  = (FILE*)NULL;
    char* uncompressed_delta_filepath = (char*)NULL;
    FILE* uncompressed_delta_fp = (FILE*)NULL;
    uint32_t supposed_delta_filesize;
    uint32_t supposed_uncompressed_delta_filesize;
    vtbz2_ios_t ios;
    off_t delta_filesize;
    off_t uncompressed_delta_filesize;

    if(sizeof(vt_diff_delta_file_header_t) != VT_DIFF_DELTA_FILE_HEADER_SIZE){
        logError("FIXME!");
        errnum = EINVAL;
        retcode = -1;
        goto done;
    }
    //
    if((meta_data_header_ptr = (vt_diff_delta_file_header_t*)malloc(sizeof(vt_diff_delta_file_header_t))) == (vt_diff_delta_file_header_t*)NULL){
        errnum = errno;
        logError("Not enough virtual memory.");
        retcode = -1;
        goto done;
    }
    if((meta_data_header_buf = (uint8_t*)malloc(VT_DIFF_DELTA_FILE_HEADER_SIZE)) == (uint8_t*)NULL){
        errnum = errno;
        logError("Not enough virtual memory.");
        retcode = -1;
        goto done;
    }

    memset(meta_data_header_ptr, 0, VT_DIFF_DELTA_FILE_HEADER_SIZE);
    memset(meta_data_header_buf, 0, VT_DIFF_DELTA_FILE_HEADER_SIZE);
    //
   //BEGIN of get the header
    delta_fp = oem_fopen(delta_filepath, "rb");
    if(delta_fp == (FILE*)NULL){
        errnum = errno;
        logError("Failed at open file:%s, errno:%d, error:%s", delta_filepath, errnum, strerror(errnum));
        retcode = -1;
        goto done;
    }
    if(fread(meta_data_header_buf, 1, VT_DIFF_DELTA_FILE_HEADER_SIZE, delta_fp) != VT_DIFF_DELTA_FILE_HEADER_SIZE){
        errnum = errno;
        logError("Failed at read file:%s, errno:%d, error:%s", delta_filepath, errnum, strerror(errnum));
        retcode = -1;
        goto done;
    }
    vt_diff_buf_to_delta_file_header(meta_data_header_ptr, meta_data_header_buf);
    fclose(delta_fp);
    delta_fp = (FILE*)NULL;
    //END of get header
    //
    if(strncasecmp(VT_DIFF_COMPR_MARK_BZ2, meta_data_header_ptr->cmpr_mark, VT_DIFF_COMPR_MARK_LEN) != 0){
        logError("File %s is tamped or it is not compressed.", delta_filepath);
        errnum = EINVAL;
        retcode = -1;
        goto done;
    }
    //
    if((uncompressed_delta_filepath = (char*)malloc(VT_DIFF_FILEPATH_BUF_SIZE)) == (char*)NULL){
        errnum = errno;
        logError("Not enough virtual memory.");
        retcode = -1;
        goto done;
    }
    memset(uncompressed_delta_filepath, 0, VT_DIFF_FILEPATH_BUF_SIZE);
    if(logDebugEnabled() && need_keep_interim_temp_files){
        strcpy(uncompressed_delta_filepath, delta_filepath);
    }
    else{
        vt_gen_tmp_filepath(uncompressed_delta_filepath, VT_DIFF_FILEPATH_BUF_SIZE);
    }
    strcat(uncompressed_delta_filepath, ".ud");
    if(need_keep_interim_temp_files){
        logNotice("Don't forget removing uncompressed_delta_filepath: '%s'", uncompressed_delta_filepath);
    }
    else{
        logDebug("uncompressed_delta_filepath: '%s'", uncompressed_delta_filepath);
    }

    // work out the uncompressed_delta_filepath
    if(vt_remove_if_existed(uncompressed_delta_filepath) < 0){
        errnum = errno;
        logError("Failed at remove: %s", uncompressed_delta_filepath);
        retcode = -1;
        goto done;
    }
    uncompressed_delta_fp = oem_fopen(uncompressed_delta_filepath, "wb");
    if(uncompressed_delta_fp == (FILE*)NULL){
        errnum = errno;
        logError("Failed at open file:%s, errno:%d, error:%s", uncompressed_delta_filepath, errnum, strerror(errnum));
        retcode = -1;
        goto done;
    }

    delta_fp = oem_fopen(delta_filepath, "rb");
    if(delta_fp == (FILE*)NULL){
        errnum = errno;
        logError("Failed at open file:%s, errno:%d, error:%s", delta_filepath, errnum, strerror(errnum));
        retcode = -1;
        goto done;
    }

    supposed_delta_filesize = VT_DIFF_DELTA_FILE_HEADER_SIZE + meta_data_header_ptr->compressed_size;
    supposed_uncompressed_delta_filesize = VT_DIFF_DELTA_FILE_HEADER_SIZE + meta_data_header_ptr->meta_data_size + meta_data_header_ptr->delta_data_size + meta_data_header_ptr->extra_data_size;

    meta_data_header_ptr->compressed_size = 0;
    memset(meta_data_header_ptr->cmpr_mark, 0, VT_DIFF_COMPR_MARK_LEN);
    vt_diff_delta_file_header_to_buf(meta_data_header_buf, meta_data_header_ptr);

    if(fwrite(meta_data_header_buf, 1, VT_DIFF_DELTA_FILE_HEADER_SIZE, uncompressed_delta_fp) != VT_DIFF_DELTA_FILE_HEADER_SIZE){
        errnum = errno;
        logError("Failed to write file:%s, errno:%d, error:%s",uncompressed_delta_filepath, errnum, strerror(errnum));
        retcode = -1;
        goto done;
    }
    if(fseeko(delta_fp, VT_DIFF_DELTA_FILE_HEADER_SIZE, SEEK_SET) < 0){
        errnum = errno;
        logError("Failed to fseeko file:%s, errno:%d, error:%s", delta_filepath, errnum, strerror(errnum));
        retcode = -1;
        goto done;
    }

    memset(&ios, 0, sizeof(vtlzma_ios_t));
    if(get_vtbz2_ios_on_dcmpr_file(&ios) < 0){
        logError("Failed at get_vtbz2_ios_on_dcmpr_file");
        errnum = EINVAL;
        retcode = -1;
        goto done;
    }
    if(vtbz2_cmprdcmpr(&ios, 2, delta_fp, uncompressed_delta_fp) < 0){
        logError("Failed at vtlzma_decompress");
        errnum = EINVAL;
        retcode = -1;
        goto done;
    }
    delta_filesize = ftello(delta_fp);
    if(delta_filesize < VT_DIFF_DELTA_FILE_HEADER_SIZE){
        errnum = EINVAL;
        logError("Failed at ftello file:%s, errno:%d, error:%s", delta_filepath, errnum, strerror(errnum));
        retcode = -1;
        goto done;
    }
    if(supposed_delta_filesize != (uint32_t)delta_filesize){
        logError("supposed_delta_filesize:%lu != delta_filesize:%lu",
            (unsigned long)supposed_delta_filesize,
            (unsigned long)delta_filesize
        );
        errnum = EINVAL;
        retcode = -1;
        goto done;
    }

    uncompressed_delta_filesize = ftello(uncompressed_delta_fp);
    if(uncompressed_delta_filesize < VT_DIFF_DELTA_FILE_HEADER_SIZE){
        errnum = EINVAL;
        logError("Failed at ftello file:%s, errno:%d, error:%s", uncompressed_delta_filepath, errnum, strerror(errnum));
        retcode = -1;
        goto done;
    }
    if(supposed_uncompressed_delta_filesize != uncompressed_delta_filesize){
        logError("supposed_uncompressed_delta_filesize:%lu != uncompressed_delta_filesize:%lu",
            (unsigned long)supposed_uncompressed_delta_filesize,
            (unsigned long)uncompressed_delta_filesize
        );
        errnum = EINVAL;
        retcode = -1;
        goto done;
    }
    //
    if(uncompressed_delta_fp != (FILE*)NULL){
        //flush and close it
        fflush(uncompressed_delta_fp);
        fclose(uncompressed_delta_fp);
        uncompressed_delta_fp = (FILE*)NULL;
    }
    if(delta_fp != (FILE*)NULL){
        //close it.
        fclose(delta_fp);
        delta_fp = (FILE*)NULL;
    }
    //
    if(vt_file_merge02(filepath2, filepath1, uncompressed_delta_filepath, need_keep_interim_temp_files) < 0){
        logError("Failed in vt_file_merge02");
        errnum = EINVAL;
        retcode = -1;
        goto done;
    }

    done:
    if(uncompressed_delta_fp != (FILE*)NULL){
        fclose(uncompressed_delta_fp);
        uncompressed_delta_fp = (FILE*)NULL;
    }
    if(delta_fp != (FILE*)NULL){
        fclose(delta_fp);
        delta_fp = (FILE*)NULL;
    }
    //
    if(!need_keep_interim_temp_files){
        if(uncompressed_delta_filepath != (char*)NULL){
            remove(uncompressed_delta_filepath);
        }
    }
    if(uncompressed_delta_filepath != (char*)NULL){
        free(uncompressed_delta_filepath);
        uncompressed_delta_filepath = (char*)NULL;
    }
    if(meta_data_header_buf != (uint8_t*)NULL){
        free(meta_data_header_buf);
        meta_data_header_buf = (uint8_t*)NULL;
    }
    if(meta_data_header_ptr != (vt_diff_delta_file_header_t*)NULL){
        free(meta_data_header_ptr);
        meta_data_header_ptr = (vt_diff_delta_file_header_t*)NULL;
    }

    errno = errnum;
    return retcode;
}

#endif // DISABLE_VTBZ2

__END_DECLS

#endif // DISABLE_VTDIFF02
