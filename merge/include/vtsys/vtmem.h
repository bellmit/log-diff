#ifndef VTMEM_H_INCLUDED
#define VTMEM_H_INCLUDED

/**  2015.07 Dehua Tan **/

#include "vtsys/vtdef.h"

__BEGIN_DECLS

    typedef struct _vt_mem_allocator_t{
       void* (*mem_alloc)(size_t size);                 //e.g. malloc(size);
       void (*mem_free)(void *ptr);                     //e.g. free(ptr);
       void* (*mem_realloc)(void *ptr, size_t size);    //e.g. realloc(ptr, size);
    } vt_mem_allocator_t;

    typedef void (*vt_malloc_register_t)(void* ptr, const size_t nbytes, const char* filepath, size_t line, const char* funcname);
    typedef void (*vt_free_register_t)(void* ptr, const char* filepath, size_t line, const char* funcname);
    /**
     * set the retisters handlers to help locating memory leaking locations if necessary.
    **/
    extern void vt_set_mem_registers(vt_malloc_register_t malloc_register, vt_free_register_t free_register);

    extern void vt_free_impl(void* ptr, const char* filepath, size_t line, const char* funcname);
    extern void* vt_malloc_impl(const size_t nbytes, const char* filepath, size_t line, const char* funcname);
    extern void* vt_realloc_impl(void* ptr, size_t nbytes, const char* filepath, size_t line, const char* funcname);
    extern void* vt_calloc_impl(const size_t nmemb, const size_t memb_nbytes, const char* filepath, size_t line, const char* funcname);

    /**
     * this API invoking macro vt_free directly for ScopeXXXDeallocator purpose.
    **/
    extern void vt_free_pointer(void* ptr);

    /**
     * the huge memory is implemented by mmap api
    **/
    extern void* vt_alloc_huge_memory(const uint64_t size);
    extern int vt_free_huge_memory(void* addr);
    extern void vt_free_all_huge_memory();

__END_DECLS

#define vt_free(ptr)                    vt_free_impl(ptr, __GOOD_FILE__, __LINE__, __GOOD_FUNCTION__)
#define vt_malloc(nbytes)               vt_malloc_impl(nbytes, __GOOD_FILE__, __LINE__, __GOOD_FUNCTION__)
#define vt_realloc(ptr, nbytes)         vt_realloc_impl(ptr, nbytes, __GOOD_FILE__, __LINE__, __GOOD_FUNCTION__)
#define vt_calloc(nmemb, memb_nbytes)   vt_calloc_impl(nmemb, memb_nbytes, __GOOD_FILE__, __LINE__, __GOOD_FUNCTION__)

#endif // VTMEM_H_INCLUDED
