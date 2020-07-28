/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include "cpabekeygen.h"


#ifndef CPABEODKEYGEN_H
#define CPABEODKEYGEN_H

#ifdef __cplusplus
extern "C" {
#endif
   CPABEKeyGenPtr new_CPABEODKeyGenDef();
    CPABEKeyGenPtr new_CPABEODKeyGen(PairingGroupPtr pairingGrp);
    void delete_CPABEODKeyGen(CPABEKeyGenPtr cryptPtr);
    UserSKPtr cpabeodCrypt_keygen(MasterKeyPairPtr mkp, char** attributes);
#ifdef __cplusplus
}
#endif

#endif /* CPABEODKEYGEN_H */

