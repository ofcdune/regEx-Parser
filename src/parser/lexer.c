#include "../../header/parser.h"

char lexer_tokenize_int(const char *restrict input_string, int *restrict pos, char len) {

    int i = pos[0];

    NEW(char, buffer, len)
    int buff_pos = 0;

    while ('0' <= input_string[i] && '9' >= input_string[i]) {
        buffer[buff_pos++] = input_string[i++];
    }

    char to_return = (char) strtol(buffer, NULL, 10);
    free(buffer);
    pos[0] = i - 1;

    return to_return;
}

/* This function turns the input string into a token stream (seekable) under the special case that this is a quantifier.
 * Input:
 * - const char *input_string: The input string;
 * - char length: The length of the string;
 * - int *pos: The position of the input so far;
 * - char len: The length of the rest of the string;
 */
void lexer_tokenize_quantifier(seek *restrict tokenstream,
                               token *restrict cur_token, const char *restrict input_string, int *restrict pos, char len) {
    int i = pos[0];

    if (i == 0) {
        fputs("Unexpected quantifier", stderr);
        exit(1);
    }

    i++;

    if (i > 0) {
        seekable_insert_node_right(tokenstream);
        seekable_seek_right(tokenstream);
    }

    seekable_set_current((void *) cur_token, tokenstream->current);

    for (; i < len; i++) {

        if ('}' == input_string[i]) {
            pos[0] = i - 1;
            return;
        }

        /* initialize token */
        cur_token = (token *) calloc(1, sizeof(*cur_token));
        VALNUL(cur_token)

        cur_token->type = input_string[i];
        cur_token->precedence = PR_LOWEST;

        switch (input_string[i]) {
            case ',':
                cur_token->precedence = PR_QUANT;
                break;
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
            case '0':
                cur_token->is_nud = 1;
                cur_token->type = INTEGER;
                cur_token->symbol = lexer_tokenize_int(input_string, &i, len);
                break;
            default:
                fprintf(stderr, "Unknown token: %c", input_string[i]);
        }

        /* insert into seeker */
        if (i > 0) {
            seekable_insert_node_right(tokenstream);
            seekable_seek_right(tokenstream);
        }

        seekable_set_current((void *) cur_token, tokenstream->current);
    }

    fputs("Expected token '}', got: EOF", stderr);
    exit(1);
}

/* This function turns the input string into a token stream (seekable) under the special case that this is an escaped character.
 * Input:
 * - const char *input_string: The input string;
 * - char length: The length of the string;
 * - int *pos: The position of the input so far;
 */
void lexer_tokenize_escaped(seek *restrict tokenstream, token *restrict cur_token, const char *restrict input_string, int *restrict pos) {
    int i = pos[0];
    i++;

    switch (input_string[i]) {
        case 'd':
        case 'D':
        case 'l':
        case 'u':
        case 'v':
        case 'h':
        case 'n':
        case 'N':
        case 'r':
        case 't':
        case 'a':
        case 'b':
        case 'e':
        case 'f':
        case 'k':
        case 'x':  // my own ( matches hexadecimal chars )
        case 'X':
        case 'w':
        case 'W':
        case 's':
        case 'S':
        case 'R':
        case '0':
        case 'E':
        case 'Q':
            cur_token->type = BACKSLASH;
            cur_token->symbol = input_string[i];
            cur_token->precedence = PR_UNION;
            cur_token->is_nud = 1;
            break;

        /* this one is a bit more special... the backslash before a default character means,
         * that the lexer is supposed to skip the current token in the input, which is not in the capabilities
         * of a normal DFA */
        default:
            cur_token->type = SYMBOL;
            cur_token->symbol = input_string[i];
            cur_token->precedence = PR_LOWEST;
            cur_token->is_nud = 1;
    }

    /* insert into seeker */
    if (i > 1) {
        seekable_insert_node_right(tokenstream);
        seekable_seek_right(tokenstream);
    }

    seekable_set_current((void *) cur_token, tokenstream->current);

    pos[0] = i;
}

