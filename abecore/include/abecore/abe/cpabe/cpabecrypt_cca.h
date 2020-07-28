/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "abe/cpabe/cpabekey.h"
#include "abe/abecrypt.h"
#include "abe/cpabe/cpabect.h"
#include "abe/cpabe/cpabecrypt.h"

#ifndef CPABECRYPT_CCA_H
#define CPABECRYPT_CCA_H

#ifdef __cplusplus
extern "C" {
#endif

    CPABECryptPtr new_CPABE_CCA_CryptDef();
    void delete_CPABE_CCA_Crypt(CPABECryptPtr cryptPtr);

    CipherTextPtr cpabeCrypt_cca_encrypt(MasterPKPtr mpk, GByteArray* msg, char* policy);

    GByteArray* cpabeCrypt_cca_decrypt(UserSKPtr decryptKey, CipherTextPtr ciphertext);

    CipherTextPtr cpabeCrypt_cca_encrypt1(MasterPKPtr mpk, char* policy, int aes_bits, cpabe_aes_ptr aes);

    int cpabeCrypt_cca_decrypt1(UserSKPtr decryptKey, CipherTextPtr ciphertext, cpabe_aes_ptr aes);
#ifdef __cplusplus
}
#endif

#endif /* CPABECRYPT_H */

