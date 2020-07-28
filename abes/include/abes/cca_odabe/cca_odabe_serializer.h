/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#ifndef CCAODABESERIALIZER_H
#define CCAODABESERIALIZER_H


#ifdef __cplusplus
extern "C" {
#endif

    GByteArray * cca_odabe_pub_serialize(cca_odabe_pub_t * pub);
    GByteArray * cca_odabe_msk_serialize(cca_odabe_msk_t * msk);
    GByteArray * cca_odabe_tk_serialize(cca_odabe_tk_t * tk);
    GByteArray * cca_odabe_dk_serialize(cca_odabe_dk_t * dk);
    GByteArray * cca_odabe_cph_serialize(cca_odabe_cph_t * cph);
    GByteArray * cca_odabe_tcph_serialize(cca_odabe_tcph_t * tcph);

    cca_odabe_pub_t* cca_odabe_pub_unserialize(GByteArray* b, int free);
    cca_odabe_msk_t* cca_odabe_msk_unserialize(pairing_t p, GByteArray* b, int free);
    cca_odabe_tk_t* cca_odabe_tk_unserialize(pairing_t p, GByteArray* b, int free);
    cca_odabe_dk_t* cca_odabe_dk_unserialize(pairing_t p, GByteArray* b, int free);
    cca_odabe_cph_t* cca_odabe_cph_unserialize(pairing_t p, GByteArray* b, int free);
    cca_odabe_tcph_t* cca_odabe_tcph_unserialize(pairing_t p, GByteArray* b, int free);
    
    cca_odabe_cph_t* cca_odabe_cph_dup(cca_odabe_cph_t* src);
    int cca_odabe_cph_unserialize_1(pairing_t p, GByteArray* b, int free, cca_odabe_cph_t* cph);

#ifdef __cplusplus
}
#endif

#endif /* CCAODABESERIALIZER_H */

