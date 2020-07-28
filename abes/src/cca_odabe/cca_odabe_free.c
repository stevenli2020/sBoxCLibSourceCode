/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "pbc.h"
#include "glib.h"
#include "base/base_obj.h"
#include "base/base_free.h"
#include "odabe/odabe_obj.h"
#include "odabe/odabe_free.h"
#include "cca_odabe/cca_odabe_obj.h"
#include "cca_odabe/cca_odabe_free.h"



void cca_odabe_pub_free(cca_odabe_pub_t * pub){
	cpabe_pub_free((cpabe_pub_t*) pub);
}

void cca_odabe_msk_free(cca_odabe_msk_t * msk){
	cpabe_msk_free((cpabe_msk_t*) msk);
}

void cca_odabe_tk_free(cca_odabe_tk_t * tk){
	cpabe_sk_free((cpabe_sk_t*) tk);
}

void cca_odabe_dk_free(cca_odabe_dk_t* dk){
    odabe_dk_free((odabe_dk_t* ) dk);
}

void cca_odabe_cph_free(cca_odabe_cph_t * cph){
    if(cph == NULL)
        return;
    element_clear(cph->cs);
    element_clear(cph->c);
    g_byte_array_free(cph->cm, 1);

    if(cph->p != NULL){
        cpabe_policy_free(cph->p);
        cph->p = NULL;
    }
    free(cph);
}

void cca_odabe_tcph_free(cca_odabe_tcph_t * tcph){
    cca_odabe_cph_free((cca_odabe_cph_t* )tcph);
}
