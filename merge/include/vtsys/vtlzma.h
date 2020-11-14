#ifndef VTLZMA_H_INCLUDED
#define VTLZMA_H_INCLUDED

/**  2016.02 Dehua Tan **/

/**
 *  **Quick Tutoring**
 *
 *  foo1(){
 *      vtlzma_ios_t    ios;
 *      FILE*           ifp;
 *      FILE*           ofp;
 *      int             retcode = 0;
 *      //
 *      retcode = vtlzma_init(error_handle, "liblzma.so");
 *      //---
 *      //set values to ifp, ofp
 *      //---
 *      memset(&ios, 0, sizeof(ios));
 *      retcode = get_vtlzma_ios_on_file_impl(&ios);
 *      retcode = vtlzma_compress(&ios, 2, ifp, ofp);
 *      //--- vtlzma_decompress(&ios, 2, ifp, ofp);
 *      //
 *      retcode = vtlzma_release(error_handle);
 *  }
 *  foo2(){
 *      vtlzma_ios_t    ios;
 *      void*           iarray;
 *      size_t          isize;
 *      void*           oarray = (void*)NULL;
 *      size_t          osize = 0;
 *      int             retcode = 0;
 *      //
 *      retcode = vtlzma_init(error_handle, "liblzma.so");
 *      //---
 *      //set values to iarray, isize;
 *      //---
 *      memset(&ios, 0, sizeof(ios));
 *      retcode = get_vtlzma_ios_on_file_impl(&ios);
 *      retcode = vtlzma_compress(&ios, 4, iarray, isize, &oarray, &osize);
 *      //--- vtlzma_decompress(&ios, 4, iarray, isize, &oarray, &osize);
 *      //--- oarray to be freed by ios.vtlzma_free manually by YOU
 *      //
 *      ios.vtlzma_free((void*)NULL, oarray);
 *      retcode = vtlzma_release(error_handle);
 *  }
**/

#include <stdio.h>
#include <stdarg.h>
#include "vtsys/vtdef.h"

__BEGIN_DECLS

typedef struct _vtlzma_errmsag_t{
    const char* the_file;
    int the_line;
    const char* the_func;
    //
    int errnum;
    char* errmsg;
    char* app_errmsg;
    //
}   vtlzma_errmsag_t;
typedef void (*vtlzma_error_handle_t)(const vtlzma_errmsag_t* errmsg_ptr);

extern void lzma_default_error_handle(const vtlzma_errmsag_t* errmsg_ptr);

#define DEFINE_VTLZMA_ERRMSG(_errmsg_) \
    vtlzma_errmsag_t _errmsg_ = { \
        __FILE__, __LINE__, __FUNCTION__, \
        0, (char*)NULL, (char*)NULL \
    }

#define RESET_VTLZMA_ERRMSG(_errmsg_) \
    do{ \
        _errmsg_.the_file = __FILE__; \
        _errmsg_.the_line = __LINE__; \
        _errmsg_.the_func = __FUNCTION__; \
        _errmsg_.errnum = 0; \
        _errmsg_.errmsg = (char*)NULL; \
        _errmsg_.app_errmsg = (char*)NULL; \
    } while(0)

#define SET_VTLZMA_ERRMSG(_errmsg_) \
    do{ \
        _errmsg_.the_file = vt_good_filename(__FILE__); \
        _errmsg_.the_line = __LINE__; \
        _errmsg_.the_func = __FUNCTION__; \
        _errmsg_.errnum = errno; \
        _errmsg_.errmsg = (_errmsg_.errnum != 0)? strerror(_errmsg_.errnum) : (char*)NULL; \
    } while(0)

#define SET_VTLZMA_APP_ERRMSG(_errmsg_, _app_errmsg_) \
    do{ \
        _errmsg_.the_file = vt_good_filename(__FILE__); \
        _errmsg_.the_line = __LINE__; \
        _errmsg_.the_func = __FUNCTION__; \
        _errmsg_.app_errmsg = _app_errmsg_; \
    } while(0)

