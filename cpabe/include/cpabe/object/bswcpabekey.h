/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include "object/bswcpabeobject.h"
#include "object/bswcpabe_cca_object.h"
#include "abe/cpabe/cpabekey.h"

#ifndef BSWCPABEKEY_H
#define BSWCPABEKEY_H

#ifdef __cplusplus
extern "C" {
#endif

    typedef struct _CPABEMasterSK {
        MasterSKPtr baseSK;
        cpabe_msk_t* msk;
    } _CPABEMasterSKS;
    typedef struct _CPABEMasterSK *CPABEMasterSKPtr;
    typedef struct _CPABEMasterSK CPABEMasterSK[1];

    typedef struct _CPABEMasterPK {
        MasterPKPtr basePK;
        cpabe_pub_t* pub;
    } CPABEMasterPKS;
    typedef struct _CPABEMasterPK *CPABEMasterPKPtr;
    typedef struct _CPABEMasterPK CPABEMasterPK[1];

    typedef struct _CPABEUserSK {
        UserSKPtr baseSK;
        cpabe_prv_ptr priv;
        void* derivedSK;
    } CPABEUserSKS;
    typedef struct _CPABEUserSK *CPABEUserSKPtr;
    typedef struct _CPABEUserSK CPABEUserSK[1];

    MasterSKPtr new_CPABEMasterSK(pairing_ptr pairing, cpabe_msk_t* msk);
    void delete_CPABEMasterSK(MasterSKPtr msk);
    int cpabeMasterSK_save(MasterSKPtr msk, char* name);
    int cpabeMasterSK_load(MasterSKPtr msk, char* name);
    GByteArray* cpabeMasterSK_serialize(MasterSKPtr msk);
    int cpabeMasterSK_unserialize(MasterSKPtr msk, GByteArray* data);

    MasterPKPtr new_CPABEMasterPKDef();
    MasterPKPtr new_CPABEMasterPK(pairing_ptr pairing, cpabe_pub_t* mpk);
    MasterPKPtr new_CPABEMasterPK1(cpabe_pub_t* mpk);
    void delete_CPABEMasterPK(MasterPKPtr mpk);
    int cpabeMasterPK_save(MasterPKPtr mpk, char* name);
    int cpabeMasterPK_load(MasterPKPtr mpk, char* name);
    GByteArray* cpabeMasterPK_serialize(MasterPKPtr mpk);
    int cpabeMasterPK_unserialize(MasterPKPtr mpk, GByteArray* data);

    UserSKPtr new_CPABEUserSK(pairing_ptr pairing, cpabe_prv_t* priv);
    void delete_CPABEUserSK(UserSKPtr priv);
    int cpabeUserSK_save(UserSKPtr priv, char* name);
    int cpabeUserSK_load(UserSKPtr priv, char* name);
    GByteArray* cpabeUserSK_serialize(UserSKPtr priv);
    int cpabeUserSK_unserialize(UserSKPtr priv, GByteArray* data);
    UserSKPtr cpabeUserSK_getOutsourcedKey(UserSKPtr userSK);

    CPABEMasterSKPtr cpabeMSK_getObject(MasterSKPtr const msk);

    CPABEMasterPKPtr cpabeMPK_getObject(MasterPKPtr const mpk);

    CPABEUserSKPtr cpabeUserSK_getObject(UserSKPtr const priv);

#ifdef __cplusplus
}
#endif

#endif /* BSWCPABEKEY_H */

