/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include "pbc.h"
#include "stdlib.h"
#include  "util/abeutil.h"
#include "abe/abekey.h"

MasterKeyPairPtr new_MasterKeyPair(MasterPKPtr mpk, MasterSKPtr msk) {
    if (mpk == NULL || msk == NULL)
        return NULL;
    MasterKeyPairPtr mk = malloc(sizeof (MasterKeyPair));
    mk->mpk = mpk;
    mk->msk = msk;
    return mk;
}

void delete_MasterKeyPair(MasterKeyPairPtr mk) {
    if (mk) {
        mk->mpk = NULL;
        mk->msk = NULL;
        free(mk);
    }
}

MasterSKPtr new_MasterSK(pairing_ptr pairing) {
    if (pairing == NULL)
        return NULL;
    MasterSKPtr bmsk = malloc(sizeof (MasterSK));
    bmsk->pairing = pairing;
    bmsk->setPairing = key_set_pairing;
    bmsk->derivedMSK = NULL;
    return bmsk;
}

void delete_MasterSK(MasterSKPtr msk) {
    if (msk){
        msk->pairing = NULL;
        msk->derivedMSK = NULL;
        free(msk);
    }
}

MasterPKPtr new_MasterPK(pairing_ptr pairing) {
    //if (pairing == NULL)
    //    return NULL;
    MasterPKPtr bmpk = malloc(sizeof (MasterPK));
    bmpk->pairing = pairing;
    bmpk->setPairing = key_set_pairing;
    bmpk->derivedMPK = NULL;
    return bmpk;
}

void delete_MasterPK(MasterPKPtr mpk) {
    if (mpk) {
        mpk->pairing = NULL;
        mpk->derivedMPK = NULL;
        free(mpk);
    }
}

UserSKPtr new_UserSK(pairing_ptr pairing) {
    if (pairing == NULL)
        return NULL;
    UserSKPtr userSK = malloc(sizeof (UserSK));
    userSK->pairing = pairing;
    userSK->setPairing = key_set_pairing;
    userSK->derivedSK = NULL;
    return userSK;
}

void delete_UserSK(UserSKPtr userSK) {
    if (userSK) {
        userSK->pairing = NULL;
        userSK->derivedSK = NULL;
        free(userSK);
    }
}

void key_set_pairing(pairing_ptr dest, pairing_ptr src) {
    dest[0] = src[0];
}

int userSK_get_keytype(char * filename) {

    if (filename == NULL) {
        printf("Invalid filename !");
        return 0;
    }
    GByteArray* config = read_bytes_from_file(filename);
    if (config) {
        int offset = 0;
        int type = read_int(config, &offset);
        g_byte_array_free(config, 1);
        return type;
    }
    return 0;

}
