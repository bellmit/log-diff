#ifndef VTLOG_H_INCLUDED
#define VTLOG_H_INCLUDED

/**  2015.07 Dehua Tan **/

/**
 * We beleive the logging level design from UNICS syslog is the best one.
 * Here is the wraper including syslog and direct filelog.
 *
 *  Here is the principle for using logging level.
 *  DEBUG:      for dev debug
 *  INFO:       for QA persormance testing
 *  NOTICE:     for production level
 *  WARNING:    as you know (AYK)
 *  ERROR:      AYK
 *  CRIT:       AYK, resource not enough, some tasks can not be handled.
 *  ALERT:      AYK, it is for non-failure/must-succeed mode design.
 *              For instance, in a XT traction,
 *              a, op-1 has ben prepared,
 *              b, op-2 is failed at preparing
 *              c, network lost for op-1 in certain time, and op-1 can not roll back,
 *                 the resources related with op-1 will be locked for a very very long time.
 *                 It is required be handled manually. So ALERT message comes out.
 *  EMERG:      AYK, for instance DISK full or broken.
 **/

#include "vtsys/vtdef.h"

#include <stdarg.h>

#if defined(__ANDROID__) || defined(_WIN32)
    #include "vtsys/vtsyslog.h"
#else
    #include <syslog.h>
#endif

#define VT_LOG_LEVEL_TAG                    "--log.level"
#define VT_LOG_LEVEL_DEBUG                  "DEBUG"
#define VT_LOG_LEVEL_INFO                   "INFO"
#define VT_LOG_LEVEL_NOTICE                 "NOTICE"
#define VT_LOG_LEVEL_WARNING                "WARNING"
#define VT_LOG_LEVEL_ERR                    "ERR"
#define VT_LOG_LEVEL_CRIT                   "CRIT"
#define VT_LOG_LEVEL_ALERT                  "ALERT"
#define VT_LOG_LEVEL_EMERG                  "EMERG"
#define VT_LOG_TID_SYSTEM_TAG               "--log.tid.system"
#define VT_LOG_TID_SYSTEM_BIN_CSTR          "BIN"
#define VT_LOG_TID_SYSTEM_DEC_CSTR          "DEC"
#define VT_LOG_TID_SYSTEM_OCT_CSTR          "OCT"
#define VT_LOG_TID_SYSTEM_HEX_CSTR          "HEX"
#define VT_LOG_TID_SYSTEM_NIL               0
#define VT_LOG_TID_SYSTEM_BIN               2
#define VT_LOG_TID_SYSTEM_DEC               10
#define VT_LOG_TID_SYSTEM_OCT               8
#define VT_LOG_TID_SYSTEM_HEX               16


