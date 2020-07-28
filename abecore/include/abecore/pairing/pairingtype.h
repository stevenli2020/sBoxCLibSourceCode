/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */



#ifndef PAIRINGTYPE_H
#define PAIRINGTYPE_H

#ifdef __cplusplus
extern "C" {
#endif
#define PAIRING_TYPE_VAL(num,name) num
#include "pairingtypedef.h"

    typedef enum _PairingTypes {
        PAIRING_TYPE_VALUES
#undef PAIRING_TYPE_VAL
    } PairingTypes;


#define PAIRING_TYPE_VAL(num,name) name
#include "pairingtypedef.h"
    static char* PAIRING_NAMES[] = {
        PAIRING_TYPE_VALUES
#undef PAIRING_TYPE_VAL
    };

#ifdef __cplusplus
}
#endif

#endif /* PAIRINGTYPE_H */

