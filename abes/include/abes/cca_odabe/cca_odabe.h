/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#ifndef CCAODABE_H
#define CCAODABE_H

#ifdef __cplusplus
extern "C" {
#endif
	#define cca_odabe_error cca_odabe_error

	#define cca_odabe_init cca_odabe_init

	int cca_odabe_setup_1(pairing_ptr pairing, char* pairing_desc, cca_odabe_pub_t** pub, cca_odabe_msk_t** msk);

    int cca_odabe_setup_2(char* pairing_desc, cca_odabe_pub_t** pub, cca_odabe_msk_t** msk);

    int cca_odabe_keygen(cca_odabe_pub_t* pub, cca_odabe_msk_t* msk, char** attributes, cca_odabe_tk_t** tk, cca_odabe_dk_t** dk);

    int cca_odabe_enc(cca_odabe_pub_t* pub, element_t m, char* policy, cca_odabe_cph_t** cph);

    int cca_odabe_trans(cca_odabe_pub_t* pub, cca_odabe_tk_t* tk, cca_odabe_cph_t* cph, cca_odabe_tcph_t** tcph);

    int cca_odabe_dec(cca_odabe_pub_t* pub, cca_odabe_dk_t* dk, cca_odabe_tcph_t* cph, element_t m);



#ifdef __cplusplus
}
#endif

#endif /* CCAODABE_H */

