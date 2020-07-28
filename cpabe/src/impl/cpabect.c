/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include "stdlib.h"
#include "abe/abect.h"
#include "util/abeutil.h"

CipherTextPtr new_CipherTextDef() {
    CipherTextPtr ctPtr = malloc(sizeof (CipherText));
    ctPtr->derivedCT = NULL;
    return ctPtr;
}

CipherTextPtr new_CipherText(GByteArray* data) {
    CipherTextPtr ctPtr = new_CipherTextDef();
    ctPtr->type = CT_ABE;
    ctPtr->data = data;
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

GByteArray* cpabeCT_serializeCT(const CipherTextPtr ciphertext) {
    GByteArray* data = g_byte_array_new();
    write_int(data, CT_ABE);
    write_byte_array(data, ciphertext->data);
    return data;
}

CipherTextPtr cpabeCT_unserializeCT(GByteArray* ciphertext) {
    int offset = 0;
    int type = read_int(ciphertext, &offset);
    if (type != CT_ABE)
        return NULL;
    GByteArray* ct = g_byte_array_new();
    read_byte_array(ciphertext, &offset, &ct);
    return new_CipherText(ct);
}
