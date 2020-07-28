/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include "abe/cpabe/cpabekeygen.h"
#include "object/bswcpabekey.h"
#include "ext/cpabeutil.h"

CPABEKeyGenPtr new_CPABEKeyGenDef() {
    CPABEKeyGenPtr cpabeKeyGen = malloc(sizeof (CPABEKeyGen));
    cpabeKeyGen->derivedABEKeyGen = NULL;
    cpabeKeyGen->loadMPK = cpabeMPK_loadMPK;
    cpabeKeyGen->loadMSK = cpabeKGMSK_loadMSK;
    cpabeKeyGen->parseMPK = cpabeKGMPK_parseMPK;
    cpabeKeyGen->parseMSK = cpabeKGMSK_parseMSK;
    cpabeKeyGen->setup = cpabeKeyGen_setup;
    cpabeKeyGen->keygen = cpabeKeyGen_keygen;
    return cpabeKeyGen;
}

CPABEKeyGenPtr new_CPABEKeyGen(PairingGroupPtr pairingGrp) {
    if (pairingGrp == NULL)
        return NULL;
    CPABEKeyGenPtr abeKeyGen = new_CPABEKeyGenDef();
    abeKeyGen->pairingGrp = pairingGrp;
    return abeKeyGen;
}

void delete_CPABEKeyGen(CPABEKeyGenPtr cryptPtr) {
    if (cryptPtr == NULL)
        return;
    //    if (cryptPtr->pairingGrp) {
    //        delete_PairingGroup(cryptPtr->pairingGrp);
    //    }
    free(cryptPtr);
}

MasterKeyPairPtr cpabeKeyGen_setup(CPABEKeyGenPtr abeKeyGen) {
    if(abeKeyGen->pairingGrp == NULL || abeKeyGen->pairingGrp->pairingDesc == NULL)
        return NULL;

    char * pairing_desc = abeKeyGen->pairingGrp->pairingDesc;

    cpabe_pub_t* pub;
    cpabe_msk_t* msk;

    cpabe_setup(&pub, &msk, pairing_desc);

    //MasterSKPtr cpMSK = new_CPABEMasterSK(abeKeyGen->pairingGrp->pairing, msk);
    MasterSKPtr cpMSK = new_CPABEMasterSK(pub->p, msk);
    MasterPKPtr cpMPK = new_CPABEMasterPK1(pub);
    return new_MasterKeyPair(cpMPK, cpMSK);
}

UserSKPtr cpabeKeyGen_keygen(MasterKeyPairPtr mkp, char** attributes) {
    if (mkp == NULL)
        return NULL;
    CPABEMasterSKPtr mskPtr = cpabeMSK_getObject(mkp->msk);
    CPABEMasterPKPtr mpkPtr = cpabeMPK_getObject(mkp->mpk);
    if (mskPtr == NULL || mpkPtr == NULL) {
        return NULL;
    }
    cpabe_prv_t* prv = cpabe_keygen(mpkPtr->pub, mskPtr->msk, attributes, 0);
    UserSKPtr cpUserSK = new_CPABEUserSK(mskPtr->baseSK->pairing, prv);
    return cpUserSK;
}

MasterSKPtr cpabeKGMSK_loadMSK(MasterPKPtr mpk, char* filename) {
    return cpabeMSK_loadMSK(mpk->pairing, filename);
}

MasterSKPtr cpabeKGMSK_parseMSK(MasterPKPtr mpk, GByteArray* data){
    MasterSKPtr msk = new_CPABEMasterSK(mpk->pairing, NULL);
    cpabeMasterSK_unserialize(msk, data);
    return msk;
}

MasterPKPtr cpabeKGMPK_parseMPK(GByteArray* data){
    MasterPKPtr mpk = new_CPABEMasterPKDef();
    cpabeMasterPK_unserialize(mpk, data);
    return mpk;
}
