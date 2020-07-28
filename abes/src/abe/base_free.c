/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include "pbc.h"
#include "glib.h"
#include "base/base_obj.h"
#include "base/base_free.h"

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
cpabe_sk_free(cpabe_sk_t* sk) {
    int i;
    if (sk->d)
        element_clear(sk->d);
    if (sk->comps) {
        for (i = 0; i < sk->comps->len; i++) {
            cpabe_sk_comp_t c;

            c = g_array_index(sk->comps, cpabe_sk_comp_t, i);
            free(c.attr);
            c.attr = NULL;
            element_clear(c.d);
            element_clear(c.dp);
        }

        g_array_free(sk->comps, 1);
        sk->comps = NULL;
    }

    free(sk);
}

void cpabe_policy_free_1(cpabe_policy_t* p, int f) {
    int i;

    if (p->attr) {
        free(p->attr);
        p->attr = NULL;
        if(f){
            element_clear(p->c);
            element_clear(p->cp);
        }
    }
    for (i = 0; i < p->children->len; i++)
        cpabe_policy_free_1(g_ptr_array_index(p->children, i), f);

    g_ptr_array_free(p->children, 1);
    p->children = NULL;

    free(p);
}
void cpabe_policy_free(cpabe_policy_t* p) {
    cpabe_policy_free_1(p, 1);
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
