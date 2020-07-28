/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#ifndef CCAEODABE_OBJ_H
#define CCAEODABE_OBJ_H

#ifdef __cplusplus
extern "C" {
#endif

    struct cca_eodabe_pub_s {
        char* pairing_desc;
        pairing_ptr p;
        element_t g1; /* G_1 */
        element_t g2; /* G_2 */
        element_t h1; /* G_1 */
        element_t h2; /* G_2 */
        element_t h3; /* G_T */
    };

	typedef struct cca_eodabe_pub_s cca_eodabe_pub_t;


    struct cca_eodabe_msk_s {
        element_t alpha; /* Z_r */
    };

    typedef struct cca_eodabe_msk_s cca_eodabe_msk_t;

    struct cca_eodabe_sk_s {
        element_t d1; /* G_2 */
        element_t d2; /* G_2 */
        GArray* comps; /* Array of hash attrs*/
    };
    typedef struct cca_eodabe_sk_s cca_eodabe_dk_t;

    struct cca_eodabe_tk_s {
        element_t z; /* Z_r */
        GArray* attrs;
    };

    typedef struct cca_eodabe_tk_s cca_eodabe_tk_t;

    struct cca_eodabe_cph_s{
    	element_t c1; /* G_T */
        element_t c2; /* G_1 */
        cpabe_policy_t* p;
        GByteArray* c0; 
    }cca_eodabe_cph_s;

    typedef struct cca_eodabe_cph_s cca_eodabe_cph_t;

    typedef struct cca_eodabe_cph_s cca_eodabe_tcph_t;


#ifdef __cplusplus
}
#endif

#endif /* CCAEODABE_OBJ_H */

