/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include "abe/cpabe/cpabekey.h"
#include "pairing/pairingfactory.h"

#ifndef CPABEKEYGEN_H
#define CPABEKEYGEN_H

#ifdef __cplusplus
extern "C" {
#endif

    typedef struct _CPABEKeyGen* CPABEKeyGenPtr;

    typedef MasterKeyPairPtr(*cpSetupPtr)(CPABEKeyGenPtr keyGen);
    typedef UserSKPtr(*cpKeygenPtr)(MasterKeyPairPtr mkp, char** attributes);

    typedef MasterPKPtr(*loadKGMPKPtr)(char* name);
    typedef MasterSKPtr(*loadKGMSKPtr)(MasterPKPtr mpk, char* name);
    typedef MasterPKPtr(*parseKGMPKPtr)(GByteArray* name);
    typedef MasterSKPtr(*parseKGMSKPtr)(MasterPKPtr mpk, GByteArray* name);

    typedef struct _CPABEKeyGen {
        PairingGroupPtr pairingGrp;
        void* derivedABEKeyGen;
        cpSetupPtr setup;
        cpKeygenPtr keygen;
        loadKGMSKPtr loadMSK;
        parseKGMSKPtr parseMSK;
        loadKGMPKPtr loadMPK;
        parseKGMPKPtr parseMPK;
    } CPABEKeyGenS;

    typedef struct _CPABEKeyGen CPABEKeyGen[1];
    CPABEKeyGenPtr new_CPABEKeyGenDef();
    CPABEKeyGenPtr new_CPABEKeyGen(PairingGroupPtr pairingGrp);
    void delete_CPABEKeyGen(CPABEKeyGenPtr cryptPtr);

    MasterKeyPairPtr cpabeKeyGen_setup(CPABEKeyGenPtr keyGen);
    UserSKPtr cpabeKeyGen_keygen(MasterKeyPairPtr mkp, char** attributes);

    MasterSKPtr cpabeKGMSK_loadMSK(MasterPKPtr mpk, char* filename);
    MasterSKPtr cpabeKGMSK_parseMSK(MasterPKPtr mpk, GByteArray* data);
    MasterPKPtr cpabeKGMPK_parseMPK(GByteArray* data);

#ifdef __cplusplus
}
#endif

#endif /* CPABEKEYGEN_H */

