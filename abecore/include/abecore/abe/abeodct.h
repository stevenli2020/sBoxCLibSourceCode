/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include "glib.h"
#include "abect.h"


#ifndef ABEODCT_H
#define ABEODCT_H

#ifdef __cplusplus
extern "C" {
#endif

    typedef struct _CipherTextOD {
        CipherTextPtr baseCT;
        GByteArray* trans;
        ctSerializePtr serialize;
        ctUnserializePtr unserialize;
    } CipherTextODS;

    typedef struct _CipherTextOD *CipherTextODPtr;
    typedef struct _CipherTextOD CipherTextOD[1];

    CipherTextPtr new_CipherTextOD(CipherTextPtr baseCT, GByteArray* data);
    void delete_CipherTextOD(CipherTextPtr ctPtr);

#ifdef __cplusplus
}
#endif

#endif /* ABEODCT_H */

