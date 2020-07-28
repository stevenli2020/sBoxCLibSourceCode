/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#ifndef ODABEOBJ_H
#define ODABEOBJ_H

#ifdef __cplusplus
extern "C" {
#endif

	typedef cpabe_pub_t odabe_pub_t;

	typedef cpabe_msk_t odabe_msk_t;

    typedef cpabe_sk_t odabe_tk_t;

    typedef struct{
    	element_t z;
    }odabe_dk_t;

    typedef cpabe_cph_t odabe_cph_t;

    typedef cpabe_cph_t odabe_tcph_t;


#ifdef __cplusplus
}
#endif

#endif /* ODABEOBJ_H */

