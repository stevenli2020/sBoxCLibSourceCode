/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "pbcdefinition.h"

#ifndef TYPEDPAIRING_H
#define TYPEDPAIRING_H

#ifdef __cplusplus
extern "C" {
#endif
#define TYPE_D_PARAMS "d.properties"

    typedef struct _TypeDPairing *TypeDPairingPtr;

    typedef struct _TypeDPairing {
        PairingGroupPtr parentPairing;
        int dvalue;
        int bitlimit;
        namePtr getGroupName;
        setupPtr setup;
        savePtr save;
        loadPtr load;
        deletePtr clear;
    } TypeDPairingS;
    typedef struct _TypeDPairing TypeDPairing[1];

    PairingGroupPtr new_TypeDPairing(char* groupName);
    void delete_TypeDPairing(PairingGroupPtr group);
    char* typeDPairing_get_groupname(const PairingGroupPtr const group);
    int typeD_generate(pbc_cm_t cm, void *data);
    int typeDPairing_setup(PairingGroupPtr const group, const GroupContextPtr const context);
    int typeDPairing_save(PairingGroupPtr const group, const GroupContextPtr const context);
    int typeDPairing_load(PairingGroupPtr const group, const GroupContextPtr const context, GByteArray *);

    TypeDPairingPtr typeD_getObject(PairingGroupPtr const group);




#ifdef __cplusplus
}
#endif

#endif /* TYPEDPAIRING_H */

