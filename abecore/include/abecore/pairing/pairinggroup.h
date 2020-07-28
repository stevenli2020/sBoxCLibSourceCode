/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include "pbc.h"
#include "groupcontext.h"
#include "pairingtype.h"

#ifndef PAIRINGGROUP_H
#define PAIRINGGROUP_H

#define PAIRING_TYPE "PAIRING_TYPE"
#define PAIRING_CONFIG "PAIRING_CONFIG"

#ifdef __cplusplus
extern "C" {
#endif

    typedef struct _PairingGroup *PairingGroupPtr;

    typedef char* (*namePtr)(const PairingGroupPtr);
    typedef int (*setupPtr)(PairingGroupPtr, const GroupContextPtr);
    typedef int (*savePtr)(PairingGroupPtr, const GroupContextPtr);
    typedef int (*loadPtr)(PairingGroupPtr, const GroupContextPtr, GByteArray *);
    typedef void (*deletePtr)(PairingGroupPtr);

    typedef struct _PairingGroup {
        void* derivedPairing;
        char* groupName;
        char* pairingDesc;
        pairing_t pairing;
        namePtr getGroupName;
        setupPtr setup;
        savePtr save;
        loadPtr load;
        deletePtr clear;
    } PairingGroupS;
    typedef struct _PairingGroup PairingGroup[1];

    PairingGroupPtr new_PairingGroup(char* groupName);
    void delete_PairingGroup(PairingGroupPtr group);
    char* pairingGroup_get_groupname(const PairingGroupPtr const group);

    int pairingGroup_setup(PairingGroupPtr const group, const GroupContextPtr const context);
    int pairingGroup_save(PairingGroupPtr const group, const GroupContextPtr const context);
    int pairingGroup_load(PairingGroupPtr const group, const GroupContextPtr const context, GByteArray * config);
    void pairingGroup_printContext(const GroupContextPtr const context);
#ifdef __cplusplus
}
#endif

#endif /* PAIRINGGROUP_H */

