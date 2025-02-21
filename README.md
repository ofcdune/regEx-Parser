# About

This is a potent and fast RegEx parser written in pure C requiring nothing but the standard library. 
The speeds are comparable to those of PCRE and the code is being frequently updated to add its functionality.

This program incooperates a range of computer science principles such as parsing, recursion and the Szudjik function.

As of now, the functions 
- ```pattern *matcher_get_pattern(char *string, char pattern_string_len)```
- ```matcher *matcher_get_match(matcher *restrict match_object, char *restrict string)```

are the center of the programs functionality.

The program works by 
1. parsing the pattern into a RegEx tree via a pratt parser (recursive descent parser)
2. creating a DFA using the regular expression derivation function
3. the input text is now being matched with the DFA, the results get stored in the matcher object

The matcher object is defined as follows:
```
typedef struct matcher {
    unsigned int from;
    unsigned int to;

    struct matcher *next;
} matcher;
```
