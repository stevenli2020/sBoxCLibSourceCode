/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "abe/cpabe/cpabekey.h"
#include "abe/abecrypt.h"
#include "abe/cpabe/cpabe_aes.h"
#include "abe/cpabe/cpabect.h"

#ifndef CPABECRYPT_H
#define CPABECRYPT_H

#ifdef __cplusplus
extern "C" {
#endif

    typedef struct _CPABECrypt *CPABECryptPtr;
    typedef UserSKPtr(*loadABEUSKPtr)(MasterPKPtr mpk, char* name);

    typedef UserSKPtr(*parseABEUSKPtr)(MasterPKPtr mpk, GByteArray* data);

    typedef MasterPKPtr(*loadCMPKPtr)(char* name);


    typedef GByteArray* (*cpDecryptPtr)(UserSKPtr decryptKey, CipherTextPtr ciphertext);
    typedef CipherTextPtr(*cpEncryptPtr)(MasterPKPtr mpk, GByteArray* msg, char* policy);
    typedef int (*cpDecryptLPtr)(UserSKPtr decryptKey, CipherTextPtr ciphertext, cpabe_aes_ptr aes);
    typedef CipherTextPtr(*cpEncryptLPtr)(MasterPKPtr mpk, char* policy, int aes_bits, cpabe_aes_ptr aes);



    typedef GByteArray* (*ctSavePtr)(const CipherTextPtr);
    typedef CipherTextPtr(*ctLoadPtr)(GByteArray*);

    typedef struct _CPABECrypt {
        ABECryptPtr baseABECrypt;
        void* derivedABECrypt;
        cpEncryptPtr encrypt;
        cpEncryptLPtr encrypt1;
        cpDecryptPtr decrypt;
        cpDecryptLPtr decrypt1;
        loadCMPKPtr loadMPK;
        parseABEUSKPtr parseUserKey;
        loadABEUSKPtr loadUserKey;
        ctSavePtr serializeCT;
        ctLoadPtr unserializeCT;
    } CPABECryptS;
    typedef struct _CPABECrypt CPABECrypt[1];
    CPABECryptPtr new_CPABECryptDef();
    void delete_CPABECrypt(CPABECryptPtr cryptPtr);

    CipherTextPtr cpabeCrypt_encrypt(MasterPKPtr mpk, GByteArray* msg, char* policy);

    GByteArray* cpabeCrypt_decrypt(UserSKPtr decryptKey, CipherTextPtr ciphertext);

    UserSKPtr cpabeCrypt_loadUSK(MasterPKPtr mpk, char* filename);
    UserSKPtr cpabeCrypt_parseUSK(MasterPKPtr mpk, GByteArray* data);

#ifdef __cplusplus
}
#endif

#endif /* CPABECRYPT_H */

