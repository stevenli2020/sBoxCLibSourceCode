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
#include "odabe/odabe_obj.h"
#include "cca_odabe/cca_odabe_obj.h"
#include "cca_odabe/cca_odabe_serializer.h"
#include "cca_eodabe/cca_eodabe_obj.h"
#include "base/base.h"
#include "odabe/odabe.h"
#include "cca_odabe/cca_odabe.h"
#include "cca_eodabe/cca_eodabe.h"
#include "util/common.h"
#include "util/abe_util.h"




int cca_eodabe_setup_1(pairing_ptr pairing, char* pairing_desc, cca_eodabe_pub_t** pub, cca_eodabe_msk_t** msk){
	if(!pairing || !pub || !msk)
		return 0;
	element_t beta;

    *pub = malloc(sizeof (cca_eodabe_pub_t));
    *msk = malloc(sizeof (cca_eodabe_msk_t));
    (*pub)->p = pairing;
    (*pub)->pairing_desc = pairing_desc;

    element_init_G1((*pub)->g1, (*pub)->p);
    element_init_G1((*pub)->h1, (*pub)->p);
    element_init_G2((*pub)->g2, (*pub)->p);
    element_init_G2((*pub)->h2, (*pub)->p);
    element_init_GT((*pub)->h3, (*pub)->p);
    element_init_Zr((*msk)->alpha, (*pub)->p);
    element_init_Zr(beta, (*pub)->p);

    element_random(beta);
    element_random((*msk)->alpha);
    element_random((*pub)->g1);
    element_random((*pub)->g2);

    element_pow_zn((*pub)->h1, (*pub)->g1, beta);
    element_pow_zn((*pub)->h2, (*pub)->g2, beta);
    pairing_apply((*pub)->h3, (*pub)->g1, (*pub)->g2, (*pub)->p);
    element_pow_zn((*pub)->h3, (*pub)->h3, (*msk)->alpha);
    return 1;

}

int cca_eodabe_setup_2(char* pairing_desc, cca_eodabe_pub_t** pub, cca_eodabe_msk_t** msk){
	if(!pairing_desc || !pub || !msk)
        return 0;
    pairing_t pairing;
    pairing_init_set_buf(pairing, pairing_desc, strlen(pairing_desc));
    return cca_eodabe_setup_1(pairing, pairing_desc, pub, msk);
}

int cca_eodabe_keygen(cca_eodabe_pub_t* pub, cca_eodabe_msk_t* msk, 
    char** attributes, cca_eodabe_tk_t** rtk, cca_eodabe_dk_t** rdk){
    if(!pub || !msk || !attributes || !rtk || !rdk){
        return 0;
    }

    element_t g_alpha;
    element_t v, z_inv;
    element_init_G2(g_alpha, pub->p);
    element_init_Zr(v, pub->p);
    element_init_Zr(z_inv, pub->p);

    cca_eodabe_dk_t* dk = malloc(sizeof(cca_eodabe_dk_t));
    cca_eodabe_tk_t* tk = malloc(sizeof(cca_eodabe_tk_t));
    dk->comps = g_array_new(0, 1, sizeof (cpabe_sk_comp_t));
    tk->attrs = g_array_new(0, 1, sizeof(char *));
    element_init_G2(dk->d1, pub->p);
    element_init_G2(dk->d2, pub->p);
    element_init_Zr(tk->z, pub->p);

    element_random(tk->z);
    element_random(v);
    //d2 = g_2^(v*z)
    element_pow_zn(dk->d2, pub->g2, v);
    element_pow_zn(dk->d2, dk->d2, tk->z);
    //d1 = g_2 ^(alpha/z+beta*v)
    element_invert(z_inv, tk->z);
    element_mul(z_inv, z_inv, msk->alpha);
    element_pow_zn(g_alpha, pub->g2, z_inv);
    element_pow_zn(dk->d1, pub->h2, v);
    element_mul(dk->d1, dk->d1, g_alpha);

    char** cur = attributes;
    char* attr;
    int blen;
    while (*cur) {
        cpabe_sk_comp_t c;

        blen = strlen(*(cur));
        c.attr = malloc((blen+1)*sizeof(char));
        attr = malloc((blen+1)*sizeof(char));
        strncpy(attr, *(cur), blen+1);
        strncpy(c.attr, *(cur++), blen+1);

        element_init_G1(c.d, pub->p);
        //c.d = hash(c.attr)^(v*z)
        element_hash_from_string(c.d, c.attr);
        element_pow_zn(c.d, c.d, v);
        element_pow_zn(c.d, c.d, tk->z);

        g_array_append_val(dk->comps, c);
        g_array_append_val(tk->attrs, attr);
    }

    *rdk = dk;
    *rtk = tk;
    element_clear(z_inv);
    element_clear(g_alpha);
    element_clear(v);
    return 1;
}

