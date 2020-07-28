/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include "bswcpabekey.h"
#ifndef CPABEODKEY_H
#define CPABEODKEY_H

#ifdef __cplusplus
extern "C" {
#endif
    typedef GByteArray* (*cpSerializeUserTKPtr)(UserSKPtr);
    typedef GByteArray* (*cpSerializeUserSKPtr)(UserSKPtr);

    typedef struct _CPABEODUserDK {
        UserSKPtr baseSK;
        cpabe_prvod_ptr priv;
    } CPABEODUserTKS;
    typedef struct _CPABEODUserDK *CPABEODUserDKPtr;
    typedef struct _CPABEODUserDK CPABEODUserDK[1];

    typedef struct _CPABEODUserSK {
        UserSKPtr baseSK;
        CPABEUserSKPtr userTK;
        CPABEODUserDKPtr userDK;
        cpSerializeUserSKPtr serializeSK;
        cpSerializeUserSKPtr serializeTK;
        cpSerializeUserSKPtr unserializeSK;
        cpSerializeUserSKPtr unserializeTK;
    } CPABEODUserSKS;
    typedef struct _CPABEODUserSK *CPABEODUserSKPtr;
    typedef struct _CPABEODUserSK CPABEODUserSK[1];

    UserSKPtr new_CPABEODUserSK(pairing_ptr pairing, cpabe_prv_ptr priv);
    void delete_CPABEODUserSK(UserSKPtr priv);
    int cpabeodUserSK_save(UserSKPtr priv, char* name);
    int cpabeodUserSK_load(UserSKPtr priv, char* name);

    UserSKPtr cpabeodUserSK_getOutsourcedKey(UserSKPtr userSK);
    CPABEODUserSKPtr cpabeodUserSK_getObject(UserSKPtr const priv);

    GByteArray* cpabeodUserSK_serialize(UserSKPtr priv);
    GByteArray* cpabeodUserSK_serializeUTK(UserSKPtr userTK);
    GByteArray* cpabeodUserSK_serializeUSK(UserSKPtr userDK);
    int cpabeodUserSK_unserialize(UserSKPtr priv, GByteArray* data);
    int cpabeodUserSK_unserializeUTK(UserSKPtr userTK, GByteArray* data);
    int cpabeodUserSK_unserializeUSK(UserSKPtr priv, GByteArray* data);
#ifdef __cplusplus
}
#endif

#endif /* CPABEODKEY_H */

