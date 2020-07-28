/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include <string.h>
#include "pairing/pairingscheme.h"

#include "pairing/pairinggroup.h"
#include "pairing/pairingfactory.h"
#include "pairing/scheme/pbctypeacurve.h"
#include "pairing/scheme/pbctypedcurve.h"
#include "pairing/scheme/pbctypefcurve.h"
#include "util/abeutil.h"

PairingFactoryPtr new_PairingFactory() {
    PairingFactoryPtr factory = malloc(sizeof (PairingFactory));
    factory->createPairing = pairingFactory_create_pairing;
    factory->createScheme = pairingFactory_create_scheme;
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
        //        int size = sizeof (char)* strlen(configFile) + 1;
        //        char* filename = malloc(size);
        //        memcpy(filename, configFile, size);
        GByteArray* config = read_bytes_from_file(configFile);
        free(configFile);
        if (config) {
            return config;
        }
    }
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
    if (strcmp(pairingType, PAIRING_NAMES[TYPE_A]) == 0) {
        pGroup = new_TypeAPairing(pairingType);
    } else if (strcmp(pairingType, PAIRING_NAMES[TYPE_D]) == 0) {
        pGroup = new_TypeDPairing(pairingType);
    } else if (strcmp(pairingType, PAIRING_NAMES[TYPE_F]) == 0) {
        pGroup = new_TypeFPairing(pairingType);
    }
    if (load)
        pGroup->load(pGroup, context, config);
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
        pGroup = new_TypeAPairing("SS512");
        g_hash_table_insert(t, TYPE_A_QBITS, "512");
        g_hash_table_insert(t, TYPE_A_RBITS, "160");
    } else if (scheme == MNT224) {
        pGroup = new_TypeDPairing("MNT224");
        g_hash_table_insert(t, TYPE_D_DVALUE, "496659");
        g_hash_table_insert(t, TYPE_D_BITLIMIT, "500");
    } else if (scheme == BN160) {
        pGroup = new_TypeFPairing("BN160");
        g_hash_table_insert(t, TYPE_F_BITS, "160");
    } else if (scheme == BN256) {
        pGroup = new_TypeFPairing("BN256");
        g_hash_table_insert(t, TYPE_F_BITS, "256");
    }
    if (pGroup) {
        context = new_GroupContext(t);
        pGroup->setup(pGroup, context);
    }
    return pGroup;
}
