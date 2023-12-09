#include "regex/regex.h"
#include "parser/processPattern.h"
#include <time.h>

// #include <fcntl.h>

#define SIZE 10

int main() {
	char *testSuite[SIZE] = {
			"(a|b|c|d|e|f)",
			"(7*e*|d*)l",
			"I+",
			"a*bc|d*",
			"ye(a*|v)",
			"yeah (boi)*",
			"colo(u|)r",
            "\\x:\\x",
            "d[oua]n",
            "d[a-f]t"
	};

	char *testSuiteMatch[SIZE] = {
			"b",
			"3",
			"I'm down bad ngl",
			"aaaaaaaaaabc",
			"yeaaaaaaa",
			"yeah boiboiboi",
			"color",
            "7",
            "don",
            "det"
	};

	clock_t begin, end;
	double time_spent;

	regexNode *tree;
	seek *tokenStream;

	dfa *dfa;

	int i;
	for (i = 0; i < SIZE; i++) {
		begin = clock();
		tokenStream = tokenize(testSuite[i], (char) strnlen(testSuite[i], 256));
		end = clock();
		time_spent = (double)(end - begin) / CLOCKS_PER_SEC;

		printf("Tokenizing \"%s\": \x1b[32m%f\x1b[0m\n", testSuite[i], time_spent);

		begin = clock();
		tree = parse(tokenStream, PR_LOWEST);
		end = clock();
		time_spent = (double)(end - begin) / CLOCKS_PER_SEC;

		printf("parsing \"%s\": \x1b[32m%f\x1b[0m\n", testSuite[i], time_spent);

        print_regExp(tree);
        putchar('\n');

        begin = clock();
        dfa = patternToDFA(tree);
        end = clock();
        time_spent = (double)(end - begin) / CLOCKS_PER_SEC;

        printf("pattern 2 dfa \"%s\": \x1b[32m%f\x1b[0m\n", testSuite[i], time_spent);

        begin = clock();
        _Bool matched = matchDFA(dfa, testSuiteMatch[i], (int) strnlen(testSuiteMatch[i],  256));
        end = clock();
        time_spent = (double)(end - begin) / CLOCKS_PER_SEC;

        printf("match \"%s\" with \"%s\" preemptively: \x1b[32m%f\x1b[0m\n", testSuite[i], testSuiteMatch[i], time_spent);

		puts("\x1b[31m------\x1b[0m");
	}

	return 0;
}
