/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#ifndef POLICYPARSER_H
#define POLICYPARSER_H

#ifdef __cplusplus
extern "C" {
#endif

    typedef struct {
        uint64_t value;
        int bits; /* zero if this is a flexint */
    }
    sized_integer_t;

    typedef struct {
        int k; /* one if leaf, otherwise threshold */
        char* attr; /* attribute string if leaf, otherwise null */
        GPtrArray* children; /* pointers to bswabe_policy_t's, len == 0 for leaves */
    }
    cpabe_policy_t;


    int yylex();
    void yyerror(const char* s);
    sized_integer_t* expint(uint64_t value, uint64_t bits);
    sized_integer_t* flexint(uint64_t value);
    cpabe_policy_t* leaf_policy(char* attr);
    char* indexp_policy(char* attr, char* value);
    cpabe_policy_t* kof2_policy(int k, cpabe_policy_t* l, cpabe_policy_t* r);
    cpabe_policy_t* kof_policy(int k, GPtrArray* list);
    cpabe_policy_t* eq_policy(sized_integer_t* n, char* attr);
    cpabe_policy_t* lt_policy(sized_integer_t* n, char* attr);
    cpabe_policy_t* gt_policy(sized_integer_t* n, char* attr);
    cpabe_policy_t* le_policy(sized_integer_t* n, char* attr);
    cpabe_policy_t* ge_policy(sized_integer_t* n, char* attr);
    char* parse_policy_lang(char* s);
    void tidy(cpabe_policy_t* p);
    char* format_policy_postfix(cpabe_policy_t* p);
    int actual_bits(uint64_t value);
    void policy_free(cpabe_policy_t* p);
    void simplify(cpabe_policy_t* p);
#ifdef __cplusplus
}
#endif

#endif /* POLICYPARSER_H */

