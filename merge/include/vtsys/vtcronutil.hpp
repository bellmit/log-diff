#ifndef VTCRONUTIL_HPP_INCLUDED
#define VTCRONUTIL_HPP_INCLUDED



#include <time.h>
#include <string>

namespace vtsys{

	/**
	 * Translated from standard cron job schedule string in its format:
	 * 		min hour mday mon wday
	 *
	 * 'min', 'hour', 'mday', 'mon' and 'wday' are digits or '*' in schedule string.
	 * If anyone is non-digit string, the related value in the struct will assigned to -1.
	**/
	struct CronJobSchedule{
		int min;
		int hour;
		int mday;
		int mon;
		int wday;
	};

	/**
	 * If CronJobScheduleStr not in standard cron job schedule definition, return false and all field in cronJobSchedule is -1
	**/
	bool parseCronJobSchedule(CronJobSchedule& cronJobSchedule, const std::string& CronJobScheduleStr);

	/**
	 * return false if (min == -1 || (wday != -1 && (mon != -1 || mday != -1)))
	**/
	bool isValidCronJobSchedule(const CronJobSchedule& cronJobSchedule);

	/**
	 * return -1 if cronJobSchedule is not valid.
	**/
	time_t timeToCronJobScheduleInSecond(struct tm& cronJobScheduleTm, const CronJobSchedule& cronJobSchedule);

}

#endif // VTCRONUTIL_HPP_INCLUDED
