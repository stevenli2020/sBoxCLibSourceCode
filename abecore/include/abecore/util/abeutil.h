/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "common.h"
#include "io.h"
#include "pbc.h"
#define TYPE_DEF_PARAMS "pbc.properties"
#ifndef ABEUTIL_H
#define ABEUTIL_H

#ifdef __cplusplus
extern "C" {
#endif


    char* get_pairing_desc(const pbc_param_ptr data);
    int write_pairing_desc(const pbc_param_ptr data, char* filename);
    char* read_pairing_desc(char* filename, int remove);


    void write_element(GByteArray* b, element_t e);
    void read_element(GByteArray* b, int* offset, element_t e);
    void copy_element(element_t dest, element_t src);
    char* abe_parse_policy(char* s);
    char* map_policy(GHashTable* attributeMaps, const char* policyStr);
    char* unmap_policy(GHashTable* attributeMaps, const char* policyStr);

    GSList* read_attrlist(char* attrF);

    void ele_set_by_hash256_bytes(element_t e, GByteArray* data);
    void ele_set_by_hash256_ele(element_t e1, element_t e2);
    void ele_set_by_hash256_eles(element_t e1, element_t* e2, size_t num);
    GByteArray* hash256_ele_len(element_t e, size_t len);
#ifdef __cplusplus
}
#endif

#endif /* ABEUTIL_H */

