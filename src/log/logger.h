#ifndef _LOGGER_H_
#define _LOGGER_H_

#include "log4c.h"

extern log4c_category_t* loggerCategory;

void loggerInit(const char* categoryName);
void loggerUninit();

#define LOG_ERROR(msg, args...)\
do\
{\
    const log4c_location_info_t locinfo = LOG4C_LOCATION_INFO_INITIALIZER(NULL);\
    log4c_category_log_locinfo(loggerCategory, &locinfo, LOG4C_PRIORITY_ERROR, msg, ##args); \
}while(0)

#define LOG_WARN(msg, args...)\
do\
{\
    const log4c_location_info_t locinfo = LOG4C_LOCATION_INFO_INITIALIZER(NULL);\
    log4c_category_log_locinfo(loggerCategory, &locinfo, LOG4C_PRIORITY_WARN, msg, ##args); \
}while(0)

#define LOG_INFO(msg, args...)\
do\
{\
    const log4c_location_info_t locinfo = LOG4C_LOCATION_INFO_INITIALIZER(NULL);\
    log4c_category_log_locinfo(loggerCategory, &locinfo, LOG4C_PRIORITY_INFO, msg, ##args); \
}while(0)

#define LOG_TRACE(msg, args...)\
do\
{\
    const log4c_location_info_t locinfo = LOG4C_LOCATION_INFO_INITIALIZER(NULL);\
    log4c_category_log_locinfo(loggerCategory, &locinfo, LOG4C_PRIORITY_DEBUG, msg, ##args); \
}while(0)

#define LOG_DEBUG(msg, args...)\
do\
{\
    const log4c_location_info_t locinfo = LOG4C_LOCATION_INFO_INITIALIZER(NULL);\
    log4c_category_log_locinfo(loggerCategory, &locinfo, LOG4C_PRIORITY_TRACE, msg, ##args); \
}while(0)

#endif