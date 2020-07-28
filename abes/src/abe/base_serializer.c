/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include <string.h>
#include "pbc.h"
#include "glib.h"
#include "base/base_obj.h"
#include "base/base_serializer.h"
#include "util/io.h"
#include "util/abe_util.h"

GByteArray* cpabe_pub_serialize(cpabe_pub_t* pub) {
    GByteArray* b;

    b = g_byte_array_new();
    write_string(b, pub->pairing_desc);
    write_element(b, pub->g);
    write_element(b, pub->h);
    write_element(b, pub->gp);
    write_element(b, pub->g_hat_alpha);

    return b;
}

cpabe_pub_t* cpabe_pub_unserialize(GByteArray* b, int f) {
    cpabe_pub_t* pub;
    int offset = 0;
    char * pdesc = read_string(b, &offset);
    pub = (cpabe_pub_t*) malloc(sizeof(cpabe_pub_t));
    pub->p = malloc(sizeof(pairing_t));
    pub->pairing_desc = pdesc;
    pairing_init_set_buf(pub->p, pub->pairing_desc, strlen(pub->pairing_desc));

    element_init_G1(pub->g, pub->p);
    element_init_G1(pub->h, pub->p);
    element_init_G2(pub->gp, pub->p);
    element_init_GT(pub->g_hat_alpha, pub->p);

    read_element(b, &offset, pub->g);
    read_element(b, &offset, pub->h);
    read_element(b, &offset, pub->gp);
    read_element(b, &offset, pub->g_hat_alpha);

    if (f)
        g_byte_array_free(b, 1);

    return pub;
}

GByteArray* cpabe_msk_serialize(cpabe_msk_t* msk) {
    GByteArray* b;

    b = g_byte_array_new();
    write_element(b, msk->beta);
    write_element(b, msk->g_alpha);

    return b;
}

cpabe_msk_t* cpabe_msk_unserialize(pairing_t p, GByteArray* b, int free) {
    cpabe_msk_t* msk;
    int offset;

    msk = (cpabe_msk_t*) malloc(sizeof (cpabe_msk_t));
    offset = 0;

    element_init_Zr(msk->beta, p);
    element_init_G2(msk->g_alpha, p);

    read_element(b, &offset, msk->beta);
    read_element(b, &offset, msk->g_alpha);

    if (free)
        g_byte_array_free(b, 1);

    return msk;
}

GByteArray* cpabe_sk_serialize(cpabe_sk_t* sk) {
    GByteArray* b;
    int i;

    b = g_byte_array_new();
    write_element(b, sk->d);
    write_int(b, sk->comps->len);

    for (i = 0; i < sk->comps->len; i++) {
        write_string(b, g_array_index(sk->comps, cpabe_sk_comp_t, i).attr);
        write_element(b, g_array_index(sk->comps, cpabe_sk_comp_t, i).d);
        write_element(b, g_array_index(sk->comps, cpabe_sk_comp_t, i).dp);
    }

    return b;
}

cpabe_sk_t* cpabe_sk_unserialize(pairing_t p, GByteArray* b, int free) {
    cpabe_sk_t* sk;
    int i;
    int len;
    int offset;

    sk = (cpabe_sk_t*) malloc(sizeof (cpabe_sk_t));
    offset = 0;

    element_init_G2(sk->d, p);
    read_element(b, &offset, sk->d);

    sk->comps = g_array_new(0, 1, sizeof (cpabe_sk_comp_t));
    len = read_int(b, &offset);

    for (i = 0; i < len; i++) {
        cpabe_sk_comp_t c;

        c.attr = read_string(b, &offset);

        element_init_G2(c.d, p);
        element_init_G1(c.dp, p);

        read_element(b, &offset, c.d);
        read_element(b, &offset, c.dp);

        g_array_append_val(sk->comps, c);
    }

    if (free)
        g_byte_array_free(b, 1);

    return sk;
}

void serialize_policy(GByteArray* b, cpabe_policy_t* p) {
    int i;

    write_int(b, (uint32_t) p->k);

    write_int(b, (uint32_t) p->children->len);
    if (p->children->len == 0) {
        write_string(b, p->attr);
        write_element(b, p->c);
        write_element(b, p->cp);
    } else
        for (i = 0; i < p->children->len; i++)
            serialize_policy(b, g_ptr_array_index(p->children, i));
}

