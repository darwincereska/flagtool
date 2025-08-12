#include <stdio.h>
#include "flagtool.h"

int main(int argc, char *argv[]) {
    // Define some flags with defaults and help messages
    Flag *flagDebug = flag_bool("--debug", 0, "Enable debug mode");
    Flag *flagUser = flag_string("--user", "guest", "Username");
    Flag *flagRetries = flag_int("--retries", 3, "Number of retries");
    Flag *flagHelp = flag_bool("--help", 0, "Shows help menu");

    // Parse the command line arguments
    if (flag_parse(argc, argv) != 0) {
        print_flag_usage(argv[0]);
        return 1;
    }
    if (flag_get_bool(flagHelp) != 0) {
        print_flag_usage(argv[0]);
        return 1;
    }
    
    // Retrieve the flag values
    int debug = flag_get_bool(flagDebug);
    const char *user = flag_get_string(flagUser);
    int retries = flag_get_int(flagRetries);

    // Display the results
    printf("Debug mode: %s\n", debug ? "ON" : "OFF");
    printf("User: %s\n", user);
    printf("Retries: %d\n", retries);

    return 0;
}
