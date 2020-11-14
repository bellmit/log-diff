#ifndef VTTIME_H_INCLUDED
#define VTTIME_H_INCLUDED

/**  2016.04 Dehua Tan **/

#include "vtclib/vtdef.h"
#include <sys/time.h>

__BEGIN_DECLS

void vt_get_current_timespec(struct timespec* ts_ptr);

void vt_inc_timespec_by_second(struct timespec* ts_ptr, const long sec);

void vt_inc_timespec_by_milisecond(struct timespec* ts_ptr, const int64_t milisec);

//return ts2-ts1 in milisecond
int64_t vt_diff_timespec(const struct timespec* ts1_ptr, const struct timespec* ts2_ptr);

/**
 * The param sysTime is returned by sys api ::time(nullptr)
 * If sysTime is -1, it means current time
 * The ansiTime format: "yyyy-mm-dd hh:mm:ss"
 * The size of ansi_time_buf should at least 20
**/
char* vt_to_ansi_localtime(char* ansi_time_buf, const time_t sys_time);

__END_DECLS

#endif // VTTIME_H_INCLUDED
