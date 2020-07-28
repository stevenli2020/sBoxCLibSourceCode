/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include "stdlib.h"
#include "ec/ecsigkey.h"
#include "util/abeutil.h"

void init_sig_param(bls_sys_param_ptr bls_p, pairing_ptr pairing) {
    bls_p->pairing = pairing;
    bls_p->signature_length = pairing_length_in_bytes_x_only_G1(bls_p->pairing);
}

GByteArray* ec_sigpubkey_serialize(ECSigPKPtr pk) {
    GByteArray* b;
    b = g_byte_array_new();
    write_element(b, pk->pk->gx);
    write_element(b, pk->pk->param->g);
    return b;
}

GByteArray* ec_sigprivkey_serialize(ECSigSKPtr sk) {
    GByteArray* b;
    b = g_byte_array_new();
    write_element(b, sk->sk->x);
    return b;
}

ECSigPKPtr ec_sigpubkey_unserialize(ECSigPKPtr pk, GByteArray* b, int free) {
    int offset;

    offset = 0;
    element_init_G2(pk->pk->gx, pk->pairing);
    element_init_G2(pk->pk->param->g, pk->pairing);
    read_element(b, &offset, pk->pk->gx);
    read_element(b, &offset, pk->pk->param->g);

    if (free)
        g_byte_array_free(b, 1);
    return pk;
}

ECSigSKPtr ec_sigprivkey_unserialize(ECSigSKPtr sk, GByteArray* b, int free) {
    int offset;

    offset = 0;
    element_init_Zr(sk->sk->x, sk->pairing);
    read_element(b, &offset, sk->sk->x);

    if (free)
        g_byte_array_free(b, 1);
    return sk;
}

GByteArray* ecSigPK_serialize(ECSigPKPtr userPK) {
    return ec_sigpubkey_serialize(userPK);
}

ECSigPKPtr ecSigPK_unserialize(ECSigPKPtr userPK, GByteArray* pk) {
    return ec_sigpubkey_unserialize(userPK, pk, 1);
}


GByteArray* ecSigSK_serialize(ECSigSKPtr userSK) {
    return ec_sigprivkey_serialize(userSK);
}

ECSigSKPtr ecSigSK_unserialize(ECSigSKPtr userSK, GByteArray* sk) {
    return ec_sigprivkey_unserialize(userSK, sk, 1);
}

ECSigSKPtr new_ECSigSK(pairing_ptr pairing) {
    if (pairing == NULL)
        return NULL;
    ECSigSKPtr ecSigSK = malloc(sizeof (ECSigSK));
    ecSigSK->sk = malloc(sizeof (bls_private_key_t));
    ecSigSK->sk->param = malloc(sizeof (bls_sys_param_t));
    init_sig_param(ecSigSK->sk->param, pairing);
    ecSigSK->sigLen = ecSigSK->sk->param->signature_length;
    ecSigSK->pairing = pairing;
    ecSigSK->load = ecSigSK_load;
    ecSigSK->save = ecSigSK_save;
    ecSigSK->serialize = ecSigSK_serialize;
    ecSigSK->unserialize = ecSigSK_unserialize;
    return ecSigSK;
}

void delete_ECSigSK(ECSigSKPtr sk) {
    if (sk) {
        if (sk->sk) {
            bls_clear_private_key(sk->sk);
        }
        free(sk);
    }

}

int ecSigSK_save(ECSigSKPtr sk, char* name) {
    if (sk == NULL) {
        return 0;
    }
    GByteArray* bytes = ec_sigprivkey_serialize(sk);
    return write_bytes_to_file(name, bytes, 1);
}

int ecSigSK_load(ECSigSKPtr sk, char* name) {
    if (sk == NULL) {
        return 0;
    }
    GByteArray* bytes = read_bytes_from_file(name);
    ec_sigprivkey_unserialize(sk, bytes, 1);
    return 1;
}

ECSigPKPtr new_ECSigPK(pairing_ptr pairing) {
    if (pairing == NULL)
        return NULL;
    ECSigPKPtr ecSigPK = malloc(sizeof (ECSigPK));
    ecSigPK->pk = malloc(sizeof (bls_public_key_t));
    ecSigPK->pk->param = malloc(sizeof (bls_sys_param_t));
    init_sig_param(ecSigPK->pk->param, pairing);
    ecSigPK->sigLen = ecSigPK->pk->param->signature_length;
    ecSigPK->pairing = pairing;
    ecSigPK->load = ecSigPK_load;
    ecSigPK->save = ecSigPK_save;
    ecSigPK->serialize = ecSigPK_serialize;
    ecSigPK->unserialize = ecSigPK_unserialize;
    return ecSigPK;
}

void delete_ECSigPK(ECSigPKPtr pk) {
    if (pk) {
        if (pk->pk) {
            bls_clear_public_key(pk->pk);
        }
        free(pk);
    }
}

int ecSigPK_save(ECSigPKPtr pk, char* name) {
    if (pk == NULL) {
        return 0;
    }
    GByteArray* bytes = ecSigPK_serialize(pk);
    return write_bytes_to_file(name, bytes, 1);
}

int ecSigPK_load(ECSigPKPtr pk, char* name) {
    if (pk == NULL) {
        return 0;
    }
    GByteArray* bytes = read_bytes_from_file(name);
    ec_sigpubkey_unserialize(pk, bytes, 1);
}

ECSigKeyPairPtr new_ECSigKeyPair(ECSigSKPtr userSK, ECSigPKPtr userPK) {
    if (userPK == NULL || userSK == NULL)
        return NULL;
    ECSigKeyPairPtr mk = malloc(sizeof (ECSigKeyPair));
    mk->sigPK = userPK;
    mk->sigSK = userSK;
    return mk;
}

void delete_ECSigKeyPair(ECSigKeyPair mskp) {
    if (mskp) {
        if (mskp->sigPK)
            delete_ECSigPK(mskp->sigPK);
        if (mskp->sigSK)
            delete_ECSigSK(mskp->sigSK);
        free(mskp);
    }
}
