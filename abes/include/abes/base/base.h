/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#ifndef BASE_H
#define BASE_H

#ifdef __cplusplus
extern "C" {
#endif
    char* cpabe_error();

    void raise_error(char* fmt, ...);

    void cpabe_init(pbc_param_ptr par);

    int cpabe_setup_1(pairing_ptr pairing, char* pairing_desc, cpabe_pub_t** pub, cpabe_msk_t** msk);

    int cpabe_setup_2(char* pairing_desc, cpabe_pub_t** pub, cpabe_msk_t** msk);

    int cpabe_keygen(cpabe_pub_t* pub, cpabe_msk_t* msk, char** attributes, cpabe_sk_t** sk);

    int cpabe_enc(cpabe_pub_t* pub, element_t m, char* policy, cpabe_cph_t** cph);

    void cpabe_encapsulate_secret(cpabe_pub_t* pub, char* policy, element_t s, int flag, cpabe_cph_t* cph);

    void cpabe_dec_policy(cpabe_pub_t* pub, GArray* sk, cpabe_cph_t* cph, element_t t);

    int cpabe_dec(cpabe_pub_t* pub, cpabe_sk_t* sk, cpabe_cph_t * cph, element_t m);

    void check_sat(cpabe_policy_t* p, GArray* sk);

    void pick_sat_min_leaves(cpabe_policy_t* p);

    cpabe_polynomial_t* rand_poly(int deg, element_t zero_val);

    void eval_poly(element_t r, cpabe_polynomial_t* q, element_t x);

    cpabe_policy_t* parse_policy_postfix(char* s);

    void lagrange_coef(element_t r, GArray* s, int i);



#ifdef __cplusplus
}
#endif

#endif /* BASE_H */

