# flagtool

A lightweight C library for parsing command-line flags with support for strings, booleans, and integers.

---

## Features

- Define flags with single or multiple names (e.g. `-help`, `h`)
- Supports string, boolean, and integer flag types
- Automatic parsing and value retrieval
- Usage help printing
- Error handling for unknown flags and invalid values
- Memory management functions for freeing flags and cleaning up resources

---

## Installation

1. Clone or download the source.

```bash
git clone https://github.com/darwincereska/flagtool.git
```

1. Build the static library, test, and example programs.

```bash
make
```

1. To build only the library

```bash
make lib
```

1. To clean the build files

```bash
make clean
```

---

## Usage

Include the  `header` and link with the static library:

```c
#include "flagtool.h"
```

Build your own program by linking with `libflagtool.a` built by the Makefile:

```bash
gcc -o myprog myprog.c libflagtool.a
```

---

## Single-name flags

```c
#include "flagtool.h"

Flag *flagHelp = flag_bool(0, "Show help message", "--help");
Flag *flagName = flag_string("default", "Name to use", "--name");
Flag *flagCount = flag_int(1, "Number of items", "--count");

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

## Multi-name flags

You can define a flag with multiple names (aliases) using the variadic multi functions:

```c
#include "flagtool.h"

Flag *flagVerbose = flag_bool_multi(0, "Enable verbose output", "--verbose", "-v", NULL);
Flag *flagOutput = flag_string_multi("out.txt", "Output file", "--output", "-o", NULL);
Flag *flagLevel = flag_int_multi(1, "Level of detail", "--level", "-l", NULL);

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

<aside>
❗

**Note:** When using multi-name functions, pass the flag names as multiple string arguments ending with `NULL`.

</aside>

---

## Example & Tests

- `make example` builds an example program using the library.
- `make test` builds and runs tests.

---

## Memory Management

- Use `flag_free(Flag *flag)` to free a flag from memory.
- Call `flags_cleanup()` to free all registered flags and clear the hash table.

## Error Handling

- The library automatically handles errors for unknown flags and invalid values, providing appropriate messages to the user.

---