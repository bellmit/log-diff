#ifndef VTBZ2_H_INCLUDED
#define VTBZ2_H_INCLUDED

/**  2016.02 Dehua Tan **/

/**
 *  **Quick Tutoring**
 *
 *  foo1(){
 *      vtbz2_ios_t     ios;
 *      FILE*           ifp;
 *      FILE*           ofp;
 *      int             retcode = 0;
 *      //
 *      retcode = vtbz2_init(error_handle, "libbz2.so");
 *      //---
 *      //set values to ifp, ofp
 *      //---
 *      memset(&ios, 0, sizeof(ios));
 *      retcode = get_vtbz2_ios_on_cmpr_file(&ios);
 *      retcode = vtbz2_cmprdcmpr(&ios, 2, ifp, ofp);
 *      //
 *      retcode = vtbz2_release(error_handle);
 *  }
 */

#include <stdio.h>
#include <stdarg.h>
#include "vtsys/vtdef.h"

__BEGIN_DECLS

typedef struct _vtbz2_errmsag_t{
    const char* the_file;
    int the_line;
    const char* the_func;
    //
    int errnum;
    char* errmsg;
    char* app_errmsg;
    //
}   vtbz2_errmsag_t;
typedef void (*vtbz2_error_handle_t)(const vtbz2_errmsag_t* errmsg_ptr);

extern void bz2_default_error_handle(const vtbz2_errmsag_t* errmsg_ptr);

#define DEFINE_VTBZ2_ERRMSG(_errmsg_) \
    vtbz2_errmsag_t _errmsg_ = { \
        __FILE__, __LINE__, __FUNCTION__, \
        0, (char*)NULL, (char*)NULL \
    }

#define RESET_VTBZ2_ERRMSG(_errmsg_) \
    do{ \
        _errmsg_.the_file = __FILE__; \
        _errmsg_.the_line = __LINE__; \
        _errmsg_.the_func = __FUNCTION__; \
        _errmsg_.errnum = 0; \
        _errmsg_.errmsg = (char*)NULL; \
        _errmsg_.app_errmsg = (char*)NULL; \
    } while(0)

#define SET_VTBZ2_ERRMSG(_errmsg_) \
    do{ \
        _errmsg_.the_file = vt_good_filename(__FILE__); \
        _errmsg_.the_line = __LINE__; \
        _errmsg_.the_func = __FUNCTION__; \
        _errmsg_.errnum = errno; \
        _errmsg_.errmsg = (_errmsg_.errnum != 0)? strerror(_errmsg_.errnum) : (char*)NULL; \
    } while(0)

#define SET_VTBZ2_APP_ERRMSG(_errmsg_, _app_errmsg_) \
    do{ \
        _errmsg_.the_file = vt_good_filename(__FILE__); \
        _errmsg_.the_line = __LINE__; \
        _errmsg_.the_func = __FUNCTION__; \
        _errmsg_.app_errmsg = _app_errmsg_; \
    } while(0)

/**
 * The libbz2_so_filename must be under directory specified by
 * env variable LD_LIBRARY_PATH as function dlopen defined.
 *
 * return 0 at successful, -1 at failed, check errno, and err_msg_buf_size for dlopen function related messages.
**/
int vtbz2_init(const vtbz2_error_handle_t error_handle, const char* libbz2_so_filename);
int vtbz2_release(const vtbz2_error_handle_t error_handle);
/**
 * return 1 if vtbz2_init called with libbz2_so_filename;
 * return 0 if not.
**/
int vtbz2_initialized_by(const char* libbz2_so_filename);
//
typedef struct _vtbz2_ios_t vtbz2_ios_t;
typedef void* (*vtbz2_malloc_t)(size_t size);
typedef void* (*vtbz2_realloc_t)(void* pre_ptr, size_t size);
typedef void (*vtbz2_free_t)(void* ptr);
typedef void* (*vtbz2_res_apply_t)(const vtbz2_ios_t* ios_ptr, const int argc, va_list ap);
typedef int (*vtbz2_res_release_t)(const vtbz2_ios_t* ios_ptr, void* res_ptr);
typedef int (*vtbz2_eoi_t)(const vtbz2_ios_t* ios_ptr, void* res_ptr);
typedef ssize_t (*vtbz2_read_t)(const vtbz2_ios_t* ios_ptr, void* buf, const size_t size, void* res_ptr);
typedef ssize_t (*vtbz2_write_t)(const vtbz2_ios_t* ios_ptr, const void* buf, const size_t size, void* res_ptr);
typedef struct _vtbz2_ios_t{
    vtbz2_error_handle_t vtbz2_error_handle;  //if null, bz2_default_error_handle
    //
    vtbz2_malloc_t vtbz2_malloc;              //if null, vtbz2_default_malloc
    vtbz2_realloc_t vtbz2_realloc;            //if null, vtbz2_default_realloc
    vtbz2_free_t vtbz2_free;                  //if null, using vtbz2_default_free
    //
    vtbz2_res_apply_t vtbz2_res_apply;        //must not null, provide the input and output streams
    vtbz2_res_release_t vtbz2_res_release;    //must not null, release the input and output streams
    //
    vtbz2_eoi_t vtbz2_eoi;                    //must not null, Determine whether or not reach End Of Input Stream
    vtbz2_read_t vtbz2_read;                  //must not null, Read the Input Stream
    vtbz2_write_t vtbz2_write;                //must not null, Write the Output Stream
    //
} vtbz2_ios_t;

extern void* vtbz2_default_malloc(size_t size);
extern void* vtbz2_default_realloc(void* pre_ptr, size_t size);
extern void vtbz2_default_free(void *ptr);
//
/**
 * The parameter res_apply_func_argc and the followed are passed through to res_apply_func
 * to do the resource application for compress or decompress based on the given ios_ptr.
**/
extern int vtbz2_cmprdcmpr(const vtbz2_ios_t* ios_ptr, const int argc, ...);

////////////////////////////////////////////////////////////////////////////////////////

/**
 *  The vtbz2_ios_t implementation based on FILE*.
 *
 *  The argument list for vtbz2_cmprdcmpr
 *      (ios_ptr, 2, (FILE*)intput_fp, (FILE*)output_fp)
 *  The argument list for vtbz2_res_apply:
 *      (ios_ptr, 2, (FILE*)intput_fp, (FILE*)output_fp)
 *
 *  If vtbz2_error_handle, vtbz2_malloc, vtbz2_default_realloc, vtbz2_free are not set within the ios_ptr,
 *  they will be set to the default implementation.
 *
 *  Return 0 on successful, -1 on failure.
**/
extern int get_vtbz2_ios_on_cmpr_file(vtbz2_ios_t* ios_ptr);


////////////////////////////////////////////////////////////////////////////////////////

/**
 *  The vtbz2_ios_t implementation based on FILE*.
 *
 *  The argument list for vtbz2_cmprdcmpr
 *      (ios_ptr, 2, (FILE*)intput_fp, (FILE*)output_fp)
 *  The argument list for vtbz2_res_apply:
 *      (ios_ptr, 2, (FILE*)intput_fp, (FILE*)output_fp)
 *
 *  If vtbz2_error_handle, vtbz2_malloc, vtbz2_default_realloc, vtbz2_free are not set within the ios_ptr,
 *  they will be set to the default implementation.
 *
 *  Return 0 on successful, -1 on failure.
**/
extern int get_vtbz2_ios_on_dcmpr_file(vtbz2_ios_t* ios_ptr);


////////////////////////////////////////////////////////////////////////////////////////

__END_DECLS

#endif // VTBZ2_H_INCLUDED
