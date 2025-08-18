#include <stdio.h>
#include "flagtool.h"

int main(int argc, char *argv[]) {
    // Help flag with single name
    Flag *flagHelp = flag_bool(0, "Show help menu", "--help");
    // Debug flag with multiple names, HAS TO END WITH NULL
    Flag *flagDebug = flag_bool_multi(1, "Enable debug mode", "--debug", "-d", NULL);
    // User flag with multiple names, HAS TO END WITH NULL
    Flag *flagUser = flag_string_multi("guest", "Username", "--user", "-u", NULL);
    // Retries flag with multiple names, HAS TO END WITH NULL
    Flag *flagRetries = flag_int_multi(3, "Number of retries", "--retries", "-r", NULL);
    
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
    printf("User: %s\n", flag_get_string(flagUser));
    printf("Retries: %d\n", flag_get_int(flagRetries));
    
    // Cleanup before exiting
    flags_cleanup();
    return 0;
}
