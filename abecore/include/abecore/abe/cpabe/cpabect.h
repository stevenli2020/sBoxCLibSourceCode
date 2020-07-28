/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include "abe/abect.h"


#ifndef CPABECT_H
#define CPABECT_H

#ifdef __cplusplus
extern "C" {
#endif

    GByteArray* cpabeCT_serializeCT(const CipherTextPtr ciphertext);
    CipherTextPtr cpabeCT_unserializeCT(GByteArray* ciphertext);



#ifdef __cplusplus
}
#endif

#endif /* CPABECT_H */

