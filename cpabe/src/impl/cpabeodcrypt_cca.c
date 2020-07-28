/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include <stdlib.h>
#include "abe/cpabe/cpabecrypt_cca.h"
#include "abe/cpabe/cpabeodcrypt.h"
#include "abe/cpabe/cpabeodcrypt_cca.h"
#include "abe/aescrypt.h"
#include "object/ghwcpabeodkey.h"
#include "ext/cpabeutil.h"
#include "util/abeutil.h"

CPABECryptPtr new_CPABEOD_CCA_CryptDef() {
    CPABECryptPtr cpabeCrypt = new_CPABEODCryptDef();
    cpabeCrypt->encrypt = cpabeCrypt_cca_encrypt;
    cpabeCrypt->decrypt = cpabeodCrypt_cca_decrypt;
    cpabeCrypt->encrypt1 = cpabeCrypt_cca_encrypt1;
    cpabeCrypt->decrypt1 = cpabeodCrypt_cca_decrypt1;
    return cpabeCrypt;
}

void delete_CPABEOD_CCA_Crypt(CPABECryptPtr cryptPtr) {
    if (cryptPtr == NULL)
        return;
    if (cryptPtr->baseABECrypt) {
        delete_ABECrypt(cryptPtr->baseABECrypt);
    }
    free(cryptPtr);
}


int cpabeodCrypt_cca_decrypt1(UserSKPtr decryptKey, CipherTextPtr ciphertext, cpabe_aes_ptr aes){
    cpabe_prv_t* prv;
    int aes_bits;
    GByteArray* cph_buf = NULL;
    element_t m;
    cpabe_cph_cca_t* cph;
    cpabe_pub_t* pub = malloc(sizeof (cpabe_pub_t));
    cpabe_trans_t* trans;

    CPABEODUserSKPtr userSKPtr = cpabeodUserSK_getObject(decryptKey);
    if (userSKPtr == NULL) {
        return 0;
    }

    CipherTextODPtr ctodPtr = cpabeodCT_getObject(ciphertext);
    if (ctodPtr == NULL) {
        return 0;
    }
    prv = userSKPtr->userDK->priv->base_prv;
    decryptKey->setPairing(pub->p, decryptKey->pairing);
    unserializeCT1(ciphertext->data, &cph_buf, NULL, &aes_bits);
    if(cph_buf == NULL)
        return 0;
    cph = cpabe_cph_cca_unserialize(pub->p, cph_buf, 1);
    trans = cpabe_transct_unserialize(pub->p, ctodPtr->trans, 0);
    if (!cpabe_cca_dec(pub, prv, cph, trans, m))
        return 0;
    cpabe_cph_cca_free(cph);
    free(pub);

    //element_printf("dec M --> %B\n", m);
    GByteArray* ccm_k = hash256_ele_len(m, aes_bits);
    init_cpabe_aes(aes, ccm_k->data, aes_bits, '\0');

    g_byte_array_free(ccm_k, 1);
    cpabe_transct_free(trans);
    element_clear(m);
    return 1;

}

int cpabeodCrypt_cca_decrypt2(MasterPKPtr pp, UserSKPtr decryptKey, CipherTextPtr ciphertext, cpabe_aes_ptr aes) {
    cpabe_prv_t* prv;
    int aes_bits;
    GByteArray* cph_buf = NULL;
    element_t m;
    cpabe_cph_cca_t* cph;
    CPABEMasterPKPtr mpkPtr = cpabeMPK_getObject(pp);
    cpabe_pub_t* pub = mpkPtr->pub;
    cpabe_trans_t* trans;

    CPABEODUserSKPtr userSKPtr = cpabeodUserSK_getObject(decryptKey);
    if (userSKPtr == NULL) {
        return 0;
    }

    CipherTextODPtr ctodPtr = cpabeodCT_getObject(ciphertext);
    if (ctodPtr == NULL) {
        return 0;
    }
    prv = userSKPtr->userDK->priv->base_prv;
    decryptKey->setPairing(pub->p, decryptKey->pairing);
    unserializeCT1(ciphertext->data, &cph_buf, NULL, &aes_bits);
    if (cph_buf == NULL)
        return 0;
    cph = cpabe_cph_cca_unserialize(pub->p, cph_buf, 1);
    trans = cpabe_transct_unserialize(pub->p, ctodPtr->trans, 0);
    if (!cpabe_cca_dec(pub, prv, cph, trans, m))
        return 0;

    //element_printf("dec M --> %B\n", m);
    GByteArray* ccm_k = hash256_ele_len(m, aes_bits);
    init_cpabe_aes(aes, ccm_k->data, aes_bits, '\0');

    element_clear(m);
    g_byte_array_free(ccm_k, 1);
    cpabe_transct_free(trans);
    cpabe_cph_cca_free(cph);
    // free(pub);
    return 1;

}