/**
 * The liblzma_so_filename must be under directory specified by
 * env variable LD_LIBRARY_PATH as function dlopen defined.
 *
 * return 0 at successful, -1 at failed, check errno, and err_msg_buf_size for dlopen function related messages.
**/
int vtlzma_init(const vtlzma_error_handle_t error_handle, const char* liblzma_so_filename);
int vtlzma_release(const vtlzma_error_handle_t error_handle);
/**
 * return 1 if vtbz2_init called with liblzma_so_filename;
 * return 0 if not.
**/
int vtlzma_initialized_by(const char* liblzma_so_filename);
//
typedef struct _vtlzma_ios_t vtlzma_ios_t;
typedef void* (*vtlzma_malloc_t)(void* opaque, size_t nmemb, size_t size);
typedef void* (*vtlzma_realloc_t)(void* pre_ptr, size_t size);
typedef void (*vtlzma_free_t)(void* opaque, void* ptr);
typedef void* (*vtlzma_res_apply_t)(const vtlzma_ios_t* ios_ptr, const int argc, va_list ap);
typedef int (*vtlzma_res_release_t)(const vtlzma_ios_t* ios_ptr, void* res_ptr);
typedef int (*vtlzma_eoi_t)(const vtlzma_ios_t* ios_ptr, void* res_ptr);
typedef ssize_t (*vtlzma_read_t)(const vtlzma_ios_t* ios_ptr, void* buf, const size_t size, void* res_ptr);
typedef ssize_t (*vtlzma_write_t)(const vtlzma_ios_t* ios_ptr, const void* buf, const size_t size, void* res_ptr);
typedef struct _vtlzma_ios_t{
    vtlzma_error_handle_t vtlzma_error_handle;  //if null, lzma_default_error_handle
    //
    vtlzma_malloc_t vtlzma_malloc;              //if null, vtlzma_default_malloc
    vtlzma_realloc_t vtlzma_realloc;            //if null, vtlzma_default_realloc
    vtlzma_free_t vtlzma_free;                  //if null, using vtlzma_default_free
    //
    vtlzma_res_apply_t vtlzma_res_apply;        //must not null, provide the input and output streams
    vtlzma_res_release_t vtlzma_res_release;    //must not null, release the input and output streams
    //
    vtlzma_eoi_t vtlzma_eoi;                    //must not null, Determine whether or not reach End Of Input Stream
    vtlzma_read_t vtlzma_read;                  //must not null, Read the Input Stream
    vtlzma_write_t vtlzma_write;                //must not null, Write the Output Stream
    //
} vtlzma_ios_t;

extern void* vtlzma_default_malloc(void* opaque, size_t nmemb, size_t size);
extern void* vtlzma_default_realloc(void* pre_ptr, size_t size);
extern void vtlzma_default_free(void* opaque, void *ptr);
//
/**
 * The parameter res_apply_func_argc and the followed are passed through to res_apply_func
 * to do the resource application for compress and decompress.
**/
extern int vtlzma_compress(const vtlzma_ios_t* ios_ptr, const int argc, ...);
extern int vtlzma_decompress(const vtlzma_ios_t* ios_ptr, const int argc, ...);

////////////////////////////////////////////////////////////////////////////////////////

/**
 *  The vtlzma_ios_t implementation based on FILE*.
 *
 *  The argument list for vtlzma_compress & vtlzma_decompress:
 *      (ios_ptr, 2, (FILE*)intput_fp, (FILE*)output_fp)
 *  The argument list for vtlzma_res_apply:
 *      (ios_ptr, 2, (FILE*)intput_fp, (FILE*)output_fp)
 *
 *  If vtlzma_error_handle, vtlzma_malloc, vtlzma_default_realloc, vtlzma_free are not set within the ios_ptr,
 *  they will be set to the default implementation.
 *
 *  Return 0 on successful, -1 on failure.
**/
extern int get_vtlzma_ios_on_file_impl(vtlzma_ios_t* ios_ptr);

/**
 *  The vtlzma_ios_t implementation based on memory.
 *
 *  The argument list for vtlzma_compress & vtlzma_decompress:
 *      (ios_ptr, 4,
 *          (const void*)input_array, (const size_t)input_size,
 *          (void**)output_array_ptr, (size_t*)output_size_ptr
 *      )
 *  The argument list for vtlzma_res_apply:
 *      (ios_ptr, 4,
 *          (const void*)input_array, (const size_t)input_size,
 *          (void**)output_array_ptr, (size_t*)output_size_ptr
 *      )
 *
 *  If vtlzma_error_handle, vtlzma_malloc, vtlzma_default_realloc, vtlzma_free are not set within the ios_ptr,
 *  they will be set to the default implementation.
 *
 *  Return 0 on successful, -1 on failure.
**/
extern int get_vtlzma_ios_on_memory_impl(vtlzma_ios_t* ios_ptr);

////////////////////////////////////////////////////////////////////////////////////////

__END_DECLS

#endif // VTLZMA_H_INCLUDED
