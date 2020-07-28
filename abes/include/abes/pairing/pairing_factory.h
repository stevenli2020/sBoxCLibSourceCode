/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */


#ifndef PAIRINGFACTORY_H
#define PAIRINGFACTORY_H

// #include "pairing_type.h"

#ifdef __cplusplus
extern "C" {
#endif

    typedef struct _PairingFactory *PairingFactoryPtr;

    typedef PairingGroupPtr (*ctPPtr)(const GroupContextPtr);
    typedef PairingGroupPtr (*ctSPtr)(PairingScheme scheme);
    typedef PairingGroupPtr (*lodPtr)(const char*);

    typedef struct _PairingFactory {
        ctPPtr createPairing;
        ctSPtr createScheme;
        lodPtr loadPairing;
    } PairingFactoryS;
    typedef struct _PairingFactory PairingFactory[1];

    PairingFactoryPtr new_PairingFactory();
    void delete_PairingFactory(PairingFactoryPtr group);
    PairingGroupPtr pairingFactory_create_pairing(const GroupContextPtr const context);
    PairingGroupPtr pairingFactory_create_scheme(PairingScheme scheme);
    PairingGroupPtr pairingFactory_load_pairing(const char* path);

#ifdef __cplusplus
}
#endif

#endif /* PAIRINGFACTORY_H */

