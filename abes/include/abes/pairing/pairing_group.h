/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#ifndef PAIRINGGROUP_H
#define PAIRINGGROUP_H

#define PAIRING_TYPE "PAIRING_TYPE"
#define PAIRING_CONFIG "PAIRING_CONFIG"

// #include "glib.h"
// #include "pbc.h"
// #include "group_context.h"

#ifdef __cplusplus
extern "C" {
#endif


    typedef struct _PairingGroup *PairingGroupPtr;

    typedef int (*setupPtr)(PairingGroupPtr, const GroupContextPtr);
    typedef GByteArray* (*serializePtr)(PairingGroupPtr);
    typedef int (*unserializePtr)(PairingGroupPtr, GByteArray *);
    typedef int (*savePtr)(PairingGroupPtr, char*);
    typedef int (*loadPtr)(PairingGroupPtr, const char*);
    typedef void (*deletePtr)(PairingGroupPtr);

    typedef struct _PairingGroup {
        char* groupName;
        char* pairingDesc;
        pairing_t pairing;
        setupPtr setup;
        serializePtr serialize;
        savePtr save;
        unserializePtr unserialize;
        loadPtr load;
        deletePtr clear;
    } PairingGroup;

    PairingGroupPtr new_PairingGroup(char* groupName);
    PairingGroupPtr new_PairingGroup2(char* groupName, char* desc);
    void delete_PairingGroup(PairingGroupPtr group);

    int pairingGroup_setup(PairingGroupPtr group, const GroupContextPtr const context);
    GByteArray* pairingGroup_serialize(PairingGroupPtr const group);
    int pairingGroup_unserialize(PairingGroupPtr group, GByteArray * config);
    void pairingGroup_printContext(const GroupContextPtr const context);
    int pairingGroup_save(PairingGroupPtr const group, char* path);
    int pairingGroup_load(PairingGroupPtr group, const char* path);
#ifdef __cplusplus
}
#endif

#endif /* PAIRINGGROUP_H */

