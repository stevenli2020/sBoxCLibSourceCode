/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */


#ifndef PAIRINGFACTORY_H
#define PAIRINGFACTORY_H



#ifdef __cplusplus
extern "C" {
#endif
#include "pairing/scheme/pbcdefinition.h"
#include "pairingscheme.h"

    typedef struct _PairingFactory *PairingFactoryPtr;

    typedef PairingGroupPtr(*createPairingPtr)(const GroupContextPtr);
    typedef PairingGroupPtr(*createSchemePtr)(PairingScheme scheme);

    typedef struct _PairingFactory {
        createPairingPtr createPairing;
        createSchemePtr createScheme;
    } PairingFactoryS;
    typedef struct _PairingFactory PairingFactory[1];

    PairingFactoryPtr new_PairingFactory();
    void delete_PairingFactory(PairingFactoryPtr group);
    PairingGroupPtr pairingFactory_create_pairing(const GroupContextPtr const context);
    PairingGroupPtr pairingFactory_create_scheme(PairingScheme scheme);
   // void loadOrSetup(PairingGroupPtr pGroup, const GroupContextPtr const context);

#ifdef __cplusplus
}
#endif

#endif /* PAIRINGFACTORY_H */

