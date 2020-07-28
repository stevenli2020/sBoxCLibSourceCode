/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#ifndef BASESERIALIZER_H
#define BASESERIALIZER_H


#ifdef __cplusplus
extern "C" {
#endif

    GByteArray * cpabe_pub_serialize(cpabe_pub_t * pub);
    GByteArray * cpabe_msk_serialize(cpabe_msk_t * msk);
    GByteArray * cpabe_sk_serialize(cpabe_sk_t * sk);
    GByteArray * cpabe_cph_serialize(cpabe_cph_t * cph);
    cpabe_cph_t* cpabe_cph_dup(cpabe_cph_t* src);
    int cpabe_cph_dup_1(cpabe_cph_t* dst, cpabe_cph_t*src);

    cpabe_pub_t * cpabe_pub_unserialize(GByteArray* b, int free);
    cpabe_msk_t * cpabe_msk_unserialize(pairing_t p, GByteArray* b, int free);
    cpabe_sk_t * cpabe_sk_unserialize(pairing_t p, GByteArray* b, int free);
    cpabe_cph_t * cpabe_cph_unserialize(pairing_t p, GByteArray* b, int free);
    int cpabe_cph_unserialize_1(pairing_t p, GByteArray* b, int free, cpabe_cph_t* cph);

    void serialize_policy(GByteArray* b, cpabe_policy_t* p);
    cpabe_policy_t* unserialize_policy(pairing_t pa, GByteArray* b, int* offset);
    cpabe_policy_t* cpabe_policy_dup(cpabe_policy_t* src);
    cpabe_policy_t* partial_unserialize_policy(pairing_t pa, GByteArray* b, int* offset);

#ifdef __cplusplus
}
#endif

#endif /* BASESERIALIZER_H */

