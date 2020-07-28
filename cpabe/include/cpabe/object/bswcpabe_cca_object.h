/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include "pbc.h"
#include "glib.h"


#ifndef CPABEOBJECT_CCA_H
#define CPABEOBJECT_CCA_H

#ifdef __cplusplus
extern "C" {
#endif

    struct cpabe_cph_cca_s {
        element_t cs; /* G_T */
        element_t c; /* G_1 */
        GByteArray* cm; /* bytes m xor H(r) */
        cpabe_policy_t* p;
    };

    typedef struct cpabe_cph_cca_s cpabe_cph_cca_t;

#ifdef __cplusplus
}
#endif

#endif /* CPABEOBJECT_H */

