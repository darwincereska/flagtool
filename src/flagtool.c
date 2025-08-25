/*
 *
 * FLAG TOOL MADE BY @darwincereska on GitHub
 * VERSION 1.0.4
 * https://github.com/darwincereska/flagtool
 *
 */

#include "flagtool.h"
#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

// Enum for flag types
typedef enum { TYPE_STRING, TYPE_BOOL, TYPE_INT } FlagType;

// Constants for hash table and maximum flag names
#define HASH_TABLE_SIZE 256
#define MAX_FLAG_NAMES 10
#define MAX_FLAG_INSTANCES 64

// Structure for hash table nodes
typedef struct HashNode {
    Flag *flag;               // Pointer to the associated flag
    struct HashNode *next;    // Pointer to the next node in the linked list
} HashNode;

// Hash table for storing flags
static HashNode *hash_table[HASH_TABLE_SIZE];

// Hash function to compute index for a given string
unsigned int hash(const char *str) {
    unsigned int hash = 0;
    while (*str) {
        hash = (hash << 5) + *str++; // Shift and add character
    }
    return hash % HASH_TABLE_SIZE; // Return index within hash table size
}

// Structure representing a flag
struct Flag {
    const char *names[MAX_FLAG_NAMES];  // Array of flag names
    int name_count;                     // Number of names
    const char *help;                   // Help description
    FlagType type;                      // Type of the flag
    FlagGroup *group;                   // Pointer to flag group

    const char *default_str;            // Default string value
    char *value_str;                    // Current string value
    char *multiple_str_values[MAX_FLAG_INSTANCES]; // Store string values

    int default_bool;                   // Default boolean value
    int value_bool;                     // Current boolean value

    int default_int;                    // Default integer value
    int value_int;                      // Current integer value
    int multiple_int_values[MAX_FLAG_INSTANCES - 1]; // Stores int values (no NULL for int)
    
    int multiple_values_count;          // Number of multi values
    int supports_multiple;              // Flag to indicate if multiple instances allowed
    int is_set;                         // Flag indicating if the value is set
};

// Structure representing a flag group
typedef struct FlagGroup {
    const char *name; // Name of flag group
    Flag **flags; // Flag in group
    int flag_count; // Number of flags in group
} FlagGroup;

#define MAX_FLAGS 100
#define MAX_GROUPS 20
static FlagGroup *groups[MAX_GROUPS];
static int group_count = 0;
static Flag *flags[MAX_FLAGS]; // Array to store registered flags
static int flag_count = 0;

// Function to log error to stderr
void log_error(const char *message) {
    fprintf(stderr,"Error: %s\n", message); 
}

// Function to log error for memory allocation and exit
void handle_allocation_failure(const char* entity) {
    fprintf(stderr, "Error: Failed to allocate memory for %s\n", entity);
    exit(1);
}

// Function to collect names from variadic arguments
static int collect_names(va_list args, const char *names[], int max_names) {
    int count = 0;
    while (count < max_names) {
        const char *name = va_arg(args, const char *);
        if (name == NULL) break; // Stop if NULL is encountered
        names[count++] = name; // Store name
    }
    return count; // Return the number of names collected
}

// Function to add a flag to the hash table
static void add_flag_to_hash_table(Flag *flag) {
    for (int i = 0; i < flag->name_count; i++) {
        unsigned int index = hash(flag->names[i]); // Compute index
        HashNode *new_node = malloc(sizeof(HashNode)); // Allocate new node
        if (!new_node) {
            handle_allocation_failure("HashNode");
        }
        new_node->flag = flag; // Set flag in node
        new_node->next = hash_table[index]; // Link to existing nodes
        hash_table[index] = new_node; // Insert at the head of the list
    }
}

// Function to create a new flag
static Flag *create_flag(const char *names[], int name_count, const char *help, FlagType type) {
    if (flag_count >= MAX_FLAGS) {
        log_error("Too many flags registered");
        exit(1);
    }
    if (name_count > MAX_FLAG_NAMES) {
        fprintf(stderr, "Too many names for one flag (max %d)\n", MAX_FLAG_NAMES);
        exit(1);
    }
    Flag *f = calloc(1, sizeof(Flag)); // Allocate memory for the flag
    if (!f) {
        handle_allocation_failure("Flag");
    }
    for (int i = 0; i < name_count; i++) {
        f->names[i] = names[i]; // Copy names
    }
    f->name_count = name_count;
    f->help = help;
    f->type = type;
    flags[flag_count++] = f; // Register the flag

    // Add flag to hash table
    add_flag_to_hash_table(f);

    return f;
}

// General function to create any flag
Flag *create_generic_flag(FlagType type, int default_val, const char *default_str, int supports_multiple, const char *help, ...) {
    const char *names[MAX_FLAG_NAMES];
    va_list args;
    va_start(args, help);
    int count = collect_names(args, names, MAX_FLAG_NAMES);
    va_end(args);
    
    Flag *f = create_flag(names, count, help, type);
    if (type == TYPE_STRING) {
        f->default_str = default_str;
    } else {
        f->default_int = default_val;
        f->value_int = default_val;
    }
    f->supports_multiple = supports_multiple;
    f->is_set = 0;
    f->multiple_values_count = 0;
    return f;
}

