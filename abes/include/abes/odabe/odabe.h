/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#ifndef ODABE_H
#define ODABE_H

#ifdef __cplusplus
extern "C" {
#endif
	#define odabe_error odabe_error

	#define odabe_init odabe_init

	int odabe_setup_1(pairing_ptr pairing, char* pairing_desc, odabe_pub_t** pub, odabe_msk_t** msk);

    int odabe_setup_2(char* pairing_desc, odabe_pub_t** pub, odabe_msk_t** msk);

    int odabe_keygen(odabe_pub_t* pub, odabe_msk_t* msk, char** attributes, odabe_tk_t** tk, odabe_dk_t** dk);

    int odabe_enc(odabe_pub_t* pub, element_t m, char* policy, odabe_cph_t** cph);

    int odabe_trans(odabe_pub_t* pub, odabe_tk_t* tk, odabe_cph_t* cph, odabe_tcph_t** tcph);

    int odabe_dec(odabe_pub_t* pub, odabe_dk_t* dk, odabe_tcph_t* cph, element_t m);



#ifdef __cplusplus
}
#endif

#endif /* ODABE_H */

