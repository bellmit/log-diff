

#ifndef DISABLE_VTDIFF01

#include <errno.h>
#include <string.h>
#include <stdlib.h>

//#include "vtclib/vtmem.h"
#include "vtclib/vtlog.h"

#ifndef DISABLE_VTLZMA
#include "vtclib/vtlzma.h"
#endif

#ifndef DISABLE_VTBZ2
#include "vtclib/vtbz2.h"
#endif

//#include "vtclib/vtfile.h"
#include "vtmerge/vt_merge01.h"
#include "vtmerge/vt_merge01util.h"
#include "vtoemadpt/oem_flashfile.h"
// #include "vtoemadpt/oem_malloc.h"

#define VT_STAGE_FILEPATH_BUF_SIZE  (16 * 1024)

static int vt_diff01_merge_delta_file(
		FILECTRL* file2_fp,
		FILECTRL* file1_fp,
		FILECTRL* delta_file_fp
);

int vt_sjaonse_file_merge01(const char* filepath2, const char* filepath1, const char* delta_filepath){
		int errnum = 0;
		int retcode = -1;
		FILECTRL* fp1 = (FILECTRL*)NULL;
		FILECTRL* delta_fp = (FILECTRL*)NULL;
		FILECTRL* fp2 =  (FILECTRL*)NULL;

		//
		if(delta_filepath == (char*)NULL || filepath1 == (char*)NULL || filepath2 == (char*)NULL){
				logError("Invalid parameters.");
				errnum = EINVAL;
				return retcode;
		}
		
		/** open filepath2 **/
		fp1 = oem_fopen(filepath1, "rb");
		if(fp1 == (FILECTRL*)NULL){
				errnum = errno;
				logError("Can not fopen filepath2(%s): %s", filepath1, strerror(errnum));
				retcode = -1;
				goto done;
		}
		
		//open delta_filepath
		delta_fp = oem_fopen(delta_filepath, "rb");
		if(delta_fp == (FILECTRL*)NULL){
				errnum = errno;
				logError("Can not fopen delta_filepath(%s): %s", delta_filepath, strerror(errnum));
				retcode = -1;
				goto done;
		}
		//open filepath2
		fp2 = oem_fopen(filepath2, "w+b");
		if(fp2 == (FILECTRL*)NULL){
				errnum = errno;
				logError("Can not fopen filepath2(%s): %s", filepath2, strerror(errnum));
				retcode = -1;
				goto done;
		}
		
		//merge it
		retcode = vt_diff01_merge_delta_file(fp2, fp1, delta_fp);
		errnum = errno;

		done:
		if(fp1 != (FILECTRL*)NULL){
				oem_fclose(fp1);
				fp1 = (FILECTRL*)NULL;
		}
		if(fp2 != (FILECTRL*)NULL){
				oem_fclose(fp2);
				fp2 = (FILECTRL*)NULL;
		}
		if(delta_fp != (FILECTRL*)NULL){
				oem_fclose(delta_fp);
				delta_fp = (FILECTRL*)NULL;
		}

		errno = errnum;
		return retcode;
}

#ifndef DISABLE_VTLZMA

