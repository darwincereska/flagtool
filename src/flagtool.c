#include "flagtool.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum { TYPE_STRING, TYPE_BOOL, TYPE_INT } FlagType;

struct Flag {
  const char *name;
  const char *help;
  FlagType type;
  
  // Values:
  
  // String
  const char *default_str;
  char *value_str;
  
  // Boolean
  int default_bool;
  int value_bool;
  
  // Integer
  int default_int;
  int value_int;
  
  int is_set; // if user provided value
};

// Internal storage for flags
#define MAX_FLAGS 100
static Flag *flags[MAX_FLAGS];
static int flag_count = 0;

static Flag *create_flag(const char *name, const char *help, FlagType type) {
    if (flag_count >= MAX_FLAGS) {
        fprintf(stderr, "Too many flags registered\n");
        exit(1);
    }
    Flag *f = calloc(1, sizeof(Flag));
    if (!f) {
        perror("calloc");
        exit(1);
    }
    f->name = name;
    f->help = help;
    f->type = type;
    flags[flag_count++] = f;
    return f;
}

Flag *flag_string(const char *name, const char *default_val, const char *help) {
    Flag *f = create_flag(name, help, TYPE_STRING);
    f->default_str = default_val;
    f->value_str = NULL;
    f->is_set = 0;
    return f;
}

Flag *flag_bool(const char *name, int default_val, const char *help) {
    Flag *f = create_flag(name, help, TYPE_BOOL);
    f->default_bool = default_val;
    f->value_bool = default_val;
    f->is_set = 0;
    return f;
}

Flag *flag_int(const char *name, int default_val, const char *help) {
    Flag *f = create_flag(name, help, TYPE_INT);
    f->default_int = default_val;
    f->value_int = default_val;
    f->is_set = 0;
    return f;
}

int flag_parse(int argc, char *argv[]) {
    for (int i = 1; i < argc; i++) {
        int matched = 0;
        for (int j = 0; j < flag_count; j++) {
            Flag *f = flags[j];
            if (strcmp(argv[i], f->name) == 0) {
                matched = 1;
                // For bool flags, treat presence as true
                if (f->type == TYPE_BOOL) {
                    f->value_bool = 1;
                    f->is_set = 1;
                } else {
                    if (i + 1 >= argc) {
                        fprintf(stderr, "Missing value for flag %s\n", f->name);
                        return 1;
                    }
                    char *val = argv[++i];
                    if (f->type == TYPE_STRING) {
                        free(f->value_str);
                        f->value_str = strdup(val);
                        if (!f->value_str) {
                            perror("strdup");
                            return 1;
                        }
                        f->is_set = 1;
                    } else if (f->type == TYPE_INT) {
                        char *endptr;
                        long v = strtol(val, &endptr, 10);
                        if (*endptr != '\0') {
                            fprintf(stderr, "Invalid integer for flag %s: %s\n", f->name, val);
                            return 1;
                        }
                        f->value_int = (int)v;
                        f->is_set = 1;
                    }
                }
                break;
            }
        }
        if (!matched) {
            fprintf(stderr, "Unknown flag: %s\n", argv[i]);
            return 1;
        }
    }
    return 0;
}

const char *flag_get_string(Flag *flag) {
    if (flag->type != TYPE_STRING) return NULL;
    if (flag->is_set) return flag->value_str;
    return flag->default_str;
}

int flag_get_bool(Flag *flag) {
    if (flag->type != TYPE_BOOL) return 0;
    return flag->value_bool;
}

int flag_get_int(Flag *flag) {
    if (flag->type != TYPE_INT) return 0;
    return flag->value_int;
}

void print_flag_usage(const char *progname) {
    printf("Usage: %s [flags]\nFlags:\n", progname);
    for (int i = 0; i < flag_count; i++) {
        Flag *f = flags[i];
        switch (f->type) {
            case TYPE_STRING:
                printf("  %s <string>\t%s (default: %s)\n", f->name, f->help, f->default_str ? f->default_str : "none");
                break;
            case TYPE_BOOL:
                printf("  %s\t\t%s (default: %s)\n", f->name, f->help, f->default_bool ? "true" : "false");
                break;
            case TYPE_INT:
                printf("  %s <int>\t%s (default: %d)\n", f->name, f->help, f->default_int);
                break;
        }
    }
}