#define VT_SYSLOG_IDENT_TAG                 "--syslog.ident"
#define VT_SYSLOG_OPTION_TAG                "--syslog.option"
#define VT_SYSLOG_OPTION_LOG_CONS           "LOG_CONS"
#define VT_SYSLOG_OPTION_LOG_NDELAY         "LOG_NDELAY"
#define VT_SYSLOG_OPTION_LOG_NOWAIT         "LOG_NOWAIT"
#define VT_SYSLOG_OPTION_LOG_ODELAY         "LOG_ODELAY"
#define VT_SYSLOG_OPTION_LOG_PERROR         "LOG_PERROR"
#define VT_SYSLOG_OPTION_LOG_PID            "LOG_PID"
#define VT_SYSLOG_FACILITY_TAG              "--syslog.facility"
#define VT_SYSLOG_FACILITY_LOG_AUTH         "LOG_AUTH"
#define VT_SYSLOG_FACILITY_LOG_AUTHPRIV     "LOG_AUTHPRIV"
#define VT_SYSLOG_FACILITY_LOG_CRON         "LOG_CRON"
#define VT_SYSLOG_FACILITY_LOG_DAEMON       "LOG_DAEMON"
#define VT_SYSLOG_FACILITY_LOG_FTP          "LOG_FTP"
#define VT_SYSLOG_FACILITY_LOG_KERN         "LOG_KERN"
#define VT_SYSLOG_FACILITY_LOG_LPR          "LOG_LPR"
#define VT_SYSLOG_FACILITY_LOG_MAIL         "LOG_MAIL"
#define VT_SYSLOG_FACILITY_LOG_NEWS         "LOG_NEWS"
#define VT_SYSLOG_FACILITY_LOG_SYSLOG       "LOG_SYSLOG"
#define VT_SYSLOG_FACILITY_LOG_USER         "LOG_USER"
#define VT_SYSLOG_FACILITY_LOG_LOCAL0       "LOG_LOCAL0"
#define VT_SYSLOG_FACILITY_LOG_LOCAL1       "LOG_LOCAL1"
#define VT_SYSLOG_FACILITY_LOG_LOCAL2       "LOG_LOCAL2"
#define VT_SYSLOG_FACILITY_LOG_LOCAL3       "LOG_LOCAL3"
#define VT_SYSLOG_FACILITY_LOG_LOCAL4       "LOG_LOCAL4"
#define VT_SYSLOG_FACILITY_LOG_LOCAL5       "LOG_LOCAL5"
#define VT_SYSLOG_FACILITY_LOG_LOCAL6       "LOG_LOCAL6"
#define VT_SYSLOG_FACILITY_LOG_LOCAL7       "LOG_LOCAL7"

#define VT_FILELOG_IDENT_TAG                "--filelog.ident"
#define VT_FILELOG_DIR_TAG                  "--filelog.dir"
#define VT_FILELOG_FILENAME_PREFIX_TAG      "--filelog.filename.prefix"
#define VT_FILELOG_FILENAME_SUFFIX_TAG      "--filelog.filename.suffix"
#define VT_FILELOG_ROTATION_MODE_TAG        "--filelog.rotation.mode"
#define VT_FILELOG_ROTATION_BY_SIZE_CSTR    "by-size"
#define VT_FILELOG_ROTATION_BY_TIME_CSTR    "by-time"
#define VT_FILELOG_ROTATION_BY_SIZE         0
#define VT_FILELOG_ROTATION_BY_TIME         1
#define VT_FILELOG_ROTATION_SIZE_TAG        "--filelog.rotation.size"
#define VT_FILELOG_ROTATION_TIME_TAG        "--filelog.rotation.time"
#define VT_FILELOG_DIR_DEFAULT              "log"
#define VT_FILELOG_FILENAME_PREFIX_DEFAULT  "log-"
#define VT_FILELOG_FILENAME_SUFFIX_DEFAULT  ".txt"
#define VT_FILELOG_ROTATION_SIZE_DEFAULT    (1ULL << 63)
#define VT_FILELOG_ROTATION_TIME_DEFAULT    "0 2 * * *"


