/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include <stdlib.h>
#include "abe/abeodct.h"

CipherTextPtr new_CipherTextOD(CipherTextPtr baseCT, GByteArray* data) {
    CipherTextODPtr ctPtr = malloc(sizeof (CipherTextOD));
    baseCT->type = CT_ABE_TRANS;
    ctPtr->trans = data;
    ctPtr->baseCT = baseCT;
    baseCT->derivedCT = ctPtr;
    return baseCT;
}

void delete_CipherTextOD(CipherTextPtr ctPtr) {
    if (ctPtr == NULL)
        return;
    if (ctPtr->derivedCT) {
        CipherTextODPtr ctodptr = (CipherTextODPtr) ctPtr->derivedCT;
        g_byte_array_free(ctodptr->trans, 1);
        free(ctodptr);
    }
    delete_CipherText(ctPtr);
}
