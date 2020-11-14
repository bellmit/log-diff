#ifndef VTPRILOG_H_INCLUDED
#define VTPRILOG_H_INCLUDED

#include "vtsys/vtdef.h"

#if defined(_WIN32)
#   define vtprilog(priority, tag, fmt, args...)   fprintf(stderr, "[%s]" fmt, tag, args)
#elif defined(__ANDROID__)
#   define vtprilog(priority, tag, fmt, args...)   __android_log_print(get_android_priority_from_syslog(priority), tag, fmt, args)
#else
#   define vtprilog(priority, tag, fmt, args...)   syslog(priority, "[%s]" fmt, tag, args)
#endif

__BEGIN_DECLS

#if defined(__ANDROID__)
int get_android_priority_from_syslog(const int syslog_priority);
#endif

__END_DECLS

#endif // VTPRILOG_H_INCLUDED

