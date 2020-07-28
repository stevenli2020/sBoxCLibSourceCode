/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#ifndef CCAEODABEFREE_H
#define CCAEODABEFREE_H


#ifdef __cplusplus
extern "C" {
#endif
    void cca_eodabe_pub_free(cca_eodabe_pub_t * pub);
    void cca_eodabe_msk_free(cca_eodabe_msk_t * msk);
    void cca_eodabe_tk_free(cca_eodabe_tk_t * tk);
    void cca_eodabe_dk_free(cca_eodabe_dk_t * dk);
    void cca_eodabe_cph_free(cca_eodabe_cph_t * cph);
    void cca_eodabe_tcph_free(cca_eodabe_tcph_t * tcph);

#ifdef __cplusplus
}
#endif

#endif /* CCAEODABEFREE_H */

