/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "glib.h"
#include "pbc.h"
#include "base/base_obj.h"
#include "base/base_free.h"
#include "base/base.h"
#include "util/common.h"
#include "util/abe_util.h"

#ifndef ABE_DEBUG
#define NDEBUG
#endif
#define RBITS 160
#define QBITS 512

char last_error[256];

char* cpabe_error() {
    return last_error;
}

void raise_error(char* fmt, ...) {
    va_list args;

#ifdef ABE_DEBUG
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
    exit(1);
#else
    va_start(args, fmt);
    vsnprintf(last_error, 256, fmt, args);
    va_end(args);
#endif
}

int cpabe_setup_1(pairing_ptr pairing, char* pairing_desc, cpabe_pub_t** pub, cpabe_msk_t** msk){
    if(!pairing || !pub || !msk)
        return 0;
    element_t alpha;

    *pub = malloc(sizeof (cpabe_pub_t));
    *msk = malloc(sizeof (cpabe_msk_t));
    (*pub)->p = pairing;
    (*pub)->pairing_desc = pairing_desc;

    element_init_G1((*pub)->g, (*pub)->p);
    element_init_G1((*pub)->h, (*pub)->p);
    element_init_G2((*pub)->gp, (*pub)->p);
    element_init_GT((*pub)->g_hat_alpha, (*pub)->p);
    element_init_Zr(alpha, (*pub)->p);
    element_init_Zr((*msk)->beta, (*pub)->p);
    element_init_G2((*msk)->g_alpha, (*pub)->p);

    element_random(alpha);
    element_random((*msk)->beta);
    element_random((*pub)->g);
    element_random((*pub)->gp);

    element_pow_zn((*msk)->g_alpha, (*pub)->gp, alpha);
    element_pow_zn((*pub)->h, (*pub)->g, (*msk)->beta);
    pairing_apply((*pub)->g_hat_alpha, (*pub)->g, (*msk)->g_alpha, (*pub)->p);
    return 1;
}

int cpabe_setup_2(char* pairing_desc, cpabe_pub_t** pub, cpabe_msk_t** msk) {
    if(!pairing_desc || !pub || !msk)
        return 0;
    pairing_t pairing;
    pairing_init_set_buf(pairing, pairing_desc, strlen(pairing_desc));
    return cpabe_setup_1(pairing, pairing_desc, pub, msk);
}

int cpabe_keygen(cpabe_pub_t* pub, cpabe_msk_t* msk, char** attributes, cpabe_sk_t** rsk) {
    if(!pub || !msk || !attributes || !rsk)
        return 0;
    element_t g_r;
    element_t r;
    element_t beta_inv;
    cpabe_sk_t* sk = malloc(sizeof(cpabe_sk_t));
    element_init_G2(sk->d, pub->p);
    element_init_G2(g_r, pub->p);
    element_init_Zr(r, pub->p);
    element_init_Zr(beta_inv, pub->p);
    sk->comps = g_array_new(0, 1, sizeof (cpabe_sk_comp_t));

    element_random(r);
    element_pow_zn(g_r, pub->gp, r);

    element_mul(sk->d, msk->g_alpha, g_r);
    element_invert(beta_inv, msk->beta);
    element_pow_zn(sk->d, sk->d, beta_inv);

    char** cur = attributes;
    int blen;
    while (*cur) {
        cpabe_sk_comp_t c;
        element_t h_rp;
        element_t rp;

        blen = strlen(*(cur));
        c.attr = malloc((blen+1)*sizeof(char));
        strncpy(c.attr, *(cur++), blen+1);

        element_init_G2(c.d, pub->p);
        element_init_G1(c.dp, pub->p);
        element_init_G2(h_rp, pub->p);
        element_init_Zr(rp, pub->p);

        element_hash_from_string(h_rp, c.attr);
        element_random(rp);

        element_pow_zn(h_rp, h_rp, rp);

        element_mul(c.d, g_r, h_rp);
        element_pow_zn(c.dp, pub->g, rp);
        element_clear(h_rp);
        element_clear(rp);

        g_array_append_val(sk->comps, c);
    }
    *rsk = sk;
    element_clear(r);
    element_clear(g_r);
    element_clear(beta_inv);
    return 1;
}

