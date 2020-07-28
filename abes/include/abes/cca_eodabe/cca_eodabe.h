/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#ifndef CCAEODABE_H
#define CCAEODABE_H

#ifdef __cplusplus
extern "C" {
#endif
	#define cca_eodabe_error cca_eodabe_error

	#define cca_eodabe_init cca_eodabe_init

	int cca_eodabe_setup_1(pairing_ptr pairing, char* pairing_desc, cca_eodabe_pub_t** pub, cca_eodabe_msk_t** msk);

    int cca_eodabe_setup_2(char* pairing_desc, cca_eodabe_pub_t** pub, cca_eodabe_msk_t** msk);

    int cca_eodabe_keygen(cca_eodabe_pub_t* pub, cca_eodabe_msk_t* msk, char** attributes, cca_eodabe_tk_t** tk, cca_eodabe_dk_t** dk);

    int cca_eodabe_enc(cca_eodabe_pub_t* pub, element_t m, char* policy, cca_eodabe_cph_t** cph);

    int cca_eodabe_trans(cca_eodabe_tk_t* tk, cca_eodabe_cph_t* cph, cca_eodabe_tcph_t** tcph);

    int cca_eodabe_dec(cca_eodabe_pub_t* pub, cca_eodabe_dk_t* dk, cca_eodabe_tcph_t* cph, element_t m);



#ifdef __cplusplus
}
#endif

#endif /* CCAEODABE_H */

