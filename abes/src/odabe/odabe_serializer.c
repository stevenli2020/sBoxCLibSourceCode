/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "glib.h"
#include "pbc.h"
#include "base/base_obj.h"
#include "odabe/odabe_obj.h"
#include "base/base_serializer.h"
#include "odabe/odabe_serializer.h"
#include "util/abe_util.h"

GByteArray * odabe_pub_serialize(odabe_pub_t * pub){
    return cpabe_pub_serialize((cpabe_pub_t*) pub);
}
odabe_pub_t* odabe_pub_unserialize(GByteArray* b, int free){
    return (odabe_pub_t*) cpabe_pub_unserialize(b, free);
}

GByteArray * odabe_msk_serialize(odabe_msk_t * msk){
    return cpabe_msk_serialize((cpabe_msk_t*) msk);
}
odabe_msk_t* odabe_msk_unserialize(pairing_t p, GByteArray* b, int free){
    return (odabe_msk_t*) cpabe_msk_unserialize(p, b, free);
}

GByteArray * odabe_tk_serialize(odabe_tk_t * tk){
    return cpabe_sk_serialize((cpabe_sk_t*) tk);
}
odabe_tk_t* odabe_tk_unserialize(pairing_t p, GByteArray* b, int free){
    return (odabe_tk_t*) cpabe_sk_unserialize(p, b, free);
}

GByteArray * odabe_cph_serialize(odabe_cph_t * cph){
    return cpabe_cph_serialize((cpabe_cph_t*) cph);
}
odabe_cph_t* odabe_cph_unserialize(pairing_t p, GByteArray* b, int free){
    return (odabe_cph_t*) cpabe_cph_unserialize(p, b, free);
}

GByteArray * odabe_tcph_serialize(odabe_tcph_t * tcph){
    return cpabe_cph_serialize((cpabe_cph_t*) tcph);
}
odabe_tcph_t* odabe_tcph_unserialize(pairing_t p, GByteArray* b, int free){
    cpabe_cph_t* cph = (cpabe_cph_t*) malloc(sizeof (cpabe_cph_t));
    element_init_GT(cph->cs, p);
    element_init_GT(cph->c, p);
    cph->p = NULL;
    cpabe_cph_unserialize_1(p, b, free, cph);
    return (odabe_tcph_t* )cph;
}

GByteArray * odabe_dk_serialize(odabe_dk_t* dk){
    GByteArray* b;

    b = g_byte_array_new();
    write_element(b, dk->z);
    return b;
}

odabe_dk_t* odabe_dk_unserialize(pairing_t p, GByteArray* b, int free){
    odabe_dk_t* dk = malloc(sizeof(odabe_dk_t));
    element_init_Zr(dk->z, p);
    int offset = 0;
    read_element(b, &offset, dk->z);
    if (!offset)
        return NULL;
    if (free)
        g_byte_array_free(b, 1);
    return dk;
}
