/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include "util/abeutil.h"
#include "pairing/pairinggroup.h"
#include "pairing/scheme/pbctypeacurve.h"
#include "pairing/pairingtype.h"

PairingGroupPtr new_TypeAPairing(char* groupName) {
    if (groupName == NULL)
        return NULL;
    TypeAPairingPtr typeAPairing = malloc(sizeof (TypeAPairing));
    PairingGroupPtr pairingGroup = new_PairingGroup(groupName);
    typeAPairing->getGroupName = typeAPairing_get_groupname;
    typeAPairing->setup = typeAPairing_setup;
    typeAPairing->load = typeAPairing_load;
    typeAPairing->save = typeAPairing_save;
    pairingGroup->setup = typeAPairing_setup;
    pairingGroup->load = typeAPairing_load;
    pairingGroup->save = typeAPairing_save;
    typeAPairing->parentPairing = pairingGroup;
    pairingGroup->derivedPairing = typeAPairing;
    pairingGroup->clear = delete_TypeAPairing;
    typeAPairing->clear = delete_TypeAPairing;
    return pairingGroup;
}

void delete_TypeAPairing(PairingGroupPtr group) {
    TypeAPairingPtr typeA = typeA_getObject(group);
    delete_PairingGroup(group);
   if (typeA){
        typeA->parentPairing = NULL;
        typeA->rbits = 0;
        typeA->qbits = 0;
        free(typeA);
   }
}

char* typeAPairing_get_groupname(const PairingGroupPtr const group) {
    TypeAPairingPtr typeA = typeA_getObject(group);
    if (!typeA)
        return group->getGroupName(group);
    return typeA->parentPairing->getGroupName(typeA->parentPairing);
}

int typeAPairing_setup(PairingGroupPtr const group, const GroupContextPtr const context) {
    TypeAPairingPtr typeA = typeA_getObject(group);
    if (!typeA || !context)
        return 0;
    char* qbits = context->getParameter(context, TYPE_A_QBITS);
    char* rbits = context->getParameter(context, TYPE_A_RBITS);
    if (qbits == NULL || rbits == NULL) {
        return 0;
    }
    if (is_numeric(qbits)) {
        typeA->qbits = atoi(qbits);
    }
    if (is_numeric(rbits)) {
        typeA->rbits = atoi(rbits);
    }
    pbc_param_ptr para = malloc(sizeof (pbc_param_t));
    pbc_param_init_a_gen(para, typeA->rbits, typeA->qbits);
    if (para) {
        char* pairDesc = get_pairing_desc(para);
        pairing_init_pbc_param(typeA->parentPairing->pairing, para);
        typeA->parentPairing->pairingDesc = pairDesc;
        char* config = context->getParameter(context, PAIRING_CONFIG);
        if (config != NULL) {
            typeAPairing_save(group, context);
        }
        return 1;
    }
    return 0;
}

int typeAPairing_save(PairingGroupPtr const group, const GroupContextPtr const context) {
    TypeAPairingPtr typeA = typeA_getObject(group);
    if (!typeA || !context)
        return 0;
    char* configFile = context->getParameter(context, PAIRING_CONFIG);
    if (configFile == NULL) {
        configFile = TYPE_A_PARAMS;
    }
    int size = sizeof (char)* strlen(configFile) + 1;
    char* filename = malloc(size);
    memcpy(filename, configFile, size);
    GByteArray* config = g_byte_array_new();
    write_string(config, PAIRING_NAMES[TYPE_A]);
    write_int(config, typeA->rbits);
    write_int(config, typeA->qbits);
    write_string(config, typeA->parentPairing->pairingDesc);
    write_bytes_to_file(filename, config, 0);
}

int typeAPairing_load(PairingGroupPtr const group, const GroupContextPtr const context, GByteArray * config) {
    TypeAPairingPtr typeA = typeA_getObject(group);
    if (!typeA || !context || !config)
        return 0;
    if (config) {
        int offset = 0;
        char* type = read_string(config, &offset);
        if (strcmp(type, PAIRING_NAMES[TYPE_A]) != 0) {
            printf("Invalid config file !");
            return 0;
        }
        typeA->rbits = read_int(config, &offset);
        typeA->qbits = read_int(config, &offset);
        char* pairDesc = read_string(config, &offset);
        if (pairing_init_set_buf(typeA->parentPairing->pairing, pairDesc, strlen(pairDesc)) == 1) {
            pbc_die("pairing init failed");
        }
        typeA->parentPairing->pairingDesc = pairDesc;
        return 1;
    }
    return 0;
}

TypeAPairingPtr typeA_getObject(PairingGroupPtr const group) {
    if (group && group->derivedPairing != NULL)
        return (TypeAPairingPtr) group->derivedPairing;
    return NULL;
}