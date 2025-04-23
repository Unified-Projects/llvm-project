#ifndef _LIBCPP___LOCALE_DIR_LOCALE_BASE_API_UNIFIED_H
#define _LIBCPP___LOCALE_DIR_LOCALE_BASE_API_UNIFIED_H

#include <stdlib.h>
#include <wchar.h>
#include <time.h>
#include <monetary.h>

#define wcscoll_l(a,b,c) wcscoll(a,b)
#define atof_l(a,b) atof(a)
#define strtod_l(a,b,c,d) strtod(a,b)
#define strtol_l(a,b,c,d) strtol(a,b,c)
#define strtoul_l(a,b,c,d) strtoul(a,b,c)
#define wcstod_l(a,b,c) wcstod(a,b)
#define wcstol_l(a,b,c) wcstol(a,b,c)
#define wcstoul_l(a,b,c) wcstoul(a,b,c)
#define mktime_l(a,b) mktime(a)
#define asctime_l(a,b) asctime(a)
#define ctime_l(a,b) ctime(a)
#define strptime_l(a,b,c,d) strptime(a,b,c)
#define strfmon_l(a,b,c,...) strfmon(a,b,c,##__VA_ARGS__)

#endif // _LIBCPP___LOCALE_DIR_LOCALE_BASE_API_UNIFIED_H
