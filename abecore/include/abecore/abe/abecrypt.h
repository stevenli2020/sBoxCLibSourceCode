/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include "pairing/pairinggroup.h"

#ifndef ABECRYPT_H
#define ABECRYPT_H

#ifdef __cplusplus
extern "C" {
#endif

    typedef struct _ABECrypt *ABECryptPtr;

    typedef struct _ABECrypt {
        void* derivedABECrypt;
    } ABECryptS;

    typedef struct _ABECrypt ABECrypt[1];

    ABECryptPtr new_ABECryptDef();
    void delete_ABECrypt(ABECryptPtr cryptPtr);


#ifdef __cplusplus
}
#endif

#endif /* ABECRYPT_H */

