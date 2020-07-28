/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include "abe/abekey.h"
#include "abe/cpabe/cpabecrypt.h"
#include "cpabeodct.h"

#ifndef CPABEODCRYPT_CCA_H
#define CPABEODCRYPT_CCA_H

#ifdef __cplusplus
extern "C" {
#endif
    CPABECryptPtr new_CPABEOD_CCA_CryptDef();
    void delete_CPABEOD_CCA_Crypt(CPABECryptPtr cryptPtr);
    GByteArray* cpabeodCrypt_cca_decrypt(UserSKPtr decryptKey, CipherTextPtr ciphertext);
    int cpabeodCrypt_cca_decrypt1(UserSKPtr decryptKey, CipherTextPtr ciphertext, cpabe_aes_ptr aes);
	UserSKPtr cpabeodCrypt_cca_parseUSK(MasterPKPtr mpk, GByteArray* data);
    CipherTextPtr cpabeCrypt_cca_reencrypt(MasterPKPtr mpk, UserSKPtr decryptKey, char* policy, CipherTextPtr ciphertext);
    int cpabeodCrypt_cca_decrypt2(MasterPKPtr pp, UserSKPtr decryptKey, CipherTextPtr ciphertext, cpabe_aes_ptr aes);
#ifdef __cplusplus
}
#endif

#endif /* CPABEODCRYPT_H */

