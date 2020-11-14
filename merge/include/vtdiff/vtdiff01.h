#ifndef VTDIFF01_H_INCLUDED
#define VTDIFF01_H_INCLUDED

#include "vtsys/vtdef.h"
#include "vtdiff/vtoemmerge01.h"

__BEGIN_DECLS

    /**
     * Implimentation logic:
     *      data(delta_filepath) = data(filepath2) - data(filepath1)
     *
     * Attn: parameters are order sensitive.
     *       and the API is thread safe.
     *
     * return value < 0,  something wrong, check error for details.
     * return value == 0, successful,
     *                    if delta_filepath not exist, it means filepath1 is identical to filepath2.
    **/
    extern int vt_file_diff01(const char* delta_filepath, const char* filepath1, const char* filepath2);
    extern int vt_file_lzma_diff01(const char* delta_filepath, const char* filepath1, const char* filepath2);
    extern int vt_file_bz2_diff01(const char* delta_filepath, const char* filepath1, const char* filepath2);

    /**
     * Implimentation logic:
     *      data(filepath2) = data(filepath1) + data(delta_filepath)
     *
     * Attn: parameter are order sensitive.
     *       and the API is thread safe.
    **/
    extern int vt_file_merge01(const char* filepath2, const char* filepath1, const char* delta_filepath);
    extern int vt_file_lzma_merge01(const char* filepath2, const char* filepath1, const char* delta_filepath);
    extern int vt_file_bz2_merge01(const char* filepath2, const char* filepath1, const char* delta_filepath);

__END_DECLS

#endif // VTDIFF_H_INCLUDED
