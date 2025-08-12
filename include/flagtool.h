#ifndef FLAGTOOL_H
#define FLAGTOOL_H

#include <stdarg.h>

typedef struct Flag Flag;

// Single-name flag creators (name last)
Flag *flag_string(const char *default_val, const char *help, const char *name);
Flag *flag_bool(int default_val, const char *help, const char *name);
Flag *flag_int(int default_val, const char *help, const char *name);

// Multi-name flag creators (variadic, NULL-terminated names)
Flag *flag_string_multi(const char *default_val, const char *help, ...);
Flag *flag_bool_multi(int default_val, const char *help, ...);
Flag *flag_int_multi(int default_val, const char *help, ...);

int flag_parse(int argc, char *argv[]);

const char *flag_get_string(Flag *flag);
int flag_get_bool(Flag *flag);
int flag_get_int(Flag *flag);

void print_flag_usage(const char *progname);

#endif