__BEGIN_DECLS

    /**
     * msg_log_flush may need be invoked periodly on upper caller layer to flush the log record out to output streams.
    **/
    typedef struct _vt_msg_log_funcs_t{
        void (*msg_log_parse_init_params)(void* params_ptr, int argc, char* argv[]);
        void (*msg_log_init_on_cmd_options)(int argc, char* argv[]);
        void (*msg_log_init_on_parsed_params)(const void* params_ptr);
        void (*msg_log_close)();
        void (*msg_log)(int priority, const char* msg);
        void (*msg_log_flush)();
    } vt_msg_log_funcs_t;

    #if defined(__ANDROID__)
    typedef struct _vt_msg_ndrdlog_init_params_t{
        int log_level;
        char tid_system[VT_LOG_MAX_TID_SYSTEM_BUF_SIZE];
        char ident[VT_LOG_MAX_IDENT_BUF_SIZE];
    } vt_msg_ndrdlog_init_params_t;
    #endif

    typedef struct _vt_msg_syslog_init_params_t{
        int log_level;
        char tid_system[8];
        char ident[256];
        int option;
        int facility;
    } vt_msg_syslog_init_params_t;

    typedef struct _vt_msg_filelog_init_params_t{
        int log_level;
        char tid_system[8];
        char ident[256];
        char dir[62 * 1024];
        char prefix[1024];
        char suffix[64];
        //char rotation_mode[64];
        int rotation_mode;
        uint64_t  rotation_size;
        char rotation_time[64];
    } vt_msg_filelog_init_params_t;

    #if defined(__ANDROID__)
    void vt_msg_ndrdlog_parse_init_params(void* params_ptr, int argc, char* argv[]);
    void vt_msg_ndrdlog_init_on_cmd_options(int argc, char* argv[]);
    void vt_msg_ndrdlog_init_on_parsed_params(const void* params_ptr);
    void vt_msg_ndrdlog_close();
    void vt_msg_ndrdlog(int priority, const char* msg);
    void vt_msg_ndrdlog_flush();
    #endif

    void vt_msg_syslog_parse_init_params(void* params_ptr, int argc, char* argv[]);
    void vt_msg_syslog_init_on_cmd_options(int argc, char* argv[]);
    void vt_msg_syslog_init_on_parsed_params(const void* params_ptr);
    void vt_msg_syslog_close();
    void vt_msg_syslog(int priority, const char* msg);
    void vt_msg_syslog_flush();

    void vt_msg_filelog_parse_init_params(void* params_ptr, int argc, char* argv[]);
    void vt_msg_filelog_init_on_cmd_options(int argc, char* argv[]);
    void vt_msg_filelog_init_on_parsed_params(const void* params_ptr);
    void vt_msg_filelog_close();
    void vt_msg_filelog(int priority, const char* msg);
    void vt_msg_filelog_flush();

    /**
     * transfering the log priority level in integer into in string.
    **/
    const char* vt_log_level_to_cstr(int priority);
    /**
     * transfer the log priority level in string to in integer
    **/
    int vt_cstr_to_log_level(const char* cstr);
    /**
     * transfer the log option in string to in integer
    **/
    int vt_cstr_to_syslog_option(const char* cstr);
    /**
     * transfer the log facility in string to in integer
    **/
    int vt_cstr_to_syslog_facility(const char* cstr);

    /**
     * 16K as default if vt_set_max_log_line_len is not invoked.
    **/
    void vt_set_max_log_line_len(const size_t max_log_line_len);
    /**
     * for information purpose
    **/
    size_t vt_get_max_log_line_len();

    /**
     * Using default internal syslog based implementation if vt_set_msg_log_funcs is not invoked.
    **/
    void vt_set_msg_log_funcs(vt_msg_log_funcs_t msg_log_funcs);
    /**
     * msg_log_funcs_ptr pointer a vt_msg_log_funcs_t variable.
    **/
    void vt_get_msg_log_funcs(vt_msg_log_funcs_t* msg_log_funcs_ptr);

    /**
     * set log level
    **/
    void vt_set_log_level(int log_priority_level);
    /**
     * get log level
    **/
    int vt_get_log_level();
    /**
     * return 0 means the given level not enabled;
     * return non-zero means the given level enabled.
    **/
    int vt_is_log_level_enabled(int log_priority_level);

    /**
     * for local syslog emulator purpose
    **/
    void vt_vfilelog(int priority, const char* fmt, va_list ap);

    /**
     * for local syslog emulator purpose
    **/
    void vt_filelog(int priority, const char* fmt, ...);

    /**
     * The major client invoking interface.
    **/
    void vt_vlog(const char* filepath, const size_t line, const char* good_function, int priority, const char* fmt, va_list ap);

    /**
     * The major client invoking interface.
    **/
    void vt_log(const char* filepath, const size_t line, const char* good_function, int priority, const char* fmt, ...);

    /**
     * The major client invoking interface to initialize the logger environment.
    **/
    void vt_log_init(int argc, char* argv[]);

    /**
     * The major client invoking interface to flush the log data out on app logic.
    **/
    void vt_log_flush();

__END_DECLS

