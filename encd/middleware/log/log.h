/*
 * debug.h
 *
 *  Created on: Dec 10, 2019
 *      Author: kychu
 */

#ifndef _LOG_H_
#define _LOG_H_

#include "logconfig.h"

#if CONFIG_LOG_OUTPUT_ENABLED
#define CONFIG_DEBUG_ALERT
#define CONFIG_DEBUG_ERROR
#define CONFIG_DEBUG_WARN
#define CONFIG_DEBUG_INFO
#endif /* CONFIG_LOG_OUTPUT_ENABLED */

typedef enum {
    KY_LOG_NONE,       /*!< No log output */
    KY_LOG_ERROR,      /*!< Critical errors, software module can not recover on its own */
    KY_LOG_WARN,       /*!< Error conditions from which recovery measures have been taken */
    KY_LOG_INFO,       /*!< Information messages which describe normal flow of events */
    KY_LOG_DEBUG,      /*!< Extra information which is not necessary for normal use (values, pointers, sizes, etc). */
    KY_LOG_VERBOSE     /*!< Bigger chunks of debugging information, or frequent messages which can potentially flood the output. */
} kylog_level_t;

typedef status_t (*log_put_t)(const char *);

#if CONFIG_LOG_OUTPUT_ENABLED
status_t log_init(log_put_t ptx);
uint32_t log_timestamp(void);
status_t log_write(const char *format, ...) __attribute__ ((__format__ (__printf__, 1, 2)));
#endif /* CONFIG_LOG_OUTPUT_ENABLED */

#if CONFIG_LOG_COLORS_ENABLED
#define LOG_COLOR_BLACK   "30"
#define LOG_COLOR_RED     "31"
#define LOG_COLOR_GREEN   "32"
#define LOG_COLOR_BROWN   "33"
#define LOG_COLOR_BLUE    "34"
#define LOG_COLOR_PURPLE  "35"
#define LOG_COLOR_CYAN    "36"
#define LOG_COLOR(COLOR)  "\033[0;" COLOR "m"
#define LOG_BOLD(COLOR)   "\033[1;" COLOR "m"
#define LOG_RESET_COLOR   "\033[0m"
#define LOG_COLOR_E       LOG_COLOR(LOG_COLOR_RED)
#define LOG_COLOR_W       LOG_COLOR(LOG_COLOR_BROWN)
#define LOG_COLOR_I       LOG_COLOR(LOG_COLOR_GREEN)
#define LOG_COLOR_D
#define LOG_COLOR_V
#else //CONFIG_LOG_COLORS_ENABLED
#define LOG_COLOR_E
#define LOG_COLOR_W
#define LOG_COLOR_I
#define LOG_COLOR_D
#define LOG_COLOR_V
#define LOG_RESET_COLOR
#endif //CONFIG_LOG_COLORS_ENABLED

#define LOG_FORMAT(letter, format)  LOG_COLOR_ ## letter #letter " (%ld) %s: " format LOG_RESET_COLOR "\n"

#ifndef CONFIG_DEBUG_ERROR
#  define ky_err                                 (void)
#else
#  define ky_err(tag, format, ...)               log_write(LOG_FORMAT(E, format), log_timestamp(), tag, ##__VA_ARGS__)
#endif

#ifndef CONFIG_DEBUG_WARN
#  define ky_warn                                (void)
#else
#  define ky_warn(tag, format, ...)              log_write(LOG_FORMAT(W, format), log_timestamp(), tag, ##__VA_ARGS__)
#endif

#ifndef CONFIG_DEBUG_INFO
#  define ky_info                                (void)
#else
#  define ky_info(tag, format, ...)              log_write(LOG_FORMAT(I, format), log_timestamp(), tag, ##__VA_ARGS__)
#endif

/* This determines the importance of the message. The levels are, in order
 * of decreasing importance:
 */

//#define LOG_EMERG     0  /* System is unusable */
//#define LOG_ALERT     1  /* Action must be taken immediately */
//#define LOG_CRIT      2  /* Critical conditions */
//#define LOG_ERR       3  /* Error conditions */
//#define LOG_WARNING   4  /* Warning conditions */
//#define LOG_NOTICE    5  /* Normal, but significant, condition */
//#define LOG_INFO      6  /* Informational message */
//#define LOG_DEBUG     7  /* Debug-level message */

#endif /* _LOG_H_ */
