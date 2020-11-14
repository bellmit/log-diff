#ifndef VTROLLHASH_H_INCLUDED
#define VTROLLHASH_H_INCLUDED

/**  2016.02 Dehua Tan **/

#include "vtsys/vtdef.h"

#define VT_ROLLHASH32_DEFAULT_HASH_WINDOW_SIZE  4
#define VT_ROLLHASH32_DEFAULT_PRIME_BASE        16777619LU
#define VT_ROLLHASH64_DEFAULT_HASH_WINDOW_SIZE  8
#define VT_ROLLHASH64_DEFAULT_PRIME_BASE        1099511628211LLU

__BEGIN_DECLS

typedef struct _vt_rollhash32_context_t{
    uint32_t hash_window_size;  //default:  4
    uint32_t prime_base;        //default:  16777619LU
    uint32_t power_base;        //initialized by vt_init_rollhash32_context_power_base if above values have been set.
}   vt_rollhash32_context_t;

typedef struct _vt_rollhash64_context_t{
    uint32_t hash_window_size;  //default:  8
    uint64_t prime_base;        //default:  1099511628211LLU
    uint64_t power_base;        //initialized by vt_init_rollhash64_context_power_base if above values have been set.
}   vt_rollhash64_context_t;

/**
 * if hash_window_size, prime_base, prime_mod are initialized, this func can be used.
**/
void vt_init_rollhash32_context_power_base(vt_rollhash32_context_t* ctx_ptr);
/**
 * if hash_window_size, prime_base, prime_mod are initialized, this func can be used.
**/
void vt_init_rollhash64_context_power_base(vt_rollhash64_context_t* ctx_ptr);

void vt_init_rollhash32_context(
    vt_rollhash32_context_t* ctx_ptr,
    const uint32_t hash_window_size,
    const uint32_t prime_base
);
void vt_init_rollhash64_context(
    vt_rollhash64_context_t* ctx_ptr,
    const uint32_t hash_window_size,
    const uint64_t prime_base
);

void vt_init_default_rollhash32_context(vt_rollhash32_context_t* ctx_ptr);
void vt_init_default_rollhash64_context(vt_rollhash64_context_t* ctx_ptr);

void vt_init_rollhash32_context_with_default(vt_rollhash32_context_t* ctx_ptr, const uint32_t hash_window_size);
void vt_init_rollhash64_context_with_default(vt_rollhash64_context_t* ctx_ptr, const uint32_t hash_window_size);

/**
 * the major function
**/
uint32_t vt_rollhash32(
    const uint32_t last_hash,
    const uint8_t* byte_array,
    const uint32_t byte_idx,
    const vt_rollhash32_context_t* ctx_ptr
);
/**
 * the marjor function
**/
uint64_t vt_rollhash64(
    const uint64_t last_hash,
    const uint8_t* byte_array,
    const uint32_t byte_idx,
    const vt_rollhash64_context_t* ctx_ptr
);

/**
 * For validation & test purpose,
 * if the result of vt_rollhash32 is not same as of vt_nonrollhash32,
 * there are bugs with vt_rollhash32!!!
**/
uint32_t vt_nonrollhash32(
    const uint8_t* byte_array,
    const uint32_t byte_idx,
    const vt_rollhash32_context_t* ctx_ptr
);
/**
 * For validation & test purpose,
 * if the result of vt_rollhash64 is not same as of vt_nonrollhash64,
 * there are bugs with vt_rollhash64!!!
**/
uint64_t vt_nonrollhash64(
    const uint8_t* byte_array,
    const uint32_t byte_idx,
    const vt_rollhash64_context_t* ctx_ptr
);

__END_DECLS

#endif // VTROLLHASH_H_INCLUDED
