/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include <string.h>
#include "glib.h"
#include "pbc.h"
#include "pairing/pairing_type.h"
#include "pairing/group_context.h"
#include "pairing/pairing_group.h"
#include "pairing/pairing_factory.h"
#include "util/io.h"
#include "util/abe_util.h"

PairingFactoryPtr new_PairingFactory() {
    PairingFactoryPtr factory = malloc(sizeof (PairingFactory));
    factory->createPairing = pairingFactory_create_pairing;
    factory->createScheme = pairingFactory_create_scheme;
    factory->loadPairing = pairingFactory_load_pairing;
    return factory;
}

void delete_PairingFactory(PairingFactoryPtr factory) {
    if (factory)
        free(factory);
}

GByteArray* getConfig(const GroupContextPtr const context) {
    if (context == NULL) {
        printf("Invalid context !");
        return NULL;
    }
    char* configFile = context->getParameter(context, PAIRING_CONFIG);
    if (configFile != NULL) {
        GByteArray* config = read_bytes_from_file(configFile);
        free(configFile);
        if (config) {
            return config;
        }
    }
    return NULL;
}

PairingGroupPtr pairingFactory_load_pairing(const char* path){
    PairingGroupPtr pGroup = new_PairingGroup("");
    int res = pGroup->load(pGroup, path);
    if(res)
        return pGroup;    
    else 
        return NULL;
}

PairingGroupPtr pairingFactory_create_pairing(const GroupContextPtr const context) {
    if (!context)
        return NULL;
    GByteArray* config = getConfig(context);
    char* loadType = NULL;
    int load = 0;
    if (config) {
        int offset = 0;
        loadType = read_string(config, &offset);
        if (loadType)
            load = 1;
    }
    char* pairingType = context->getParameter(context, PAIRING_TYPE);
    if (pairingType) {
        if (loadType && strcmp(pairingType, loadType) != 0) {
            printf("Invalid type specified !");
            free(loadType);
            return NULL;
        }
    } else {
        if (loadType)
            pairingType = loadType;
    }
    PairingGroupPtr pGroup = NULL;
    pGroup = new_PairingGroup(pairingType);
    if (load)
        pGroup->unserialize(pGroup, config);
    else
        pGroup->setup(pGroup, context);
    if (config)
        g_byte_array_free(config, 1);
    free(pairingType);
    return pGroup;
}

PairingGroupPtr pairingFactory_create_scheme(PairingScheme scheme) {
    PairingGroupPtr pGroup = NULL;
    GroupContextPtr context;
    GHashTable* t = g_hash_table_new(g_str_hash, g_str_equal);
    if (scheme == SS512) {
        pGroup = new_PairingGroup("SS512");
        g_hash_table_insert(t, TYPE_A_QBITS, "512");
        g_hash_table_insert(t, TYPE_A_RBITS, "160");
        g_hash_table_insert(t, PAIRING_TYPE, PAIRING_NAMES[TYPE_A]);
    } else if (scheme == MNT224) {
        pGroup = new_PairingGroup("MNT159");
        g_hash_table_insert(t, TYPE_D_DVALUE, "62003");
        g_hash_table_insert(t, TYPE_D_BITLIMIT, "200");
        g_hash_table_insert(t, PAIRING_TYPE, PAIRING_NAMES[TYPE_D]);
    } else if (scheme == MNT224) {
        pGroup = new_PairingGroup("MNT202");
        g_hash_table_insert(t, TYPE_D_DVALUE, "621771");
        g_hash_table_insert(t, TYPE_D_BITLIMIT, "250");
        g_hash_table_insert(t, PAIRING_TYPE, PAIRING_NAMES[TYPE_D]);
    } else if (scheme == MNT224) {
        pGroup = new_PairingGroup("MNT224");
        g_hash_table_insert(t, TYPE_D_DVALUE, "496659");
        g_hash_table_insert(t, TYPE_D_BITLIMIT, "240");
        g_hash_table_insert(t, PAIRING_TYPE, PAIRING_NAMES[TYPE_D]);
    } else if (scheme == MNT224) {
        pGroup = new_PairingGroup("MNT359");
        g_hash_table_insert(t, TYPE_D_DVALUE, "481843");
        g_hash_table_insert(t, TYPE_D_BITLIMIT, "400");
        g_hash_table_insert(t, PAIRING_TYPE, PAIRING_NAMES[TYPE_D]);
    } else if (scheme == BN160) {
        pGroup = new_PairingGroup("BN160");
        g_hash_table_insert(t, TYPE_F_BITS, "160");
        g_hash_table_insert(t, PAIRING_TYPE, PAIRING_NAMES[TYPE_F]);
    } else if (scheme == BN256) {
        pGroup = new_PairingGroup("BN256");
        g_hash_table_insert(t, TYPE_F_BITS, "256");
        g_hash_table_insert(t, PAIRING_TYPE, PAIRING_NAMES[TYPE_F]);
    }else if (scheme == BN512) {
        pGroup = new_PairingGroup("BN512");
        g_hash_table_insert(t, TYPE_F_BITS, "512");
        g_hash_table_insert(t, PAIRING_TYPE, PAIRING_NAMES[TYPE_F]);
    }else{
        pGroup = new_PairingGroup("SS512");
        g_hash_table_insert(t, TYPE_A_QBITS, "512");
        g_hash_table_insert(t, TYPE_A_RBITS, "160");
        g_hash_table_insert(t, PAIRING_TYPE, PAIRING_NAMES[TYPE_A]);
    }
    if (pGroup) {
        context = new_GroupContext(t);
        int res = pGroup->setup(pGroup, context);
        if(res)
            return pGroup;
    }
    return NULL;
}
