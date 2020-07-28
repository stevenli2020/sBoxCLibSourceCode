/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#ifndef ODABESERIALIZER_H
#define ODABESERIALIZER_H


#ifdef __cplusplus
extern "C" {
#endif

    GByteArray * odabe_pub_serialize(odabe_pub_t * pub);
    GByteArray * odabe_msk_serialize(odabe_msk_t * msk);
    GByteArray * odabe_tk_serialize(odabe_tk_t * tk);
    GByteArray * odabe_dk_serialize(odabe_dk_t * dk);
    GByteArray * odabe_cph_serialize(odabe_cph_t * cph);
    GByteArray * odabe_tcph_serialize(odabe_tcph_t * tcph);

    odabe_pub_t* odabe_pub_unserialize(GByteArray* b, int free);
    odabe_msk_t* odabe_msk_unserialize(pairing_t p, GByteArray* b, int free);
    odabe_tk_t* odabe_tk_unserialize(pairing_t p, GByteArray* b, int free);
    odabe_dk_t* odabe_dk_unserialize(pairing_t p, GByteArray* b, int free);
    odabe_cph_t* odabe_cph_unserialize(pairing_t p, GByteArray* b, int free);
    odabe_tcph_t* odabe_tcph_unserialize(pairing_t p, GByteArray* b, int free);

#ifdef __cplusplus
}
#endif

#endif /* ODABESERIALIZER_H */

