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
#include "base/base_free.h"
#include "odabe/odabe_obj.h"
#include "cca_odabe/cca_odabe_obj.h"
#include "base/base.h"
#include "odabe/odabe.h"
#include "cca_odabe/cca_odabe.h"
#include "util/common.h"
#include "util/abe_util.h"




int cca_odabe_setup_1(pairing_ptr pairing, char* pairing_desc, cca_odabe_pub_t** pub, cca_odabe_msk_t** msk){
	if(!pairing || !pub || !msk)
		return 0;
	return cpabe_setup_1(pairing, pairing_desc, (cpabe_pub_t**)pub, (cpabe_msk_t**)msk);

}

int cca_odabe_setup_2(char* pairing_desc, cca_odabe_pub_t** pub, cca_odabe_msk_t** msk){
	if(!pairing_desc || !pub || !msk)
		return 0;
	return cpabe_setup_2(pairing_desc, (cpabe_pub_t**)pub, (cpabe_msk_t**)msk);
}

int cca_odabe_enc(cca_odabe_pub_t* pub, element_t m, char* policy, cca_odabe_cph_t** rcph){
	if(!pub || !policy || !rcph)
		return 0;
    int res = 0;
    cca_odabe_cph_t * cph = malloc(sizeof(cca_odabe_cph_t));
    element_t s, r;
    element_t hashsrc[2];
    GByteArray* hash_r;

    element_init_Zr(s, pub->p);
    element_init_GT(r, pub->p);

    /* compute */

    element_random(r);
    element_init_same_as(hashsrc[0], r);
    element_set(hashsrc[0], r);
    element_init_same_as(hashsrc[1], m);
    element_set(hashsrc[1], m);
    ele_set_by_hash256_eles(s, hashsrc, 2);

    cpabe_encapsulate_secret((cpabe_pub_t* )pub, policy, s, 1, (cpabe_cph_t* )cph);

    if(cph->cs){
        element_mul(cph->cs, cph->cs, r);
        /* compute cph->cm = m xor Hash(r) */
        int mlen = element_length_in_bytes(m);
        unsigned char mb[mlen];
        element_to_bytes(mb, m);
        hash_r = hash256_ele_len(r, mlen);
        cph->cm = xor_a_b(mb, hash_r->data, mlen);
        *rcph = cph;
        res = 1;
    }

    if(res){
        g_byte_array_free(hash_r, 1);
    }
    element_clear(s);
    element_clear(r);
    element_clear(hashsrc[0]);
    element_clear(hashsrc[1]);
    return res;
}

int cca_odabe_trans(cca_odabe_pub_t* pub, cca_odabe_tk_t* tk, 
	cca_odabe_cph_t* cph, cca_odabe_tcph_t** tcph){
	if(!cph || !pub || !tk || !tcph)
		return 0;
    cpabe_cph_t* trans_tmp;
    int res = odabe_trans((odabe_pub_t* )pub, (odabe_tk_t* )tk, 
                (odabe_cph_t* )cph, (odabe_tcph_t** )(&trans_tmp));
    if(res){
        *tcph = malloc(sizeof(cca_odabe_tcph_t));
        element_init_same_as((*tcph)->cs, trans_tmp->cs);
        element_set((*tcph)->cs, trans_tmp->cs);
        element_init_same_as((*tcph)->c, trans_tmp->c);
        element_set((*tcph)->c, trans_tmp->c);
        (*tcph)->p = NULL;
        (*tcph)->cm = g_byte_array_new();
        g_byte_array_append((*tcph)->cm, cph->cm->data, cph->cm->len);
        res = 1;
        cpabe_cph_free(trans_tmp);
    }
    return res;
}

int cca_odabe_dec(cca_odabe_pub_t* pub, cca_odabe_dk_t* dk, 
	cca_odabe_tcph_t* tcph, element_t m){
    element_t r;
    int res = odabe_dec((odabe_pub_t* )pub, (odabe_dk_t* )dk, (odabe_tcph_t* )tcph, r);
    
    if(res){
        // compute m = cm xor hash(r)
        element_t tm;
        element_init_GT(tm, pub->p);
        int mlen = tcph->cm->len;
        GByteArray* hash_r = hash256_ele_len(r, mlen);
        GByteArray* buf = xor_a_b(tcph->cm->data, hash_r->data, mlen);
        element_from_bytes(tm, buf->data);

        // compute s = hash(r, m)
        element_t s;
        element_t hashsrc[2];
        element_init_Zr(s, pub->p);
        element_init_same_as(hashsrc[0], r);
        element_set(hashsrc[0], r);
        element_init_same_as(hashsrc[1], tm);
        element_set(hashsrc[1], tm);
        ele_set_by_hash256_eles(s, hashsrc, 2);

        // check if cs == r * e_g_g_alpha^s
        element_t cs;
        element_init_GT(cs, pub->p);
        element_pow_zn(cs, pub->g_hat_alpha, s);
        element_mul(cs, cs, r);
        res = !element_cmp(cs, tcph->cs);
        if(res){
            element_init_same_as(m, tm);
            element_set(m, tm);
        }
        element_clear(tm);
        element_clear(s);
        element_clear(cs);
        element_clear(hashsrc[0]);
        element_clear(hashsrc[1]);
        g_byte_array_free(hash_r, 1);
        g_byte_array_free(buf, 1);
    }
    element_clear(r);
	return res;
}

int cca_odabe_keygen(cca_odabe_pub_t* pub, cca_odabe_msk_t* msk, 
    char** attributes, cca_odabe_tk_t** rtk, cca_odabe_dk_t** rdk){
	return odabe_keygen((odabe_pub_t* )pub, (odabe_msk_t* ) msk, attributes, 
                        (odabe_tk_t** ) rtk, (odabe_dk_t** ) rdk);
}

