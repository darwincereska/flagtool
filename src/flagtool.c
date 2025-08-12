#include "flagtool.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

typedef enum { TYPE_STRING, TYPE_BOOL, TYPE_INT } FlagType;

#define MAX_FLAG_NAMES 10

struct Flag {
    const char *names[MAX_FLAG_NAMES];
    int name_count;
    const char *help;
    FlagType type;

    const char *default_str;
    char *value_str;

    int default_bool;
    int value_bool;

    int default_int;
    int value_int;

    int is_set;
};

#define MAX_FLAGS 100
static Flag *flags[MAX_FLAGS];
static int flag_count = 0;

static Flag *create_flag(const char *names[], int name_count, const char *help, FlagType type) {
    if (flag_count >= MAX_FLAGS) {
        fprintf(stderr, "Too many flags registered\n");
        exit(1);
    }
    if (name_count > MAX_FLAG_NAMES) {
        fprintf(stderr, "Too many names for one flag (max %d)\n", MAX_FLAG_NAMES);
        exit(1);
    }
    Flag *f = calloc(1, sizeof(Flag));
    if (!f) {
        perror("calloc");
        exit(1);
    }
    for (int i = 0; i < name_count; i++) {
        f->names[i] = names[i];
    }
    f->name_count = name_count;
    f->help = help;
    f->type = type;
    flags[flag_count++] = f;
    return f;
}

static int collect_names(va_list args, const char *names[], int max_names) {
    int count = 0;
    while (count < max_names) {
        const char *name = va_arg(args, const char *);
        if (name == NULL) break;
        names[count++] = name;
    }
    return count;
}

// Single-name functions (name last)
Flag *flag_string(const char *default_val, const char *help, const char *name) {
    const char *names[1] = {name};
    Flag *f = create_flag(names, 1, help, TYPE_STRING);
    f->default_str = default_val;
    f->value_str = NULL;
    f->is_set = 0;
    return f;
}

Flag *flag_bool(int default_val, const char *help, const char *name) {
    const char *names[1] = {name};
    Flag *f = create_flag(names, 1, help, TYPE_BOOL);
    f->default_bool = default_val;
    f->value_bool = default_val;
    f->is_set = 0;
    return f;
}

Flag *flag_int(int default_val, const char *help, const char *name) {
    const char *names[1] = {name};
    Flag *f = create_flag(names, 1, help, TYPE_INT);
    f->default_int = default_val;
    f->value_int = default_val;
    f->is_set = 0;
    return f;
}

// Multi-name variadic functions
Flag *flag_string_multi(const char *default_val, const char *help, ...) {
    const char *names[MAX_FLAG_NAMES];
    va_list args;
    va_start(args, help);
    int count = collect_names(args, names, MAX_FLAG_NAMES);
    va_end(args);

    Flag *f = create_flag(names, count, help, TYPE_STRING);
    f->default_str = default_val;
    f->value_str = NULL;
    f->is_set = 0;
    return f;
}

Flag *flag_bool_multi(int default_val, const char *help, ...) {
    const char *names[MAX_FLAG_NAMES];
    va_list args;
    va_start(args, help);
    int count = collect_names(args, names, MAX_FLAG_NAMES);
    va_end(args);

    Flag *f = create_flag(names, count, help, TYPE_BOOL);
    f->default_bool = default_val;
    f->value_bool = default_val;
    f->is_set = 0;
    return f;
}

Flag *flag_int_multi(int default_val, const char *help, ...) {
    const char *names[MAX_FLAG_NAMES];
    va_list args;
    va_start(args, help);
    int count = collect_names(args, names, MAX_FLAG_NAMES);
    va_end(args);

    Flag *f = create_flag(names, count, help, TYPE_INT);
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
            for (int n = 0; n < f->name_count; n++) {
                if (strcmp(argv[i], f->names[n]) == 0) {
                    matched = 1;
                    if (f->type == TYPE_BOOL) {
                        f->value_bool = 1;
                        f->is_set = 1;
                    } else {
                        if (i + 1 >= argc) {
                            fprintf(stderr, "Missing value for flag %s\n", f->names[0]);
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
                                fprintf(stderr, "Invalid integer for flag %s: %s\n", f->names[0], val);
                                return 1;
                            }
                            f->value_int = (int)v;
                            f->is_set = 1;
                        }
                    }
                    break;
                }
            }
            if (matched) break;
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
        printf("  ");
        for (int n = 0; n < f->name_count; n++) {
            printf("%s", f->names[n]);
            if (n + 1 < f->name_count) printf(", ");
        }
        switch (f->type) {
            case TYPE_STRING:
                printf(" <string>\t%s (default: %s)\n", f->help, f->default_str ? f->default_str : "none");
                break;
            case TYPE_BOOL:
                printf("\t%s (default: %s)\n", f->help, f->default_bool ? "true" : "false");
                break;
            case TYPE_INT:
                printf(" <int>\t%s (default: %d)\n", f->help, f->default_int);
                break;
        }
    }
}
