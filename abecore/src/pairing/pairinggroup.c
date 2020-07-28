/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "pbc.h"
#include "pairing/pairinggroup.h"

PairingGroupPtr new_PairingGroup(char* groupName) {
    if (groupName == NULL)
        return NULL;
    PairingGroupPtr pairingGroup = malloc(sizeof (PairingGroup));
    int size = sizeof (char)* strlen(groupName) + 1;
    pairingGroup->groupName = malloc(size);
    memcpy(pairingGroup->groupName, groupName, size);
    pairingGroup->getGroupName = pairingGroup_get_groupname;
    pairingGroup->setup = pairingGroup_setup;
    pairingGroup->save = pairingGroup_save;
    pairingGroup->load = pairingGroup_load;
    return pairingGroup;
}

void delete_PairingGroup(PairingGroupPtr group) {
    if (!group)
        return;
    if (group->groupName){
        free(group->groupName);
        group->groupName = NULL;
    }
    if (group->pairing) {
        pairing_clear(group->pairing);
    }
    if (group->pairingDesc) {
        free(group->pairingDesc);
    }
//    if (group->derivedPairing) {
//        free(group->derivedPairing);
//    }
    group->derivedPairing = NULL;
    free(group);
}

char* pairingGroup_get_groupname(const PairingGroupPtr const group) {
    if (!group)
        return NULL;
    int size = sizeof (char)*strlen(group->groupName) + 1;
    char* temp = malloc(size);
    memcpy(temp, group->groupName, size);
    return temp;
}

int pairingGroup_setup(PairingGroupPtr const group, const GroupContextPtr const context) {
    pairingGroup_printContext(context);
    return 1;
}

int pairingGroup_save(PairingGroupPtr const group, const GroupContextPtr const context) {
    pairingGroup_printContext(context);
    return 1;
}

int pairingGroup_load(PairingGroupPtr const group, const GroupContextPtr const context, GByteArray * config) {
    pairingGroup_printContext(context);
    return 1;
}

void pairingGroup_printContext(const GroupContextPtr const context) {
    if (!context)
        return;
    GHashTable* t2 = context->getParameters(context);
    GList* keys = g_hash_table_get_keys(t2);
    GList* l;
    for (l = keys; l; l = l->next) {
        char* key = l->data;
        char* val = g_hash_table_lookup(t2, key);
        printf("%s:%s\n", key, val);
    }
}