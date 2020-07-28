/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include "pbc.h"
#include "glib.h"


#ifndef CPABEOBJECT_H
#define CPABEOBJECT_H

#ifdef __cplusplus
extern "C" {
#endif

    struct cpabe_pub_s {
        char* pairing_desc;
        pairing_t p;
        element_t g; /* G_1 */
        element_t h; /* G_1 */
        element_t gp; /* G_2 */
        element_t g_hat_alpha; /* G_T */
    };

    struct cpabe_msk_s {
        element_t beta; /* Z_r */
        element_t g_alpha; /* G_2 */
    };

    typedef struct {
        char* attr;
        element_t d; /* G_2 */
        element_t dp; /* G_2 */

        int used;
        element_t z; /* G_1 */
        element_t zp; /* G_1 */
    } cpabe_prv_comp_t;

    struct cpabe_prv_s {
        int type;
        void* derivedKey;
        //element_t z; /* Z_r */ //added for od
        element_t d; /* G_2 */
        GArray* comps;
    };
    typedef struct cpabe_prv_s *cpabe_prv_ptr;

    struct cpabe_prvod_s {
        cpabe_prv_ptr base_prv;
        element_t z; /* Z_r */ //added for od
    };

    typedef struct cpabe_prvod_s *cpabe_prvod_ptr;

    typedef struct {
        int deg;
        element_t* coef; /* G_T (of length deg + 1) */
    } cpabe_polynomial_t;

    typedef struct {
        int k; /* one if leaf, otherwise threshold */
        char* attr; /* attribute string if leaf, otherwise null */
        element_t c; /* G_1, only for leaves */
        element_t cp; /* G_1, only for leaves */
        GPtrArray* children; /* pointers to cpabe_policy_t's, len == 0 for leaves */

        cpabe_polynomial_t* q;
        int satisfiable;
        int min_leaves;
        int attri;
        GArray* satl;
    } cpabe_policy_t;

    struct cpabe_cph_s {
        element_t cs; /* G_T */
        element_t c; /* G_1 */
        cpabe_policy_t* p;
    };

    struct cpabe_trans_ct {
        element_t trans; /* G_T */
    };

    typedef struct cpabe_pub_s cpabe_pub_t;

    typedef struct cpabe_msk_s cpabe_msk_t;

    typedef struct cpabe_prv_s cpabe_prv_t;

    typedef struct cpabe_cph_s cpabe_cph_t;

    typedef struct cpabe_trans_ct cpabe_trans_t;

    typedef struct cpabe_prvod_s cpabe_prvod_t;


#ifdef __cplusplus
}
#endif

#endif /* CPABEOBJECT_H */

