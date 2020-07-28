/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include <stdlib.h>
#include <string.h>
#include "object/ghwcpabeodkey.h"
#include "ext/cpabeutil.h"
#include "util/abeutil.h"

UserSKPtr new_CPABEODUserSK(pairing_ptr pairing, cpabe_prv_ptr priv) {
    if (pairing == NULL)
        return NULL;
    UserSKPtr baseSK = new_UserSK(pairing);
    CPABEODUserSKPtr cpabepriv = malloc(sizeof (CPABEODUserSK));
    baseSK->derivedSK = cpabepriv;
    cpabepriv->baseSK = baseSK;

    UserSKPtr usertk = new_CPABEUserSK(pairing, priv);
    CPABEUserSKPtr userTK = (CPABEUserSKPtr) usertk->derivedSK;
    cpabepriv->userTK = userTK;

    UserSKPtr userdk = new_UserSK(pairing);
    CPABEODUserDKPtr userDK = malloc(sizeof (CPABEODUserDK));
    userdk->derivedSK = userDK;
    userDK->baseSK = userdk;
    cpabepriv->userDK = userDK;
    if (priv){
        userDK->priv = priv->derivedKey;
        cpabe_prv_ptr tdkbase = malloc(sizeof(cpabe_prv_t));
        userDK->priv->base_prv = tdkbase;
        tdkbase->derivedKey = cpabepriv->userDK->priv;
        
        priv->derivedKey = NULL;
    }else
        cpabepriv->userDK->priv = NULL;

    baseSK->load = cpabeodUserSK_load;
    baseSK->save = cpabeodUserSK_save;
    baseSK->serialize = cpabeodUserSK_serialize;
    baseSK->unserialize = cpabeodUserSK_unserialize;
    baseSK->getOutsourcedKey = cpabeodUserSK_getOutsourcedKey;

    cpabepriv->userTK->baseSK->serialize = cpabeodUserSK_serializeUTK;
    cpabepriv->userTK->baseSK->unserialize = cpabeodUserSK_unserializeUTK;
    cpabepriv->userDK->baseSK->serialize = cpabeodUserSK_serializeUSK;
    cpabepriv->userDK->baseSK->unserialize = cpabeodUserSK_unserializeUSK;
    return baseSK;
}

void delete_CPABEODUserSK(UserSKPtr userSK) {
    CPABEODUserSKPtr cpabeUserSK = cpabeodUserSK_getObject(userSK);
    delete_UserSK(userSK);
    if (cpabeUserSK) {
        if (cpabeUserSK->userDK->priv) {
            delete_UserSK(cpabeUserSK->userDK->baseSK);
            cpabe_prvod_free(cpabeUserSK->userDK->priv);
            free(cpabeUserSK->userDK);
            cpabeUserSK->userDK = NULL;
        }
        if (cpabeUserSK->userTK->priv) {
            delete_CPABEUserSK(cpabeUserSK->userTK->baseSK);
            cpabeUserSK->userTK = NULL;
        }
        free(cpabeUserSK);
    }
}

UserSKPtr cpabeodUserSK_getOutsourcedKey(UserSKPtr priv) {
    CPABEODUserSKPtr userSKPtr = cpabeodUserSK_getObject(priv);
    if (userSKPtr == NULL) {
        return NULL;
    }
    return userSKPtr->userTK->baseSK;
}

CPABEODUserSKPtr cpabeodUserSK_getObject(UserSKPtr const priv) {
    if (priv && priv->derivedSK != NULL) {
        if (priv->derivedSK) {
            return (CPABEODUserSKPtr) priv->derivedSK;
        }
    }
    return NULL;
}

int cpabeodUserSK_save(UserSKPtr priv, char* name) {
    CPABEODUserSKPtr userSKPtr = cpabeodUserSK_getObject(priv);
    if (userSKPtr == NULL) {
        return 0;
    }
    int size = sizeof (char)*strlen(name) + 4;
    char* dk = malloc(size);
    snprintf(dk, size, "%s.dk", name);
    GByteArray* dkData = cpabeodUserSK_serializeUSK(priv);
    write_bytes_to_file(dk, dkData, 0);
    free(dk);
    if (priv->getOutsourcedKey(priv)) {
        char* tk = malloc(size);
        snprintf(tk, size, "%s.tk", name);
        GByteArray* tkData = cpabeodUserSK_serializeUTK(userSKPtr->userTK->baseSK);
        write_bytes_to_file(tk, tkData, 0);
        free(tk);
    }
    return 1;
}

