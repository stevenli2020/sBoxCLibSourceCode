/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include <string.h>
#include "glib.h"
#include "pbc.h"
#include "base/base_obj.h"
#include "odabe/odabe_obj.h"
#include "cca_odabe/cca_odabe_obj.h"
#include "cca_eodabe/cca_eodabe_obj.h"
#include "base/base_serializer.h"
#include "odabe/odabe_serializer.h"
#include "cca_odabe/cca_odabe_serializer.h"
#include "cca_eodabe/cca_eodabe_serializer.h"
#include "util/io.h"
#include "util/abe_util.h"

GByteArray * cca_eodabe_pub_serialize(cca_eodabe_pub_t * pub){
    GByteArray* b;

    b = g_byte_array_new();
    write_string(b, pub->pairing_desc);
    write_element(b, pub->g1);
    write_element(b, pub->g2);
    write_element(b, pub->h1);
    write_element(b, pub->h2);
    write_element(b, pub->h3);

    return b;
}

cca_eodabe_pub_t* cca_eodabe_pub_unserialize(GByteArray* b, int free){
    cca_eodabe_pub_t* pub;
    int offset = 0;
    char * pdesc = read_string(b, &offset);
    pub = (cca_eodabe_pub_t*) malloc(sizeof(cca_eodabe_pub_t));
    pub->p = malloc(sizeof(pairing_t));
    pub->pairing_desc = pdesc;
    pairing_init_set_buf(pub->p, pub->pairing_desc, strlen(pub->pairing_desc));

    element_init_G1(pub->g1, pub->p);
    element_init_G1(pub->h1, pub->p);
    element_init_G2(pub->g2, pub->p);
    element_init_G2(pub->h2, pub->p);
    element_init_GT(pub->h3, pub->p);

    read_element(b, &offset, pub->g1);
    read_element(b, &offset, pub->g2);
    read_element(b, &offset, pub->h1);
    read_element(b, &offset, pub->h2);
    read_element(b, &offset, pub->h3);

    if (free)
        g_byte_array_free(b, 1);

    return pub;
}

GByteArray * cca_eodabe_msk_serialize(cca_eodabe_msk_t * msk){
    GByteArray* b;

    b = g_byte_array_new();
    write_element(b, msk->alpha);

    return b;
}
cca_eodabe_msk_t* cca_eodabe_msk_unserialize(pairing_t p, GByteArray* b, int free){
    cca_eodabe_msk_t* msk;
    int offset;

    msk = (cca_eodabe_msk_t*) malloc(sizeof (cca_eodabe_msk_t));
    offset = 0;

    element_init_Zr(msk->alpha, p);

    read_element(b, &offset, msk->alpha);

    if (free)
        g_byte_array_free(b, 1);

    return msk;
}

GByteArray * cca_eodabe_tk_serialize(cca_eodabe_tk_t * tk){
    GByteArray* b = cca_eodabe_msk_serialize((cca_eodabe_msk_t*) tk);
    char* attr;
    write_int(b, tk->attrs->len);
    for(int i = 0; i < tk->attrs->len; i++){
        attr = g_array_index(tk->attrs, char*, i);
        write_string(b, attr);
    }
    return b;
}
cca_eodabe_tk_t* cca_eodabe_tk_unserialize(pairing_t p, GByteArray* b, int free){
    cca_eodabe_tk_t* tk;
    int offset = 0;
    char* attr;

    tk = (cca_eodabe_tk_t*) malloc(sizeof (cca_eodabe_tk_t));
    tk->attrs = g_array_new(0, 1, sizeof (char*));
    element_init_Zr(tk->z, p);
    read_element(b, &offset, tk->z);

    int len = read_int(b, &offset);
    for(int i = 0; i < len; i++){
        attr = read_string(b, &offset);
        g_array_append_val(tk->attrs, attr);
    }

    if (free)
        g_byte_array_free(b, 1);

    return tk;
}

GByteArray * cca_eodabe_cph_serialize(cca_eodabe_cph_t * cph){
    return cca_odabe_cph_serialize((cca_odabe_cph_t *) cph);
}
cca_eodabe_cph_t* cca_eodabe_cph_unserialize(pairing_t p, GByteArray* b, int free){
    cca_eodabe_cph_t* cph = malloc(sizeof(cca_eodabe_cph_t));
    cca_odabe_cph_unserialize_1(p, b, free, (cca_odabe_cph_t* )cph);
    return cph;
}

int extract_pairing_from_pub_bytes(GByteArray *b, int free, pairing_t pairing){
    int offset = 0;
    char * pdesc = read_string(b, &offset);
    pairing_init_set_buf(pairing, pdesc, strlen(pdesc));
    return 1;
}

cca_eodabe_cph_t* cca_eodabe_cph_partial_unserialize(pairing_t p, GByteArray* b){
    cca_eodabe_cph_t* cph = malloc(sizeof(cca_eodabe_cph_t));
    int offset = sizeof(int) + pairing_length_in_bytes_GT(p);
    element_init_G1(cph->c2, p);
    read_element(b, &offset, cph->c2);
    offset += 1;
    cph->p = partial_unserialize_policy(p, b, &offset);
    cph->c0 = NULL;
    return cph;
}

GByteArray * cca_eodabe_cph_recompress_c2(pairing_t p, cca_eodabe_cph_t* cph, GByteArray *src, int free){
    if(src == NULL)
        return NULL;
    GByteArray* res = g_byte_array_new();
    g_byte_array_append(res, src->data, src->len);
    int offset = sizeof(int)*2 + pairing_length_in_bytes_GT(p);
    element_to_bytes(res->data + offset, cph->c2);
    if(free)
        g_byte_array_free(src, 1);
    return res;
}

GByteArray * cca_eodabe_tcph_serialize(cca_eodabe_tcph_t * tcph){
    return cca_eodabe_cph_serialize((cca_eodabe_cph_t*) tcph);
}
cca_eodabe_tcph_t* cca_eodabe_tcph_unserialize(pairing_t p, GByteArray* b, int free){
    return (cca_eodabe_tcph_t*) cca_eodabe_cph_unserialize( p, b, free);;
}

GByteArray * cca_eodabe_dk_serialize(cca_eodabe_dk_t* dk){
    GByteArray* b;
    int i;

    b = g_byte_array_new();
    write_element(b, dk->d1);
    write_element(b, dk->d2);
    write_int(b, dk->comps->len);

    for (i = 0; i < dk->comps->len; i++) {
        write_string(b, g_array_index(dk->comps, cpabe_sk_comp_t, i).attr);
        write_element(b, g_array_index(dk->comps, cpabe_sk_comp_t, i).d);
    }

    return b;
}

cca_eodabe_dk_t* cca_eodabe_dk_unserialize(pairing_t p, GByteArray* b, int free){
    cca_eodabe_dk_t* dk;
    int i;
    int len;
    int offset;

    dk = (cca_eodabe_dk_t*) malloc(sizeof (cca_eodabe_dk_t));
    offset = 0;

    element_init_G2(dk->d1, p);
    element_init_G2(dk->d2, p);
    read_element(b, &offset, dk->d1);
    read_element(b, &offset, dk->d2);

    dk->comps = g_array_new(0, 1, sizeof (cpabe_sk_comp_t));
    len = read_int(b, &offset);

    for (i = 0; i < len; i++) {
        cpabe_sk_comp_t c;

        c.attr = read_string(b, &offset);

        element_init_G1(c.d, p);

        read_element(b, &offset, c.d);

        g_array_append_val(dk->comps, c);
    }

    if (free)
        g_byte_array_free(b, 1);

    return dk;
}
