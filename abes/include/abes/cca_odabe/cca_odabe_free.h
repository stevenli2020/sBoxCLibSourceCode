/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#ifndef CCAODABEFREE_H
#define CCAODABEFREE_H


#ifdef __cplusplus
extern "C" {
#endif
    void cca_odabe_pub_free(cca_odabe_pub_t * pub);
    void cca_odabe_msk_free(cca_odabe_msk_t * msk);
    void cca_odabe_tk_free(cca_odabe_tk_t * tk);
    void cca_odabe_dk_free(cca_odabe_dk_t * dk);
    void cca_odabe_cph_free(cca_odabe_cph_t * cph);
    void cca_odabe_tcph_free(cca_odabe_tcph_t * tcph);

#ifdef __cplusplus
}
#endif

#endif /* CCAODABEFREE_H */

