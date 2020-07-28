/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include <stdlib.h>
#include "object/bswcpabekey.h"
#include "ext/cpabeutil.h"
#include "util/abeutil.h"

MasterSKPtr new_CPABEMasterSK(pairing_ptr pairing, cpabe_msk_t* msk) {
    if (pairing == NULL)
        return NULL;
    MasterSKPtr bmsk = new_MasterSK(pairing);
    CPABEMasterSKPtr cpabemsk = malloc(sizeof (CPABEMasterSK));
    cpabemsk->msk = msk;
    cpabemsk->baseSK = bmsk;
    bmsk->derivedMSK = cpabemsk;
    bmsk->load = cpabeMasterSK_load;
    bmsk->save = cpabeMasterSK_save;
    bmsk->serialize = cpabeMasterSK_serialize;
    bmsk->unserialize = cpabeMasterSK_unserialize;
    return bmsk;
}

void delete_CPABEMasterSK(MasterSKPtr msk) {
    CPABEMasterSKPtr cpabeMSK = cpabeMSK_getObject(msk);
    delete_MasterSK(msk);
    if (cpabeMSK) {
        cpabe_msk_free(cpabeMSK->msk);
        cpabeMSK->msk = NULL;
        free(cpabeMSK);
    }
}
MasterPKPtr new_CPABEMasterPKDef(){
    MasterPKPtr bmpk = new_MasterPK(NULL);
    CPABEMasterPKPtr cpabempk = malloc(sizeof (CPABEMasterPK));
    cpabempk->basePK = bmpk;
    bmpk->derivedMPK = cpabempk;
    bmpk->load = cpabeMasterPK_load;
    bmpk->save = cpabeMasterPK_save;
    bmpk->serialize = cpabeMasterPK_serialize;
    bmpk->unserialize = cpabeMasterPK_unserialize;
    return bmpk;
}
MasterPKPtr new_CPABEMasterPK(pairing_ptr pairing, cpabe_pub_t* mpk) {
    if (pairing == NULL)
        return NULL;
    MasterPKPtr bmpk = new_MasterPK(pairing);
    CPABEMasterPKPtr cpabempk = malloc(sizeof (CPABEMasterPK));
    cpabempk->pub = mpk;
    cpabempk->basePK = bmpk;
    bmpk->derivedMPK = cpabempk;
    bmpk->load = cpabeMasterPK_load;
    bmpk->save = cpabeMasterPK_save;
    bmpk->serialize = cpabeMasterPK_serialize;
    bmpk->unserialize = cpabeMasterPK_unserialize;
    return bmpk;
}

MasterPKPtr new_CPABEMasterPK1(cpabe_pub_t* mpk) {
    if (mpk == NULL)
        return NULL;
    MasterPKPtr bmpk = new_CPABEMasterPKDef();
    bmpk->pairing = mpk->p;
    CPABEMasterPKPtr cpabempk = (CPABEMasterPKPtr)bmpk->derivedMPK;
    cpabempk->pub = mpk;
    return bmpk;
}

void delete_CPABEMasterPK(MasterPKPtr mpk) {
    CPABEMasterPKPtr cpabeMPK = cpabeMPK_getObject(mpk);
    delete_MasterPK(mpk);
    if (cpabeMPK) {
        cpabe_pub_free(cpabeMPK->pub);
        cpabeMPK->pub = NULL;
        free(cpabeMPK);
    }
}

UserSKPtr new_CPABEUserSK(pairing_ptr pairing, cpabe_prv_t* priv) {
    if (pairing == NULL)
        return NULL;
    UserSKPtr bpriv = new_UserSK(pairing);
    CPABEUserSKPtr cpabepriv = malloc(sizeof (CPABEUserSK));
    cpabepriv->baseSK = bpriv;
    cpabepriv->priv = priv;
    bpriv->derivedSK = cpabepriv;
    cpabepriv->derivedSK = NULL;
    bpriv->setPairing = key_set_pairing;
    bpriv->load = cpabeUserSK_load;
    bpriv->save = cpabeUserSK_save;
    bpriv->serialize = cpabeUserSK_serialize;
    bpriv->unserialize = cpabeUserSK_unserialize;
    bpriv->getOutsourcedKey = cpabeUserSK_getOutsourcedKey;
    return bpriv;
}

void delete_CPABEUserSK(UserSKPtr userSK) {
    CPABEUserSKPtr cpabeUserSK = cpabeUserSK_getObject(userSK);
    delete_UserSK(userSK);
    if (cpabeUserSK) {
        cpabe_prv_free(cpabeUserSK->priv);
        cpabeUserSK->priv = NULL;
        free(cpabeUserSK);
    }
}

UserSKPtr cpabeUserSK_getOutsourcedKey(UserSKPtr userSK) {
    return NULL;
}

