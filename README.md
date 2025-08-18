# FlagTool

A lightweight C library for parsing command-line flags with support for strings, booleans, and integers.

---

## Features

-  Define flags with single or multiple names (e.g., `-help`, `h`).
-  Supports string, boolean, and integer flag types.
-  Supports multi-instance flags for both strings and integers.
-  Automatic parsing and value retrieval.
-  Usage help printing.
-  Error handling for unknown flags and invalid values.
-  Memory management functions for freeing flags and cleaning up resources.

---

## Installation

1. Clone or download the source:

    ```bash
    git clone https://github.com/darwincereska/flagtool.git
    ```

2. Build the static library, test, and example programs:

    ```bash
    make
    ```

3. To build only the library:

    ```bash
    make lib
    ```

4. To clean the build files:

    ```bash
    make clean
    ```

---

## Usage

Include the header and link with the static library:

```c
#include "flagtool.h"
```

Build your own program by linking with `libflagtool.a` built by the Makefile:

```bash
gcc -o myprog myprog.c libflagtool.a
```

---

## Example of Single-Name Flags

Define and parse single-name flags:

```c
#include "flagtool.h"

Flag *flagHelp = flag_bool(0, "Show help message", "--help", NULL);
Flag *flagName = flag_string("default", "Name to use", "--name", NULL);
Flag *flagCount = flag_int(1, "Number of items", "--count", NULL);

int main(int argc, char *argv[]) {
    if (flag_parse(argc, argv) != 0) {
        print_flag_usage(argv[0]);
        return 1;
    }

    if (flag_get_bool(flagHelp)) {
        print_flag_usage(argv[0]);
        return 0;
    }

    printf("Name: %s\n", flag_get_string(flagName));
    printf("Count: %d\n", flag_get_int(flagCount));

    return 0;
}
```

---

## Example of Multi-Name Flags

Define and parse multi-name flags:

```c
#include "flagtool.h"

Flag *flagVerbose = flag_bool(0, "Enable verbose output", "--verbose", "-v", NULL);
Flag *flagOutput = flag_string("out.txt", "Output file", "--output", "-o", NULL);
Flag *flagLevel = flag_int(1, "Level of detail", "--level", "-l", NULL);

int main(int argc, char *argv[]) {
    if (flag_parse(argc, argv) != 0) {
        print_flag_usage(argv[0]);
        return 1;
    }

    if (flag_get_bool(flagVerbose)) {
        printf("Verbose mode enabled\n");
    }
    printf("Output file: %s\n", flag_get_string(flagOutput));
    printf("Detail level: %d\n", flag_get_int(flagLevel));

    return 0;
}
```

**IMPORTANT:** When using flag functions, end the function with `NULL`.

---

## Multi-Instance Flags

FlagTool supports defining flags that can accept multiple instances for both strings and integers. Use the following functions to retrieve the values:

### Accessing Multi-Instance String Values

For a string flag that allows multiple instances:

```c
#include "flagtool.h"

Flag *flagNames = flag_string_multi(NULL, "List of names", "--name", "-n", NULL);

int main(int argc, char *argv[]) {
    if (flag_parse(argc, argv) != 0) {
        print_flag_usage(argv[0]);
        return 1;
    }

    const char **names = flag_get_string_multi(flagNames);
    if (names) {
        for (int i = 0; names[i] != NULL; i++) {
            printf("Name[%d]: %s\n", i, names[i]);
        }
    }

    return 0;
}
```

### Accessing Multi-Instance Integer Values

For an integer flag that allows multiple instances:

```c
#include "flagtool.h"

Flag *flagNumbers = flag_int_multi(0, "List of numbers", "--number", "-num", NULL);

int main(int argc, char *argv[]) {
    if (flag_parse(argc, argv) != 0) {
        print_flag_usage(argv[0]);
        return 1;
    }

    const int *numbers = flag_get_int_multi(flagNumbers);
    int count = flag_get_multiple_int_count(flagNumbers);
    for (int i = 0; i < count; i++) {
        printf("Number[%d]: %d\n", i, numbers[i]);
    }

    return 0;
}
```

These functions allow you to collect and process multiple values set through command-line flags, accommodating both strings and integers in your application.

---

## Examples & Tests

-  `make example` builds an example program using the library.
-  `make test` builds and runs tests.

---

## Memory Management

-  Use `flag_free(Flag *flag)` to free a flag from memory.
-  Call `flags_cleanup()` to free all registered flags and clear the hash table.

---

## Error Handling

The library automatically handles errors for unknown flags and invalid values, providing appropriate messages to the user.

---