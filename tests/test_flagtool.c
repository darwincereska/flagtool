#include <stdio.h>
#include "flagtool.h"

int main(int argc, char *argv[]) {
    // Define flags
    Flag *flagName = flag_string("--name", "World", "Name to greet");
    Flag *flagVerbose = flag_bool("--verbose", 0, "Enable verbose output");
    Flag *flagCount = flag_int("--count", 1, "Number of greetings");

    // Parse command line flags
    if (flag_parse(argc, argv) != 0) {
        print_flag_usage(argv[0]);
        return 1;
    }

    // Get flag values
    const char *name = flag_get_string(flagName);
    int verbose = flag_get_bool(flagVerbose);
    int count = flag_get_int(flagCount);

    // Print greetings
    for (int i = 0; i < count; i++) {
        if (verbose) {
            printf("Greeting %d: Hello, %s!\n", i + 1, name);
        } else {
            printf("Hello, %s!\n", name);
        }
    }

    return 0;
}