int vt_sjaonse_file_lzma_merge01(const char* filepath2, const char* filepath1, const char* delta_filepath){
		int errnum = 0;
		int retcode = 0;
		// char* uncompressed_delta_filepath = (char*)NULL;
		FILE* ifp = (FILE*)NULL;
		FILE* ofp = (FILE*)NULL;
		//char* delta_file_header_buf = (char*)NULL;
		//vt_diff_delta_file_header_t* delta_file_header_ptr = (vt_diff_delta_file_header_t*)NULL;
		// struct stat* delta_file_stat_ptr = (struct stat*)NULL;
        struct stat __delta_file_stat;
        struct stat* delta_file_stat_ptr=&__delta_file_stat;
        vtlzma_ios_t _ios;
		vtlzma_ios_t* ios_ptr = &_ios;
		off_t uncompressed_delta_filesize = 0;
		char uncompressed_delta_filepath[VT_STAGE_FILEPATH_BUF_SIZE];
        char delta_file_header_buf[VT_STAGE_FILEPATH_BUF_SIZE];
        vt_diff_delta_file_header_t  _delta_file_header;
        vt_diff_delta_file_header_t* delta_file_header_ptr=&_delta_file_header;
		if(vt_gen_tmp_filepath(uncompressed_delta_filepath, VT_STAGE_FILEPATH_BUF_SIZE) < 0){
				errnum = errno;
				logError("Failed at vt_gen_tmp_filepath");
				retcode = -1;
				goto done;
		}
		


		if(stat(delta_filepath, delta_file_stat_ptr) < 0){
				errnum = errno;
				logError("Failed at stat(%s): %s", delta_filepath, strerror(errnum));
				retcode = -1;
				goto done;
		}
		if((ifp = oem_fopen(delta_filepath, "rb")) == (FILE*)NULL){
				errnum = errno;
				logError("Failed at fopen(%s): %s", delta_filepath, strerror(errnum));
				retcode = -1;
				goto done;
		}
		if(fread(delta_file_header_buf, 1, VT_DIFF_DELTA_FILE_HEADER_SIZE, ifp) != VT_DIFF_DELTA_FILE_HEADER_SIZE){
				errnum = errno;
				logError("Failed at fread(%s): %s", uncompressed_delta_filepath, strerror(errnum));
				retcode = -1;
				goto done;
		}
		if(vt_diff_buf_to_delta_file_header(delta_file_header_ptr, delta_file_header_buf) < 0){
				logError("Failed at vt_diff_buf_to_delta_file_header");
				errnum = EINVAL;
				retcode = -1;
				goto done;
		}
		if(VT_DIFF_DELTA_FILE_HEADER_SIZE + delta_file_header_ptr->compressed_size != (uint32_t)(delta_file_stat_ptr->st_size)){
				logError("VT_DIFF_DELTA_FILE_HEADER_SIZE:%lu + delta_file_header_ptr->compressed_size:%lu != delta_file_stat_ptr->st_size:%lu for delta file: %s",
						(unsigned long)(VT_DIFF_DELTA_FILE_HEADER_SIZE),
						(unsigned long)(delta_file_header_ptr->compressed_size),
						(unsigned long)(delta_file_stat_ptr->st_size),
						delta_filepath
				);
				errnum = EINVAL;
				retcode = -1;
				goto done;
		}
		if(strncasecmp(delta_file_header_ptr->diff_mark, VT_DIFF_DELTA_MARK_V01, VT_DIFF_DELTA_MARK_LEN) != 0){
				logError("The delta file: %s, not starting with: %s", delta_filepath, VT_DIFF_DELTA_MARK_V01);
				errnum = EINVAL;
				retcode = -1;
				goto done;
		}
		if(strncasecmp(delta_file_header_ptr->cmpr_mark, VT_DIFF_COMPR_MARK_LZMA, VT_DIFF_COMPR_MARK_LEN) != 0){
				logError("The delta file: %s, not secondly starting with: %s", delta_filepath, VT_DIFF_COMPR_MARK_LZMA);
				errnum = EINVAL;
				retcode = -1;
				goto done;
		}
		memset(delta_file_header_ptr->cmpr_mark, 0, VT_DIFF_COMPR_MARK_LEN);
		delta_file_header_ptr->compressed_size = 0;
		if(vt_diff_delta_file_header_to_buf(delta_file_header_buf, delta_file_header_ptr) < 0){
				logError("Failed at vt_diff_delta_file_header_to_buf");
				errnum = EINVAL;
				retcode = -1;
				goto done;
		}
		if((ofp = oem_fopen(uncompressed_delta_filepath, "wb")) == (FILE*)NULL){
				errnum = errno;
				logError("Failed at fopen(%s): %s", uncompressed_delta_filepath, strerror(errnum));
				retcode = -1;
				goto done;
		}
		if(fwrite(delta_file_header_buf, 1, VT_DIFF_DELTA_FILE_HEADER_SIZE, ofp) != VT_DIFF_DELTA_FILE_HEADER_SIZE){
				errnum = errno;
				logError("Failed at fwrite(%s): %s", uncompressed_delta_filepath, strerror(errnum));
				retcode = -1;
				goto done;
		}

		memset(ios_ptr, 0, sizeof(vtlzma_ios_t));
		if(get_vtlzma_ios_on_file_impl(ios_ptr) < 0){
				logError("Failed at get_vtlzma_ios_on_file_impl");
				errno = EINVAL;
				retcode = -1;
				goto done;
		}
		if(vtlzma_decompress(ios_ptr, 2, ifp, ofp) < 0){
				logError("Failed at vtlzma_compress");
				errno = EINVAL;
				retcode = -1;
				goto done;
		}
		uncompressed_delta_filesize = ftello(ofp);
		if(uncompressed_delta_filesize < VT_DIFF_DELTA_FILE_HEADER_SIZE){
				errnum = errno;
				logError("Failed at ftello file:%s, errno:%d, error:%s", uncompressed_delta_filepath, errnum, strerror(errnum));
				retcode = -1;
				goto done;
		}
		logInfo("The decompressed delta_filesize: %lu", (unsigned long)uncompressed_delta_filesize);
		if(VT_DIFF_DELTA_FILE_HEADER_SIZE + delta_file_header_ptr->delta_data_size != (uint32_t)uncompressed_delta_filesize){
				logError(
						"VT_DIFF_DELTA_FILE_HEADER_SIZE:%lu + delta_file_header_ptr->delta_data_size:%lu != uncompressed_delta_filesize:%lu",
						(unsigned long)(VT_DIFF_DELTA_FILE_HEADER_SIZE),
						(unsigned long)(delta_file_header_ptr->delta_data_size),
						(unsigned long)(uncompressed_delta_filesize)
				);
				errnum = EINVAL;
				retcode = -1;
				goto done;
		}
		//

		done:
		if(ofp != (FILE*)NULL){
				fclose(ofp);
				ofp = (FILE*)NULL;
		}
		if(ifp != (FILE*)NULL){
				fclose(ifp);
				ifp = (FILE*)NULL;
		}
		if(delta_file_header_buf != (char*)NULL){
				free(delta_file_header_buf);
				delta_file_header_buf= (char*)NULL;
		}
		if(delta_file_header_ptr != (vt_diff_delta_file_header_t*)NULL){
				free(delta_file_header_ptr);
				delta_file_header_ptr = (vt_diff_delta_file_header_t*)NULL;
		}
		if(delta_file_stat_ptr != (struct stat*)NULL){
				free(delta_file_stat_ptr);
				delta_file_stat_ptr = (struct stat*)NULL;
		}
		if(ios_ptr != (vtlzma_ios_t*)NULL){
				free(ios_ptr);
				ios_ptr = (vtlzma_ios_t*)NULL;
		}
		if(uncompressed_delta_filepath != (char*)NULL){
				if(retcode == 0){
						if(vt_file_merge01(filepath2, filepath1, uncompressed_delta_filepath) < 0){
								errnum = errno;
								logError("Failed at vt_file_merge01(%s, %s, %s)", filepath2, filepath1, uncompressed_delta_filepath);
								retcode = -1;
						}
				}
				remove(uncompressed_delta_filepath);
				free(uncompressed_delta_filepath);
				uncompressed_delta_filepath = (char*)NULL;
		}

		errno = errnum;
		return retcode;
}

