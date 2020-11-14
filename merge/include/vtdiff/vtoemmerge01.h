#ifndef VTOEMMERGE01_H_INCLUDED
#define VTOEMMERGE01_H_INCLUDED


#include "vtdiff/vtoemmerge.h"

__BEGIN_DECLS

    /**
     * The generic algorithm framework for VTDIFF01
     *
     * file2_fp, file1_fp, delta_file_fp are similar to FILE* in stdio.h
     * file_operators_ptr includes necessary file operators
    **/
    extern int vt_oem_fp_merge01(
        vt_oem_file_t* file2_fp,
        vt_oem_file_t* file1_fp,
        vt_oem_file_t* delta_file_fp,
        const vt_oem_file_operators_t* file2_operators_ptr,
        const vt_oem_file_operators_t* file1_operators_ptr,
        const vt_oem_file_operators_t* delta_file_operators_ptr
    );

    /**
     * The generic algorithm framework for VTDIFF01
     *
     * file2_id, file1_id, delta_file_id are similar to file path in stdio.h
     * file_operators_ptr includes necessary file operators
     * preclose_file2_obj_ptr is the first paramter for file_operators_ptr->oem_preclose_files_handle
    **/
    extern int vt_oem_file_merge01(
        vt_oem_file_id_t* file2_id,
        vt_oem_file_id_t* file1_id,
        vt_oem_file_id_t* delta_file_id,
        const vt_oem_file_operators_t* file2_operators_ptr,
        const vt_oem_file_operators_t* file1_operators_ptr,
        const vt_oem_file_operators_t* delta_file_operators_ptr,
        const vt_oem_files_auxiliaries_t* files_auxiliaries_ptr
    );

__END_DECLS


#endif // VTOEMMERGE01_H_INCLUDED
