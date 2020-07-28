/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

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

GByteArray * cca_odabe_pub_serialize(cca_odabe_pub_t * pub){
    return cpabe_pub_serialize((cpabe_pub_t*) pub);
}
cca_odabe_pub_t* cca_odabe_pub_unserialize(GByteArray* b, int free){
    return (cca_odabe_pub_t*) cpabe_pub_unserialize(b, free);
}

GByteArray * cca_odabe_msk_serialize(cca_odabe_msk_t * msk){
    return cpabe_msk_serialize((cpabe_msk_t*) msk);
}
cca_odabe_msk_t* cca_odabe_msk_unserialize(pairing_t p, GByteArray* b, int free){
    return (cca_odabe_msk_t*) cpabe_msk_unserialize(p, b, free);
}

GByteArray * cca_odabe_tk_serialize(cca_odabe_tk_t * tk){
    return cpabe_sk_serialize((cpabe_sk_t*) tk);
}
cca_odabe_tk_t* cca_odabe_tk_unserialize(pairing_t p, GByteArray* b, int free){
    return (cca_odabe_tk_t*) cpabe_sk_unserialize(p, b, free);
}

GByteArray * cca_odabe_cph_serialize(cca_odabe_cph_t * cph){
    GByteArray* b = cpabe_cph_serialize((cpabe_cph_t* )cph);
    write_byte_array(b, cph->cm);
    return b;
}
cca_odabe_cph_t* cca_odabe_cph_unserialize(pairing_t p, GByteArray* b, int free){
    cca_odabe_cph_t* cph = (cca_odabe_cph_t*) malloc(sizeof (cca_odabe_cph_t));
    cca_odabe_cph_unserialize_1(p, b, free, cph);
    return cph;
}
int cca_odabe_cph_unserialize_1(pairing_t p, GByteArray* b, int free, cca_odabe_cph_t* cph){
    element_init_GT(cph->cs, p);
    element_init_G1(cph->c, p);
    cph->p = NULL;
    int offset = cpabe_cph_unserialize_1(p, b, 0, (cpabe_cph_t* )cph);
    read_byte_array(b, &offset, &(cph->cm));
    if(free)
        g_byte_array_free(b, 1);
    return 1;
}
cca_odabe_cph_t* cca_odabe_cph_dup(cca_odabe_cph_t* src){
    cca_odabe_cph_t* cph = malloc(sizeof(cca_odabe_cph_t));
    cpabe_cph_dup_1((cpabe_cph_t* )cph, (cpabe_cph_t* )src);
    cph->cm = g_byte_array_new();
    g_byte_array_append(cph->cm, src->cm->data, src->cm->len);
    return cph;
}

GByteArray * cca_odabe_tcph_serialize(cca_odabe_tcph_t * tcph){
    return cca_odabe_cph_serialize((cca_odabe_cph_t*) tcph);
}
cca_odabe_tcph_t* cca_odabe_tcph_unserialize(pairing_t p, GByteArray* b, int free){
    cca_odabe_tcph_t* cph = (cca_odabe_tcph_t*) malloc(sizeof (cca_odabe_tcph_t));
    element_init_GT(cph->cs, p);
    element_init_GT(cph->c, p);
    cph->p = NULL;
    int offset = cpabe_cph_unserialize_1(p, b, 0, (cpabe_cph_t* )cph);
    read_byte_array(b, &offset, &(cph->cm));
    if(free)
        g_byte_array_free(b, 1);
    return cph;
}

GByteArray * cca_odabe_dk_serialize(cca_odabe_dk_t* dk){
    return odabe_dk_serialize((odabe_dk_t* )dk);
}
cca_odabe_dk_t* cca_odabe_dk_unserialize(pairing_t p, GByteArray* b, int free){
    return (odabe_dk_t* )odabe_dk_unserialize(p, b, free);
}
