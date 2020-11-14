#include <stdio.h>
#include "FlashFile/FlashFile.h"
#include "vtoemadpt/oem_flashfile.h"

FILECTRL* oem_fopen(const char* filename, const char* mode){

    return fopen(filename, mode);
}

int oem_fclose(FILECTRL* fp){
    // flashclose(fp);
    fclose(fp);
    return 0;
}

int oem_fgetc(FILECTRL* fp){
    return fgetc(fp);
    // return flashgetc(fp);
}

ssize_t oem_fread(void* ptr, const size_t size, const size_t nmemb, FILECTRL* fp){
    ssize_t rb = fread(ptr,nmemb,size,fp);
    return rb;
    // return flashread(fp, ptr, size * nmemb);
}

ssize_t oem_fwrite(void* ptr, const size_t size, const size_t nmemb, FILECTRL* fp){
    return fwrite(ptr,nmemb,size,fp);
    // return flashwrite(fp, ptr, size * nmemb);
}

int oem_fseek(FILECTRL* fp, ssize_t offset, int whence){
	  return fseek(fp, offset, whence);
}

ssize_t oem_ftell(FILECTRL* fp){
    return ftell(fp);
	//   return flashtell(fp);
}

int oem_feof(FILECTRL* fp){
    return feof(fp);
}
