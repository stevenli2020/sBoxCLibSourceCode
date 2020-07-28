/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include <stdlib.h>
#include "object/bswcpabekey.h"
#include "abe/aescrypt.h"
#include "abe/cpabe/cpabecrypt.h"
#include "abe/cpabe/cpabe_aes.h"
#include "abe/cpabe/cpabecrypt_cca.h"
#include "util/abeutil.h"
#include "ext/cpabeutil.h"

CPABECryptPtr new_CPABE_CCA_CryptDef() {
    CPABECryptPtr cpabeCrypt = new_CPABECryptDef();
    cpabeCrypt->encrypt = cpabeCrypt_cca_encrypt;
    cpabeCrypt->decrypt = cpabeCrypt_cca_decrypt;
    cpabeCrypt->encrypt1 = cpabeCrypt_cca_encrypt1;
    cpabeCrypt->decrypt1 = cpabeCrypt_cca_decrypt1;
    return cpabeCrypt;
}

void delete_CPABE_CCA_Crypt(CPABECryptPtr cryptPtr) {
    if (cryptPtr == NULL)
        return;
    if (cryptPtr->baseABECrypt) {
        delete_ABECrypt(cryptPtr->baseABECrypt);
    }
    free(cryptPtr);
}

CipherTextPtr cpabeCrypt_cca_encrypt(MasterPKPtr mpk, GByteArray* msg, char* policy) {
    cpabe_cph_cca_t* cph;
    int file_len;
    GByteArray* cph_buf;
    GByteArray* aes_buf;
    element_t m;

    CPABEMasterPKPtr mpkPtr = cpabeMPK_getObject(mpk);
    if (mpkPtr == NULL) {
        return NULL;
    }
    cpabe_pub_t* pub = mpkPtr->pub;
    char* end = abe_parse_policy(policy);
    if (!(cph = cpabe_cca_enc(pub, m, end, 0)))
        return NULL;
    free(end);

    cph_buf = cpabe_cph_cca_serialize(cph);
    if (cph->p->q) {
        for (int i = 0; i < cph->p->q->deg; i++) {
            element_clear(cph->p->q->coef[i]);
        }
        free(cph->p->q);
    }
    cpabe_cph_cca_free(cph);
    file_len = msg->len;

    //element_printf("mele ---> %B\n", m);
    aes_buf = aes_256_ccm_encrypt(msg, m);
    //g_byte_array_free(msg, 1);
    element_clear(m);

    GByteArray* ct = serializeCT(cph_buf, file_len, aes_buf);

    g_byte_array_free(cph_buf, 1);
    g_byte_array_free(aes_buf, 1);

    CipherTextPtr ctPtr = new_CipherText(ct);
    ctPtr->serialize = cpabeCT_serializeCT;
    ctPtr->unserialize = cpabeCT_unserializeCT;

    return ctPtr;
}



CipherTextPtr cpabeCrypt_cca_encrypt1(MasterPKPtr mpk, char* policy, int aes_bits, cpabe_aes_ptr aes){
    cpabe_cph_cca_t* cph;
    int file_len;
    GByteArray* cph_buf;
    GByteArray* aes_buf;
    element_t m;

    CPABEMasterPKPtr mpkPtr = cpabeMPK_getObject(mpk);
    if (mpkPtr == NULL) {
        return NULL;
    }
    cpabe_pub_t* pub = mpkPtr->pub;
    char* end = abe_parse_policy(policy);
    if (!(cph = cpabe_cca_enc(pub, m, end, 0)))
        return NULL;
    free(end);

    cph_buf = cpabe_cph_cca_serialize(cph);
    if (cph->p->q) {
        for (int i = 0; i < cph->p->q->deg; i++) {
            element_clear(cph->p->q->coef[i]);
        }
        free(cph->p->q);
    }
    cpabe_cph_cca_free(cph);

    //element_printf("enc M --> %B\n", m);
    //aes_buf = aes_256_ccm_encrypt(msg, m);
    GByteArray* ccm_k = hash256_ele_len(m, aes_bits);
    init_cpabe_aes(aes, ccm_k->data, aes_bits, '\1');
    //g_byte_array_free(msg, 1);

    GByteArray* ct = serializeCT1(cph_buf, 0, aes_bits);

    g_byte_array_free(cph_buf, 1);
    g_byte_array_free(ccm_k, 1);

    CipherTextPtr ctPtr = new_CipherText(ct);
    ctPtr->serialize = cpabeCT_serializeCT;
    ctPtr->unserialize = cpabeCT_unserializeCT;

    return ctPtr;
}

int cpabeCrypt_cca_decrypt1(UserSKPtr decryptKey, CipherTextPtr ciphertext, cpabe_aes_ptr aes){
    cpabe_prv_t* prv;
    int aes_bits;
    GByteArray* cph_buf = NULL;
    element_t m;
    cpabe_cph_cca_t* cph;
    cpabe_pub_t* pub = malloc(sizeof (cpabe_pub_t));

    CPABEUserSKPtr userSKPtr = cpabeUserSK_getObject(decryptKey);
    if (userSKPtr == NULL) {
        return 0;
    }
    prv = userSKPtr->priv;
    decryptKey->setPairing(pub->p, decryptKey->pairing);
    unserializeCT1(ciphertext->data, &cph_buf, NULL, &aes_bits);
    if(cph_buf == NULL)
        return 0;
    cph = cpabe_cph_cca_unserialize(pub->p, cph_buf, 1);
    if (!cpabe_cca_dec(pub, prv, cph, NULL, m))
        return 0;
    cpabe_cph_cca_free(cph);

    GByteArray* ccm_k = hash256_ele_len(m, aes_bits);
    init_cpabe_aes(aes, ccm_k->data, aes_bits, '\0');

    g_byte_array_free(ccm_k, 1);
    element_clear(m);
    free(pub);
    return 1;
}

GByteArray* cpabeCrypt_cca_decrypt(UserSKPtr decryptKey, CipherTextPtr ciphertext) {
    cpabe_prv_t* prv;
    int file_len;
    GByteArray* aes_buf;
    GByteArray* cph_buf;
    element_t m;
    GByteArray* plt;
    cpabe_cph_cca_t* cph;
    cpabe_pub_t* pub = malloc(sizeof (cpabe_pub_t));

    CPABEUserSKPtr userSKPtr = cpabeUserSK_getObject(decryptKey);
    if (userSKPtr == NULL) {
        return NULL;
    }
    prv = userSKPtr->priv;
    decryptKey->setPairing(pub->p, decryptKey->pairing);
    unserializeCT(ciphertext->data, &cph_buf, &file_len, &aes_buf);
    cph = cpabe_cph_cca_unserialize(pub->p, cph_buf, 1);
    if (!cpabe_cca_dec(pub, prv, cph, NULL, m))
        return NULL;
    cpabe_cph_cca_free(cph);
    plt = aes_256_ccm_decrypt(aes_buf, m);
    g_byte_array_set_size(plt, file_len);
    g_byte_array_free(aes_buf, 1);
    element_clear(m);
    free(pub);
    return plt;
}
