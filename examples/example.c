#include <stdio.h>
#include "flagtool.h"

int main(int argc, char *argv[]) {
    // Help flag with single name
    Flag *flagHelp = flag_bool(0, "Show help menu", "--help", NULL);

    // Debug flag with multiple names, HAS TO END WITH NULL
    Flag *flagDebug = flag_bool(1, "Enable debug mode", "--debug", "-d", NULL);

    // User flag with multiple names, supporting multiple instances, HAS TO END WITH NULL
    Flag *flagUser = flag_string_multi("guest", "Username (can be specified multiple times)", "--user", "-u", NULL);

    // Retries flag with multiple names, supporting multiple instances, HAS TO END WITH NULL
    Flag *flagRetries = flag_int_multi(3, "Number of retries (can be specified multiple times)", "--retries", "-r", NULL);

    // Group example
    FlagGroup *inputGroup = create_flag_group("Input Options");
    add_flag_to_group(inputGroup, flagUser);
    add_flag_to_group(inputGroup, flagRetries);

    FlagGroup *outputGroup = create_flag_group("Output Options");
    add_flag_to_group(outputGroup, flagDebug);

    // Show flag usage
    if (flag_parse(argc, argv)) {
        print_flag_usage(argv[0]);
        return 1;
    }

    // Show help menu
    if (flag_get_bool(flagHelp)) {
        print_flag_usage("example");
        return 1;
    }

    printf("Debug: %s\n", flag_get_bool(flagDebug) ? "ON" : "OFF");

    // Retrieve and print multiple user string values using NULL terminator
    const char **userValues = flag_get_string_multi(flagUser);
    printf("Usernames:\n");
    for (int i = 0; userValues[i] != NULL; i++) {
        printf("  - %s\n", userValues[i]);
    }

    // Retrieve and print multiple retry integer values using count function
    int retryCount = flag_get_multiple_int_count(flagRetries);
    const int *retryValues = flag_get_int_multi(flagRetries);
    printf("Retries:\n");
    for (int i = 0; i < retryCount; i++) {
        printf("  - %d\n", retryValues[i]);
    }

    // Cleanup before exiting
    flags_cleanup();
    return 0;
}
