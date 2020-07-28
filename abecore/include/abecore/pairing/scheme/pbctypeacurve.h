/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include "pbcdefinition.h"
#ifndef TYPEAPAIRING_H
#define TYPEAPAIRING_H

#ifdef __cplusplus
extern "C" {
#endif

#define TYPE_A_PARAMS "a.properties"

    typedef struct _TypeAPairing *TypeAPairingPtr;

    typedef struct _TypeAPairing {
        PairingGroupPtr parentPairing;
        int rbits;
        int qbits;
        namePtr getGroupName;
        setupPtr setup;
        savePtr save;
        loadPtr load;
        deletePtr clear;
    } TypeAPairingS;
    typedef struct _TypeAPairing TypeAPairing[1];

    PairingGroupPtr new_TypeAPairing(char* groupName);
    void delete_TypeAPairing(PairingGroupPtr group);
    char* typeAPairing_get_groupname(const PairingGroupPtr const group);
    int typeAPairing_setup(PairingGroupPtr const group, const GroupContextPtr const context);
    int typeAPairing_save(PairingGroupPtr const group, const GroupContextPtr const context);
    int typeAPairing_load(PairingGroupPtr const group, const GroupContextPtr const context, GByteArray *);

    TypeAPairingPtr typeA_getObject(PairingGroupPtr const group);


#ifdef __cplusplus
}
#endif

#endif /* TYPEAPAIRING_H */

