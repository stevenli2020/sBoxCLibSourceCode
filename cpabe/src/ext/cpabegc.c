/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include "glib.h"
#include "object/bswcpabeobject.h"
#include "object/bswcpabe_cca_object.h"
#include "ext/cpabegc.h"

void
cpabe_prvod_free(cpabe_prvod_ptr pub) {
    element_clear(pub->z);
    if (pub->base_prv){
        free(pub->base_prv);
        pub->base_prv = NULL;
    }
    free(pub);
}

void
cpabe_pub_free(cpabe_pub_t* pub) {
    element_clear(pub->g);
    element_clear(pub->h);
    element_clear(pub->gp);
    element_clear(pub->g_hat_alpha);
    pairing_clear(pub->p);
    free(pub->pairing_desc);
    pub->pairing_desc = NULL;
    free(pub);
}

void
cpabe_msk_free(cpabe_msk_t* msk) {
    element_clear(msk->beta);
    element_clear(msk->g_alpha);
    free(msk);
}

void
cpabe_prv_free(cpabe_prv_t* prv) {
    int i;
    if (prv->d)
        element_clear(prv->d);
    if (prv->comps) {
        for (i = 0; i < prv->comps->len; i++) {
            cpabe_prv_comp_t c;

            c = g_array_index(prv->comps, cpabe_prv_comp_t, i);
            free(c.attr);
            c.attr = NULL;
            element_clear(c.d);
            element_clear(c.dp);
        }

        g_array_free(prv->comps, 1);
        prv->comps = NULL;
    }

    free(prv);
}

void
cpabe_policy_free(cpabe_policy_t* p) {
    int i;

    if (p->attr) {
        free(p->attr);
        p->attr = NULL;
        element_clear(p->c);
        element_clear(p->cp);
    }
    for (i = 0; i < p->children->len; i++)
        cpabe_policy_free(g_ptr_array_index(p->children, i));

    g_ptr_array_free(p->children, 1);
    p->children = NULL;

    free(p);
}

void
cpabe_cph_free(cpabe_cph_t* cph) {
    if(cph == NULL)
        return;

    element_clear(cph->cs);
    element_clear(cph->c);

    if(cph->p != NULL){
        cpabe_policy_free(cph->p);
        cph->p = NULL;
    }
    free(cph);
}

void
cpabe_cph_cca_free(cpabe_cph_cca_t* cph) {
    if(cph == NULL)
        return;
    element_clear(cph->cs);
    element_clear(cph->c);
    g_byte_array_free(cph->cm, 1);
    cph->cm = NULL;
    if(cph->p != NULL){
        cpabe_policy_free(cph->p);
        cph->p = NULL;
    }
    free(cph);
}

void
cpabe_transct_free(cpabe_trans_t* trans) {
    element_clear(trans->trans);
    free(trans);
}
