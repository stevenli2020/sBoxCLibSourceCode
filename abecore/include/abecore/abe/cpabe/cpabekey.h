/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include "abe/abekey.h"

#ifndef CPABEKEY_H
#define CPABEKEY_H

#ifdef __cplusplus
extern "C" {
#endif

    MasterPKPtr cpabeMPK_loadMPK(char* filename);
    MasterSKPtr cpabeMSK_loadMSK(pairing_ptr pairing, char* filename);

#ifdef __cplusplus
}
#endif

#endif /* CPABEKEY_H */