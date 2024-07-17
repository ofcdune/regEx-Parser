#include "header/parser.h"
#include <time.h>

// #include <fcntl.h>

/* token *cur;
*  while (NULL != (cur = (token *) seekable_peek(tokenstream))) {
*       switch (cur->type) {
*           case SYMBOL:
*               printf("%c\n", cur->symbol);
*               break;
*           case INTEGER:
*               printf("%d\n", cur->symbol);
*               break;
*           default:
*               printf("%c\n", cur->type);
*               break;
*       }
*
*       seekable_seek_right(tokenstream);
*   }
*/

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
            "d[abcd]n",
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
            "dat"
	};

	clock_t begin, end;
	double time_spent;

	regex_node *tree;
	seek *tokenStream;

	dfa *dfa;

	int i;
	for (i = 0; i < SIZE; i++) {
		begin = clock();
		tokenStream = lexer_tokenize(testSuite[i], (char) strnlen(testSuite[i], 256));
		end = clock();
		time_spent = (double)(end - begin) / CLOCKS_PER_SEC;

		printf("Tokenizing \"%s\": \x1b[32m%f\x1b[0m\n", testSuite[i], time_spent);

		begin = clock();
		tree = parser_parse(tokenStream, PR_LOWEST);
		end = clock();
		time_spent = (double)(end - begin) / CLOCKS_PER_SEC;

		printf("parsing \"%s\": \x1b[32m%f\x1b[0m\n", testSuite[i], time_spent);

        regex_print_regexp(tree);
        putchar('\n');

        begin = clock();
        dfa = regex_pattern_to_dfa(tree);
        end = clock();
        time_spent = (double)(end - begin) / CLOCKS_PER_SEC;

        printf("pattern 2 dfa \"%s\": \x1b[32m%f\x1b[0m\n", testSuite[i], time_spent);

        begin = clock();
        _Bool matched = match_dfa(dfa, testSuiteMatch[i], (int) strnlen(testSuiteMatch[i], 256));
        end = clock();
        time_spent = (double)(end - begin) / CLOCKS_PER_SEC;

        printf("regex_match \"%s\" with \"%s\" preemptively: \x1b[32m%f\x1b[0m\n", testSuite[i], testSuiteMatch[i], time_spent);

		puts("\x1b[31m------\x1b[0m");
	}

	return 0;
}
