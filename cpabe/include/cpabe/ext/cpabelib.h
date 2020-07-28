/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#ifndef CPABELIB_H
#define CPABELIB_H

#ifdef __cplusplus
extern "C" {
#endif
    char* cpabe_error();

    void cpabe_init(pbc_param_ptr par);

    void cpabe_setup(cpabe_pub_t** pub, cpabe_msk_t** msk, char* pairing_desc);

    cpabe_prv_t * cpabe_keygen(cpabe_pub_t* pub, cpabe_msk_t* msk, char** attributes, int isOutsourced);

    cpabe_cph_t * cpabe_enc(cpabe_pub_t* pub, element_t m, char* policy);

    cpabe_cph_cca_t * cpabe_cca_enc(cpabe_pub_t* pub, element_t m, char* policy, unsigned char flag);

    cpabe_trans_t * cpabe_trans(cpabe_pub_t* pub, cpabe_prv_t* prv, cpabe_cph_t * cph);

    int cpabe_dec(cpabe_pub_t* pub, cpabe_prv_t* prv,
            cpabe_cph_t* cph, cpabe_trans_t* trans, element_t m);

    int cpabe_cca_dec(cpabe_pub_t* pub, cpabe_prv_t* prv,
            cpabe_cph_cca_t* cph, cpabe_trans_t* trans, element_t m);

    cpabe_cph_t* convert_cca_cph_to_cph(cpabe_cph_cca_t* cph);



#ifdef __cplusplus
}
#endif

#endif /* CPABELIB_H */

