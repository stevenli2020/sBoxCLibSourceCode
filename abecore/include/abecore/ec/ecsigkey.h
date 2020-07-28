/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include "util/abeutil.h"
#include "pbc_sig.h"
#ifndef USERKEY_H
#define USERKEY_H

#ifdef __cplusplus
extern "C" {
#endif
    typedef struct _ECSigSK *ECSigSKPtr;

    typedef int (*loadSigSKPtr)(ECSigSKPtr, char*);
    typedef int (*saveSigSKPtr)(ECSigSKPtr, char*);
    typedef GByteArray* (*serializeSigSKPtr)(ECSigSKPtr);
    typedef ECSigSKPtr(*unserializeSigSKPtr)(ECSigSKPtr, GByteArray*);

    typedef struct _ECSigSK {
        pairing_ptr pairing;
        //element_t beta;
        int sigLen;
        bls_private_key_ptr sk;
        loadSigSKPtr load;
        saveSigSKPtr save;
        serializeSigSKPtr serialize;
        unserializeSigSKPtr unserialize;
    } ECSigSKS;
    typedef struct _ECSigSK ECSigSK[1];

    typedef struct _ECSigPK *ECSigPKPtr;

    typedef int (*loadSigPKPtr)(ECSigPKPtr, char*);
    typedef int (*saveSigPKPtr)(ECSigPKPtr, char*);
    typedef GByteArray* (*serializeSigPKPtr)(ECSigPKPtr);
    typedef ECSigPKPtr(*unserializeSigPKPtr)(ECSigPKPtr, GByteArray*);

    typedef struct _ECSigPK {
        pairing_ptr pairing;
        //element_t gBeta;
        int sigLen;
        bls_public_key_ptr pk;
        loadSigPKPtr load;
        saveSigPKPtr save;
        serializeSigPKPtr serialize;
        unserializeSigPKPtr unserialize;
    } ECSigPKS;
    typedef struct _ECSigPK ECSigPK[1];

    typedef struct _ECSigKeyPair {
        ECSigSKPtr sigSK;
        ECSigPKPtr sigPK;
    } ECSigKeyPairS;
    typedef struct _ECSigKeyPair *ECSigKeyPairPtr;
    typedef struct _ECSigKeyPair ECSigKeyPair[1];


    ECSigSKPtr new_ECSigSK(pairing_ptr pairing);
    void delete_ECSigSK(ECSigSKPtr priv);
    int ecSigSK_save(ECSigSKPtr priv, char* name);
    int ecSigSK_load(ECSigSKPtr priv, char* name);

    ECSigPKPtr new_ECSigPK(pairing_ptr pairing);
    void delete_ECSigPK(ECSigPKPtr priv);
    int ecSigPK_save(ECSigPKPtr priv, char* name);
    int ecSigPK_load(ECSigPKPtr priv, char* name);

    GByteArray* ecSigPK_serialize(ECSigPKPtr abePK);
    ECSigPKPtr ecSigPK_unserialize(ECSigPKPtr abePK, GByteArray* pk);

    GByteArray* ecSigSK_serialize(ECSigSKPtr abeSK);
    ECSigSKPtr ecSigSK_unserialize(ECSigSKPtr abeSK, GByteArray* pk);

    ECSigKeyPairPtr new_ECSigKeyPair(ECSigSKPtr abeSK, ECSigPKPtr abePK);
    void delete_ECSigKeyPair(ECSigKeyPairPtr mskp);
#ifdef __cplusplus
}
#endif

#endif /* USERKEY_H */

