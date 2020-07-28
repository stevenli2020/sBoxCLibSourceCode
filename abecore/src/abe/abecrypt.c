/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include "stdlib.h"
#include "abe/abecrypt.h"

ABECryptPtr new_ABECryptDef() {
    ABECryptPtr crypt = malloc(sizeof (ABECrypt));
    crypt->derivedABECrypt = NULL;
    return crypt;
}

void delete_ABECrypt(ABECryptPtr cryptPtr) {
    if (!cryptPtr)
        return;
    //delete_PairingGroup(cryptPtr->pairingGrp);
    free(cryptPtr);
}

