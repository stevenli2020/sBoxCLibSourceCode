/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "abe/abeodct.h"
#include "cpabect.h"

#ifndef CPABEODCT_H
#define CPABEODCT_H

#ifdef __cplusplus
extern "C" {
#endif
    GByteArray* cpabeodCT_serializeCT(const CipherTextPtr ciphertext);
    CipherTextPtr cpabeodCT_unserializeCT(GByteArray* ciphertext);
    CipherTextODPtr cpabeodCT_getObject(CipherTextPtr const ct);
#ifdef __cplusplus
}
#endif

#endif /* CPABEODCT_H */

