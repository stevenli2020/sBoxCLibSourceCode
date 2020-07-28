/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include "ec/ecsigkey.h"

ECSigKeyPairPtr genECSigPair(pairing_ptr p, element_t g) {
    bls_sys_param_ptr bls_p = malloc(sizeof (bls_sys_param_t));
    copy_element(bls_p->g, g);
    bls_p->pairing = p;
    bls_p->signature_length = pairing_length_in_bytes_x_only_G1(bls_p->pairing);
    ECSigSKPtr sk = new_ECSigSK(p);
    ECSigPKPtr pk = new_ECSigPK(p);
    bls_gen(pk->pk, sk->sk, bls_p);
    return new_ECSigKeyPair(sk, pk);
}