/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include <stdlib.h>
#include "abe/cpabe/cpabeodcrypt.h"
#include "abe/aescrypt.h"
#include "object/ghwcpabeodkey.h"
#include "ext/cpabeutil.h"
#include "util/abeutil.h"

CPABECryptPtr new_CPABEODCryptDef() {
    CPABECryptPtr cpabeCrypt = new_CPABECryptDef();
    cpabeCrypt->decrypt = cpabeodCrypt_decrypt;
    cpabeCrypt->loadUserKey = cpabeodCrypt_loadUSK;
    cpabeCrypt->parseUserKey = cpabeodCrypt_parseUSK;
    cpabeCrypt->unserializeCT = cpabeodCT_unserializeCT;
    cpabeCrypt->serializeCT = cpabeodCT_serializeCT;
    return cpabeCrypt;
}

void delete_CPABEODCrypt(CPABECryptPtr cryptPtr) {
    if (cryptPtr == NULL)
        return;
    if (cryptPtr->baseABECrypt) {
        delete_ABECrypt(cryptPtr->baseABECrypt);
    }
    free(cryptPtr);
}

GByteArray* cpabeodCrypt_decrypt(UserSKPtr decryptKey, CipherTextPtr ciphertext) {
    cpabe_prv_t* prv;
    int file_len;
    GByteArray* aes_buf;
    GByteArray* cph_buf;
    element_t m;
    GByteArray* plt;
    cpabe_cph_t* cph;
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
    cph = cpabe_cph_unserialize(pub->p, cph_buf, 1);
    trans = cpabe_transct_unserialize(pub->p, ctodPtr->trans, 0);
    if (!cpabe_dec(pub, prv, cph, trans, m))
        return NULL;
    cpabe_cph_free(cph);
    free(pub);
    plt = aes_128_cbc_decrypt(aes_buf, m);
    g_byte_array_set_size(plt, file_len);
    g_byte_array_free(aes_buf, 1);
    cpabe_transct_free(trans);
    element_clear(m);
    return plt;
}

UserSKPtr cpabeodCrypt_loadUSK(MasterPKPtr mpk, char* filename) {
    FILE* exist = open_read(filename);
    if (!exist) {
        fclose(exist);
    }
    UserSKPtr cpUSK = cpabeodCrypt_parseUSK(mpk, read_bytes_from_file(filename));
    return cpUSK;
}

UserSKPtr cpabeodCrypt_parseUSK(MasterPKPtr mpk, GByteArray* data) {
    UserSKPtr cpUSK = new_CPABEODUserSK(mpk->pairing, NULL);
    cpabeodUserSK_unserializeUSK(cpUSK, data);
    return cpUSK;
}

