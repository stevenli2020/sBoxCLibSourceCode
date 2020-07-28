/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include "abe/abekey.h"
#include "abe/abeodcrypt.h"
#include "abe/cpabe/cpabecrypt.h"
#include "abe/cpabe/cpabeodct.h"

#ifndef CPABEODTRANSCRYPT_CCA_H
#define CPABEODTRANSCRYPT_CCA_H

#ifdef __cplusplus
extern "C" {
#endif
    CPABEODTransCryptPtr new_CPABEOD_CCA_TransCryptDef();
    void delete_CPABEOD_CCA_TransCrypt(CPABEODTransCryptPtr cryptPtr);

    CipherTextPtr cpabeodTransCrypt_cca_transform(UserSKPtr decryptKey, CipherTextPtr msg);
    UserSKPtr cpabeodTransCrypt_cca_parseUTK(MasterPKPtr mpk, GByteArray* keyData);

#ifdef __cplusplus
}
#endif

#endif /* CPABEODTRANSCRYPT_H */