void eabe_fill_policy(cpabe_policy_t* p, cca_eodabe_pub_t* pub, element_t e) {
    int index;
    element_t r;
    element_t i;
    element_t q;
    element_t h;

    element_init_Zr(r, pub->p);
    element_init_Zr(i, pub->p);
    element_init_Zr(q, pub->p);
    element_init_G1(h, pub->p);

    element_random(r);
    p->q = rand_poly(p->k - 1, e);

    if (p->children->len == 0) {
        element_init_G1(p->c, pub->p);
        element_init_G2(p->cp, pub->p);

        // p->cp = g2^r
        element_pow_zn(p->cp, pub->g2, r);
        // p->cp = g1^(beta*q(0)) * hash(p->attr)^r
        element_hash_from_string(h, p->attr);
        element_pow_zn(h, h, r);
        element_pow_zn(p->c, pub->h1, p->q->coef[0]);
        element_mul(p->c, p->c, h);
    } else{
        for (index = 0; index < p->children->len; index++) {
            element_set_si(i, index + 1);
            eval_poly(q, p->q, i);
            eabe_fill_policy(g_ptr_array_index(p->children, index), pub, q);
        }
    }

    element_clear(r);
    element_clear(q);
    element_clear(i);
    element_clear(h);
}
// flag -- 1 already initialized s, 2 need to set a value
void cca_eodabe_encapsulate_secret(cca_eodabe_pub_t* pub, char* policy, element_t s, int flag, cca_eodabe_cph_t* cph) {
    
    policy = abe_parse_policy(policy);
    if (!flag){
        element_init_Zr(s, pub->p);
        element_random(s);
    }
    element_init_GT(cph->c1, pub->p);
    element_init_G1(cph->c2, pub->p);
    cph->p = parse_policy_postfix(policy);

    /* compute */
    // e_g_g_alpha ^ s
    element_pow_zn(cph->c1, pub->h3, s);
    // g1 ^ s
    element_pow_zn(cph->c2, pub->g1, s);
    eabe_fill_policy(cph->p, pub, s);
}

int cca_eodabe_enc(cca_eodabe_pub_t* pub, element_t m, char* policy, cca_eodabe_cph_t** rcph){
	if(!pub || !policy || !rcph)
		return 0;
    int res = 0;
    cca_eodabe_cph_t * cph = malloc(sizeof(cca_eodabe_cph_t));
    element_t s, r;
    element_t hashsrc[2];
    GByteArray* hash_r;

    element_init_Zr(s, pub->p);
    element_init_GT(r, pub->p);

    /* compute */
    // s = hash(r, m)
    element_random(r);
    element_init_same_as(hashsrc[0], r);
    element_set(hashsrc[0], r);
    element_init_same_as(hashsrc[1], m);
    element_set(hashsrc[1], m);
    ele_set_by_hash256_eles(s, hashsrc, 2);

    cca_eodabe_encapsulate_secret(pub, policy, s, 1, cph);

    if(cph->c1){
        element_mul(cph->c1, cph->c1, r);
        /* compute cph->cm = m xor Hash(r) */
        int mlen = element_length_in_bytes(m);
        unsigned char mb[mlen];
        element_to_bytes(mb, m);
        hash_r = hash256_ele_len(r, mlen);
        cph->c0 = xor_a_b(mb, hash_r->data, mlen);
        *rcph = cph;
        res = 1;
    }

    if(res){
        g_byte_array_free(hash_r, 1);
    }
    element_clear(s);
    element_clear(r);
    element_clear(hashsrc[0]);
    element_clear(hashsrc[1]);
    return res;
}

void check_sat_from_attrs(cpabe_policy_t* p, GArray* attrs) {
    int i, l;

    p->satisfiable = 0;
    if (p->children->len == 0) {
        for (i = 0; i < attrs->len; i++){
            if (!strcmp(g_array_index(attrs, char*, i),
                    p->attr)) {
                p->satisfiable = 1;
                p->attri = i;
                break;
            }
        }
    } else {
        for (i = 0; i < p->children->len; i++){
            check_sat_from_attrs(g_ptr_array_index(p->children, i), attrs);
        }

        l = 0;
        for (i = 0; i < p->children->len; i++)
            if (((cpabe_policy_t*) g_ptr_array_index(p->children, i))->satisfiable)
                l++;

        if (l >= p->k)
            p->satisfiable = 1;
    }
}

int cca_eodabe_trans(cca_eodabe_tk_t* tk, 
    cca_eodabe_cph_t* cph, cca_eodabe_tcph_t** tcph){
    if(!cph || !tk || !tcph)
        return 0;
    check_sat_from_attrs(cph->p, tk->attrs);
    if(!cph->p->satisfiable)
        return 0;
    cca_eodabe_tcph_t* ttcph;
    if(cph->c0 != NULL){
        ttcph = (cca_eodabe_tcph_t* )cca_odabe_cph_dup((cca_odabe_cph_t* ) cph);
        element_pow_zn(ttcph->c2, ttcph->c2, tk->z);
    }else{
        ttcph = malloc(sizeof(cca_eodabe_tcph_t));
        element_init_same_as(ttcph->c2, cph->c2);
        element_pow_zn(ttcph->c2, cph->c2, tk->z);
        ttcph->c0 = NULL;
        ttcph->p = NULL;
    }
    *tcph = ttcph;
    return 1;
}

