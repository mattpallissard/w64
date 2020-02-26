/*
 * yea, this sucks but it works.  Totally not thread safe
 */
#include <log.h>
const char *label[] = { "emerge",  "alert",  "crit", "err",
			"warning", "notice", "info", "debug" };

const char *color[] = {
	"\x1b[35m", "\x1b[31m", "\x1b[31m", "\x1b[31m", "\x1b[33m",
	"\x1b[32m", "\x1b[36m", "\x1b[94m", "\x1b[94m", "\x1b[94m",
};
const char *color_reset = "\x1b[0m";

void get_time(char *c_time)
{
	time_t t = time(NULL);
	struct tm *lt = localtime(&t);
	int ct = strftime(c_time, 20, "%Y-%m-%d %H:%M:%S", lt);
	c_time[ct++] = ' ';
	c_time[ct] = '\0';
}

void print_string(const char *format, uint8_t log_level, va_list m)
{
	char c_time[20];
	c_time[0] = '\0';
	if (date) {
		get_time(c_time);
	};
	printf("%s%s%-7s%s", c_time, color[log_level], label[log_level],
	       color_reset);
	vprintf(format, m);
	printf("\n");
}

void printu(uint8_t log_level, const char *format, ...)
{
	if (log_level <= level) {
		va_list m;
		va_start(m, format);
		print_string(format, log_level, m);
		va_end(m);
	};
}
