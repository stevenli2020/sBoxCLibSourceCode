
/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "util/abeutil.h"
#include "pairing/pairinggroup.h"
#include "pairing/scheme/pbctypefcurve.h"

PairingGroupPtr new_TypeFPairing(char* groupName) {
    if (groupName == NULL)
        return NULL;
    TypeFPairingPtr typeFPairing = malloc(sizeof (TypeFPairing));
    PairingGroupPtr pairingGroup = new_PairingGroup(groupName);
    typeFPairing->getGroupName = typeFPairing_get_groupname;
    typeFPairing->setup = typeFPairing_setup;
    typeFPairing->load = typeFPairing_load;
    typeFPairing->save = typeFPairing_save;
    pairingGroup->setup = typeFPairing_setup;
    pairingGroup->load = typeFPairing_load;
    pairingGroup->save = typeFPairing_save;
    typeFPairing->parentPairing = pairingGroup;
    pairingGroup->derivedPairing = typeFPairing;
    pairingGroup->clear = delete_TypeFPairing;
    typeFPairing->clear = delete_TypeFPairing;
    return pairingGroup;
}

void delete_TypeFPairing(PairingGroupPtr group) {
    TypeFPairingPtr typeF = typeF_getObject(group);
    delete_PairingGroup(group);
    if (typeF){
        typeF->parentPairing = NULL;
        typeF->bits = 0;
        free(typeF);
    }
}

char* typeFPairing_get_groupname(const PairingGroupPtr const group) {
    TypeFPairingPtr typeF = typeF_getObject(group);
    if (!typeF)
        return group->getGroupName(group);
    return typeF->parentPairing->getGroupName(typeF->parentPairing);
}

int typeFPairing_setup(PairingGroupPtr const group, const GroupContextPtr const context) {
    TypeFPairingPtr typeF = typeF_getObject(group);
    if (!typeF || !context)
        return 0;
    char* bits = context->getParameter(context, TYPE_F_BITS);
    if (bits == NULL) {
        return 0;
    }
    if (is_numeric(bits)) {
        typeF->bits = atoi(bits);
    }
    pbc_param_ptr para = malloc(sizeof (pbc_param_t));
    pbc_param_init_f_gen(para, typeF->bits);
    if (para) {
        char* pairDesc = get_pairing_desc(para);
        pairing_init_pbc_param(typeF->parentPairing->pairing, para);
        typeF->parentPairing->pairingDesc = pairDesc;
        char* config = context->getParameter(context, PAIRING_CONFIG);
        if (config != NULL) {
            typeFPairing_save(group, context);
        }
        return 1;
    }
    return 0;
}

int typeFPairing_save(PairingGroupPtr const group, const GroupContextPtr const context) {
    TypeFPairingPtr typeF = typeF_getObject(group);
    if (!typeF || !context)
        return 0;
    char* configFile = context->getParameter(context, PAIRING_CONFIG);
    if (configFile == NULL) {
        configFile = TYPE_F_PARAMS;
    }
    int size = sizeof (char)* strlen(configFile) + 1;
    char* filename = malloc(size);
    memcpy(filename, configFile, size);
    GByteArray* config = g_byte_array_new();
    write_string(config, PAIRING_NAMES[TYPE_F]);
    write_int(config, typeF->bits);
    write_string(config, typeF->parentPairing->pairingDesc);
    write_bytes_to_file(filename, config, 0);
}

int typeFPairing_load(PairingGroupPtr const group, const GroupContextPtr const context, GByteArray * config) {
    TypeFPairingPtr typeF = typeF_getObject(group);
    if (!typeF || !context)
        return 0;

    if (config) {
        int offset = 0;
        char* type = read_string(config, &offset);
        if (strcmp(type, PAIRING_NAMES[TYPE_F]) != 0) {
            printf("Invalid config file !");
            return 0;
        }
        typeF->bits = read_int(config, &offset);
        char* pairDesc = read_string(config, &offset);
        if (pairing_init_set_buf(typeF->parentPairing->pairing, pairDesc, strlen(pairDesc)) == 1) {
            pbc_die("pairing init failed");
        }
        typeF->parentPairing->pairingDesc = pairDesc;
        free(type);
        return 1;
    }
    return 0;
}

TypeFPairingPtr typeF_getObject(PairingGroupPtr const group) {
    if (group && group->derivedPairing != NULL)
        return (TypeFPairingPtr) group->derivedPairing;
    return NULL;
}