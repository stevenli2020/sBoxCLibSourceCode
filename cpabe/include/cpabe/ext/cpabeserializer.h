/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#ifndef CPABESERIALIZER_H
#define CPABESERIALIZER_H


#ifdef __cplusplus
extern "C" {
#endif

    GByteArray * cpabe_pub_serialize(cpabe_pub_t * pub);
    GByteArray * cpabe_msk_serialize(cpabe_msk_t * msk);
    GByteArray * cpabe_prv_serialize(cpabe_prv_t * prv);
    GByteArray * cpabe_cph_serialize(cpabe_cph_t * cph);
    GByteArray * cpabe_cph_cca_serialize(cpabe_cph_cca_t * cph);
    GByteArray * cpabe_udk_serialize(cpabe_prv_t * prv);
    GByteArray * cpabe_transct_serialize(cpabe_trans_t * trans);

    cpabe_pub_t * cpabe_pub_unserialize(GByteArray* b, int free);
    cpabe_msk_t * cpabe_msk_unserialize(pairing_t p, GByteArray* b, int free);
    cpabe_prv_t * cpabe_prv_unserialize(pairing_t p, GByteArray* b, int free);
    cpabe_cph_t * cpabe_cph_unserialize(pairing_t p, GByteArray* b, int free);
    cpabe_cph_cca_t * cpabe_cph_cca_unserialize(pairing_t p, GByteArray* b, int free);
    cpabe_prv_t * cpabe_udk_unserialize(pairing_t p, GByteArray* b, int free);
    cpabe_trans_t * cpabe_transct_unserialize(pairing_t p, GByteArray* b, int free);

#ifdef __cplusplus
}
#endif

#endif /* CPABESERIALIZER_H */

