/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include "stdlib.h"
#include "glib.h"
#include "abe/cpabe/cpabeodct.h"
#include "util/abeutil.h"

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

GByteArray* cpabeodCT_serializeCT(const CipherTextPtr ciphertext) {
    if (ciphertext->type == CT_ABE) {
        return cpabeCT_serializeCT(ciphertext);
    } else if (ciphertext->type == CT_ABE_TRANS) {
        CipherTextODPtr tCTPtr = cpabeodCT_getObject(ciphertext);
        GByteArray* data = g_byte_array_new();
        write_int(data, CT_ABE_TRANS);
        write_byte_array(data, tCTPtr->baseCT->data);
        write_byte_array(data, tCTPtr->trans);
        return data;
    }
    return NULL;
}

CipherTextPtr cpabeodCT_unserializeCT(GByteArray* ciphertext) {
    int offset = 0;
    int type = read_int(ciphertext, &offset);
    if (type == CT_ABE) {
        return cpabeCT_unserializeCT(ciphertext);
    } else if (type == CT_ABE_TRANS) {
        GByteArray* baseCT = g_byte_array_new();
        GByteArray* transCT = g_byte_array_new();
        read_byte_array(ciphertext, &offset, &baseCT);
        read_byte_array(ciphertext, &offset, &transCT);
        return new_CipherTextOD(new_CipherText(baseCT), transCT);
    }
    return NULL;
}

CipherTextODPtr cpabeodCT_getObject(CipherTextPtr const ct) {
    if (ct && ct->derivedCT != NULL) {
        return (CipherTextODPtr) ct->derivedCT;
    }
    return NULL;
}