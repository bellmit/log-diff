#ifndef VTTIME_HPP_INCLUDED
#define VTTIME_HPP_INCLUDED

/**  2016.02 Dehua Tan **/

#include "vtsys/vtdef.h"

#include <sys/time.h>
#include <string>

namespace vtsys {

	void getCurrentTimespec(struct ::timespec& ts);

	void incTimespecBySecond(struct ::timespec& ts, const long sec);

	void incTimespecByMilisecond(struct ::timespec& ts, const int64_t milisec);

	//return ts2-ts1 in milisecond
	int64_t diffTimespec(const struct ::timespec& ts1, const struct ::timespec& ts2);

	/**
	 * The param sysTime is returned by sys api ::time(nullptr)
	 * If sysTime is -1, it means current time
	 * The ansiTime format: "yyyy.mm.dd hh:mm:ss"
	**/
	std::string& toAnsiLocalTime(std::string& ansiTime, const ::time_t sysTime = (::time_t)-1);
}

#endif // VTTIME_HPP_INCLUDED