/* This function turns the input string into a token stream (seekable) under the special case that this is a set.
 * Input:
 * - const char *input_string: The input string;
 * - char length: The length of the string;
 * - int *pos: The position of the input so far;
 * - char len: The length of the rest of the string;
 */
void lexer_tokenize_set(seek *restrict tokenstream,
                        token *restrict cur_token, const char *restrict input_string, int *restrict pos, char len) {
    int i = pos[0];

    if (i > 0) {
        seekable_insert_node_right(tokenstream);
        seekable_seek_right(tokenstream);
    }

    seekable_set_current((void *) cur_token, tokenstream->current);

    i++;

    for (; i < len; i++) {

        if (']' == input_string[i]) {
            pos[0] = i - 1;
            return;
        }

        /* initialize token */
        cur_token = (token *) calloc(1, sizeof(*cur_token));
        VALNUL(cur_token)

        if (input_string[i] == '\\') {

            if (i + 1 == len) {
                fputs("Expected token, got: EOF", stderr);
                exit(1);
            }

            lexer_tokenize_escaped(tokenstream, cur_token, input_string, &i);
            continue;
        }

        cur_token->type = input_string[i];
        cur_token->precedence = PR_LOWEST;

        switch (input_string[i]) {
            case '-':
                cur_token->precedence = PR_SETRANGE;
                break;
            case '^':
                cur_token->is_nud = 1;
                break;
            default:
                cur_token->type = SYMBOL;
                cur_token->symbol = input_string[i];
                cur_token->is_nud = 1;
        }

        /* insert into seeker */
        if (i > 0) {
            seekable_insert_node_right(tokenstream);
            seekable_seek_right(tokenstream);
        }

        seekable_set_current((void *) cur_token, tokenstream->current);
    }

    fputs("Expected token ']', got: EOF", stderr);
    exit(1);
}

/* This function turns the input string into a token stream (seekable).
 * Input:
 * - const char *input_string: The input string;
 * - char length: The length of the string;
 * Output:
 * - seek *tokenstream: The token stream to use later on;
 */
seek *lexer_tokenize(const char *input_string, char length) {
    /* initialize token stream */
    seek *tokenstream = seekable_initialize();

	int i;
	for (i = 0; i < length; i++) {
        NEW(token, cur_token, 1)

        /* Escaped character */
		if (input_string[i] == '\\') {

			if (i + 1 == length) {
				fputs("Expected token, got: <EOF>", stderr);
				exit(1);
			}

            lexer_tokenize_escaped(tokenstream, cur_token, input_string, &i);
            continue;
		}

        cur_token->type = input_string[i];
		switch (input_string[i]) {
			case '[':
                cur_token->precedence = PR_UNION;
                cur_token->is_nud = 1;
                lexer_tokenize_set(tokenstream, cur_token, input_string, &i, length);
                continue;
			case '^':
			case '$':
			case '.':
			case '(':
                cur_token->precedence = PR_LOWEST;
                cur_token->is_nud = 1;
				break;
			case '{':
                cur_token->precedence = PR_QUANT;
                lexer_tokenize_quantifier(tokenstream, cur_token, input_string, &i, length);
                continue;
			case ')':
			case '>':
			case ']':
			case '}':
			case ',':
			case '<':
                cur_token->precedence = PR_LOWEST;
				break;
			case '|':
			case '?':
                cur_token->precedence = PR_UNION;
				break;
			case '*':
			case '+':
                cur_token->precedence = PR_KLEENE;
				break;

			default:
                cur_token->type = SYMBOL;
                cur_token->symbol = input_string[i];
                cur_token->precedence = PR_LOWEST;
                cur_token->is_nud = 1;
				break;
		}

		/* insert into seeker */
		if (i > 0) {
            seekable_insert_node_right(tokenstream);
            seekable_seek_right(tokenstream);
		}

        seekable_set_current((void *) cur_token, tokenstream->current);
	}

    tokenstream->current = tokenstream->start;
	return tokenstream;
}