#endif // DISABLE_VTLZMA

#ifndef DISABLE_VTBZ2

int vt_sjaonse_file_bz2_merge01(const char* filepath2, const char* filepath1, const char* delta_filepath){
		int errnum = 0;
		int retcode = 0;
		char* uncompressed_delta_filepath = (char*)NULL;
        char buff_uncompressed_delta_filepath[VT_STAGE_FILEPATH_BUF_SIZE];
		FILE* ifp = (FILE*)NULL;
		FILE* ofp = (FILE*)NULL;
		char* delta_file_header_buf = (char*)NULL;
		vt_diff_delta_file_header_t* delta_file_header_ptr = (vt_diff_delta_file_header_t*)NULL;
		struct stat* delta_file_stat_ptr = (struct stat*)NULL;
		vtbz2_ios_t* ios_ptr = (vtbz2_ios_t*)NULL;
		off_t uncompressed_delta_filesize = 0;

		uncompressed_delta_filepath = (char*)malloc(VT_STAGE_FILEPATH_BUF_SIZE);
		if(uncompressed_delta_filepath == (char*)NULL){
				errnum = errno;
				logError("Not enough virtual memory.");
				retcode = -1;
				goto done;
		}
		if(vt_gen_tmp_filepath(uncompressed_delta_filepath, VT_STAGE_FILEPATH_BUF_SIZE) < 0){
				errnum = errno;
				logError("Failed at vt_gen_tmp_filepath");
				retcode = -1;
				goto done;
		}
		//
		delta_file_header_buf = (char*)malloc(VT_DIFF_DELTA_FILE_HEADER_SIZE);
		if(delta_file_header_buf == (char*)NULL){
				errnum = errno;
				logError("Not enough virtual memory");
				retcode = -1;
				goto done;
		}
		delta_file_header_ptr = (vt_diff_delta_file_header_t*)malloc(sizeof(vt_diff_delta_file_header_t));
		if(delta_file_header_ptr == (vt_diff_delta_file_header_t*)NULL){
				errnum = errno;
				logError("Not enough virtual memory");
				retcode = -1;
				goto done;
		}
		delta_file_stat_ptr = (struct stat*)malloc(sizeof(struct stat));
		if(delta_file_stat_ptr == (struct stat*)NULL){
				errnum = errno;
				logError("Not enough virtual memory");
				retcode = -1;
				goto done;
		}
		//
		if(stat(delta_filepath, delta_file_stat_ptr) < 0){
				errnum = errno;
				logError("Failed at stat(%s): %s", delta_filepath, strerror(errnum));
				retcode = -1;
				goto done;
		}
		if((ifp = oem_fopen(delta_filepath, "rb")) == (FILE*)NULL){
				errnum = errno;
				logError("Failed at fopen(%s): %s", delta_filepath, strerror(errnum));
				retcode = -1;
				goto done;
		}
		if(fread(delta_file_header_buf, 1, VT_DIFF_DELTA_FILE_HEADER_SIZE, ifp) != VT_DIFF_DELTA_FILE_HEADER_SIZE){
				errnum = errno;
				logError("Failed at fread(%s): %s", uncompressed_delta_filepath, strerror(errnum));
				retcode = -1;
				goto done;
		}
		if(vt_diff_buf_to_delta_file_header(delta_file_header_ptr, delta_file_header_buf) < 0){
				logError("Failed at vt_diff_buf_to_delta_file_header");
				errnum = EINVAL;
				retcode = -1;
				goto done;
		}
		if(VT_DIFF_DELTA_FILE_HEADER_SIZE + delta_file_header_ptr->compressed_size != (uint32_t)(delta_file_stat_ptr->st_size)){
				logError("VT_DIFF_DELTA_FILE_HEADER_SIZE:%lu + delta_file_header_ptr->compressed_size:%lu != delta_file_stat_ptr->st_size:%lu for delta file: %s",
						(unsigned long)(VT_DIFF_DELTA_FILE_HEADER_SIZE),
						(unsigned long)(delta_file_header_ptr->compressed_size),
						(unsigned long)(delta_file_stat_ptr->st_size),
						delta_filepath
				);
				errnum = EINVAL;
				retcode = -1;
				goto done;
		}
		if(strncasecmp(delta_file_header_ptr->diff_mark, VT_DIFF_DELTA_MARK_V01, VT_DIFF_DELTA_MARK_LEN) != 0){
				logError("The delta file: %s, not starting with: %s", delta_filepath, VT_DIFF_DELTA_MARK_V01);
				errnum = EINVAL;
				retcode = -1;
				goto done;
		}
		if(strncasecmp(delta_file_header_ptr->cmpr_mark, VT_DIFF_COMPR_MARK_BZ2, VT_DIFF_COMPR_MARK_LEN) != 0){
				logError("The delta file: %s, not secondly starting with: %s", delta_filepath, VT_DIFF_COMPR_MARK_BZ2);
				errnum = EINVAL;
				retcode = -1;
				goto done;
		}
		memset(delta_file_header_ptr->cmpr_mark, 0, VT_DIFF_COMPR_MARK_LEN);
		delta_file_header_ptr->compressed_size = 0;
		if(vt_diff_delta_file_header_to_buf(delta_file_header_buf, delta_file_header_ptr) < 0){
				logError("Failed at vt_diff_delta_file_header_to_buf");
				errnum = EINVAL;
				retcode = -1;
				goto done;
		}
		if((ofp = oem_fopen(uncompressed_delta_filepath, "wb")) == (FILE*)NULL){
				errnum = errno;
				logError("Failed at fopen(%s): %s", uncompressed_delta_filepath, strerror(errnum));
				retcode = -1;
				goto done;
		}
		if(fwrite(delta_file_header_buf, 1, VT_DIFF_DELTA_FILE_HEADER_SIZE, ofp) != VT_DIFF_DELTA_FILE_HEADER_SIZE){
				errnum = errno;
				logError("Failed at fwrite(%s): %s", uncompressed_delta_filepath, strerror(errnum));
				retcode = -1;
				goto done;
		}
		//
		if((ios_ptr = (vtbz2_ios_t*)malloc(sizeof(vtbz2_ios_t))) == (vtbz2_ios_t*)NULL){
				errnum = errno;
				logError("Failed at malloc: %s", strerror(errnum));
				retcode = -1;
				goto done;
		}
		memset(ios_ptr, 0, sizeof(vtbz2_ios_t));
		if(get_vtbz2_ios_on_dcmpr_file(ios_ptr) < 0){
				logError("Failed at get_vtbz2_ios_on_dcmpr_file");
				errno = EINVAL;
				retcode = -1;
				goto done;
		}
		if(vtbz2_cmprdcmpr(ios_ptr, 2, ifp, ofp) < 0){
				logError("Failed at vtbz2_cmprdcmpr");
				errno = EINVAL;
				retcode = -1;
				goto done;
		}
		uncompressed_delta_filesize = ftello(ofp);
		if(uncompressed_delta_filesize < VT_DIFF_DELTA_FILE_HEADER_SIZE){
				errnum = errno;
				logError("Failed at ftello file:%s, errno:%d, error:%s", uncompressed_delta_filepath, errnum, strerror(errnum));
				retcode = -1;
				goto done;
		}
		logInfo("The uncompressed delta_filesize: %lu", (unsigned long)uncompressed_delta_filesize);
		if(VT_DIFF_DELTA_FILE_HEADER_SIZE + delta_file_header_ptr->delta_data_size != (uint32_t)uncompressed_delta_filesize){
				logError(
						"VT_DIFF_DELTA_FILE_HEADER_SIZE:%lu + delta_file_header_ptr->delta_data_size:%lu != uncompressed_delta_filesize:%lu",
						(unsigned long)(VT_DIFF_DELTA_FILE_HEADER_SIZE),
						(unsigned long)(delta_file_header_ptr->delta_data_size),
						(unsigned long)(uncompressed_delta_filesize)
				);
				errnum = EINVAL;
				retcode = -1;
				goto done;
		}
		//

		done:
		if(ofp != (FILE*)NULL){
				fclose(ofp);
				ofp = (FILE*)NULL;
		}
		if(ifp != (FILE*)NULL){
				fclose(ifp);
				ifp = (FILE*)NULL;
		}
		if(delta_file_header_buf != (char*)NULL){
				free(delta_file_header_buf);
				delta_file_header_buf= (char*)NULL;
		}
		if(delta_file_header_ptr != (vt_diff_delta_file_header_t*)NULL){
				free(delta_file_header_ptr);
				delta_file_header_ptr = (vt_diff_delta_file_header_t*)NULL;
		}
		if(delta_file_stat_ptr != (struct stat*)NULL){
				free(delta_file_stat_ptr);
				delta_file_stat_ptr = (struct stat*)NULL;
		}
		if(ios_ptr != (vtbz2_ios_t*)NULL){
				free(ios_ptr);
				ios_ptr = (vtbz2_ios_t*)NULL;
		}
		if(uncompressed_delta_filepath != (char*)NULL){
				if(retcode == 0){
						if(vt_file_merge01(filepath2, filepath1, uncompressed_delta_filepath) < 0){
								errnum = errno;
								logError("Failed at vt_file_merge01(%s, %s, %s)", filepath2, filepath1, uncompressed_delta_filepath);
								retcode = -1;
						}
				}
				remove(uncompressed_delta_filepath);
				free(uncompressed_delta_filepath);
				uncompressed_delta_filepath = (char*)NULL;
		}

		errno = errnum;
		return retcode;
}

