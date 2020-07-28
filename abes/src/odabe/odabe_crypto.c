/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include <stdlib.h>
#include <string.h>
#include "glib.h"
#include "pbc.h"
#include "base/base_obj.h"
#include "odabe/odabe_obj.h"
#include "base/base.h"
#include "odabe/odabe.h"
#include "util/abe_util.h"




int odabe_setup_1(pairing_ptr pairing, char* pairing_desc, odabe_pub_t** pub, odabe_msk_t** msk){
	if(!pairing || !pub || !msk)
		return 0;
	return cpabe_setup_1(pairing, pairing_desc, (cpabe_pub_t**)pub, (cpabe_msk_t**)msk);

}

int odabe_setup_2(char* pairing_desc, odabe_pub_t** pub, odabe_msk_t** msk){
	if(!pairing_desc || !pub || !msk)
		return 0;
	return cpabe_setup_2(pairing_desc, (cpabe_pub_t**)pub, (cpabe_msk_t**)msk);
}

int odabe_enc(odabe_pub_t* pub, element_t m, char* policy, odabe_cph_t** cph){
	if(!pub || !policy || !cph)
		return 0;
	return cpabe_enc((cpabe_pub_t* )pub, m, policy, (cpabe_cph_t** )cph);
}

int odabe_trans(odabe_pub_t* pub, odabe_tk_t* tk, 
	odabe_cph_t* cph, odabe_tcph_t** rtcph){
	if(!cph || !pub || !tk || !rtcph)
		return 0;
	cpabe_pub_t* tpub = (cpabe_pub_t* )pub;
    cpabe_cph_t* scph = (cpabe_cph_t* )cph;
    cpabe_cph_t* ttcph = malloc(sizeof(cpabe_cph_t));
    cpabe_sk_t* sk = (cpabe_sk_t*) tk;

	element_t t;
    element_t m;
    element_t trans;

    element_init_GT(trans, tpub->p);
    element_init_GT(m, tpub->p);
    element_init_GT(t, tpub->p);
    cpabe_dec_policy(tpub, sk->comps, scph, t);
    if(!scph->p->satisfiable)
        return 0;

    pairing_apply(m, scph->c, sk->d, tpub->p); /* num_pairings++; */
    element_div(trans, m, t);

    element_init_GT(ttcph->cs, tpub->p);
    element_set(ttcph->cs, scph->cs);
    element_init_GT(ttcph->c, tpub->p);
    element_set(ttcph->c, trans);
    ttcph->p = NULL;
    *rtcph = ttcph;
    // element_printf("cpabe_dec%B\n", res);
    element_clear(m);
    element_clear(trans);
    element_clear(t);

    return 1;
}

int odabe_dec(odabe_pub_t* pub, odabe_dk_t* dk, 
	odabe_tcph_t* tcph, element_t m){
	if(!pub || !dk || !tcph)
		return 0;

	cpabe_pub_t* tpub = (cpabe_pub_t* )pub;
    cpabe_cph_t* ttcph = (cpabe_cph_t*) tcph;

	element_init_GT(m, tpub->p);
	element_pow_zn(m, ttcph->c, dk->z);
	element_div(m, ttcph->cs, m);
	return 1;
}

int odabe_keygen(odabe_pub_t* pub, odabe_msk_t* msk, 
    char** attributes, odabe_tk_t** rtk, odabe_dk_t** rdk){
	if(!pub || !msk || !attributes || !rtk || !rdk){
		return 0;
	}
	cpabe_pub_t* tpub = (cpabe_pub_t* )pub;
	cpabe_msk_t* tmsk = (cpabe_msk_t* )msk;
	int res = cpabe_keygen(tpub, tmsk, attributes, (cpabe_sk_t**)rtk);
	if(!res){
		*rtk = NULL;
		return 0;
	}
	cpabe_sk_t* tk = (cpabe_sk_t*)(*rtk);

	// d
	element_t z, z_invert;
    element_init_Zr(z, tpub->p);
    element_init_Zr(z_invert, tpub->p);
    element_random(z);
    element_invert(z_invert, z);
    element_pow_zn(tk->d, tk->d, z_invert);

    // d_i, dp_i
	cpabe_sk_comp_t *c;
    for(int i = 0; i < tk->comps->len; i++){
    	c = &(g_array_index(tk->comps, cpabe_sk_comp_t, i));
        element_pow_zn(c->d, c->d, z_invert);
        element_pow_zn(c->dp, c->dp, z_invert);
    }

	odabe_dk_t* dk = malloc(sizeof(odabe_dk_t));
	*rdk = dk;
	element_init_Zr(dk->z, tpub->p);
    element_set(dk->z, z);
    element_clear(z);
    element_clear(z_invert);
    return 1;
}

