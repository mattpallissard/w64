/*
 * yea, this sucks but it works.  Totally not thread safe
 */
#ifndef H_LOG
#define H_LOG
#include <stdarg.h>
#include <stdio.h>
#include <stdint.h>
#include <time.h>

#define LOG_EMERG 0
#define LOG_ALERT 1
#define LOG_CRIT 2
#define LOG_ERR 3
#define LOG_WARNING 4
#define LOG_NOTICE 5
#define LOG_INFO 6
#define LOG_DEBUG 7

uint8_t level;
uint8_t date;
#define setprintmask(i) level = i;

#define setprintdate(i) date = i;

void get_time(char *c_time);
void print_string(const char *format, uint8_t log_level, va_list m);
void printu(uint8_t log_level, const char *format, ...);
#endif
