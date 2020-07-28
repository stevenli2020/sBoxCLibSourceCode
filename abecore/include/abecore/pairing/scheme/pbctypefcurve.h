/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include "pbcdefinition.h"


#ifndef TYPEFPAIRING_H
#define TYPEFPAIRING_H

#ifdef __cplusplus
extern "C" {
#endif


#define TYPE_F_PARAMS "f.properties"


    typedef struct _TypeFPairing *TypeFPairingPtr;

    typedef struct _TypeFPairing {
        PairingGroupPtr parentPairing;
        int bits;
        namePtr getGroupName;
        setupPtr setup;
        savePtr save;
        loadPtr load;
        deletePtr clear;
    } TypeFPairingS;
    typedef struct _TypeFPairing TypeFPairing[1];

    PairingGroupPtr new_TypeFPairing(char* groupName);
    void delete_TypeFPairing(PairingGroupPtr group);
    char* typeFPairing_get_groupname(const PairingGroupPtr const group);
    int typeF_generate(pbc_cm_t cm, void *data);
    int typeFPairing_setup(PairingGroupPtr const group, const GroupContextPtr const context);
    int typeFPairing_save(PairingGroupPtr const group, const GroupContextPtr const context);
    int typeFPairing_load(PairingGroupPtr const group, const GroupContextPtr const context, GByteArray *);

    TypeFPairingPtr typeF_getObject(PairingGroupPtr const group);


#ifdef __cplusplus
}
#endif

#endif /* TYPEFPAIRING_H */

