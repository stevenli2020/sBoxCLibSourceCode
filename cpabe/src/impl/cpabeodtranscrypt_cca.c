/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include <stdlib.h>
#include <string.h>
#include "abe/cpabe/cpabeodtranscrypt.h"
#include "abe/cpabe/cpabeodtranscrypt_cca.h"
#include "object/ghwcpabeodkey.h"
#include "util/abeutil.h"
#include "ext/cpabeutil.h"

CPABEODTransCryptPtr new_CPABEOD_CCA_TransCryptDef() {
    CPABEODTransCryptPtr cpabeCrypt = new_CPABEODTransCryptDef();
    cpabeCrypt->transform = cpabeodTransCrypt_cca_transform;
    return cpabeCrypt;
}

void delete_CPABEOD_CCA_TransCrypt(CPABEODTransCryptPtr cryptPtr) {
    if (cryptPtr == NULL)
        return;
    delete_ABECrypt(cryptPtr->baseABECrypt);
    free(cryptPtr);
}

CipherTextPtr cpabeodTransCrypt_cca_transform(UserSKPtr decryptKey, CipherTextPtr msg) {
    GByteArray* cph_buf;

    GByteArray* transM;
    cpabe_cph_cca_t* cph;
    cpabe_trans_t* trans;

    CPABEODUserSKPtr userSKPtr = cpabeodUserSK_getObject(decryptKey);
    if (userSKPtr == NULL) {
        return NULL;
    }
    cpabe_pub_t* pub = malloc(sizeof (cpabe_pub_t));
    cpabe_prv_t* prv = userSKPtr->userTK->priv;
    decryptKey->setPairing(pub->p, decryptKey->pairing);
    if(msg->data->data[0] == 0)
        unserializeCT(msg->data, &cph_buf, NULL, NULL);
    else if(msg->data->data[0] == 1)
        unserializeCT1(msg->data, &cph_buf, NULL, NULL);
    else
        return NULL;
    cph = cpabe_cph_cca_unserialize(pub->p, cph_buf, 1);
    cpabe_cph_t* tcph = convert_cca_cph_to_cph(cph);
    if (!(trans = cpabe_trans(pub, prv, tcph))) {
        return NULL;
    }
    element_clear(tcph->c);
    element_clear(tcph->cs);
    free(tcph);
    free(pub);
    cpabe_cph_cca_free(cph);
    transM = cpabe_transct_serialize(trans);
    cpabe_transct_free(trans);
    CipherTextPtr ctPtr = new_CipherTextOD(msg, transM);
    ctPtr->serialize = cpabeodCT_serializeCT;
    ctPtr->unserialize = cpabeodCT_unserializeCT;
    return ctPtr;
}
