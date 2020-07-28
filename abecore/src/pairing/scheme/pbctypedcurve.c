/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include "util/abeutil.h"
#include "pairing/pairinggroup.h"
#include "pairing/scheme/pbctypedcurve.h"

PairingGroupPtr new_TypeDPairing(char* groupName) {
    if (groupName == NULL)
        return NULL;
    TypeDPairingPtr typeDPairing = malloc(sizeof (TypeDPairing));
    PairingGroupPtr pairingGroup = new_PairingGroup(groupName);
    typeDPairing->getGroupName = typeDPairing_get_groupname;
    typeDPairing->setup = typeDPairing_setup;
    typeDPairing->load = typeDPairing_load;
    typeDPairing->save = typeDPairing_save;
    pairingGroup->setup = typeDPairing_setup;
    pairingGroup->load = typeDPairing_load;
    pairingGroup->save = typeDPairing_save;
    typeDPairing->parentPairing = pairingGroup;
    pairingGroup->derivedPairing = typeDPairing;
    pairingGroup->clear = delete_TypeDPairing;
    typeDPairing->clear = delete_TypeDPairing;

    return pairingGroup;
}

void delete_TypeDPairing(PairingGroupPtr group) {
    TypeDPairingPtr typeD = typeD_getObject(group);
    delete_PairingGroup(group);
    if (typeD){
        typeD->parentPairing = NULL;
        typeD->dvalue = 0;
        typeD->bitlimit = 0;
        free(typeD);
    }
}

char* typeDPairing_get_groupname(const PairingGroupPtr const group) {
    TypeDPairingPtr typeD = typeD_getObject(group);
    if (!typeD)
        return group->getGroupName(group);
    return typeD->parentPairing->getGroupName(typeD->parentPairing);
}

int typeD_generate(pbc_cm_t cm, void *data) {
    UNUSED_VAR(data);
    pbc_param_t param;
    //pbc_info("gendparam: computing Hilbert polynomial and finding roots...");
    pbc_param_init_d_gen(param, cm);
    //pbc_info("gendparam: bits in q = %zu\n", mpz_sizeinbase(cm->q, 2));
    write_pairing_desc(param, TYPE_D_PARAMS);
    return 1;
}

int typeDPairing_setup(PairingGroupPtr const group, const GroupContextPtr const context) {
    TypeDPairingPtr typeD = typeD_getObject(group);
    if (!typeD || !context)
        return 0;
    char* dvalue = context->getParameter(context, TYPE_D_DVALUE);
    char* bitlimit = context->getParameter(context, TYPE_D_BITLIMIT);
    if (dvalue == NULL || bitlimit == NULL) {
        return 0;
    }
    if (is_numeric(dvalue)) {
        typeD->dvalue = atoi(dvalue);
    }
    if (is_numeric(bitlimit)) {
        typeD->bitlimit = atoi(bitlimit);
    }
    pbc_cm_search_d(typeD_generate, TYPE_D_PARAMS, typeD->dvalue, typeD->bitlimit);
    char* pairDesc = read_pairing_desc(TYPE_D_PARAMS, 1);
    if (pairDesc) {
        pairing_init_set_buf(typeD->parentPairing->pairing, pairDesc, strlen(pairDesc));
        typeD->parentPairing->pairingDesc = pairDesc;
        char* config = context->getParameter(context, PAIRING_CONFIG);
        if (config != NULL) {
            typeDPairing_save(group, context);
        }
        return 1;
    }
    return 0;
}

int typeDPairing_save(PairingGroupPtr const group, const GroupContextPtr const context) {
    TypeDPairingPtr typeD = typeD_getObject(group);
    if (!typeD || !context)
        return 0;
    char* configFile = context->getParameter(context, PAIRING_CONFIG);
    if (configFile == NULL) {
        configFile = TYPE_D_PARAMS;
    }
    int size = sizeof (char)* strlen(configFile) + 1;
    char* filename = malloc(size);
    memcpy(filename, configFile, size);
    GByteArray* config = g_byte_array_new();
    write_string(config, PAIRING_NAMES[TYPE_D]);
    write_int(config, typeD->dvalue);
    write_int(config, typeD->bitlimit);
    write_string(config, typeD->parentPairing->pairingDesc);
    write_bytes_to_file(filename, config, 0);
}

int typeDPairing_load(PairingGroupPtr const group, const GroupContextPtr const context, GByteArray * config) {
    TypeDPairingPtr typeD = typeD_getObject(group);
    if (!typeD || !context)
        return 0;
    if (config) {
        int offset = 0;
        char* type = read_string(config, &offset);
        if (strcmp(type, PAIRING_NAMES[TYPE_D]) != 0) {
            printf("Invalid config file !");
            return 0;
        }
        typeD->dvalue = read_int(config, &offset);
        typeD->bitlimit = read_int(config, &offset);
        char* pairDesc = read_string(config, &offset);
        if (pairing_init_set_buf(typeD->parentPairing->pairing, pairDesc, strlen(pairDesc)) == 1) {
            pbc_die("pairing init failed");
        }
        typeD->parentPairing->pairingDesc = pairDesc;
        return 1;
    }
    return 0;
}

TypeDPairingPtr typeD_getObject(PairingGroupPtr const group) {
    if (group && group->derivedPairing != NULL)
        return (TypeDPairingPtr) group->derivedPairing;
    return NULL;
}
