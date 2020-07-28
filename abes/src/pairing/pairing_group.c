/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "pbc.h"
#include "glib.h"
#include "pairing/pairing_type.h"
#include "pairing/group_context.h"
#include "pairing/pairing_group.h"
#include "util/io.h"
#include "util/common.h"
#include "util/abe_util.h"

PairingGroupPtr new_PairingGroup(char* groupName) {
    if (groupName == NULL)
        return NULL;
    PairingGroupPtr pairingGroup = malloc(sizeof (PairingGroup));
    int size = sizeof (char)* strlen(groupName) + 1;
    pairingGroup->groupName = malloc(size);
    memcpy(pairingGroup->groupName, groupName, size);
    pairingGroup->setup = pairingGroup_setup;
    pairingGroup->load = pairingGroup_load;
    pairingGroup->save = pairingGroup_save;
    pairingGroup->unserialize = pairingGroup_unserialize;
    pairingGroup->serialize = pairingGroup_serialize;
    pairingGroup->clear = delete_PairingGroup;
    return pairingGroup;
}

PairingGroupPtr new_PairingGroup2(char* groupName, char* desc) {
    if (groupName == NULL)
        return NULL;

    PairingGroupPtr pairingGroup = new_PairingGroup(groupName);
    int size = sizeof (char)* strlen(desc) + 1;
    pairingGroup->pairingDesc = malloc(size);
    memcpy(pairingGroup->pairingDesc, desc, size);
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
    free(group);
}

GByteArray* pairingGroup_serialize(PairingGroupPtr const group) {
    if (!group)
        return NULL;
    if (group->groupName == NULL || group->pairing == NULL || group->pairingDesc == NULL) {
        return NULL;
    }
    GByteArray* config = g_byte_array_new();
    write_string(config, group->groupName);
    write_string(config, group->pairingDesc);
    return config;
}

int pairingGroup_unserialize(PairingGroupPtr const group, GByteArray * config) {
    if (config) {
        int offset = 0;
        char* type = read_string(config, &offset);

        if (strcmp(type, PAIRING_NAMES[TYPE_A]) != 0 &&
            strcmp(type, PAIRING_NAMES[TYPE_D]) != 0 &&
            strcmp(type, PAIRING_NAMES[TYPE_F]) != 0) {
            printf("Invalid config file !");
            return 0;
        }

        char* pairDesc = read_string(config, &offset);
        if (pairing_init_set_buf(group->pairing, pairDesc, strlen(pairDesc)) == 1) {
            pbc_die("pairing init failed");
        }

        group->pairingDesc = pairDesc;
        group->groupName = type;
        return 1;
    }
    return 0;
}

int pairingGroup_save(PairingGroupPtr const group, char* path){
    if(group && path){
        GByteArray* config = group->serialize(group);
        if(config){
            return write_bytes_to_file_ex(path, config, 1, 1);
        }
    }
    return 0;
}

int pairingGroup_load(PairingGroupPtr const group, const char* path){
    if(group && path){
        GByteArray* config = read_bytes_from_file(path);
        if(config){
            int res = group->unserialize(group, config);
            g_byte_array_free(config, 1);
            return res;            
        }
    }
    return 0;

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


int gen_typeA_parameters(PairingGroupPtr const group, const GroupContextPtr const context) {
    if (!group || !context)
        return 0;
    group->groupName = PAIRING_NAMES[TYPE_A];
    char* cqbits = context->getParameter(context, TYPE_A_QBITS);
    char* crbits = context->getParameter(context, TYPE_A_RBITS);
    if (cqbits == NULL || crbits == NULL) {
        return 0;
    }
    int rbits = 0, qbits = 0;
    if (is_numeric(cqbits)) {
        qbits = atoi(cqbits);
    }else{
        printf("Invalid qbits number for TypeA!");
        return 0;
    }
    if (is_numeric(crbits)) {
        rbits = atoi(crbits);
    }else{
        printf("Invalid qbits number for TypeA!");
        return 0;
    }
    pbc_param_ptr para = malloc(sizeof (pbc_param_t));
    pbc_param_init_a_gen(para, rbits, qbits);
    if (para) {
        pairing_init_pbc_param(group->pairing, para);
        char* pairDesc = get_pairing_desc(para);
        group->pairingDesc = pairDesc;
        return 1;
    }
    return 0;
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

int gen_typeD_parameters(PairingGroupPtr const group, const GroupContextPtr const context) {
    if (!group || !context)
        return 0;
    group->groupName = PAIRING_NAMES[TYPE_D];
    char* cdvalue = context->getParameter(context, TYPE_D_DVALUE);
    char* cbitlimit = context->getParameter(context, TYPE_D_BITLIMIT);
    if (cdvalue == NULL || cbitlimit == NULL) {
        return 0;
    }
    int dvalue = 0, bitlimit = 0;
    if (is_numeric(cdvalue)) {
        dvalue = atoi(cdvalue);
    }else{
        printf("Invalid dvalue number for TypeD!");
        return 0;
    }
    if (is_numeric(cbitlimit)) {
        bitlimit = atoi(cbitlimit);
    }else{
        printf("Invalid bitlimit number for TypeD!");
        return 0;
    }
    pbc_cm_search_d(typeD_generate, TYPE_D_PARAMS, dvalue, bitlimit);
    char* pairDesc = read_pairing_desc(TYPE_D_PARAMS, 1);
    if (pairDesc) {
        pairing_init_set_buf(group->pairing, pairDesc, strlen(pairDesc));
        group->pairingDesc = pairDesc;
        return 1;
    }
    return 0;
}

int gen_typeF_parameters(PairingGroupPtr const group, const GroupContextPtr const context) {
    if (!group || !context)
        return 0;
    group->groupName = PAIRING_NAMES[TYPE_F];
    char* cbits = context->getParameter(context, TYPE_F_BITS);
    if (cbits == NULL) {
        return 0;
    }
    int bits = 0;
    if (is_numeric(cbits)) {
        bits = atoi(cbits);
    }else{
        printf("Invalid bits number for TypeF!");
        return 0;
    }
    pbc_param_ptr para = malloc(sizeof (pbc_param_t));
    pbc_param_init_f_gen(para, bits);
    if (para) {
        pairing_init_pbc_param(group->pairing, para);
        char* pairDesc = get_pairing_desc(para);
        group->pairingDesc = pairDesc;
        return 1;
    }
    return 0;
}

int pairingGroup_setup(PairingGroupPtr const group, const GroupContextPtr const context) {
    char* type = context->getParameter(context, PAIRING_TYPE);
    int res = 0;
    if (strcmp(type, PAIRING_NAMES[TYPE_A]) == 0){
        res = gen_typeA_parameters(group, context);
    }else if (strcmp(type, PAIRING_NAMES[TYPE_D]) == 0){
        res = gen_typeD_parameters(group, context);
    }else if (strcmp(type, PAIRING_NAMES[TYPE_F]) == 0) {
        res = gen_typeF_parameters(group, context);
    }else{
        printf("Invalid type name!");
        return 0;
    }
    if (res){
        char* path = context->getParameter(context, PAIRING_CONFIG);
        if(path)
            group->save(group, path);
    }
    return res;
}