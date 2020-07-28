/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include <stdlib.h>
#include "glib.h"
#include "ec/ecsigkey.h"
#include "abe/abekey.h"

#ifndef ABEUSER_H
#define ABEUSER_H

#ifdef __cplusplus
extern "C" {
#endif
    typedef struct ABEUserS *ABEUserPtr;

    typedef char* (*getUserIDPtr) (ABEUserPtr);
    typedef char* (*getDPPtr) (ABEUserPtr);
    typedef char* (*getUNPtr) (ABEUserPtr);
    typedef char* (*getPwdPtr) (ABEUserPtr);
    typedef char* (*getPKPtr) (ABEUserPtr);
    typedef char* (*getTKPtr) (ABEUserPtr);
    typedef char** (*getAttrsPtr) (ABEUserPtr);
    typedef void (*setUserIDPtr) (ABEUserPtr, char*);
    typedef void (*setDPPtr) (ABEUserPtr, char*);
    typedef void (*setUNPtr) (ABEUserPtr, char*);
    typedef void (*setPwdPtr) (ABEUserPtr, char*);
    typedef void (*setPKPtr) (ABEUserPtr, char*);
    typedef void (*setTKPtr) (ABEUserPtr, char*);
    typedef void (*setAttrsPtr) (ABEUserPtr, GSList*);

    enum ABEUserValue {
        ABE_USER_ID,
        ABE_USER_CN,
        ABE_USER_SN,
        ABE_USER_IATTR,
        ABE_USER_ABEPWD,
        ABE_USER_PWD,
        ABE_USER_PK,
        ABE_USER_TK,
    };
    static char* ABEUserValues[] = {
        "uid",
        "cn",
        "sn",
        "abeIAttr",
        "abePwd",
        "userPassword",
        "abePK",
        "abeTK",
        NULL,
    };

    struct ABEUserS {
        void* derivedEntry;
        //        char* uid;
        //        char* displayName;
        //        char* username;
        //        char* password;
        //        char* pk;
        //        char* tk;
        GHashTable* values;
        GSList* attributes;
        ECSigKeyPairPtr keyPair;
        UserSKPtr userSK;
        getUserIDPtr getID;
        getUNPtr getUsername;
        getDPPtr getDisplayName;
        getPwdPtr getPassword;
        getPKPtr getPK;
        getTKPtr getTK;
        getAttrsPtr getAttributes;
        setUserIDPtr setID;
        setUNPtr setUsername;
        setDPPtr setDisplayName;
        setPwdPtr setPassword;
        setPKPtr setPK;
        setTKPtr setTK;
        setAttrsPtr setAttributes;
    } ABEUserS;

    typedef struct ABEUserS ABEUser[1];

    ABEUserPtr new_ABEUser(char* uid, char* displayName, char* username, char* password, char* pk, char* tk, GSList* attributes);
    void delete_ABEUser(ABEUserPtr user);
    char* abeUser_get_id(const ABEUserPtr const user);
    char* abeUser_get_displayname(const ABEUserPtr const user);
    char* abeUser_get_username(const ABEUserPtr const user);
    char* abeUser_get_password(const ABEUserPtr const user);
    char* abeUser_get_pk(const ABEUserPtr const user);
    char* abeUser_get_tk(const ABEUserPtr const user);
    char** abeUser_get_attributes(const ABEUserPtr const user);
    void abeUser_set_id(const ABEUserPtr const user, char* value);
    void abeUser_set_displayname(const ABEUserPtr const user, char* value);
    void abeUser_set_username(const ABEUserPtr const user, char* value);
    void abeUser_set_id(const ABEUserPtr const user, char* value);
    void abeUser_set_password(const ABEUserPtr const user, char* value);
    void abeUser_set_pk(const ABEUserPtr const user, char* value);
    void abeUser_set_tk(const ABEUserPtr const user, char* value);
    void abeUser_set_attributes(const ABEUserPtr const user, GSList* value);

    gboolean abeUser_set_values(const ABEUserPtr const user, char* valueType, char* value);
    char* abeUser_get_values(const ABEUserPtr const user, char* valueType);


#ifdef __cplusplus
}
#endif

#endif /* ABEUSER_H */

