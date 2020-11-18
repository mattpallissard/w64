#include <w64.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <log.h>
#include <argp.h>
const char *argp_program_version = "w64 v0.1";
const char *argp_program_bug_address = "Matt Pallissard <matpp@uw.edu>";
static char doc[] = "write some words to disk and check 'em";
static char args_doc[] = "";

static struct argp_option options[] = {
	{ "path", 'p', "*char", 0, "i/o path, should be a file" },
	{ "iter", 'i', "u64", 0, "number of iterations" },
	{ "size", 's', "u64", 0, "size in gibibytes" },
	{ "flush", 'f', "bool", 0, "whether we flush and wash" },
	{ 0 }
};

struct arguments {
	char *args[3];
	char *path;
	uint16_t size;
	uint16_t iter;
	bool flush;
};

static error_t parse_opt(int k, char *arg, struct argp_state *s)
{
	struct arguments *a = s->input;
	switch (k) {
	case 'p':
		a->path = arg;
		break;
	case 's':
		a->size = atoi(arg);
		break;
	case 'i':
		a->iter = atoi(arg);
		break;
	case 'f':
		a->flush = atoi(arg);
		break;
		//case ARGP_KEY_ARG:
		//	if (s->arg_num >= 1)
		//		argp_usage (s);
		//	a->args[s->arg_num] = arg;
		//	break;
		//case ARGP_KEY_END:
		//	if (s->arg_num < 1)
		//	argp_usage (s);
		//	break;

	default:
		return ARGP_ERR_UNKNOWN;
	}
	return 0;
}

static struct argp argp = { options, parse_opt, args_doc, doc };

size_t to_gib(uint32_t i)
{
	return i *= 1024 * 1024 * 1024;
}

int main(int argc, char **argv)
{
	size_t s;
	struct arguments a;
	uint16_t i = 0;
	uint8_t *dest;
	uint8_t *src;

	a.path = "/dev/null";
	setprintmask(LOG_DEBUG);
	setprintdate(1);
	argp_parse(&argp, argc, argv, 0, 0, &a);

	s = to_gib(a.size);
	dest = prep(s);
	src = prep(s);

	while (i++ != a.iter) {
		run(src, dest, a.path, s, a.flush);
		printu(LOG_INFO, "completed iteration: %u / %u", i, a.iter);
	}

	free(dest);
	free(src);
}
