/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include "abecrypt.h"


#ifndef ABEODCRYPT_H
#define ABEODCRYPT_H

#ifdef __cplusplus
extern "C" {
#endif

    typedef struct _ABEODCrypt {
        void* derivedABEODCrypt;
        void* baseABEODCrypt;
    } ABEODCryptS;

    typedef struct _ABEODCrypt ABEODCrypt[1];

    ABECryptPtr new_ABEODCryptDef();
    void delete_ABEODCrypt(ABECryptPtr cryptPtr);

#ifdef __cplusplus
}
#endif

#endif /* ABEODCRYPT_H */

