#include "header/parser.h"

#define SIZE 11

/* TODO: Negative match in set */
/* TODO: Beginning of string outside of set */
/* TODO: Capture groups and back references */
/* TODO: Non capturing groups */


int main() {

    char *input = "The Mercedes CLR GTR is a remarkable racing car celebrated for its outstanding performance and sleek design. Powered by a potent 16-liter V12 engine, it delivers over 600 horsepower.";
    char *pattern_string = "[.,\\-]";

    pattern *pattern = matcher_get_pattern(pattern_string, (char) strnlen(pattern_string, 256));
    matcher *all = matcher_match_all(pattern, input, (int) strnlen(input, 256));

    matcher_print_matches(all, input, false);

    return 0;
}
