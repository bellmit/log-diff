#include "vtoemadpt/oem_flashfile.h"
#include "FlashFile/FlashFile.h"
#include "vtclib/vtlog.h"

#include <errno.h>
#include <stdlib.h>`
#include <string.h>


#define VT_FILE_MODE_READ   1
#define VT_FILE_MODE_WRITE  2

FILECTRL* oem_fopen(const char* filename, const char* mode){
    
	// unsigned int flash_file_addr;
    // long flash_file_size;
	//  if (oem_addr_and_size(&flash_file_addr, &flash_file_size, filename) == -1)
	// 		  return NULL;

    // return flashopen(flash_file_addr, flash_file_size);
}

int oem_fclose(FILECTRL* fp){
    flashclose(fp);
    return 0;
}

int oem_fgetc(FILECTRL* fp){
    return flashgetc(fp);
}

ssize_t oem_fread(void* ptr, const size_t size, const size_t nmemb, FILECTRL* fp){
    ssize_t rb =  flashread(fp, ptr, size * nmemb);
    return rb;
}

ssize_t oem_fwrite(void* ptr, const size_t size, const size_t nmemb, FILECTRL* fp){
    return flashwrite(fp, ptr, size * nmemb);
}

int oem_fseek(FILECTRL* fp, ssize_t offset, int whence){
	  return flashseek(fp, offset, whence);
}

ssize_t oem_ftell(FILECTRL* fp){
	  return flashtell(fp);
}

int oem_feof(FILECTRL* fp){
    return flasheof(fp);
}

int vt_flash_file_fflush(FILECTRL* fp){
	  return flashflush(fp);
}

FILECTRL* oem_fopen_mode_is_ignored(const char* filename){
    unsigned int flash_file_addr;
    long flash_file_size;
    if (oem_addr_and_size(&flash_file_addr, &flash_file_size, filename) == -1)
        return NULL;

    return flashopen(flash_file_addr, flash_file_size);
}

size_t oem_get_max_tmp_file_id_length(){
    return L_tmpnam;
}

int oem_gen_tmp_file_id(FILECTRL* filepath_buf, const size_t filepath_buf_size){
    int retcode = 0;
	  //TBD
    return retcode;
}

int oem_file_compare(const char* filename1, const char* filename2)
{
		FILECTRL* fp1 = NULL;
  	FILECTRL* fp2 = NULL;
	
    if(filename1 == (void*)NULL){
        return -1;
    }
    if(filename2 == (void*)NULL){
        return 1;
    }
    if(filename1 == filename2){
        return 0;
    }
		if (memcmp(filename1, filename2, strlen(filename1)) == 0){
		    return 0;
		}
		
		if ((fp1 = oem_fopen(filename1, NULL)) == NULL)
				return -1;
		if ((fp2 = oem_fopen(filename2, NULL)) == NULL)
				return 1;
		
		if (fp1->totalLen != fp2->totalLen)
				return -1;
			
		return memcmp((void *) fp1->baseAddr, (void *) fp2->baseAddr, fp1->totalLen);
}

int oem_addr_and_size(unsigned int* addr_ptr, long* size_ptr, const char* filename){
	  if (memcmp(filename, "target.bin", 10) == 0){
			  *addr_ptr = 0x08010000;
			  *size_ptr = 10 * 1024;
		}else if (memcmp(filename, "delta.diff", 10) == 0){
			  *addr_ptr = 0x08020000;
			  *size_ptr = 424;
		}else if (memcmp(filename, "target2.bin", 11) == 0){
			  *addr_ptr = 0x08028000;
			  *size_ptr = 424;
		}else if (memcmp(filename, "temp.bin", 8) == 0){
			  *addr_ptr = 0x08030000;
			  *size_ptr = 10 * 1024;
		}else
			  return -1;

	  return 0;
}