cpabe_policy_t* unserialize_policy(pairing_t pa, GByteArray* b, int* offset) {
    int i;
    int n;
    cpabe_policy_t* p;

    p = (cpabe_policy_t*) malloc(sizeof (cpabe_policy_t));

    p->k = (int) read_int(b, offset);
    p->attr = 0;
    p->children = g_ptr_array_new();

    n = read_int(b, offset);
    if (n == 0) {
        int alen = *offset;
        int al = read_int(b, &alen);
        p->attr = read_string(b, offset);
        element_init_G1(p->c, pa);
        element_init_G2(p->cp, pa);
        read_element(b, offset, p->c);
        read_element(b, offset, p->cp);

    } else{
        for (i = 0; i < n; i++)
            g_ptr_array_add(p->children, unserialize_policy(pa, b, offset));

    }

    return p;
}


cpabe_policy_t* partial_unserialize_policy(pairing_t pa, GByteArray* b, int* offset) {
    int i;
    int n;
    cpabe_policy_t* p;

    p = (cpabe_policy_t*) malloc(sizeof (cpabe_policy_t));

    p->k = (int) read_int(b, offset);
    p->attr = 0;
    p->children = g_ptr_array_new();

    n = read_int(b, offset);
    if (n == 0) {
        int alen = *offset;
        int al = read_int(b, &alen);
        p->attr = read_string(b, offset);
        *offset += sizeof(int)*2 + pairing_length_in_bytes_G1(pa) + pairing_length_in_bytes_G2(pa);
    } else{
        for (i = 0; i < n; i++)
            g_ptr_array_add(p->children, partial_unserialize_policy(pa, b, offset));

    }

    return p;
}

cpabe_policy_t* cpabe_policy_dup(cpabe_policy_t* src){
    cpabe_policy_t* dst;

    dst = (cpabe_policy_t*) malloc(sizeof (cpabe_policy_t));
    dst->k = src->k;
    dst->attr = 0;
    dst->children = g_ptr_array_new();
    if (src->children->len == 0) {
        int len = strlen(src->attr);
        dst->attr = malloc((len + 1)*sizeof(char));
        strncpy(dst->attr, src->attr, len+1);
        element_init_same_as(dst->c, src->c);
        element_init_same_as(dst->cp, src->cp);
        element_set(dst->c, src->c);
        element_set(dst->cp, src->cp);
    } else{
        for (int i = 0; i < src->children->len; i++)
            g_ptr_array_add(dst->children, cpabe_policy_dup(g_ptr_array_index(src->children, i)));
    }
    return dst;
}

GByteArray* cpabe_cph_serialize(cpabe_cph_t* cph) {
    GByteArray* b;

    b = g_byte_array_new();
    write_element(b, cph->cs);
    write_element(b, cph->c);
    unsigned char flag = cph->p != NULL ?1:0;
    g_byte_array_append(b, &flag, 1);
    if(flag){
        serialize_policy(b, cph->p);
    }

    return b;
}

int cpabe_cph_unserialize_1(pairing_t p, GByteArray* b, int free, cpabe_cph_t* cph) {
    int offset = 0;
    read_element(b, &offset, cph->cs);
    read_element(b, &offset, cph->c);
    unsigned char flag = b->data[offset++];
    if(flag){
        cph->p = unserialize_policy(p, b, &offset);
    }
    else{
        cph->p = NULL;
    }

    if (free){
        g_byte_array_free(b, 1);
    }
    return offset;
}

cpabe_cph_t* cpabe_cph_unserialize(pairing_t p, GByteArray* b, int free) {
    cpabe_cph_t* cph = (cpabe_cph_t*) malloc(sizeof (cpabe_cph_t));
    element_init_GT(cph->cs, p);
    element_init_G1(cph->c, p);

    cpabe_cph_unserialize_1(p, b, free, cph);
    return cph;
}

int cpabe_cph_dup_1(cpabe_cph_t* dst, cpabe_cph_t*src){
    element_init_same_as(dst->cs, src->cs);
    element_init_same_as(dst->c, src->c);
    element_set(dst->cs, src->cs);
    element_set(dst->c, src->c);
    if(src->p)
        dst->p = cpabe_policy_dup(src->p);
    return 1;
}

cpabe_cph_t* cpabe_cph_dup(cpabe_cph_t* src){
    cpabe_cph_t* cph = (cpabe_cph_t*) malloc(sizeof (cpabe_cph_t));
    cpabe_cph_dup_1(cph, src);
    return cph;
}