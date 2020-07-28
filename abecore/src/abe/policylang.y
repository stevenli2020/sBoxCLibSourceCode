%{
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <glib.h>
#include <pbc.h>

#include "util/abeutil.h"
#include "abe/policyparser.h"

    cpabe_policy_t* final_policy = 0;

%}

%union
{
	char* str;
	char* str2;
	char* val;
	uint64_t nat;
  sized_integer_t* sint;
	cpabe_policy_t* tree;
	GPtrArray* list;
}

%token <str>  TAG
%token <str2>  NAME
%token <nat>  INTLIT
%type  <val> string
%type  <sint> number
%type  <tree> policy
%type  <list> arg_list

%left OR
%left AND
%token OF
%token LEQ
%token GEQ

%%

result: policy { final_policy = $1; }

number:   INTLIT '#' INTLIT          { $$ = expint($1, $3); }
        | INTLIT                     { $$ = flexint($1);    }

string:   NAME ':' TAG		     { $$ = indexp_policy($1, $3); }
        | TAG                     	{ $$ = $1;    }

policy:   string                     { $$ = leaf_policy($1);        }
  	| policy OR  policy          { $$ = kof2_policy(1, $1, $3); }
        | policy AND policy          { $$ = kof2_policy(2, $1, $3); }
        | INTLIT OF '(' arg_list ')' { $$ = kof_policy($1, $4);     }
        | TAG '=' number             { $$ = eq_policy($3, $1);      }
        | TAG '<' number             { $$ = lt_policy($3, $1);      }
        | TAG '>' number             { $$ = gt_policy($3, $1);      }
        | TAG LEQ number             { $$ = le_policy($3, $1);      }
        | TAG GEQ number             { $$ = ge_policy($3, $1);      }
        | number '=' TAG             { $$ = eq_policy($1, $3);      }
        | number '<' TAG             { $$ = gt_policy($1, $3);      }
        | number '>' TAG             { $$ = lt_policy($1, $3);      }
        | number LEQ TAG             { $$ = ge_policy($1, $3);      }
        | number GEQ TAG             { $$ = le_policy($1, $3);      }
        | '(' policy ')'             { $$ = $2;                     }

arg_list: policy                     { $$ = g_ptr_array_new();
                                       g_ptr_array_add($$, $1); }
        | arg_list ',' policy        { $$ = $1;
                                       g_ptr_array_add($$, $3); }
;

%%

char* cur_string = 0;

#define PEEK_CHAR ( *cur_string ? *cur_string     : EOF )
#define NEXT_CHAR ( *cur_string ? *(cur_string++) : EOF )

int handle_token(GString* s) {
    int r = 0;
    while (isalnum(PEEK_CHAR) || PEEK_CHAR == '_' || PEEK_CHAR == ' ')
        g_string_append_c(s, NEXT_CHAR);

    if (!strcmp(s->str, "and")) {
        g_string_free(s, 1);
        r = AND;
    } else if (!strcmp(s->str, "or")) {
        g_string_free(s, 1);
        r = OR;
    } else if (!strcmp(s->str, "of")) {
        g_string_free(s, 1);
        r = OF;
    } else {
        while (PEEK_CHAR == ' ')
            NEXT_CHAR;
        int i = 0;
        while (isspace(s->str[i]))
            i++;
        int length = s->len;
        while (isspace(s->str[length - 1]))
            length--;
        int newLength = s->len - length;
 if (newLength > 0)
        g_string_erase(s, length, newLength);
    if (i > 0)
        g_string_erase(s, 0, i);
  
        GByteArray* value = g_byte_array_new_take(s->str, s->len);
        GByteArray* v64 = encode64(value);
        char* val = to_string(v64);
        if (PEEK_CHAR == ':') {
            yylval.str2 = val;
            r = NAME;
        } else {
            yylval.str = val;
            r = TAG;
        }
        g_byte_array_free(value, 0);
        g_byte_array_free(v64, 0);
        g_string_free(s, 1);
    }
    return r;
}

int
yylex() {
    int c;
    int r;

    while (isspace(c = NEXT_CHAR));

    r = 0;
    if (c == EOF)
        r = 0;
    else if (c == '&')
        r = AND;
    else if (c == '|')
        r = OR;
    else if (strchr("(),=#:", c) || (strchr("<>", c) && PEEK_CHAR != '='))
        r = c;
    else if (c == '<' && PEEK_CHAR == '=') {
        NEXT_CHAR;
        r = LEQ;
    } else if (c == '>' && PEEK_CHAR == '=') {
        NEXT_CHAR;
        r = GEQ;
    } else if (isdigit(c)) {
        GString* s;

        s = g_string_new("");
        g_string_append_c(s, c);
        while (isdigit(PEEK_CHAR))
            g_string_append_c(s, NEXT_CHAR);
        if (PEEK_CHAR == ' ' || PEEK_CHAR == EOF) {
            sscanf(s->str, "%llu", &(yylval.nat));
            g_string_free(s, 1);
            r = INTLIT;
        } else {
            r = handle_token(s);
        }
    } else if (isalpha(c)) {
        GString* s;
        s = g_string_new("");
        g_string_append_c(s, c);
        r = handle_token(s);
    } else
        die("syntax error at \"%c%s\"\n", c, cur_string);

    return r;
}

void
yyerror(const char* s) {
    die("error parsing policy: %s\n", s);
}


char* parse_policy_lang(char* s) {
    char* parsed_policy;

    cur_string = s;

    yyparse();
    simplify(final_policy);
    tidy(final_policy);
    parsed_policy = format_policy_postfix(final_policy);

    policy_free(final_policy);

    return parsed_policy;
}
