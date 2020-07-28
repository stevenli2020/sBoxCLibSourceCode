/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include "pbc.h"
#include "glib.h"
#include "pairing/pairinggroup.h"
#ifndef ABEKEY_H
#define ABEKEY_H

const static int USER_SK = 1;
const static int USER_TK = 2;
const static int USER_DK = 3;

#define OUTSOURCED_CRYPT_TYPE "OUTSOURCED_CRYPT_TYPE"
#define MSK_FILE "MSK_FILE"
#define MPK_FILE "MPK_FILE"

#ifdef __cplusplus
extern "C" {
#endif

    typedef void (*setPairingPtr)(pairing_ptr, pairing_ptr);

    typedef struct _MasterSK *MasterSKPtr;

    typedef int (*loadMSKPtr)(MasterSKPtr, char* name);
    typedef int (*saveMSKPtr)(MasterSKPtr, char* name);

    typedef int (*unserializeMSKPtr)(MasterSKPtr, GByteArray*);
    typedef GByteArray* (*serializeMSKPtr)(MasterSKPtr);

    typedef struct _MasterSK {
        pairing_ptr pairing;
        void *derivedMSK;
        loadMSKPtr load;
        saveMSKPtr save;
        serializeMSKPtr serialize;
        unserializeMSKPtr unserialize;
        setPairingPtr setPairing;
    } MasterSKS;

    typedef struct _MasterSK MasterSK[1];

    typedef struct _MasterPK *MasterPKPtr;

    typedef int (*loadPSKPtr)(MasterPKPtr, char* name);
    typedef int (*savePSKPtr)(MasterPKPtr, char* name);
    typedef int (*unserializePSKPtr)(MasterPKPtr, GByteArray*);
    typedef GByteArray* (*serializePSKPtr)(MasterPKPtr);

    typedef struct _MasterPK {
        pairing_ptr pairing;
        void *derivedMPK;
        loadPSKPtr load;
        savePSKPtr save;
        setPairingPtr setPairing;
        serializePSKPtr serialize;
        unserializePSKPtr unserialize;
    } MasterPKS;
    typedef struct _MasterPK MasterPK[1];

    typedef struct _MasterKeyPair {
        MasterPKPtr mpk;
        MasterSKPtr msk;
    } MasterKeyPairS;
    typedef struct _MasterKeyPair *MasterKeyPairPtr;
    typedef struct _MasterKeyPair MasterKeyPair[1];

    typedef struct _UserSK *UserSKPtr;

    typedef int (*loadUSKPtr)(UserSKPtr, char* name);
    typedef int (*saveUSKPtr)(UserSKPtr, char* name);
    typedef int (*unserializeUSKPtr)(UserSKPtr, GByteArray*);
    typedef GByteArray* (*serializeUSKPtr)(UserSKPtr);
    typedef UserSKPtr(*getOutsourcedKeyPtr)(UserSKPtr);

    typedef struct _UserSK {
        pairing_ptr pairing;
        void *derivedSK;
        loadUSKPtr load;
        saveUSKPtr save;
        serializeUSKPtr serialize;
        unserializeUSKPtr unserialize;
        getOutsourcedKeyPtr getOutsourcedKey;
        setPairingPtr setPairing;
    } UserSKS;

    typedef struct _UserSK UserSK[1];

    MasterKeyPairPtr new_MasterKeyPair(MasterPKPtr mpk, MasterSKPtr msk);
    void delete_MasterKeyPair(MasterKeyPairPtr mk);
    //
    MasterSKPtr new_MasterSK(pairing_ptr pairing);
    void delete_MasterSK(MasterSKPtr msk);
    //    int masterSK_save(MasterSKPtr msk, char* name);
    //    int masterSK_load(MasterSKPtr msk, char* name);
    //    
    MasterPKPtr new_MasterPK(pairing_ptr pairing);
    void delete_MasterPK(MasterPKPtr mpk);
    //    int masterPK_save(MasterPKPtr mpk, char* name);
    //    int masterPK_load(MasterPKPtr mpk, char* name);
    //   
    UserSKPtr new_UserSK(pairing_ptr pairing);
    void delete_UserSK(UserSKPtr userSK);
    //    int userSK_save(UserSKPtr userSK, char* name);
    //    int userSK_load(UserSKPtr userSK, char* name);
    UserSKPtr userSK_getOutsourcedKey(UserSKPtr userSK);
    
    void key_set_pairing(pairing_ptr dest, pairing_ptr src);
    int userSK_get_keytype(char * filename);



#ifdef __cplusplus
}
#endif

#endif /* ABEKEY_H */

