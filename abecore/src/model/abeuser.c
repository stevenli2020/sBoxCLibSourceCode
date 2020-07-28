/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */


#include  "glib.h"
#include "stdlib.h"
#include "string.h"
#include "model/abeuser.h"

ABEUserPtr new_ABEUser(char* uid, char* displayName, char* username, char* password, char* pk, char* tk, GSList* attributes) {
    if (uid == NULL || username == NULL)
        return NULL;
    ABEUserPtr user = malloc(sizeof (ABEUser));
    user->keyPair = NULL;
    user->values = g_hash_table_new(g_str_hash, g_str_equal);
    user->attributes = attributes;
    user->getID = abeUser_get_id;
    user->getDisplayName = abeUser_get_displayname;
    user->getUsername = abeUser_get_username;
    user->getPassword = abeUser_get_password;
    user->getPK = abeUser_get_pk;
    user->getTK = abeUser_get_tk;
    user->getAttributes = abeUser_get_attributes;
    user->setID = abeUser_set_id;
    user->setDisplayName = abeUser_set_displayname;
    user->setUsername = abeUser_set_username;
    user->setPassword = abeUser_set_password;
    user->setPK = abeUser_set_pk;
    user->setTK = abeUser_set_tk;
    user->setAttributes = abeUser_set_attributes;
    abeUser_set_id(user, uid);
    abeUser_set_displayname(user, displayName);
    abeUser_set_username(user, username);
    abeUser_set_password(user, password);
    abeUser_set_pk(user, pk);
    abeUser_set_tk(user, tk);
    abeUser_set_attributes(user, attributes);
    return user;
}

void delete_ABEUser(ABEUserPtr user) {
    if (user) {
        //        GList* keys = g_hash_table_get_keys(user->values);
        //        int size = g_list_length(keys);
        //        char* value;
        //        for (int i = 0; i < size; i++) {
        //            char* key = g_list_nth(keys, i)->data;
        //            if (g_hash_table_contains(user->values, key)) {
        //                value = g_hash_table_lookup(user->values, key);
        //                if (value)
        //                    free(value);
        //            }
        //        }
        //        g_list_free(keys);
        g_hash_table_remove_all(user->values);
        g_hash_table_destroy(user->values);
        //        if (user->attributes)
        //            g_slist_free(user->attributes);
        free(user);
    }
}

char* abeUser_get_id(const ABEUserPtr const user) {
    return abeUser_get_values(user, ABEUserValues[ABE_USER_ID]);
}

char* abeUser_get_displayname(const ABEUserPtr const user) {
    return abeUser_get_values(user, ABEUserValues[ABE_USER_SN]);
}

char* abeUser_get_username(const ABEUserPtr const user) {
    return abeUser_get_values(user, ABEUserValues[ABE_USER_CN]);
}

char* abeUser_get_password(const ABEUserPtr const user) {
    return abeUser_get_values(user, ABEUserValues[ABE_USER_PWD]);
}

char* abeUser_get_pk(const ABEUserPtr const user) {
    return abeUser_get_values(user, ABEUserValues[ABE_USER_PK]);
}

char* abeUser_get_tk(const ABEUserPtr const user) {
    return abeUser_get_values(user, ABEUserValues[ABE_USER_TK]);
}

char** abeUser_get_attributes(const ABEUserPtr const user) {
    GSList* ap;
    int n = g_slist_length(user->attributes);
    char** attrs = malloc((n + 1) * sizeof (char*));
    int z = 0;
    for (int i = 0; i < n; i++) {
        ap = g_slist_nth(user->attributes, i);
        attrs[z++] = strdup(ap->data);
    }
    attrs[z] = 0;
    return attrs;
}

void abeUser_set_id(const ABEUserPtr const user, char* value) {
    abeUser_set_values(user, ABEUserValues[ABE_USER_ID], value);
}

void abeUser_set_displayname(const ABEUserPtr const user, char* value) {
    abeUser_set_values(user, ABEUserValues[ABE_USER_SN], value);
}

void abeUser_set_username(const ABEUserPtr const user, char* value) {
    abeUser_set_values(user, ABEUserValues[ABE_USER_CN], value);
}

void abeUser_set_password(const ABEUserPtr const user, char* value) {
    abeUser_set_values(user, ABEUserValues[ABE_USER_PWD], value);
}

void abeUser_set_pk(const ABEUserPtr const user, char* value) {
    abeUser_set_values(user, ABEUserValues[ABE_USER_PK], value);
}

void abeUser_set_tk(const ABEUserPtr const user, char* value) {
    abeUser_set_values(user, ABEUserValues[ABE_USER_TK], value);
}

void abeUser_set_attributes(const ABEUserPtr const user, GSList* value) {
    user->attributes = value;
}

gboolean abeUser_set_values(const ABEUserPtr const user, char* valueType, char* value) {
    return g_hash_table_replace(user->values, valueType, value);
}

char* abeUser_get_values(const ABEUserPtr const user, char* valueType) {
    if (g_hash_table_contains(user->values, valueType)) {
        return g_hash_table_lookup(user->values, valueType);
    }
    return NULL;
}