cpabe_policy_t* base_node(int k, char* s) {
    cpabe_policy_t* p;

    p = (cpabe_policy_t*) malloc(sizeof (cpabe_policy_t));
    p->k = k;
    p->attr = s ? strdup(s) : 0;
    p->children = g_ptr_array_new();
    p->q = 0;

    return p;
}

cpabe_policy_t* parse_policy_postfix(char* s) {
    char** toks;
    char** cur_toks;
    char* tok;
    GPtrArray* stack; /* pointers to cpabe_policy_t's */
    cpabe_policy_t* root;

    toks = g_strsplit(s, " ", 0);

    cur_toks = toks;
    stack = g_ptr_array_new();

    while (*cur_toks) {
        int i, k, n;

        tok = *(cur_toks++);

        if (!*tok)
            continue;

        if (sscanf(tok, "%dof%d", &k, &n) != 2) {
            /* push leaf token */
            GByteArray* k64 = g_byte_array_new();
            g_byte_array_append(k64, tok, strlen(tok)+1);
            GByteArray* data = decode64(k64);
            char* val = to_string(data)->data;
            g_byte_array_free(k64, 1);
            g_byte_array_free(data, 0);
            g_ptr_array_add(stack, base_node(1, val));
        } else {
            cpabe_policy_t* node;

            /* parse "kofn" operator */

            if (k < 1) {
                raise_error("error parsing \"%s\": trivially satisfied operator \"%s\"\n", s, tok);
                return 0;
            } else if (k > n) {
                raise_error("error parsing \"%s\": unsatisfiable operator \"%s\"\n", s, tok);
                return 0;
            } else if (n == 1) {
                raise_error("error parsing \"%s\": identity operator \"%s\"\n", s, tok);
                return 0;
            } else if (n > stack->len) {
                raise_error("error parsing \"%s\": stack underflow at \"%s\"\n", s, tok);
                return 0;
            }

            /* pop n things and fill in children */
            node = base_node(k, 0);
            g_ptr_array_set_size(node->children, n);
            for (i = n - 1; i >= 0; i--)
                node->children->pdata[i] = g_ptr_array_remove_index(stack, stack->len - 1);
                // g_ptr_array_add(node->children, g_ptr_array_remove_index(stack, stack->len - 1));
            // printf("ptr --> 0 --> %s\n", ((cpabe_policy_t*)(node->children->pdata[0]))->attr);
            // printf("ptr --> 1 --> %s\n", ((cpabe_policy_t*)(node->children->pdata[1]))->attr);
            // printf("%d\n", node->children->len);
            /* push result */
            g_ptr_array_add(stack, node);
        }
    }

    if (stack->len > 1) {
        raise_error("error parsing \"%s\": extra tokens left on stack\n", s);
        return 0;
    } else if (stack->len < 1) {
        raise_error("error parsing \"%s\": empty policy\n", s);
        return 0;
    }

    root = g_ptr_array_index(stack, 0);

    g_strfreev(toks);
    g_ptr_array_free(stack, 0);

    return root;
}

cpabe_polynomial_t* rand_poly(int deg, element_t zero_val) {
    int i;
    cpabe_polynomial_t* q;

    q = (cpabe_polynomial_t*) malloc(sizeof (cpabe_polynomial_t));
    q->deg = deg;
    q->coef = (element_t*) malloc(sizeof (element_t) * (deg + 1));

    for (i = 0; i < q->deg + 1; i++)
        element_init_same_as(q->coef[i], zero_val);

    element_set(q->coef[0], zero_val);

    for (i = 1; i < q->deg + 1; i++)
        element_random(q->coef[i]);

    return q;
}

void eval_poly(element_t r, cpabe_polynomial_t* q, element_t x) {
    int i;
    element_t s, t;

    element_init_same_as(s, r);
    element_init_same_as(t, r);

    element_set0(r);
    element_set1(t);

    for (i = 0; i < q->deg + 1; i++) {
        /* r += q->coef[i] * t */
        element_mul(s, q->coef[i], t);
        element_add(r, r, s);

        /* t *= x */
        element_mul(t, t, x);
    }

    element_clear(s);
    element_clear(t);
}

