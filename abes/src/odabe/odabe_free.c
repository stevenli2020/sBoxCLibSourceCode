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



void odabe_pub_free(odabe_pub_t * pub){
	cpabe_pub_free((cpabe_pub_t*) pub);
}

void odabe_msk_free(odabe_msk_t * msk){
	cpabe_msk_free((cpabe_msk_t*) msk);
}

void odabe_tk_free(odabe_tk_t * tk){
	cpabe_sk_free((cpabe_sk_t*) tk);
}

void odabe_dk_free(odabe_dk_t* dk){
    element_clear(dk->z);
    free(dk);
}

void odabe_cph_free(odabe_cph_t * cph){
	cpabe_cph_free((cpabe_cph_t*) cph);
}

void odabe_tcph_free(odabe_tcph_t * tcph){
	cpabe_cph_free((cpabe_cph_t*) tcph);
}
