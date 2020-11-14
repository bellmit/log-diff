#ifndef VTCRONUTIL_HPP_INCLUDED
#define VTCRONUTIL_HPP_INCLUDED


#include <time.h>
#include <string.h>

#include "vtclib/vtdef.h"

/**
 * Translated from standard cron job schedule string in its format:
 * 		min hour mday mon wday
 *
 * 'min', 'hour', 'mday', 'mon' and 'wday' are digits or '*' in schedule string.
 * If anyone is non-digit string, the related value in the struct will assigned to -1.
**/
typedef struct _vt_cronjob_schedule_t{
    int min;
    int hour;
    int mday;
    int mon;
    int wday;
}   vt_cronjob_schedule_t;

__BEGIN_DECLS

/**
 * If CronJobScheduleStr not in standard cron job schedule definition, return false and all field in cronJobSchedule is -1
**/
int parseVTCronJobSchedule(vt_cronjob_schedule_t* cronJobSchedulePtr, const char* CronJobScheduleStr);

/**
 * return false if (min == -1 || (wday != -1 && (mon != -1 || mday != -1)))
**/
int isValidVTCronJobSchedule(const vt_cronjob_schedule_t* cronJobSchedulePtr);

/**
 * return -1 if cronJobSchedule is not valid.
**/
time_t timeToVTCronJobScheduleInSecond(struct tm* cronJobScheduleTmPtr, const vt_cronjob_schedule_t* cronJobSchedulePtr);

__END_DECLS

#endif // VTCRONUTIL_HPP_INCLUDED
