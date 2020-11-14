#ifndef VTSTRING_H_INCLUDED
#define VTSTRING_H_INCLUDED

#include "vtclib/vtdef.h"

__BEGIN_DECLS

char* vt_strndup (const char *s, size_t n);

/**
 * buf size: 20
 * buf will be '\0' ending.
**/
char* vt_uint64_to_hex_cstr(char* buf, const uint64_t val);


/**
 * buf size: 24
 * buf will be '\0' ending.
**/
char* vt_uint64_to_dec_cstr(char* buf, const uint64_t val);

/**
 * buf size: 24
 * buf will be '\0' ending.
**/
char* vt_uint64_to_oct_cstr(char* buf, const uint64_t val);

/**
 * buf size: 65
 * buf will be '\0' ending.
**/
char* vt_uint64_to_bin_cstr(char* buf, const uint64_t val);


char* vt_cstr_trim(char* cstr);
char* vt_cstr_ltrim(char* cstr);
char* vt_cstr_rtrim(char* cstr);

char* vt_cstr_trim_pad(char* cstr, const char pad);
char* vt_cstr_ltrim_pad(char* cstr, const char pad);
char* vt_cstr_rtrim_pad(char* cstr, const char pad);

int64_t vt_kmgtp_cstr_to_int64(const char* cstr);
uint64_t vt_kmgtp_cstr_to_uint64(const char* cstr);

int vt_cstr_to_bool(const char* cstr);
int8_t vt_cstr_to_int8(const char* cstr);
uint8_t vt_cstr_to_uint8(const char* cstr);
int16_t vt_cstr_to_int16(const char* cstr);
uint16_t vt_cstr_to_uint16(const char* cstr);
int32_t vt_cstr_to_int32(const char* cstr);
uint32_t vt_cstr_to_uint32(const char* cstr);
int64_t vt_cstr_to_int64(const char* cstr);
uint64_t vt_cstr_to_uint64(const char* cstr);
float vt_cstr_to_float(const char* cstr);
double vt_cstr_to_double(const char* cstr);
long double vt_cstr_to_longdouble(const char* cstr);

__END_DECLS

#endif // VTSTRING_H_INCLUDED
