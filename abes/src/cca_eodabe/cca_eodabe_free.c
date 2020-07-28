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
#include "cca_eodabe/cca_eodabe_obj.h"
#include "cca_eodabe/cca_eodabe_free.h"



void cca_eodabe_pub_free(cca_eodabe_pub_t * pub){
    element_clear(pub->g1);
    element_clear(pub->g2);
    element_clear(pub->h1);
    element_clear(pub->h2);
    element_clear(pub->h3);
    pairing_clear(pub->p);
    free(pub->pairing_desc);
    pub->pairing_desc = NULL;
    free(pub);
}

void cca_eodabe_msk_free(cca_eodabe_msk_t * msk){
    element_clear(msk->alpha);
    free(msk);
}

void cca_eodabe_tk_free(cca_eodabe_tk_t * tk){

    element_clear(tk->z);
    for(int i = 0; i < tk->attrs->len; i++){
        free(g_array_index(tk->attrs, char*, i));
    }
    g_array_free(tk->attrs, 1);
    free(tk);
}

void cca_eodabe_dk_free(cca_eodabe_dk_t* dk){
    int i;
    if (dk->d1)
        element_clear(dk->d1);
    if (dk->d2)
        element_clear(dk->d2);
    if (dk->comps) {
        for (i = 0; i < dk->comps->len; i++) {
            cpabe_sk_comp_t c;

            c = g_array_index(dk->comps, cpabe_sk_comp_t, i);
            free(c.attr);
            c.attr = NULL;
            element_clear(c.d);
        }

        g_array_free(dk->comps, 1);
        dk->comps = NULL;
    }

    free(dk);
}

void cca_eodabe_cph_free(cca_eodabe_cph_t * cph){
    if(cph == NULL)
        return;
    element_clear(cph->c2);
    if(cph->c0){
        g_byte_array_free(cph->c0, 1);
        element_clear(cph->c1);
    }

    if(cph->p != NULL){
        cpabe_policy_free_1(cph->p, cph->c0 != NULL);
        cph->p = NULL;
    }
    free(cph);
}

void cca_eodabe_tcph_free(cca_eodabe_tcph_t * tcph){
    cca_eodabe_cph_free((cca_eodabe_cph_t* )tcph);
}
