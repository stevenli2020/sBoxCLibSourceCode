/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#ifndef CCAODABEOBJ_H
#define CCAODABEOBJ_H

#ifdef __cplusplus
extern "C" {
#endif

	typedef cpabe_pub_t cca_odabe_pub_t;

	typedef cpabe_msk_t cca_odabe_msk_t;

    typedef cpabe_sk_t cca_odabe_tk_t;

    typedef odabe_dk_t cca_odabe_dk_t;

    typedef struct{
    	element_t cs; /* G_T */
        element_t c; /* G_1 */
        cpabe_policy_t* p;
        GByteArray* cm; 
    }cca_odabe_cph_t;

    typedef cca_odabe_cph_t cca_odabe_tcph_t;


#ifdef __cplusplus
}
#endif

#endif /* CCAODABEOBJ_H */

