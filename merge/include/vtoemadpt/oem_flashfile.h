#ifndef _VTARMSRC_FLASHFILE_H_
#define _VTARMSRC_FLASHFILE_H_

#include "FlashFile/FlashFile.h"

/**
 * The implementation of "int oem_addr_and_size(unsigned int* addr_ptr, long* size_ptr, const char* filepath)"
 * in file "oem_flashfile.h" is dummpy.
 *
 * OEM developer should complete it based on the runing environment.
 *
**/

#include "vtclib/vtdef.h"

__BEGIN_DECLS

typedef char vt_flash_file_id_t;

//The core adaptor APIs:
FILECTRL* oem_fopen(const char* filename, const char* mode);
int oem_fclose(FILECTRL* fp);
int oem_fgetc(FILECTRL* fp);
ssize_t oem_fread(void* ptr, const size_t size, const size_t nmemb, FILECTRL* fp);
ssize_t oem_fwrite(void* ptr, const size_t size, const size_t nmemb, FILECTRL* fp);
int oem_fseek(FILECTRL* fp, ssize_t offset, int whence);
ssize_t oem_ftell(FILECTRL* fp);
int oem_feof(FILECTRL* fp);
int oem_fflush(FILECTRL* fp);

//The extended adaptor APIs:
FILECTRL* oem_fopen_mode_is_ignored(const char* filename);
size_t oem_get_max_tmp_file_id_length(void);
int oem_gen_tmp_file_id(FILECTRL* file_id_buf, const size_t file_id_buf_size);

int oem_file_compare(const char* filename1, const char* filename2);
int oem_addr_and_size(unsigned int* addr_ptr, long* size_ptr, const char* filepath);

__END_DECLS

#endif // _ARMCLIB_FLASHFILE_H_
