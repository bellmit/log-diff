#ifndef VTFILE_H_INCLUDED
#define VTFILE_H_INCLUDED



#include "vtclib/vtdef.h"
#include "vtclib/vtmem.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>

__BEGIN_DECLS

    ssize_t vt_get_filesize_by_name(const char* filepath);
    ssize_t vt_get_filesize_by_file(FILE* fp);
    ssize_t vt_get_filesize_by_desc(int fd);

    ssize_t vt_read_file(void* buf, const void* filepath, const ssize_t offset, const size_t size);
    int vt_get_file_content(void** content_ptr, ssize_t* filesize_ptr, const char* filepath, const vt_mem_allocator_t* mem_allocator_ptr);

    void vt_set_tmp_dirpath(const char* tmp_dirpath);
    const char* vt_get_tmp_dirpath();
    int vt_gen_tmp_filepath(char* filepath_buf, const size_t filepath_buf_size);

    /**
     * the implemention of something like "mkdir -p <dirpath>"
     * some OR items for parameter mode:
     *      S_IRWXU    00700     mask for file owner permissions
     *      S_IRUSR    00400     owner has read permission
     *      S_IWUSR    00200     owner has write permission
     *      S_IXUSR    00100     owner has execute permission
     *      S_IRWXG    00070     mask for group permissions
     *      S_IRGRP    00040     group has read permission
     *      S_IWGRP    00020     group has write permission
     *      S_IXGRP    00010     group has execute permission
     *      S_IRWXO    00007     mask for permissions for others (not in group)
     *      S_IROTH    00004     others have read permission
     *      S_IWOTH    00002     others have write permission
     *      S_IXOTH    00001     others have execute permission
    **/
    int vt_mkdir(const char* dirpath, const mode_t mode);

    /**
     * Generate time stamped filepath.
     * Return value is parameter filepath, filepath is allocated by caller with enough space.
     * filepath <- dirpath + "/" + filename_prefix + "yyyy.mm.dd-HH.MM.SS" + filename_suffix
     * allocated space for filepath requires: sizeof(dirpath) + sizeof(filename_prefix) + sizeof(filename_suffix) + 20 + 1
     *
     * if timestamp_format is null, use "%Y.%m.%d-%H.%M.%S" as default
    **/
    char* vt_gen_ts_filepath(
        char* filepath,
        const char* dirpath,
        const char* filename_prefix,
        const char* filename_suffix,
        const char* timestamp_format
    );

    typedef struct _vt_file_mmap_t{
        int fd;
        uint64_t size;
        void* addr;
        int mmap_prot;
    }   vt_file_mmap_t;

    vt_file_mmap_t* vt_open_file_mmap(
        const char* filepath,
        const int fileflags,
        const int mmap_prot,
        const int mmap_flags
    );
    int vt_close_file_mmap(vt_file_mmap_t* file_mmap_ptr);

    /**
     * if fileflags != 0 && filemode != 0,
     *      int fd = open(const char *filepath, int fileflags, mode_t filemode)
     * else if fileflags != 0,
     *      int fd = open(const char *filepath, int fileflags);
     * else
     *      int fd = creat(const char *filepath, mode_t filemode);
     *
     * if offset != (off_t)-1
     *      lseek(fd, offset, SEEK_SET);
    **/
    int vt_put_file(
        const char* filepath,
        const int fileflags,
        const mode_t filemode,
        const int64_t offset,
        const void* buf,
        const uint64_t bufsize
    );

    /**
     * first, fopen(filepath, mode) to open the file
     * second, fseek(offset, SEEK_SET), if offset >= 0
     * third, fwrite(buf, bufsize)
    **/
    int vt_fput_file(
        const char* filepath,
        const char* mode,
        const int64_t offset,
        const void* buf,
        const uint64_t bufsize
    );

    /**
     * copy src_filepath into dest_filepath
    **/
    int vt_file_copy(const char* dest_filepath, const char* src_filepath);

    /**
     * If filepath not existed or it is removed successfully, return 0
     * otherwise return -1
    **/
    int vt_remove_if_existed(const char* filepath);


    /**
     * return -1, somthing wrong, check errno for details
     * return 0, compared
     *           if(*result_ptr == 0), identical
     *           if(*result_ptr < 0), content in fp1 smaller than in fp2
     *           if(*result_ptr > 0), content in fp1 larger than in fp2
    **/
    int vt_fp_cmp(int* result_ptr, FILE* fp1, FILE* fp2);

    /**
     * return -1, somthing wrong, check errno for details
     * return 0, compared
     *           if(*result_ptr == 0), identical
     *           if(*result_ptr < 0), content in filepath1 smaller than in filepath2
     *           if(*result_ptr > 0), content in filepath1 larger than in filepath2
    **/
    int vt_filecmp(int* result_ptr, const char* filepath1, const char* filepath2);

__END_DECLS

#endif // VTFILE_H_INCLUDED