int cpabeodUserSK_load(UserSKPtr priv, char* name) {
    CPABEODUserSKPtr userSKPtr = cpabeodUserSK_getObject(priv);
    if (userSKPtr == NULL) {
        return 0;
    }
    int size = sizeof (char)*strlen(name) + 4;
    char* tk = malloc(size);
    snprintf(tk, size, "%s.tk", name);
    char* dk = malloc(size);
    snprintf(dk, size, "%s.dk", name);
    GByteArray* tkData = read_bytes_from_file(tk);
    GByteArray* dkData = read_bytes_from_file(dk);
    if (dkData)
        cpabeodUserSK_unserializeUSK(priv, dkData);
    if (tkData)
        cpabeodUserSK_unserializeUTK(userSKPtr->userTK->baseSK, tkData);
    free(tk);
    free(dk);
    return 1;
}

GByteArray* cpabeodUserSK_serialize(UserSKPtr userSK) {
    CPABEODUserSKPtr userSKPtr = cpabeodUserSK_getObject(userSK);
    if (userSKPtr == NULL) {
        return NULL;
    }
    return cpabeodUserSK_serializeUSK(userSK);

    //    if (userSKPtr->userTK->priv) {
    //        GByteArray* userTK = cpabeodUserSK_serializeUTK(userSK);
    //        write_byte_array(allKeys, userTK);
    //        g_byte_array_free(userTK, 0);
    //    }
}

int cpabeodUserSK_unserialize(UserSKPtr priv, GByteArray* data) {
    CPABEODUserSKPtr userSKPtr = cpabeodUserSK_getObject(priv);
    if (userSKPtr == NULL) {
        return 0;
    }
    int offset = 0;
    GByteArray* userDK = g_byte_array_new();
    read_byte_array(data, &offset, &userDK);
    if (userSKPtr->userDK && userDK->len) {
        cpabeodUserSK_unserializeUSK(priv, userDK);
    } else
        return 0;
    //    if (offset < data->len) {
    //        GByteArray* userTK = g_byte_array_new();
    //        read_byte_array(data, &offset, &userTK);
    //        if (userSKPtr->userTK && userTK->len) {
    //            cpabeodUserSK_unserializeUTK(priv, userTK);
    //        }
    //    }
    return 1;
}

GByteArray* cpabeodUserSK_serializeUSK(UserSKPtr userDK) {
    CPABEODUserSKPtr userSKPtr = cpabeodUserSK_getObject(userDK);
    if (userSKPtr == NULL) {
        return 0;
    }
    userSKPtr->userDK->priv->base_prv->type = USER_DK;
    return cpabe_udk_serialize(userSKPtr->userDK->priv->base_prv);
}

GByteArray* cpabeodUserSK_serializeUTK(UserSKPtr userTK) {
    CPABEUserSKPtr userTKPtr = cpabeUserSK_getObject(userTK);
    if (userTKPtr == NULL) {
        return 0;
    }
    userTKPtr->priv->type = USER_TK;
    return cpabe_prv_serialize(userTKPtr->priv);
}

int cpabeodUserSK_unserializeUSK(UserSKPtr priv, GByteArray* data) {
    CPABEODUserSKPtr userSKPtr = cpabeodUserSK_getObject(priv);
    if (userSKPtr == NULL) {
        return 0;
    }
    cpabe_prv_t* udk = cpabe_udk_unserialize(priv->pairing, data, 1);
    userSKPtr->userDK->priv = (cpabe_prvod_ptr) udk->derivedKey;
    return 1;
}

int cpabeodUserSK_unserializeUTK(UserSKPtr userTK, GByteArray* data) {
    CPABEODUserSKPtr userSKPtr = cpabeodUserSK_getObject(userTK);
    if (userSKPtr == NULL) {
        return 0;
    }
    cpabe_prv_t* utk = cpabe_prv_unserialize(userTK->pairing, data, 1);
    userSKPtr->userTK->priv = utk;
    return 1;
}
