#ifndef FLAGTOOL_H
#define FLAGTOOL_H

typedef struct Flag Flag;

Flag *flag_string(const char *name, const char *default_val, const char *help);
Flag *flag_bool(const char *name, int default_val, const char *help);
Flag *flag_int(const char *name, int default_val, const char *help);

int flag_parse(int argc, char *argv[]);

const char *flag_get_string(Flag *flag);
int flag_get_bool(Flag *flag);
int flag_get_int(Flag *flag);

void print_flag_usage(const char *progname);

#endif