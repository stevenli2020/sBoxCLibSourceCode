/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include <stdlib.h>
#include "abe/cpabe/cpabeodkeygen.h"
#include "object/ghwcpabeodkey.h"
#include "ext/cpabeutil.h"

CPABEKeyGenPtr new_CPABEODKeyGenDef() {
    CPABEKeyGenPtr cpabeKeyGen = malloc(sizeof (CPABEKeyGen));
    cpabeKeyGen->derivedABEKeyGen = NULL;
    cpabeKeyGen->loadMPK = cpabeMPK_loadMPK;
    cpabeKeyGen->loadMSK = cpabeKGMSK_loadMSK;
    cpabeKeyGen->setup = cpabeKeyGen_setup;
    cpabeKeyGen->keygen = cpabeodCrypt_keygen;
    return cpabeKeyGen;
}

CPABEKeyGenPtr new_CPABEODKeyGen(PairingGroupPtr pairingGrp) {
    if (pairingGrp == NULL)
        return NULL;
    CPABEKeyGenPtr abeKeyGen = new_CPABEODKeyGenDef();
    abeKeyGen->pairingGrp = pairingGrp;
    return abeKeyGen;
}

void delete_CPABEODKeyGen(CPABEKeyGenPtr cryptPtr) {
    if (cryptPtr == NULL)
        return;
    //    if (cryptPtr) {
    //        delete_PairingGroup(cryptPtr->pairingGrp);
    //    }
    free(cryptPtr);
}

UserSKPtr cpabeodCrypt_keygen(MasterKeyPairPtr mkp, char** attributes) {
    if (mkp == NULL)
        return NULL;
    CPABEMasterSKPtr mskPtr = cpabeMSK_getObject(mkp->msk);
    CPABEMasterPKPtr mpkPtr = cpabeMPK_getObject(mkp->mpk);
    if (mskPtr == NULL || mpkPtr == NULL) {
        return NULL;
    }
    cpabe_prv_t* prv = cpabe_keygen(mpkPtr->pub, mskPtr->msk, attributes, 1);
    UserSKPtr cpUserSK = new_CPABEODUserSK(mskPtr->baseSK->pairing, prv);
    return cpUserSK;
}