void fill_policy(cpabe_policy_t* p, cpabe_pub_t* pub, element_t e) {
    int i;
    element_t r;
    element_t t;
    element_t h;

    element_init_Zr(r, pub->p);
    element_init_Zr(t, pub->p);
    element_init_G2(h, pub->p);

    p->q = rand_poly(p->k - 1, e);

    if (p->children->len == 0) {
        element_init_G1(p->c, pub->p);
        element_init_G2(p->cp, pub->p);

        element_hash_from_string(h, p->attr);
        element_pow_zn(p->c, pub->g, p->q->coef[0]);
        element_pow_zn(p->cp, h, p->q->coef[0]);
    } else{
        for (i = 0; i < p->children->len; i++) {
            element_set_si(r, i + 1);
            eval_poly(t, p->q, r);
            fill_policy(g_ptr_array_index(p->children, i), pub, t);
        }
    }

    element_clear(r);
    element_clear(t);
    element_clear(h);
}

// flag -- 1 already initialized s, 2 need to set a value
void cpabe_encapsulate_secret(cpabe_pub_t* pub, char* policy, element_t s, int flag, cpabe_cph_t* cph) {
    
    policy = abe_parse_policy(policy);
    if (!flag){
        element_init_Zr(s, pub->p);
        element_random(s);
    }
    element_init_GT(cph->cs, pub->p);
    element_init_G1(cph->c, pub->p);
    cph->p = parse_policy_postfix(policy);

    /* compute */
    // e_g_g_alpha ^ s
    element_pow_zn(cph->cs, pub->g_hat_alpha, s);
    // h ^ s
    element_pow_zn(cph->c, pub->h, s);
    fill_policy(cph->p, pub, s);
}

int cpabe_enc(cpabe_pub_t* pub, element_t m, char* policy, cpabe_cph_t** rcph){
    if(!pub || !policy || !rcph)
        return 0;
    element_t s;
    cpabe_cph_t * cph = malloc(sizeof(cpabe_cph_t));
    cpabe_encapsulate_secret(pub, policy, s, 0, cph);
    if(cph->cs){
        element_mul(cph->cs, cph->cs, m);
        *rcph = cph;
        element_clear(s);
        return 1;
    }
    return 0;
}

void check_sat(cpabe_policy_t* p, GArray* sk) {
    int i, l;

    p->satisfiable = 0;
    if (p->children->len == 0) {
        for (i = 0; i < sk->len; i++){
            if (!strcmp(g_array_index(sk, cpabe_sk_comp_t, i).attr,
                    p->attr)) {
                p->satisfiable = 1;
                p->attri = i;
                break;
            }
        }
    } else {
        for (i = 0; i < p->children->len; i++){
            check_sat(g_ptr_array_index(p->children, i), sk);
        }

        l = 0;
        for (i = 0; i < p->children->len; i++)
            if (((cpabe_policy_t*) g_ptr_array_index(p->children, i))->satisfiable)
                l++;

        if (l >= p->k)
            p->satisfiable = 1;
    }
}
cpabe_policy_t* cur_comp_pol;

int cmp_int(const void* a, const void* b) {
    int k, l;

    k = ((cpabe_policy_t*) g_ptr_array_index(cur_comp_pol->children, *((int*) a)))->min_leaves;
    l = ((cpabe_policy_t*) g_ptr_array_index(cur_comp_pol->children, *((int*) b)))->min_leaves;

    return
    k < l ? -1 :
            k == l ? 0 : 1;
}