// Function to create a string flag (multiple names)
Flag *flag_string(const char *default_val, const char *help, ...) {
    va_list args;
    va_start(args, help);
    Flag *f = create_generic_flag(TYPE_STRING, 0, default_val, 0, help, args);
    va_end(args);
    return f;
}

// Function to create a boolean flag (multiple names)
Flag *flag_bool(int default_val, const char *help, ...) {
    va_list args;
    va_start(args, help);
    Flag *f = create_generic_flag(TYPE_BOOL, default_val, NULL, 0, help, args);
    va_end(args);
    return f;
}

// Function to create an integer flag (multiple names)
Flag *flag_int(int default_val, const char *help, ...) {
    va_list args;
    va_start(args, help);
    Flag *f = create_generic_flag(TYPE_INT, default_val, NULL, 0, help, args);
    va_end(args);
    return f;
}

// Function to create string flag (multiple names and instances)
Flag *flag_string_multi(const char *default_val, const char *help, ...) {
    va_list args;
    va_start(args, help);
    Flag *f = create_generic_flag(TYPE_STRING, 0, default_val, 1, help, args);
    va_end(args);
    return f;
}

// Function to create int flag (multiple names and instances)
Flag *flag_int_multi(int default_val, const char *help, ...) {
    va_list args;
    va_start(args, help);
    Flag *f = create_generic_flag(TYPE_INT, default_val, NULL, 1, help, args);
    va_end(args);
    return f;
}

// Function to get the string value of a flag
const char *flag_get_string(Flag *flag) {
    if (flag->type != TYPE_STRING) return NULL; // Check type
    if (flag->is_set) return flag->value_str; // Return current value if set
    return flag->default_str; // Return default value
}

// Function to get the boolean value of a flag
int flag_get_bool(Flag *flag) {
    if (flag->type != TYPE_BOOL) return 0; // Check type
    return flag->value_bool; // Return current boolean value
}

// Function to get the integer value of a flag
int flag_get_int(Flag *flag) {
    if (flag->type != TYPE_INT) return 0; // Check type
    return flag->value_int; // Return current integer value
}

// Function to get the string values from a multi-instance flag
const char **flag_get_string_multi(Flag *flag) {
    if (flag->type != TYPE_STRING || !flag->supports_multiple) return NULL;
    return (const char **)flag->multiple_str_values;
}

// Function to get the int values from a multi-instance flag
const int *flag_get_int_multi(Flag *flag) {
    if (flag->type != TYPE_INT || !flag->supports_multiple) return NULL;
    return flag->multiple_int_values;
}

// Function to return the count of a multi-instance integer flag
int flag_get_multiple_int_count(Flag *flag) {
    if (flag->type != TYPE_INT || !flag->supports_multiple) return 0;
    return flag->multiple_values_count;
}

// Function to create a flag group
FlagGroup *create_flag_group(const char *name) {
    // Check if we can add more groups
    if (group_count >= MAX_GROUPS) {
        fprintf(stderr, "Cannot create more groups, maximum limit reached\n");
        exit(1);
    }

    // Allocate memory for the group
    FlagGroup *group = malloc(sizeof(FlagGroup));
    if (!group) {
        handle_allocation_failure("FlagGroup");
    }

    // Allocate memory for the group name and copy it
    group->name = strdup(name);
    if (!group->name) {
        free(group); // Free previously allocated memory
        handle_allocation_failure("group name");
    }

    // Allocate memory for flags
    group->flags = malloc(MAX_FLAGS * sizeof(Flag *));
    if (!group->flags) {
        perror("Failed to allocate memory for flags");
        free(group);       // Free group
        exit(1);
    }

    group->flag_count = 0;

    // Automatically add the group to the global groups array
    groups[group_count++] = group; // Add to global groups array

    return group;
}

// Function to add flag to a group
void add_flag_to_group(FlagGroup *group, Flag *flag) {
    if (group->flag_count < MAX_FLAGS) {
        group->flags[group->flag_count++] = flag;
        flag->group = group; // Set group pointer to flag
    }
}

