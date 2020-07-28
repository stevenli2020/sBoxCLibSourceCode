/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#ifndef CCAEODABESERIALIZER_H
#define CCAEODABESERIALIZER_H


#ifdef __cplusplus
extern "C" {
#endif

    GByteArray * cca_eodabe_pub_serialize(cca_eodabe_pub_t * pub);
    GByteArray * cca_eodabe_msk_serialize(cca_eodabe_msk_t * msk);
    GByteArray * cca_eodabe_tk_serialize(cca_eodabe_tk_t * tk);
    GByteArray * cca_eodabe_dk_serialize(cca_eodabe_dk_t * dk);
    GByteArray * cca_eodabe_cph_serialize(cca_eodabe_cph_t * cph);
    GByteArray * cca_eodabe_tcph_serialize(cca_eodabe_tcph_t * tcph);

    cca_eodabe_pub_t* cca_eodabe_pub_unserialize(GByteArray* b, int free);
    cca_eodabe_msk_t* cca_eodabe_msk_unserialize(pairing_t p, GByteArray* b, int free);
    cca_eodabe_tk_t* cca_eodabe_tk_unserialize(pairing_t p, GByteArray* b, int free);
    cca_eodabe_dk_t* cca_eodabe_dk_unserialize(pairing_t p, GByteArray* b, int free);
    cca_eodabe_cph_t* cca_eodabe_cph_unserialize(pairing_t p, GByteArray* b, int free);
    cca_eodabe_tcph_t* cca_eodabe_tcph_unserialize(pairing_t p, GByteArray* b, int free);

    int extract_pairing_from_pub_bytes(GByteArray *b, int free, pairing_t pairing);
    cca_eodabe_cph_t* cca_eodabe_cph_partial_unserialize(pairing_t p, GByteArray* b);
    GByteArray * cca_eodabe_cph_recompress_c2(pairing_t p, cca_eodabe_cph_t* cph, GByteArray *src, int free);

#ifdef __cplusplus
}
#endif

#endif /* CCAEODABESERIALIZER_H */