void pick_sat_min_leaves(cpabe_policy_t* p) {
    int i, k, l;
    int* c;

    if(p->satisfiable != 1)
        return;

    if (p->children->len == 0)
        p->min_leaves = 1;
    else {
        for (i = 0; i < p->children->len; i++)
            if (((cpabe_policy_t*) g_ptr_array_index(p->children, i))->satisfiable)
                pick_sat_min_leaves(g_ptr_array_index(p->children, i));

        c = alloca(sizeof (int) * p->children->len);
        for (i = 0; i < p->children->len; i++)
            c[i] = i;

        cur_comp_pol = p;
        qsort(c, p->children->len, sizeof (int), cmp_int);

        p->satl = g_array_new(0, 0, sizeof (int));
        p->min_leaves = 0;
        l = 0;

        for (i = 0; i < p->children->len && l < p->k; i++)
            if (((cpabe_policy_t*) g_ptr_array_index(p->children, c[i]))->satisfiable) {
                l++;
                p->min_leaves += ((cpabe_policy_t*) g_ptr_array_index(p->children, c[i]))->min_leaves;
                k = c[i] + 1;
                g_array_append_val(p->satl, k);
            }
        assert(l == p->k);
        if(l != p->k){
            return;
        }
    }
}

void lagrange_coef(element_t r, GArray* s, int i) {
    int j, k;
    element_t t;

    element_init_same_as(t, r);

    element_set1(r);
    for (k = 0; k < s->len; k++) {
        j = g_array_index(s, int, k);
        if (j == i)
            continue;
        element_set_si(t, -j);
        element_mul(r, r, t); /* num_muls++; */
        element_set_si(t, i - j);
        element_invert(t, t);
        element_mul(r, r, t); /* num_muls++; */
    }

    element_clear(t);
}

void dec_leaf_merge(element_t exp, cpabe_policy_t* p, cpabe_sk_t* sk, cpabe_pub_t* pub) {
    cpabe_sk_comp_t* c;
    element_t s;

    c = &(g_array_index(sk->comps, cpabe_sk_comp_t, p->attri));

    if (!c->used) {
        c->used = 1;
        element_init_G1(c->z, pub->p);
        element_init_G1(c->zp, pub->p);
        element_set1(c->z);
        element_set1(c->zp);
    }

    element_init_G1(s, pub->p);

    element_pow_zn(s, p->c, exp); /* num_exps++; */
    element_mul(c->z, c->z, s); /* num_muls++; */

    element_pow_zn(s, p->cp, exp); /* num_exps++; */
    element_mul(c->zp, c->zp, s); /* num_muls++; */

    element_clear(s);
}
void dec_node_merge(element_t exp, cpabe_policy_t* p, cpabe_sk_t* sk, cpabe_pub_t* pub);

void dec_internal_merge(element_t exp, cpabe_policy_t* p, cpabe_sk_t* sk, cpabe_pub_t* pub) {
    int i;
    element_t t;
    element_t expnew;

    element_init_Zr(t, pub->p);
    element_init_Zr(expnew, pub->p);

    for (i = 0; i < p->satl->len; i++) {
        lagrange_coef(t, p->satl, g_array_index(p->satl, int, i));
        element_mul(expnew, exp, t); /* num_muls++; */
        dec_node_merge(expnew, g_ptr_array_index
                (p->children, g_array_index(p->satl, int, i) - 1), sk, pub);
    }

    element_clear(t);
    element_clear(expnew);
}

void dec_node_merge(element_t exp, cpabe_policy_t* p, cpabe_sk_t* sk, cpabe_pub_t* pub) {
    assert(p->satisfiable);
    if(!p->satisfiable){
        return;
    }
    if (p->children->len == 0)
        dec_leaf_merge(exp, p, sk, pub);
    else
        dec_internal_merge(exp, p, sk, pub);
}

void dec_merge(element_t r, cpabe_policy_t* p, cpabe_sk_t* sk, cpabe_pub_t* pub) {
    int i;
    element_t one;
    element_t s;

    /* first mark all attributes as unused */
    for (i = 0; i < sk->comps->len; i++)
        g_array_index(sk->comps, cpabe_sk_comp_t, i).used = 0;

    /* now fill in the z's and zp's */
    element_init_Zr(one, pub->p);
    element_set1(one);
    dec_node_merge(one, p, sk, pub);
    element_clear(one);

    /* now do all the pairings and multiply everything together */
    element_set1(r);
    element_init_GT(s, pub->p);
    for (i = 0; i < sk->comps->len; i++)
        if (g_array_index(sk->comps, cpabe_sk_comp_t, i).used) {
            cpabe_sk_comp_t* c = &(g_array_index(sk->comps, cpabe_sk_comp_t, i));

            pairing_apply(s, c->z, c->d, pub->p); /* num_pairings++; */
            element_mul(r, r, s); /* num_muls++; */

            pairing_apply(s, c->dp, c->zp, pub->p); /* num_pairings++; */
            element_invert(s, s);
            element_mul(r, r, s); /* num_muls++; */
        }
    element_clear(s);
}

