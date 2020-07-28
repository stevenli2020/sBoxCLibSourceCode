/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include "abe/abect.h"
#include "stdlib.h"

CipherTextPtr new_CipherTextDef() {
    CipherTextPtr ctPtr = malloc(sizeof (CipherText));
    ctPtr->derivedCT = NULL;
    ctPtr->type = CT_ABE;
    return ctPtr;
}

void delete_CipherText(CipherTextPtr ctPtr) {
    if (ctPtr == NULL)
        return;
    if (ctPtr->data){
        g_byte_array_free(ctPtr->data, 1);
    }
    free(ctPtr);
}
