/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include <glib.h>
#include "object/bswcpabeobject.h"
#include "object/bswcpabe_cca_object.h"
#include "util/abeutil.h"

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

GByteArray* cpabe_prv_serialize(cpabe_prv_t* prv) {
    GByteArray* b;
    int i;

    b = g_byte_array_new();
    write_int(b, prv->type);
    write_element(b, prv->d);
    write_int(b, prv->comps->len);

    for (i = 0; i < prv->comps->len; i++) {
        write_string(b, g_array_index(prv->comps, cpabe_prv_comp_t, i).attr);
        write_element(b, g_array_index(prv->comps, cpabe_prv_comp_t, i).d);
        write_element(b, g_array_index(prv->comps, cpabe_prv_comp_t, i).dp);
    }

    return b;
}

GByteArray* cpabe_udk_serialize(cpabe_prv_t* prv) {
    GByteArray* b;

    b = g_byte_array_new();
    if (prv->derivedKey) {
        cpabe_prvod_ptr prvod = prv->derivedKey;
        write_int(b, prv->type);
        write_element(b, prvod->z);
    }
    return b;
}

cpabe_prv_ptr cpabe_udk_unserialize(pairing_t p, GByteArray* b, int free) {
    cpabe_prv_ptr prv;
    cpabe_prvod_ptr prvod;
    int offset;

    prv = (cpabe_prv_ptr) malloc(sizeof (cpabe_prv_t));
    prvod = (cpabe_prvod_ptr) malloc(sizeof (cpabe_prvod_t));
    prv->derivedKey = prvod;
    prvod->base_prv = prv;
    offset = 0;

    element_init_Zr(prvod->z, p);
    prv->type = read_int(b, &offset);
    read_element(b, &offset, prvod->z);

    if (free)
        g_byte_array_free(b, 1);

    return prv;
}

cpabe_prv_t* cpabe_prv_unserialize(pairing_t p, GByteArray* b, int free) {
    cpabe_prv_t* prv;
    int i;
    int len;
    int offset;

    prv = (cpabe_prv_t*) malloc(sizeof (cpabe_prv_t));
    offset = 0;

    element_init_G2(prv->d, p);
    prv->type = read_int(b, &offset);
    read_element(b, &offset, prv->d);

    prv->comps = g_array_new(0, 1, sizeof (cpabe_prv_comp_t));
    len = read_int(b, &offset);

    for (i = 0; i < len; i++) {
        cpabe_prv_comp_t c;

        c.attr = read_string(b, &offset);

        element_init_G2(c.d, p);
        element_init_G1(c.dp, p);

        read_element(b, &offset, c.d);
        read_element(b, &offset, c.dp);

        g_array_append_val(prv->comps, c);
    }

    if (free)
        g_byte_array_free(b, 1);

    return prv;
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

GByteArray* cpabe_cph_serialize(cpabe_cph_t* cph) {
    GByteArray* b;

    b = g_byte_array_new();
    write_element(b, cph->cs);
    write_element(b, cph->c);
    serialize_policy(b, cph->p);

    return b;
}

GByteArray* cpabe_cph_cca_serialize(cpabe_cph_cca_t* cph) {
    GByteArray* b;

    b = g_byte_array_new();
    write_element(b, cph->cs);
    write_element(b, cph->c);
    write_byte_array(b, cph->cm);
    serialize_policy(b, cph->p);

    return b;
}

GByteArray* cpabe_transct_serialize(cpabe_trans_t* trans) {
    GByteArray* b;

    b = g_byte_array_new();
    write_element(b, trans->trans);
    return b;
}

cpabe_trans_t* cpabe_transct_unserialize(pairing_t p, GByteArray* b, int free) {
    cpabe_trans_t* trans;
    int offset;

    trans = (cpabe_trans_t*) malloc(sizeof (cpabe_trans_t));
    offset = 0;

    element_init_GT(trans->trans, p);
    read_element(b, &offset, trans->trans);

    if (free)
        g_byte_array_free(b, 1);
    return trans;
}

cpabe_cph_t* cpabe_cph_unserialize(pairing_t p, GByteArray* b, int free) {
    cpabe_cph_t* cph;
    int offset;

    cph = (cpabe_cph_t*) malloc(sizeof (cpabe_cph_t));
    offset = 0;

    element_init_GT(cph->cs, p);
    element_init_G1(cph->c, p);
    read_element(b, &offset, cph->cs);
    read_element(b, &offset, cph->c);
    cph->p = unserialize_policy(p, b, &offset);

    if (free)
        g_byte_array_free(b, 1);

    return cph;
}

cpabe_cph_cca_t* cpabe_cph_cca_unserialize(pairing_t p, GByteArray* b, int free) {
    cpabe_cph_cca_t* cph;
    int offset;

    cph = (cpabe_cph_cca_t*) malloc(sizeof (cpabe_cph_cca_t));
    offset = 0;

    element_init_GT(cph->cs, p);
    element_init_G1(cph->c, p);
    read_element(b, &offset, cph->cs);
    read_element(b, &offset, cph->c);
    cph->cm = g_byte_array_new();
    read_byte_array(b, &offset, &(cph->cm));
    cph->p = unserialize_policy(p, b, &offset);

    if (free)
        g_byte_array_free(b, 1);

    return cph;
}

