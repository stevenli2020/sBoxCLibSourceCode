/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#ifndef ODABEFREE_H
#define ODABEFREE_H


#ifdef __cplusplus
extern "C" {
#endif
    void odabe_pub_free(odabe_pub_t * pub);
    void odabe_msk_free(odabe_msk_t * msk);
    void odabe_tk_free(odabe_tk_t * tk);
    void odabe_dk_free(odabe_dk_t * dk);
    void odabe_cph_free(odabe_cph_t * cph);
    void odabe_tcph_free(odabe_tcph_t * tcph);

#ifdef __cplusplus
}
#endif

#endif /* ODABEFREE_H */

