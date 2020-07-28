/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#ifndef BASEFREE_H
#define BASEFREE_H


#ifdef __cplusplus
extern "C" {
#endif


    void cpabe_pub_free(cpabe_pub_t * pub);

    void cpabe_msk_free(cpabe_msk_t * msk);

    void cpabe_sk_free(cpabe_sk_t * sk);

    void cpabe_cph_free(cpabe_cph_t * cph);

	void cpabe_policy_free(cpabe_policy_t* p);

	void cpabe_policy_free_1(cpabe_policy_t* p, int f);

#ifdef __cplusplus
}
#endif

#endif /* BASEFREE_H */

