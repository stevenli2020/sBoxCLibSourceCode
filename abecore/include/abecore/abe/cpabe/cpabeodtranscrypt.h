/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include "abe/abekey.h"
#include "abe/abeodcrypt.h"
#include "abe/cpabe/cpabecrypt.h"
#include "abe/cpabe/cpabeodct.h"

#ifndef CPABEODTRANSCRYPT_H
#define CPABEODTRANSCRYPT_H

#ifdef __cplusplus
extern "C" {
#endif

    typedef struct _CPABEODTransCrypt *CPABEODTransCryptPtr;
    typedef CipherTextPtr(*cpTransformPtr)(UserSKPtr decryptKey, CipherTextPtr msg);
    typedef UserSKPtr(*cpLoadUserTKPtr)(MasterPKPtr mpk,char* filename);
    typedef UserSKPtr(*cpParseUserTKPtr)(MasterPKPtr mpk,GByteArray* filename);
    typedef MasterPKPtr(*loadTCMPKPtr)(char* filename);

    typedef struct _CPABEODTransCrypt {
        ABECryptPtr baseABECrypt;
        cpTransformPtr transform;
        loadTCMPKPtr loadMPK;
        cpLoadUserTKPtr loadUserTKey;
        cpParseUserTKPtr parseUserTKey;
        ctSavePtr serializeCT;
        ctLoadPtr unserializeCT;
    } CPABEODTransCryptS;

    typedef struct _CPABEODTransCrypt CPABEODTransCrypt[1];
    CPABEODTransCryptPtr new_CPABEODTransCryptDef();
    void delete_CPABEODTransCrypt(CPABEODTransCryptPtr cryptPtr);

    CipherTextPtr cpabeodTransCrypt_transform(UserSKPtr decryptKey, CipherTextPtr msg);

    UserSKPtr cpabeodTransCrypt_loadUTK(MasterPKPtr mpk, char* filename);

    UserSKPtr cpabeodTransCrypt_parseUTK(MasterPKPtr mpk, GByteArray* keyData);
    
    MasterPKPtr cpabeTCMPK_loadMPK(CPABEODTransCryptPtr abeCrypt, char* filename);

#ifdef __cplusplus
}
#endif

#endif /* CPABEODTRANSCRYPT_H */

