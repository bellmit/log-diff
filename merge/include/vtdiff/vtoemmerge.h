#ifndef _VTOEMMERGE_H_
#define _VTOEMMERGE_H_


#include "vtsys/vtdef.h"

__BEGIN_DECLS

    ///////////////////////////////////////////////////////////////////////////////////////////////

    //// BEGIN of types' declartion

    /**
     *  It is a named void type, its pointer is similar to file path in standard OS.
     *  Its pointer type will be a opaque object, the meaning depending on implementation.
     *  Using opaque object is for generic purpose.
    **/
    typedef struct vt_oem_file_id_t vt_oem_file_id_t;

    /**
     *  It is a named void type, its pointer is similar to FILE* in stdio.h, or file descriptor integer.
     *  Its pointer type will be a opaque object, the meaning depending on implementation.
     *  Using opaque object is for generic purpose.
    **/
    typedef struct vt_oem_file_t vt_oem_file_t;

    /**
     * The file operators need be used in VTMERGE01.
    **/
    //Core Adaptor APIs Types
    typedef vt_oem_file_t* (*vt_oem_fopen_t)(vt_oem_file_id_t* file_id, const char* mode);
    typedef int (*vt_oem_fclose_t)(vt_oem_file_t* fp);
    typedef int (*vt_oem_fgetc_t)(vt_oem_file_t* fp);
    typedef ssize_t (*vt_oem_fread_t)(void* ptr, const size_t size, const size_t nmemb, vt_oem_file_t* fp);
    typedef ssize_t (*vt_oem_fwrite_t)(void* ptr, const size_t size, const size_t nmemb, vt_oem_file_t* fp);
    typedef int (*vt_oem_fseek_t)(vt_oem_file_t* fp, ssize_t offset, int whence);
    typedef ssize_t (*vt_oem_ftell_t)(vt_oem_file_t* fp);
    typedef int (*vt_oem_feof_t)(vt_oem_file_t* fp);
    typedef int (*vt_oem_fflush_t)(vt_oem_file_t* fp);
    //Extended Adaptor APIs Types
    typedef int (*vt_oem_fopen_mode_is_ignored_t)(vt_oem_file_t* fp);
    typedef size_t (*vt_oem_get_max_tmp_file_id_size_t)();
    typedef int (*vt_oem_gen_tmp_file_id_t)(vt_oem_file_id_t* file_id_buf, const size_t file_id_buf_size);


    //The collection type of adaptor APIs
    typedef struct _vt_oem_file_operators_t{
        //Core Adaptor APIs:
        vt_oem_fopen_t oem_fopen;
        vt_oem_fclose_t oem_fclose;
        vt_oem_fgetc_t oem_fgetc;
        vt_oem_fread_t oem_fread;
        vt_oem_fwrite_t oem_fwrite;
        vt_oem_fseek_t oem_fseek;
        vt_oem_ftell_t oem_ftell;
        vt_oem_feof_t oem_feof;
        vt_oem_fflush_t oem_fflush;
        //
        //Extended Adaptor APIs:
        vt_oem_fopen_mode_is_ignored_t oem_fopen_mode_is_ignored;
        vt_oem_get_max_tmp_file_id_size_t oem_get_max_tmp_file_id_size;
        vt_oem_gen_tmp_file_id_t oem_gen_tmp_file_id;
        //
    } vt_oem_file_operators_t;

    /**
     * Something be in attentation.
     * For example, use memory for media to store file, the memory file will be removed if the file cosed.
     * So certain kind of handles are necessary for this purpose as followed.
    **/

    /**
     * Just after merging completed but all files are not yet closed, this handle will be invoked in the framework.
     * If file2 or file1 are based on volatile media, this handle must be implemented on application logics.
     *
     * udp: User Defined Paramerter, for application special requirement.
     * The application send it to the framework via API of "vt_oem_set_preclose_files_handle_udp_t"
    **/
    typedef int (*vt_oem_preclose_files_handle_t)(
        void* udp,
        vt_oem_file_t* file2_fp,
        vt_oem_file_id_t* file2_id,
        const vt_oem_file_operators_t* file2_operators,
        vt_oem_file_t* file1_fp,
        vt_oem_file_id_t* file1_id,
        const vt_oem_file_operators_t* file1_operators,
        vt_oem_file_t* delta_file_fp,
        vt_oem_file_id_t* delta_file_id,
        const vt_oem_file_operators_t* delta_file_operators
    );
    typedef void* (*vt_oem_get_preclose_files_handle_udp_t)();
    typedef void (*vt_oem_set_preclose_files_handle_udp_t)(void* udp);

    /**
     * Just after merging completed and all files are closed. this handle will be invoked in the faramework.
     * If file2 are based on persistent media, file2 should be removed on application logics.
     *
     * udp: User Defined Paramerter, for application special requirement.
     * The application send it to the framework via API of "vt_oem_set_postclose_files_handle_udp_t"
    **/
    typedef int (*vt_oem_postclose_files_handle_t)(
        void* udp,
        vt_oem_file_id_t* file2_id,
        const vt_oem_file_operators_t* file2_operators,
        vt_oem_file_id_t* file1_id,
        const vt_oem_file_operators_t* file1_operators,
        vt_oem_file_id_t* delta_file_id,
        const vt_oem_file_operators_t* delta_file_operators
    );
    typedef void* (*vt_oem_get_postclose_files_handle_udp_t)();
    typedef void (*vt_oem_set_postclose_files_handle_udp_t)(void* udp);

    //The collection type of framework handles, we name them  as auxiliaries.
    typedef struct _vt_oem_files_auxiliaries_t{
        vt_oem_preclose_files_handle_t oem_preclose_files_handle;
        vt_oem_get_preclose_files_handle_udp_t oem_get_preclose_files_handle_udp;
        vt_oem_set_preclose_files_handle_udp_t oem_set_preclose_files_handle_udp;

        vt_oem_postclose_files_handle_t oem_postclose_files_handle;
        vt_oem_get_postclose_files_handle_udp_t oem_get_postclose_files_handle_udp;
        vt_oem_set_postclose_files_handle_udp_t oem_set_postclose_files_handle_udp;
    }   vt_oem_files_auxiliaries_t;

    /**
     * For each kind of OEM FILE SYSTEM(NOT REAL FS), there should be a related vt_oem_get_file_operators_t implementation.
     * Return null means no implementation
    **/
    typedef const vt_oem_file_operators_t* (*vt_oem_get_file_operators_t)();

    /**
     * For each used composition of 3 OEM FILE SYSTEM(NOT REAL FS) , there should be related vt_oem_files_auxiliaries_t impementation.
     * Return null means no implementation
    **/
    typedef const vt_oem_files_auxiliaries_t* (*vt_oem_get_files_auxiliaries_t)();

    //// END of types' declartion

    ///////////////////////////////////////////////////////////////////////////////////////////////

    //// BEGIN of framework

    /**
     *  This API is a generic meging framework based on VT merging algorithm.
     *  All parameters' types are "interfaces" that need concrete implementations.
     *
     *  target_file_id:
     *      It must not be NULL.
     *      Before merging, its content is version 1, it is file1;
     *      After merging and before closing, version 2 content is in file2(tmp abstract file);
     *      After merging and after closed, its content is version 2.
     *  delta_file_id:
     *      It must not be NULL.
     *      Its content is delta data.
     *  oem_get_file2_operators:
     *      It must not be NULL.
     *      It is function pointer. It will get all necessary abstract file operators for file2(tmp abstract file).
     *  oem_get_file1_operators:
     *      It must not be NULL.
     *      It is function pointer. It will get all necessary abstract file operators for file1(target file).
     *  oem_get_delta_file_operators:
     *      It must not be NULL.
     *      It is function pointer. It will get all necessary abstract file operators for delta file.
     *  oem_get_files_auxiliaries:
     *      It may be NULL that depending on app logics.
     *      It is function pointer for for get the above 3 files's auxiliaries.
     *      For example, if file2 is RAMFILE, oem_preclose_files_handle should be used.
     *      It is because the ramfile will be lost after it closed.
     *
     *  Why we need 3 kind of file operators?
     *      It is because the file operators are similar to OS file access drive.
     *      Different files may be stored on different media types (Built-in FLash, USB, RAM, SSD and so on).
     *      Different media type, different file access drive.
     *      So differnt file operators are on differnt media typpes.
    **/
    int vt_merge_oem_file(
        vt_oem_file_id_t* target_file_id,
        vt_oem_file_id_t* delta_file_id,
        vt_oem_get_file_operators_t oem_get_file2_operators,
        vt_oem_get_file_operators_t oem_get_file1_operators,
        vt_oem_get_file_operators_t oem_get_delta_file_operators,
        vt_oem_get_files_auxiliaries_t oem_get_files_auxiliaries
    );

    //// END of framework

    ///////////////////////////////////////////////////////////////////////////////////////////////

    /**
     *  This API is added some restriction on API "int vt_merge_oem_file(...)"
     *  whose parameters:
     *      oem_get_file2_operators,
     *      oem_get_file1_operators,
     *      oem_get_delta_file_operators
     *  are same all same as parameter:
     *      oem_get_file_operators
     *  in this API.
     *
     *  This is for targe file, delta file and interim file using same type of file media.
    **/
    int vt_merge_oem_file_in_same_file_operators(
        vt_oem_file_id_t* target_file_id,
        vt_oem_file_id_t* delta_file_id,
        vt_oem_get_file_operators_t oem_get_file_operators,
        vt_oem_get_files_auxiliaries_t oem_get_files_auxiliaries
    );

    /**
     *  This API is added some restriction on API "int vt_merge_oem_file(...)"
     *  whose parameters:
     *      oem_get_file2_operators,
     *      oem_get_file1_operators,
     *      oem_get_delta_file_operators
     *  are mapped into parameters:
     *      oem_get_interim_file_operators,
     *      oem_get_file_operators,
     *      oem_get_file_operators
     *  in this API.
     *
     *  This is for targe file, delta file using one type of file media, and interim file using another type.
     *  For example, target_file and delta_file are all in persistemt media,
     *  but we use volatile midia for interim files.
    **/
    int vt_merge_oem_file_in_different_interim_file_operators(
        vt_oem_file_id_t* target_file_id,
        vt_oem_file_id_t* delta_file_id,
        vt_oem_get_file_operators_t oem_get_interim_file_operators,
        vt_oem_get_file_operators_t oem_get_file_operators,
        vt_oem_get_files_auxiliaries_t oem_get_files_auxiliaries
    );

    ///////////////////////////////////////////////////////////////////////////////////////////////

    /**
     * This API is added some restrictions on API "int vt_merge_oem_file_in_same_file_operators(...)"
     * whose parameters:
     *      oem_get_file_operators
     *      oem_get_files_auxiliaries
     * are moved out into global area.
     *
     * And, API "vt_merge_file" is on "VT C style" instead of "standard C style":
     *      "standard C style": int std_c_foo(...); the retrun value: 0 means successful, -1 means failed and check errno for details;
     *      "VT style": uint32_t vt_c_foo(...); the return value: 0 means successful, others for errno or error code.
    **/
    extern vt_oem_get_file_operators_t oem_get_file_operators;              //function pointer initialized as NULL
    extern vt_oem_get_files_auxiliaries_t oem_get_files_auxiliaries;        //function pointer initialized as NULL
    // uint32_t vt_merge_file(char* target_filepath, char* delta_filepath);

    ///////////////////////////////////////////////////////////////////////////////////////////////

    /**
     * This API is added some restriction on API "int vt_merge_oem_file_in_different_interim_file_operators(...)"
     * whoe parameters:
     *      oem_get_interim_file_operators,
     *      oem_get_file_operators,
     *      oem_get_files_auxiliaries
     * are moved out into global are and renamed into
     *      oem_get_volatile_partition_operators,
     *      oem_get_persistent_partition_operators,
     *      oem_get_partitions_auxiliaries
     *
     *  And, API "vt_merge_partition" is on "VT C style" instead of "standard C style":
     *      "standard C style": int std_c_foo(...); the retrun value: 0 means successful, -1 means failed and check errno for details;
     *      "VT style": uint32_t vt_c_foo(...); the return value: 0 means successful, others for errno or error code.
    **/
    extern vt_oem_get_file_operators_t oem_get_volatile_partition_operators;    //function pointer initialized as NULL
    extern vt_oem_get_file_operators_t oem_get_persistent_partition_operators;  //function pointer initialized as NULL
    extern vt_oem_get_files_auxiliaries_t oem_get_partitions_auxiliaries;       //function pointer initialized as NULL
    uint32_t vt_merge_partition(char* target_partition_name, char* delta_partition_name);

    ///////////////////////////////////////////////////////////////////////////////////////////////

__END_DECLS

#endif // _VTOEMMERGE_H_