#endif // DISABLE_VTBZ2

int vt_diff01_merge_delta_file(
		FILECTRL* file2_fp,
		FILECTRL* file1_fp,
		FILECTRL* delta_file_fp)
{
		int retcode = 0;
		int errnum = 0;
		char* ver_buf1 = (char*)NULL;
		char* ver_buf2 = (char*)NULL;
		char* chunk = (char*)NULL;
        char temprambuff[1024*3];/*max chunk size 2K*/
		size_t chunk_allocated_size = 0;
		char prefix_buf[VT_DIFF_MAX_PREFIX_BUF_SIZE];
		int chr = -1;
		vt_diff01_first_op_byte_t first_op_byte;
		size_t prefix_buf_size;
		uint8_t op = 0;
		size_t chunk_len = 0;
		size_t chunk_pos = (size_t)(-1);
		size_t tmp_size;
		char* tmp_chunk;
		off_t file2_tmp_pos;
        vt_diff_delta_file_header_t delta_file_header;
		vt_diff_delta_file_header_t* delta_file_header_ptr = (vt_diff_delta_file_header_t*)NULL;
		delta_file_header_ptr =&delta_file_header;
        char delta_file_header_buf[VT_DIFF_DELTA_FILE_HEADER_SIZE];
		off_t delta_file_size = 0;

		if(delta_file_fp == (FILECTRL*)NULL){
				logError("delta file is NULL");
				retcode = -1;
				goto done;
		}

		if(file1_fp == (FILECTRL*)NULL){
				logError("file 1 is NULL");
				retcode = -1;
				goto done;
		}

		if(file2_fp == (FILECTRL*)NULL){
				logError("file 2 is NULL");
				retcode = -1;
				goto done;
		}

		//
		if(oem_fseek(delta_file_fp, 0, SEEK_SET) < 0){
				logError("error ---- TODO");
				retcode = -1;
				goto done;
		}


		if(oem_fread(delta_file_header_buf, VT_DIFF_DELTA_FILE_HEADER_SIZE, 1, delta_file_fp) != VT_DIFF_DELTA_FILE_HEADER_SIZE){
				logError("error ---- TODO");
				retcode = -1;
				goto done;
		}
		
		vt_diff_buf_to_delta_file_header(delta_file_header_ptr, delta_file_header_buf);
		if(memcmp(delta_file_header_ptr->diff_mark, VT_DIFF_VER01_CSTR, VT_DIFF_VER01_SIZE) != 0){
				logError("The diff version  is not acceptable!");
				errno = EINVAL;
				retcode = -1;
				goto done;
		}
		
		//TODO ... from here
		//
		chunk = (char*)NULL;
		chunk_allocated_size = 0;
		chr = -1;
		while((chr = oem_fgetc(delta_file_fp)) != -1){
				first_op_byte = vt_diff01_to_first_op_byte((uint8_t)chr);
				prefix_buf_size = vt_diff01_get_prefix_buf_size_on_first_op_byte(first_op_byte);
				if(prefix_buf_size == 0 || prefix_buf_size > 10){
						logError("FIXME: prefix_buf_size(%lu)", prefix_buf_size);
						errnum = EINVAL;
						retcode = -1;
						goto done;
				}
				prefix_buf[0] = (char)chr;
				if(oem_fread(prefix_buf + 1, prefix_buf_size - 1, 1, delta_file_fp) != (prefix_buf_size - 1)){
						errnum = errno;
						retcode = -1;
						logError("errno(%d), error(%s) at fread(%s)", errnum, strerror(errnum), delta_file_filepath);
						goto done;
				}
				//
				op = 0;
				chunk_len = 0;
				chunk_pos = (size_t)(-1);
				tmp_size = vt_diff01_from_op_prefix(&op, &chunk_len, &chunk_pos, prefix_buf);
				if(tmp_size != prefix_buf_size){
						logError("FIXME: tmp_size(%lu) != prefix_buf_size(%lu)", tmp_size, prefix_buf_size);
						errnum = EINVAL;
						retcode = -1;
						goto done;
				}
				if(chunk_len == 0){
						logError("FIXME: chunk_len(%lu)", chunk_len);
						errnum = EINVAL;
						retcode = -1;
						goto done;
				}

				if(chunk_allocated_size < chunk_len){
						// tmp_chunk = (char*)oem_realloc(chunk, chunk_len);
                        // if(chunk_len>1024)
                        // {
                        //     printf("shit %d",chunk_len);
                        // }
                        tmp_chunk=(char*)temprambuff;
						if(tmp_chunk == (char*)NULL){
								errnum = errno;
								logError("errno(%d), error(%s) at malloc(%lu)", errnum, strerror(errnum), chunk_len);
								retcode = -1;
								goto done;
						}
						chunk = tmp_chunk;
						chunk_allocated_size = chunk_len;
				}

				file2_tmp_pos = (off_t)-1;
				switch(first_op_byte.op){
						case VT_DIFF_OP_ADD:
								if(oem_fread(chunk, chunk_len, 1, delta_file_fp) != chunk_len){
										errnum = errno;
										retcode = -1;
										logError("errno(%d), error(%s) at fread(%s)", errnum, strerror(errnum), delta_file_filepath);
										goto done;
								}
								break;

						case VT_DIFF_OP_PAD:
								if((chr = oem_fgetc(delta_file_fp)) == EOF){
										errnum = errno;
										retcode = -1;
										logError("errno(%d), error(%s) at fgetc(%s)", errnum, strerror(errnum), delta_file_filepath);
										goto done;
								}
								memset(chunk, chr, chunk_len);
								break;

						case VT_DIFF_OP_CP1:
								if(chunk_pos == (size_t)(-1)){
										logError("FIXME: chunk_pos == -1");
										errnum = EINVAL;
										retcode = -1;
										goto done;
								}
								if(oem_fseek(file1_fp, chunk_pos, SEEK_SET) == -1){
										errnum = errno;
										retcode = -1;
										logError("errno(%d), error(%s) at fseeko(%s)", errnum, strerror(errnum), file1_filepath);
										goto done;
								}
								if(oem_fread(chunk, chunk_len, 1, file1_fp) != chunk_len){
										errnum = errno;
										retcode = -1;
										logError("errno(%d), error(%s) at fread(%s)", errnum, strerror(errnum), file1_filepath);
										goto done;
								}
								break;

						case VT_DIFF_OP_CP2:
								/**
								 * not implemented in diff side yet. So there is something wrong if here is reached.
								**/
								logError("FIXME: VT_DIFF_OP_CP2 is not implemented yet!");
								//
								//
								if((file2_tmp_pos = oem_ftell(file2_fp)) == (off_t)-1){
										errnum = errno;
										retcode = -1;
										logError("errno(%d), error(%s) at ftello(%s)", errnum, strerror(errnum), file2_filepath);
										goto done;
								}
								//
								if(chunk_pos == (size_t)(-1)){
										logError("FIXME: chunk_pos == -1");
										errnum = EINVAL;
										retcode = -1;
										goto done;
								}
								if(oem_fseek(file2_fp, chunk_pos, SEEK_SET) == -1){
										errnum = errno;
										retcode = -1;
										logError("errno(%d), error(%s) at fseeko(%s)", errnum, strerror(errnum), file2_filepath);
										goto done;
								}
								if(oem_fread(chunk, chunk_len, 1, file2_fp) != chunk_len){
										errnum = errno;
										retcode = -1;
										logError("errno(%d), error(%s) at fread(%s)", errnum, strerror(errnum), file2_filepath);
										goto done;
								}
								//
								if(oem_fseek(file2_fp, file2_tmp_pos, SEEK_SET) == -1){
										errnum = errno;
										retcode = -1;
										logError("errno(%d), error(%s) at fseeko(%s)", errnum, strerror(errnum), file2_filepath);
										goto done;
								}
								break;

						default:
								logError("FIXME!");
								errnum = EINVAL;
								retcode = -1;
								goto done;
				}
				//
				if(oem_fwrite(chunk, chunk_len, 1, file2_fp) != chunk_len){
						errnum = errno;
						retcode = -1;
						logError("errno(%d), error(%s) at fwrite(%s)", errnum, strerror(errnum), file2_filepath);
						goto done;
				}
		}
		
		//
		if(!oem_feof(delta_file_fp)){
				errnum = errno;
				retcode = -1;
				logError("Failed at fgetc(%s): %s", delta_file_filepath, strerror(errnum));
				goto done;
		}

		if((delta_file_size = oem_ftell(delta_file_fp)) < VT_DIFF_DELTA_FILE_HEADER_SIZE){
				errnum = errno;
				retcode = -1;
				logError("Failed at ftello(%s): %s", delta_file_filepath, strerror(errnum));
				goto done;
		}
		if((uint32_t)delta_file_size != VT_DIFF_DELTA_FILE_HEADER_SIZE + delta_file_header_ptr->delta_data_size){
				logError("For delta_delta_file:'%s', delta_file_size:%lu != VT_DIFF_DELTA_FILE_HEADER_SIZE:%lu + delta_file_header.delta_data_size:%lu",
						delta_file_filepath,
						(unsigned long)(delta_file_size),
						(unsigned long)(VT_DIFF_DELTA_FILE_HEADER_SIZE),
						(unsigned long)(delta_file_header_ptr->delta_data_size)
				);
				errnum = EINVAL;
				retcode = -1;
				goto done;
		}

		done:
		// if(chunk != (char*)NULL){
		// 		free(chunk);
		// 		chunk = (char*)NULL;
		// }
		// if(ver_buf1 != (char*)NULL){
		// 		free(ver_buf1);
		// 		ver_buf1 = (char*)NULL;
		// }
		// if(ver_buf2 != (char*)NULL){
		// 		free(ver_buf2);
		// 		ver_buf2 = (char*)NULL;
		// }
		// if(delta_file_header_ptr != (vt_diff_delta_file_header_t*)NULL){
		// 		free(delta_file_header_ptr);
		// 		delta_file_header_ptr = (vt_diff_delta_file_header_t*)NULL;
		// }
		// if(delta_file_header_buf != (char*)NULL){
		// 		free(delta_file_header_buf);
		// 		//delta_file_header_buf = (char*)NULL;
		// }
		errno = errnum;
		return retcode;
}

#endif // DISABLE_VTDIFF01
