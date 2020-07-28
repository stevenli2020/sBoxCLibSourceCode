/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include <stdlib.h>
#include "object/bswcpabekey.h"
#include "abe/aescrypt.h"
#include "abe/cpabe/cpabecrypt.h"
#include "util/abeutil.h"
#include "ext/cpabeutil.h"

CPABECryptPtr new_CPABECryptDef() {
    ABECryptPtr abeCrypt = new_ABECryptDef();
    CPABECryptPtr cpabeCrypt = malloc(sizeof (CPABECrypt));
    cpabeCrypt->encrypt = cpabeCrypt_encrypt;
    cpabeCrypt->decrypt = cpabeCrypt_decrypt;
    cpabeCrypt->baseABECrypt = abeCrypt;
    cpabeCrypt->derivedABECrypt = NULL;
    abeCrypt->derivedABECrypt = cpabeCrypt;
    cpabeCrypt->loadMPK = cpabeMPK_loadMPK;
    cpabeCrypt->loadUserKey = cpabeCrypt_loadUSK;
    cpabeCrypt->parseUserKey = cpabeCrypt_parseUSK;
    cpabeCrypt->unserializeCT = cpabeCT_unserializeCT;
    cpabeCrypt->serializeCT = cpabeCT_serializeCT;
    return cpabeCrypt;
}

void delete_CPABECrypt(CPABECryptPtr cryptPtr) {
    if (cryptPtr == NULL)
        return;
    if (cryptPtr->baseABECrypt) {
        delete_ABECrypt(cryptPtr->baseABECrypt);
    }
    free(cryptPtr);
}

CipherTextPtr cpabeCrypt_encrypt(MasterPKPtr mpk, GByteArray* msg, char* policy) {
    cpabe_cph_t* cph;
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
    if (!(cph = cpabe_enc(pub, m, end)))
        return NULL;
    free(end);

    cph_buf = cpabe_cph_serialize(cph);
    if (cph->p->q) {
        for (int i = 0; i < cph->p->q->deg; i++) {
            element_clear(cph->p->q->coef[i]);
        }
        free(cph->p->q);
    }
    cpabe_cph_free(cph);
    file_len = msg->len;
    aes_buf = aes_128_cbc_encrypt(msg, m);
    g_byte_array_free(msg, 1);
    element_clear(m);

    GByteArray* ct = serializeCT(cph_buf, file_len, aes_buf);

    g_byte_array_free(cph_buf, 1);
    g_byte_array_free(aes_buf, 1);

    CipherTextPtr ctPtr = new_CipherText(ct);
    ctPtr->serialize = cpabeCT_serializeCT;
    ctPtr->unserialize = cpabeCT_unserializeCT;
}

GByteArray* cpabeCrypt_decrypt(UserSKPtr decryptKey, CipherTextPtr ciphertext) {
    cpabe_prv_t* prv;
    int file_len;
    GByteArray* aes_buf;
    GByteArray* cph_buf;
    element_t m;
    GByteArray* plt;
    cpabe_cph_t* cph;
    cpabe_pub_t* pub = malloc(sizeof (cpabe_pub_t));

    CPABEUserSKPtr userSKPtr = cpabeUserSK_getObject(decryptKey);
    if (userSKPtr == NULL) {
        return NULL;
    }
    prv = userSKPtr->priv;
    decryptKey->setPairing(pub->p, decryptKey->pairing);
    unserializeCT(ciphertext->data, &cph_buf, &file_len, &aes_buf);
    cph = cpabe_cph_unserialize(pub->p, cph_buf, 1);
    if (!cpabe_dec(pub, prv, cph, NULL, m))
        return NULL;
    cpabe_cph_free(cph);
    free(pub);
    plt = aes_128_cbc_decrypt(aes_buf, m);
    g_byte_array_set_size(plt, file_len);
    g_byte_array_free(aes_buf, 1);
    element_clear(m);
    return plt;
}

UserSKPtr cpabeCrypt_loadUSK(MasterPKPtr mpk, char* filename) {
    UserSKPtr cpUSK = new_CPABEUserSK(mpk->pairing, NULL);
    cpabeUserSK_load(cpUSK, filename);
    return cpUSK;
}

UserSKPtr cpabeCrypt_parseUSK(MasterPKPtr mpk, GByteArray* data) {
    UserSKPtr cpUSK = new_CPABEUserSK(mpk->pairing, NULL);
    cpabeUserSK_unserialize(cpUSK, data);
    return cpUSK;
}