#define logDebugEnabled()         vt_is_log_level_enabled(LOG_DEBUG)
#define logInfoEnabled()          vt_is_log_level_enabled(LOG_INFO)
#define logInformationEnabled()   vt_is_log_level_enabled(LOG_INFO)
#define logNoticeEnabled()        vt_is_log_level_enabled(LOG_NOTICE)
#define logWarnEnabled()          vt_is_log_level_enabled(LOG_WARNING)
#define logWarningEnabled()       vt_is_log_level_enabled(LOG_WARNING)
#define logErrEnabled()           vt_is_log_level_enabled(LOG_ERR)
#define logErrorEnabled()         vt_is_log_level_enabled(LOG_ERR)

#define logDebug(fmt, args...) \
    do { \
        if(vt_is_log_level_enabled(LOG_DEBUG)) { \
            vt_log(__GOOD_FILE__, __LINE__, __GOOD_FUNCTION__, LOG_DEBUG, fmt, ##args); \
        } \
    } while (0)

#define logInfo(fmt, args...) \
    do { \
        if(vt_is_log_level_enabled(LOG_INFO)) { \
            vt_log(__GOOD_FILE__, __LINE__, __GOOD_FUNCTION__, LOG_INFO, fmt, ##args); \
        } \
    } while (0)

#define logInformation(fmt, args...) \
    do { \
        if(vt_is_log_level_enabled(LOG_INFO)) { \
            vt_log(__GOOD_FILE__, __LINE__, __GOOD_FUNCTION__, LOG_INFO, fmt, ##args); \
        } \
    } while (0)

#define logNotice(fmt, args...) \
    do { \
        if(vt_is_log_level_enabled(LOG_NOTICE)) { \
            vt_log(__GOOD_FILE__, __LINE__, __GOOD_FUNCTION__, LOG_NOTICE, fmt, ##args); \
        } \
    } while (0)

#define logWarn(fmt, args...) \
    do { \
        if(vt_is_log_level_enabled(LOG_WARNING)) { \
            vt_log(__GOOD_FILE__, __LINE__, __GOOD_FUNCTION__, LOG_WARNING, fmt, ##args); \
        } \
    } while (0)

#define logWarning(fmt, args...) \
    do { \
        if(vt_is_log_level_enabled(LOG_WARNING)) { \
            vt_log(__GOOD_FILE__, __LINE__, __GOOD_FUNCTION__, LOG_WARNING, fmt, ##args); \
        } \
    } while (0)

#define logErr(fmt, args...) \
    do { \
        if(vt_is_log_level_enabled(LOG_ERR)) { \
            vt_log(__GOOD_FILE__, __LINE__, __GOOD_FUNCTION__, LOG_ERR, fmt, ##args); \
        } \
    } while (0)

#define logError(fmt, args...) \
    do { \
        if(vt_is_log_level_enabled(LOG_ERR)) { \
            vt_log(__GOOD_FILE__, __LINE__, __GOOD_FUNCTION__, LOG_ERR, fmt, ##args); \
        } \
    } while (0)

#define logCrit(fmt, args...) \
    vt_log(__GOOD_FILE__, __LINE__, __GOOD_FUNCTION__, LOG_CRIT, fmt, ##args)

#define logCritical(fmt, args...) \
    vt_log(__GOOD_FILE__, __LINE__, __GOOD_FUNCTION__, LOG_CRIT, fmt, ##args)

#define logAlert(fmt, args...) \
    vt_log(__GOOD_FILE__, __LINE__, __GOOD_FUNCTION__, LOG_ALERT, fmt, ##args)

#define logEmerg(fmt, args...) \
    vt_log(__GOOD_FILE__, __LINE__, __GOOD_FUNCTION__, LOG_EMERG, fmt, ##args)

#define logEmergence(fmt, args...) \
    vt_log(__GOOD_FILE__, __LINE__, __GOOD_FUNCTION__, LOG_EMERG, fmt, ##args)


#endif // VTLOG_H_INCLUDED