// Function to set a flag value
static int set_flag_value(Flag *f, const char *val, int is_negative_bool) {
    if (!f) return 1;

    if (f->type == TYPE_BOOL) {
        f->value_bool = is_negative_bool ? 0 : 1;
        f->is_set = 1;
        return 0;
    }

    if (!val && f->type != TYPE_BOOL) {
        fprintf(stderr, "Missing value for flag %s\n", f->names[0]);
        return 1;
    }

    if (f->supports_multiple) {
        if (f->multiple_values_count < MAX_FLAG_INSTANCES - 1) {
            if (f->type == TYPE_STRING) {
                f->multiple_str_values[f->multiple_values_count++] = strdup(val);
                f->multiple_str_values[f->multiple_values_count] = NULL;
                return 0;
            } else if (f->type == TYPE_INT) {
                char *endptr;
                long v = strtol(val, &endptr, 10);
                if (*endptr != '\0') return 1; // Error
                f->multiple_int_values[f->multiple_values_count++] = (int)v;
                return 0;
            }
        }
    } else {
        // Single-instance setting
        if (f->type == TYPE_STRING) {
            if (f->value_str) free(f->value_str);
            f->value_str = strdup(val);
            if (!f->value_str) return 1;
            f->is_set = 1;
        } else if (f->type == TYPE_INT) {
            char *endptr;
            long v = strtol(val, &endptr, 10);
            if (*endptr != '\0') return 1; // Error
            f->value_int = (int)v;
            f->is_set = 1;
        }
    }

    return 0;
}

// Function to find a flag by name in the hash table
Flag *flag_find(const char *name) {
    unsigned int index = hash(name); // Compute index
    HashNode *node = hash_table[index]; // Get the head of the linked list
    while (node) { // Traverse the linked list
        for (int n = 0; n < node->flag->name_count; n++) { // Loop through flag names
            if (strcmp(node->flag->names[n], name) == 0) {
                return node->flag; // Return the found flag
            }
        }
        node = node->next; // Move to the next node
    }
    return NULL; // Return NULL if not found
}

// Functions to free a flag from memory
void flag_free(Flag *flag) {
    if (flag) {
        free(flag->value_str);
        free(flag);
    }
}

// Function to free flag group
void flag_free_group(FlagGroup *group) {
    if (group) {
        free(group->flags);
        free(group);
    }
}

// Function to free hash table
void free_hash_table() {
    for (int i = 0; i < HASH_TABLE_SIZE; i++) {
        HashNode *node = hash_table[i];
        while (node) {
            HashNode *temp = node;
            node = node->next;
            free(temp); // Free each node
        }
        hash_table[i] = NULL; // Clear the pointer
    }
}

// Function to cleanup all flag related functions
void flags_cleanup() {
    for (int i = 0; i < flag_count; i++) {
        flag_free(flags[i]);
    }
    flag_count = 0; // Reset the count
    for (int n = 0; n < group_count; n++) {
        flag_free_group(groups[n]);
    }
    group_count = 0;
    free_hash_table(); // Clear hash table
}

// Function to print flag usage
void print_flag_usage(const char *progname) {
    printf("Usage: %s [flags]\nFlags:\n", progname);

    // Loop through all groups and print their flags
    for (int i = 0; i < group_count; i++) {
        FlagGroup *group = groups[i];
        printf("  %s:\n", group->name);
        for (int j = 0; j < group->flag_count; j++) {
            Flag *f = group->flags[j];
            printf("    ");
            for (int n = 0; n < f->name_count; n++) {
                printf("%s", f->names[n]);
                if (n + 1 < f->name_count) printf(", "); // Comma separation
            }
            // Print help based on type
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

    // Print ungrouped flags
    printf("\nUngrouped Flags:\n");
    for (int i = 0; i < flag_count; i++) {
        Flag *f = flags[i];
        if (f->group == NULL) { // Check if the flag is not part of any group
            printf("    ");
            for (int n = 0; n < f->name_count; n++) {
                printf("%s", f->names[n]);
                if (n + 1 < f->name_count) printf(", "); // Comma separation
            }
            // Print help based on type
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
}

// Function to parse flags
int flag_parse(int argc, char *argv[]) {
    for (int i = 1; i < argc; i++) { // Loop through each argument
        const char *original_arg = argv[i];
        char *value_from_equal = NULL;
        int is_negative_bool = 0;
 
        // Make a safe copy for matching
        char argbuf[256];
        strncpy(argbuf, original_arg, sizeof(argbuf) - 1);
        argbuf[sizeof(argbuf) - 1] = '\0';

        // Detect --no-flag for booleans
        if (strncmp(argbuf, "--no-", 5) == 0) {
            is_negative_bool = 1;
            memmove(argbuf + 2, argbuf + 5, strlen(argbuf + 5) + 1); // Remove "no-"
        }
 
        // Check for --flag=value form
        char *eq = strchr(argbuf, '=');
        if (eq) {
            *eq = '\0'; // Split the argument at '='
            value_from_equal = eq + 1;
        }
 
        // Use the hash table to find the flag
        Flag *f = flag_find(argbuf);
        if (f) {
            const char *val = value_from_equal;
            if (!val && f->type != TYPE_BOOL) {
                if (i + 1 >= argc) {
                    fprintf(stderr, "Missing value for flag %s\n", f->names[0]);
                    return 1;
                }
                val = argv[++i]; // Get the next argument as value
            }
            if (set_flag_value(f, val, is_negative_bool) != 0) {
                return 1; // Error setting value
            }
        } else {
            fprintf(stderr, "Unknown flag: %s\n", original_arg);
            return 1; // Return error for unknown flag
        }
    }
    return 0; // Success
}