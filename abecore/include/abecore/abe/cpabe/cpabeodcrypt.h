/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include "abe/abekey.h"
#include "abe/cpabe/cpabecrypt.h"
#include "cpabeodct.h"

#ifndef CPABEODCRYPT_H
#define CPABEODCRYPT_H

#ifdef __cplusplus
extern "C" {
#endif
    CPABECryptPtr new_CPABEODCryptDef();
    GByteArray* cpabeodCrypt_decrypt(UserSKPtr decryptKey, CipherTextPtr ciphertext);
    UserSKPtr cpabeodCrypt_loadUSK(MasterPKPtr mpk,char* filename);
    UserSKPtr cpabeodCrypt_parseUSK(MasterPKPtr mpk,GByteArray* data);
#ifdef __cplusplus
}
#endif

#endif /* CPABEODCRYPT_H */