GByteArray* cpabeodCrypt_cca_decrypt(UserSKPtr decryptKey, CipherTextPtr ciphertext) {
    cpabe_prv_t* prv;
    int file_len;
    GByteArray* aes_buf;
    GByteArray* cph_buf;
    element_t m;
    GByteArray* plt;
    cpabe_cph_cca_t* cph;
    cpabe_pub_t* pub = malloc(sizeof (cpabe_pub_t));
    cpabe_trans_t* trans;

    CPABEODUserSKPtr userSKPtr = cpabeodUserSK_getObject(decryptKey);
    if (userSKPtr == NULL) {
        return NULL;
    }

    CipherTextODPtr ctodPtr = cpabeodCT_getObject(ciphertext);
    if (ctodPtr == NULL) {
        return NULL;
    }
    prv = userSKPtr->userDK->priv->base_prv;
    decryptKey->setPairing(pub->p, decryptKey->pairing);
    unserializeCT(ciphertext->data, &cph_buf, &file_len, &aes_buf);
    cph = cpabe_cph_cca_unserialize(pub->p, cph_buf, 1);
    trans = cpabe_transct_unserialize(pub->p, ctodPtr->trans, 0);
    if (!cpabe_cca_dec(pub, prv, cph, trans, m))
        return NULL;
    cpabe_cph_cca_free(cph);
    free(pub);

    //element_printf("mele ---> %B\n", m);
    plt = aes_256_ccm_decrypt(aes_buf, m);
    //g_byte_array_set_size(plt, file_len);
    g_byte_array_free(aes_buf, 1);
    cpabe_transct_free(trans);
    element_clear(m);
    return plt;
}


CipherTextPtr cpabeCrypt_cca_reencrypt(MasterPKPtr mpk, UserSKPtr decryptKey, char* policy, CipherTextPtr ciphertext){
    
    // decrypt
    cpabe_prv_t* prv;
    int aes_bits;
    GByteArray* cph_buf = NULL;
    element_t m;
    cpabe_cph_cca_t* cph;

    CPABEMasterPKPtr mpkPtr = cpabeMPK_getObject(mpk);
    if (mpkPtr == NULL) {
        return NULL;
    }
    cpabe_pub_t* pub = mpkPtr->pub;

    cpabe_trans_t* trans;

    CPABEODUserSKPtr userSKPtr = cpabeodUserSK_getObject(decryptKey);
    if (userSKPtr == NULL) {
        return NULL;
    }

    CipherTextODPtr ctodPtr = cpabeodCT_getObject(ciphertext);
    if (ctodPtr == NULL) {
        return NULL;
    }
    
    prv = userSKPtr->userDK->priv->base_prv;
    decryptKey->setPairing(pub->p, decryptKey->pairing);
    unserializeCT1(ciphertext->data, &cph_buf, NULL, &aes_bits);
    if(cph_buf == NULL)
        return NULL;
    cph = cpabe_cph_cca_unserialize(pub->p, cph_buf, 1);
    trans = cpabe_transct_unserialize(pub->p, ctodPtr->trans, 0);
    int decr = cpabe_cca_dec(pub, prv, cph, trans, m);
    if (!decr)
        return NULL;
    cpabe_cph_cca_free(cph);

    // encrypt
    int file_len;
    char* end = abe_parse_policy(policy);
    
    cph = cpabe_cca_enc(pub, m, end, 1);
    if (!cph)
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

    GByteArray* ct = serializeCT1(cph_buf, 0, aes_bits);

    g_byte_array_free(cph_buf, 1);

    CipherTextPtr ctPtr = new_CipherText(ct);
    ctPtr->serialize = cpabeCT_serializeCT;
    ctPtr->unserialize = cpabeCT_unserializeCT;

    cpabe_transct_free(trans);
    element_clear(m);
    return ctPtr;
}

