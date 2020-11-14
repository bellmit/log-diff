#ifndef VTMERGE_H_INCLUDED
#define VTMERGE_H_INCLUDED

/**  2016.02 Dehua Tan **/
/**  6/30/2016 James Fan **/

#include "vtclib/vtdef.h"
__BEGIN_DECLS

/**
 *  Differential update initialization.
 *
 *	return value: 0 - success
 *				  			non 0 - failed
**/
int vt_merge_init(void);

/**
 *  Implement a dry run to check the target file and delta file before trigger the real update.
 *
 *  target_filepath (input):
 *		The file path and name which to be updated
 *      It must not be NULL.
 *  delta_filepath (input):
 *		The diff update package file path and name which use for target file update
 *      It must not be NULL.
 *
 *	return value: 0 - success
 *				  			non 0 - failed
**/
int vt_merge_check(const char* target_filepath,
									 const char* delta_filepath);

/**
 *  Implement the target file (or partition image) differential update.
 *
 *  target_filepath (input):
 *		The file path and name which to be updated
 *      It must not be NULL.
 *  delta_filepath (input):
 *		The diff update package file path and name which use for target file update
 *      It must not be NULL.
 *  merge_type (input):
 *      Define the way for update.
*
*	return value: TBD
**/
int vt_merge_file(const char* target_filepath,
									const char* delta_filepath,const char *megered_filepath,
									int merge_type);
__END_DECLS
#endif // VTDIFF_H_INCLUDED
