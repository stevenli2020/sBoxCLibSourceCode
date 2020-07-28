/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include <stdlib.h>
#include <string.h>
#include "abe/cpabe/cpabeodtranscrypt.h"
#include "object/ghwcpabeodkey.h"
#include "util/abeutil.h"
#include "ext/cpabeutil.h"

CPABEODTransCryptPtr new_CPABEODTransCryptDef() {
    ABECryptPtr abeCrypt = new_ABECryptDef();
    CPABEODTransCryptPtr cpabeCrypt = malloc(sizeof (CPABEODTransCrypt));
    cpabeCrypt->transform = cpabeodTransCrypt_transform;
    cpabeCrypt->baseABECrypt = abeCrypt;
    abeCrypt->derivedABECrypt = cpabeCrypt;
    cpabeCrypt->loadMPK = cpabeMPK_loadMPK;
    cpabeCrypt->loadUserTKey = cpabeodTransCrypt_loadUTK;
    cpabeCrypt->parseUserTKey = cpabeodTransCrypt_parseUTK;
    cpabeCrypt->unserializeCT = cpabeodCT_unserializeCT;
    cpabeCrypt->serializeCT = cpabeodCT_serializeCT;
    return cpabeCrypt;
}

void delete_CPABEODTransCrypt(CPABEODTransCryptPtr cryptPtr) {
    if (cryptPtr == NULL)
        return;
    delete_ABECrypt(cryptPtr->baseABECrypt);
    free(cryptPtr);
}

CipherTextPtr cpabeodTransCrypt_transform(UserSKPtr decryptKey, CipherTextPtr msg) {
    int file_len;
    GByteArray* aes_buf;
    GByteArray* cph_buf;

    GByteArray* transM;
    cpabe_cph_t* cph;
    cpabe_trans_t* trans;

    CPABEODUserSKPtr userSKPtr = cpabeodUserSK_getObject(decryptKey);
    if (userSKPtr == NULL) {
        return NULL;
    }
    cpabe_pub_t* pub = malloc(sizeof (cpabe_pub_t));
    cpabe_prv_t* prv = userSKPtr->userTK->priv;
    decryptKey->setPairing(pub->p, decryptKey->pairing);
    unserializeCT(msg->data, &cph_buf, &file_len, &aes_buf);
    cph = cpabe_cph_unserialize(pub->p, cph_buf, 1);
    if (!(trans = cpabe_trans(pub, prv, cph))) {
        return NULL;
    }
    cpabe_cph_free(cph);
    free(pub);
    transM = cpabe_transct_serialize(trans);
    cpabe_transct_free(trans);
    g_byte_array_free(aes_buf, 1);
    CipherTextPtr ctPtr = new_CipherTextOD(msg, transM);
    ctPtr->serialize = cpabeodCT_serializeCT;
    ctPtr->unserialize = cpabeodCT_unserializeCT;
    return ctPtr;
}

UserSKPtr cpabeodTransCrypt_loadUTK(MasterPKPtr mpk, char* filename) {
    GByteArray* data = read_bytes_from_file(filename);
    return cpabeodTransCrypt_parseUTK(mpk, data);
}

UserSKPtr cpabeodTransCrypt_parseUTK(MasterPKPtr mpk, GByteArray* keyData) {
    UserSKPtr cpUTK = new_CPABEODUserSK(mpk->pairing, NULL);
    cpabeodUserSK_unserializeUTK(cpUTK, keyData);
    return cpUTK;
}
