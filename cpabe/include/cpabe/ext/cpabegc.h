/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#ifndef CPABEGC_H
#define CPABEGC_H


#ifdef __cplusplus
extern "C" {
#endif


    void cpabe_pub_free(cpabe_pub_t * pub);

    void cpabe_msk_free(cpabe_msk_t * msk);

    void cpabe_prv_free(cpabe_prv_t * prv);

    void cpabe_cph_free(cpabe_cph_t * cph);

    void cpabe_cph_cca_free(cpabe_cph_cca_t * cph);
    
    void cpabe_prvod_free(cpabe_prvod_ptr pub);
    
    void cpabe_transct_free(cpabe_trans_t * cph);

#ifdef __cplusplus
}
#endif

#endif /* CPABEGC_H */