void cca_eodabe_dec_leaf_flatten(element_t r, element_t exp,
        cpabe_policy_t* policy, cca_eodabe_dk_t* dk, pairing_t p) {
    cpabe_sk_comp_t* c;
    element_t s;
    element_t t;

    c = &(g_array_index(dk->comps, cpabe_sk_comp_t, policy->attri));

    element_init_GT(s, p);
    element_init_GT(t, p);

    pairing_apply(s, policy->c, dk->d2, p); /* num_pairings++; */
    pairing_apply(t, c->d, policy->cp, p); /* num_pairings++; */
    element_invert(t, t);
    element_mul(s, s, t); /* num_muls++; */
    element_pow_zn(s, s, exp); /* num_exps++; */

    element_mul(r, r, s); /* num_muls++; */

    element_clear(s);
    element_clear(t);
}

void cca_eodabe_dec_node_flatten(element_t r, element_t exp,
        cpabe_policy_t* policy, cca_eodabe_dk_t* dk, pairing_t p);

void cca_eodabe_dec_internal_flatten(element_t r, element_t exp,
        cpabe_policy_t* policy, cca_eodabe_dk_t* dk, pairing_t p) {
    int i;
    element_t t;
    element_t expnew;

    element_init_Zr(t, p);
    element_init_Zr(expnew, p);

    for (i = 0; i < policy->satl->len; i++) {
        lagrange_coef(t, policy->satl, g_array_index(policy->satl, int, i));
        element_mul(expnew, exp, t); /* num_muls++; */
        cca_eodabe_dec_node_flatten(r, expnew, g_ptr_array_index
                (policy->children, g_array_index(policy->satl, int, i) - 1), dk, p);
    }

    element_clear(t);
    element_clear(expnew);
}

void cca_eodabe_dec_node_flatten(element_t r, element_t exp,
        cpabe_policy_t* policy, cca_eodabe_dk_t* dk, pairing_t p) {
    assert(policy->satisfiable);
    if(!policy->satisfiable)
        return;
    if (policy->children->len == 0)
        cca_eodabe_dec_leaf_flatten(r, exp, policy, dk, p);
    else
        cca_eodabe_dec_internal_flatten(r, exp, policy, dk, p);
}

void cca_eodabe_dec_policy(pairing_t p, cca_eodabe_dk_t* dk, cpabe_policy_t* policy, element_t t) {
    check_sat(policy, dk->comps);
    if (!policy->satisfiable) {
        raise_error("cannot decrypt, attributes in key do not satisfy policy\n");
        return;
    }

    pick_sat_min_leaves(policy);
    element_t one;
    element_init_Zr(one, p);
    element_set1(one);
    element_set1(t);
    cca_eodabe_dec_node_flatten(t, one, policy, dk, p);
    element_clear(one);
}

int cca_eodabe_dec(cca_eodabe_pub_t* pub, cca_eodabe_dk_t* dk, 
	cca_eodabe_tcph_t* tcph, element_t m){

    if(!tcph || !pub || !dk)
        return 0;

    int res = 0;
    element_t t;
    element_t r;
    element_t tm;
    element_t s;
    element_t hashsrc[2];

    element_init_GT(r, pub->p);
    element_init_GT(t, pub->p);
    cca_eodabe_dec_policy(pub->p, dk, tcph->p, t);
    if(!tcph->p->satisfiable)
        return 0;

    //r = c1 / (e(c2, d1)/t)
    pairing_apply(r, tcph->c2, dk->d1, pub->p); /* num_pairings++; */
    element_div(r, r, t);
    element_div(r, tcph->c1, r);

    // compute m = c0 xor hash(r)
    element_init_GT(tm, pub->p);
    int mlen = tcph->c0->len;
    GByteArray* hash_r = hash256_ele_len(r, mlen);
    GByteArray* buf = xor_a_b(tcph->c0->data, hash_r->data, mlen);
    element_from_bytes(tm, buf->data);

    // compute s = hash(r, m)
    element_init_Zr(s, pub->p);
    element_init_same_as(hashsrc[0], r);
    element_set(hashsrc[0], r);
    element_init_same_as(hashsrc[1], tm);
    element_set(hashsrc[1], tm);
    ele_set_by_hash256_eles(s, hashsrc, 2);

    // check if t == r * e_g1_g2_alpha^s
    element_pow_zn(t, pub->h3, s);
    element_mul(t, t, r);
    res = !element_cmp(t, tcph->c1);
    if(res){
        element_init_same_as(m, tm);
        element_set(m, tm);
    }
    element_clear(tm);
    element_clear(s);
    element_clear(t);
    element_clear(hashsrc[0]);
    element_clear(hashsrc[1]);
    g_byte_array_free(hash_r, 1);
    g_byte_array_free(buf, 1);

    element_clear(r);
	return res;
}

