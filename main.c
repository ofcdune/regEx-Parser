#include "regex.h"
#include <time.h>

int main1() {

	clock_t begin, end;
	double time_spent;

	char *pattern = "(0|1)+";
	char *string = "00000000000";

	printf("Pattern: %s\n", pattern);
	regexNode *tree = parse(pattern);
	printf("Resulting regular expression tree: ");
	print_regExp(tree);
	putchar('\n');

	begin = clock();
	_Bool hasMatched = match(tree, string);
	end = clock();
	time_spent = (double)(end - begin) / CLOCKS_PER_SEC;

	printf("Execution time: %fs\n", time_spent);

	printf("Input: %s\n", string);
	printf("Matched: %s\n", hasMatched ? "true" : "false");

	return 0;
}

int main() {

    clock_t begin, end;
    double time_spent;

    char *pattern = "(0|1)+";
    char *string = "0002000";

    printf("Pattern: %s\n", pattern);
    regexNode *tree = parse(pattern);
    printf("Resulting regular expression tree: ");
    print_regExp(tree);
    putchar('\n');

    printf("Input: %s\n", string);
    begin = clock();
    int *matchResult = matchAny(tree, string);
    end = clock();
    time_spent = (double) (end - begin) / CLOCKS_PER_SEC;

    printf("Execution time: %fs\n", time_spent);

    int i, j;
    for (i = 1; i <= matchResult[0]; i++) {
        printf("%d\n", matchResult[i]);
    }

    // for (i = 1; i < matchResult[0]; i++) {
    //     for (j = matchResult[i]; j < matchResult[i + 1]; j++) {
    //         printf("%c", string[j]);
    //     }
    //     putchar('\n');
    // }

    return 0;
}