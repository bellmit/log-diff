#ifndef VTTEST_H_INCLUDED
#define VTTEST_H_INCLUDED

#include "vtclib/vtdef.h"

#define VT_TEST_FUNCS_NAMES_TAG   "--test_func_names"

/**
 *
**/
__BEGIN_DECLS

/**
 * all test functions' type
**/
typedef int (*vt_test_func_t)(int argc, char* argv[]);

/**
 * call this API to register one test function.
 *
 * in the value of "--test_func_names" / (argc, argv),
 * if no test_func_name, register fails and return -1, otherwise register sucesses and return 0.
**/
int vt_test_register(const char* test_func_name, vt_test_func_t test_func_ptr, int argc, char* argv[]);

/**
 * test_func_names and test_func_ptrs are allocated by caller, its size are max_registered_count.
 * return the registered func names and ptrs, output the names to test_func_names, ptrs to test_func_ptrs.
**/
size_t vt_test_get_registered(const size_t max_registered_count, const char** test_func_names, vt_test_func_t* test_func_ptrs);

/**
 * test all of registered test funcs.
 * return 0, all test funcs passed. otherwise return -1
**/
int vt_test_run(int argc, char* argv[]);

__END_DECLS

#endif // VTTEST_H_INCLUDED
