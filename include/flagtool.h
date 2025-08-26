#ifndef FLAGTOOL_H
#define FLAGTOOL_H

#include <stdarg.h>

typedef struct Flag Flag;
typedef struct FlagGroup FlagGroup;

// flag creators (variadic, NULL-terminated names)
Flag *flag_string(const char *default_val, const char *help, ...);
Flag *flag_bool(int default_val, const char *help, ...);
Flag *flag_int(int default_val, const char *help, ...);

// Multi-instance flag creators
Flag *flag_string_multi(const char *default_val, const char *help,...);
Flag *flag_int_multi(int default_val, const char *help, ...);

Flag *flag_find(const char *name);
void flag_free(Flag *flag);
void free_hash_table();
void flags_cleanup();
int flag_parse(int argc, char *argv[]);

// Flag grouping
FlagGroup *create_flag_group(const char *name);
void add_flag_to_group(FlagGroup *group, Flag *flag);
void flag_free_group(FlagGroup *group);

const char *flag_get_string(Flag *flag);
int flag_get_bool(Flag *flag);
int flag_get_int(Flag *flag);
const char **flag_get_string_multi(Flag *flag);
const int *flag_get_int_multi(Flag *flag);
int flag_get_multiple_int_count(Flag *flag);
void print_flag_usage(const char *progname);

#endif
