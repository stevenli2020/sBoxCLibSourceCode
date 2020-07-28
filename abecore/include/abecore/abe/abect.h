/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include  "glib.h"

#ifndef ABECT_H
#define ABECT_H


#ifdef __cplusplus
extern "C" {
#endif

    const static int CT_ABE = 1;
    const static int CT_ABE_TRANS = 2;

    typedef struct _CipherText *CipherTextPtr;
    typedef GByteArray* (*ctSerializePtr)(const CipherTextPtr);
    typedef CipherTextPtr(*ctUnserializePtr)(GByteArray*);

    typedef struct _CipherText {
        int type;
        GByteArray* data;
        void *derivedCT;
        ctSerializePtr serialize;
        ctUnserializePtr unserialize;
    } CipherTextS;

    typedef struct _CipherText CipherText[1];
  //  CipherTextPtr new_CipherTextDef();
    CipherTextPtr new_CipherText(GByteArray* data);
    void delete_CipherText(CipherTextPtr ctPtr);

#ifdef __cplusplus
}
#endif

#endif /* ABECT_H */