void dec_leaf_flatten(element_t r, element_t exp,
        cpabe_policy_t* p, GArray* sk, cpabe_pub_t* pub) {
    cpabe_sk_comp_t* c;
    element_t s;
    element_t t;

    c = &(g_array_index(sk, cpabe_sk_comp_t, p->attri));

    element_init_GT(s, pub->p);
    element_init_GT(t, pub->p);

    pairing_apply(s, p->c, c->d, pub->p); /* num_pairings++; */
    pairing_apply(t, c->dp, p->cp, pub->p); /* num_pairings++; */
    element_invert(t, t);
    element_mul(s, s, t); /* num_muls++; */
    element_pow_zn(s, s, exp); /* num_exps++; */

    element_mul(r, r, s); /* num_muls++; */

    element_clear(s);
    element_clear(t);
}

void dec_node_flatten(element_t r, element_t exp,
        cpabe_policy_t* p, GArray* sk, cpabe_pub_t* pub);

void dec_internal_flatten(element_t r, element_t exp,
        cpabe_policy_t* p, GArray* sk, cpabe_pub_t* pub) {
    int i;
    element_t t;
    element_t expnew;

    element_init_Zr(t, pub->p);
    element_init_Zr(expnew, pub->p);

    for (i = 0; i < p->satl->len; i++) {
        lagrange_coef(t, p->satl, g_array_index(p->satl, int, i));
        element_mul(expnew, exp, t); /* num_muls++; */
        dec_node_flatten(r, expnew, g_ptr_array_index
                (p->children, g_array_index(p->satl, int, i) - 1), sk, pub);
    }

    element_clear(t);
    element_clear(expnew);
}

void dec_node_flatten(element_t r, element_t exp,
        cpabe_policy_t* p, GArray* sk, cpabe_pub_t* pub) {
    assert(p->satisfiable);
    if(!p->satisfiable)
        return;
    if (p->children->len == 0)
        dec_leaf_flatten(r, exp, p, sk, pub);
    else
        dec_internal_flatten(r, exp, p, sk, pub);
}

void dec_flatten(element_t r, cpabe_policy_t* p, GArray* sk, cpabe_pub_t* pub) {
    element_t one;

    element_init_Zr(one, pub->p);

    element_set1(one);
    element_set1(r);

    dec_node_flatten(r, one, p, sk, pub);

    element_clear(one);
}

void cpabe_dec_policy(cpabe_pub_t* pub, GArray* sk, cpabe_cph_t* cph, element_t t) {
    check_sat(cph->p, sk);
    if (!cph->p->satisfiable) {
        raise_error("cannot decrypt, attributes in key do not satisfy policy\n");
        return;
    }

    pick_sat_min_leaves(cph->p);
    dec_flatten(t, cph->p, sk, pub);
}

int cpabe_dec(cpabe_pub_t* pub, cpabe_sk_t* sk, cpabe_cph_t* cph, element_t res) {
    if(!pub || !sk || !cph)
        return 0;
    element_t t;
    element_t m;

    element_init_GT(res, pub->p);
    element_init_GT(m, pub->p);
    element_init_GT(t, pub->p);
    cpabe_dec_policy(pub, sk->comps, cph, t);
    if(!cph->p->satisfiable)
        return 0;

    pairing_apply(m, cph->c, sk->d, pub->p); /* num_pairings++; */

    element_div(res, m, t);
    element_div(res, cph->cs, res);
    // element_printf("cpabe_dec%B\n", res);
    element_clear(m);
    element_clear(t);

    return 1;
}