CPABEMasterSKPtr cpabeMSK_getObject(MasterSKPtr const msk) {
    if (msk && msk->derivedMSK != NULL)
        return (CPABEMasterSKPtr) msk->derivedMSK;
    return NULL;
}

CPABEMasterPKPtr cpabeMPK_getObject(MasterPKPtr const mpk) {
    if (mpk && mpk->derivedMPK != NULL)
        return (CPABEMasterPKPtr) mpk->derivedMPK;
    return NULL;
}

CPABEUserSKPtr cpabeUserSK_getObject(UserSKPtr const priv) {
    if (priv && priv->derivedSK != NULL)
        return (CPABEUserSKPtr) priv->derivedSK;
    return NULL;
}

int cpabeMasterSK_save(MasterSKPtr msk, char* name) {
    GByteArray* data = cpabeMasterSK_serialize(msk);
    if (data)
        return write_bytes_to_file(name, data, 1);
    return 0;
}

int cpabeMasterSK_load(MasterSKPtr msk, char* name) {
    GByteArray* data = read_bytes_from_file(name);
    if (data)
        return cpabeMasterSK_unserialize(msk, data);
    return 0;
}

GByteArray* cpabeMasterSK_serialize(MasterSKPtr msk) {
    CPABEMasterSKPtr mskPtr = cpabeMSK_getObject(msk);
    if (mskPtr == NULL) {
        return 0;
    }
    return cpabe_msk_serialize(mskPtr->msk);
}

int cpabeMasterSK_unserialize(MasterSKPtr msk, GByteArray* data) {
    CPABEMasterSKPtr mskPtr = cpabeMSK_getObject(msk);
    if (mskPtr == NULL) {
        return 0;
    }
    mskPtr->msk = cpabe_msk_unserialize(msk->pairing, data, 1);
}

int cpabeMasterPK_save(MasterPKPtr mpk, char* name) {
    GByteArray* data = cpabeMasterPK_serialize(mpk);
    if (data)
        return write_bytes_to_file(name, data, 1);
    return 0;
}

int cpabeMasterPK_load(MasterPKPtr mpk, char* name) {
    GByteArray* data = read_bytes_from_file(name);
    if (data)
        return cpabeMasterPK_unserialize(mpk, data);
    return 0;
}

GByteArray* cpabeMasterPK_serialize(MasterPKPtr mpk) {
    CPABEMasterPKPtr mpkPtr = cpabeMPK_getObject(mpk);
    if (mpkPtr == NULL) {
        return 0;
    }
    return cpabe_pub_serialize(mpkPtr->pub);
}

int cpabeMasterPK_unserialize(MasterPKPtr mpk, GByteArray* data) {
    CPABEMasterPKPtr mpkPtr = cpabeMPK_getObject(mpk);
    if (mpkPtr == NULL) {
        return 0;
    }
    mpkPtr->pub = cpabe_pub_unserialize(data, 1);
    if(!mpk->pairing)
        mpk->pairing = mpkPtr->pub->p;
    return 1;
}

int cpabeUserSK_save(UserSKPtr priv, char* name) {
    GByteArray* data = cpabeUserSK_serialize(priv);
    if (data)
        return write_bytes_to_file(name, data, 1);
    return 0;
}

int cpabeUserSK_load(UserSKPtr priv, char* name) {
    GByteArray* data = read_bytes_from_file(name);
    if (data)
        return cpabeUserSK_unserialize(priv, data);
    return 0;
}

GByteArray* cpabeUserSK_serialize(UserSKPtr priv) {
    CPABEUserSKPtr userSKPtr = cpabeUserSK_getObject(priv);
    if (userSKPtr == NULL) {
        return 0;
    }
    userSKPtr->priv->type = USER_SK;
    return cpabe_prv_serialize(userSKPtr->priv);
}

int cpabeUserSK_unserialize(UserSKPtr priv, GByteArray* data) {
    CPABEUserSKPtr userSKPtr = cpabeUserSK_getObject(priv);
    if (userSKPtr == NULL) {
        return 0;
    }
    userSKPtr->priv = cpabe_prv_unserialize(priv->pairing, data, 1);
    userSKPtr->priv->derivedKey = NULL;
    return 1;
}

MasterPKPtr cpabeMPK_loadMPK(char* filename) {
    MasterPKPtr cpMPK = new_CPABEMasterPKDef();
    cpMPK->load(cpMPK, filename);
    return cpMPK;
}

MasterSKPtr cpabeMSK_loadMSK(pairing_ptr pairing, char* filename) {
    MasterSKPtr cpMSK = new_CPABEMasterSK(pairing, NULL);
    cpMSK->load(cpMSK, filename);
    return cpMSK;
}
