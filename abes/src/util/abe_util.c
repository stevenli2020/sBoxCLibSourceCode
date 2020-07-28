/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include <string.h>
#include "openssl/sha.h"
#include "pbc.h"
#include "glib.h"
#include "util/common.h"
#include "util/io.h"
#include "policy/policy_parser.h"
#include "util/abe_util.h"


void element_hash_from_string(element_t h, char* s) {
    GByteArray* m = g_byte_array_new();
    g_byte_array_append(m, s, strlen(s)+1);
    ele_set_by_hash256_bytes(h, m);
    g_byte_array_free(m, 1);
}

char* get_pairing_desc(const pbc_param_ptr para) {
    if (write_pairing_desc(para, TYPE_DEF_PARAMS))
        return read_pairing_desc(TYPE_DEF_PARAMS, 1);
    return "";
}

int write_pairing_desc(const pbc_param_ptr para, char* filename) {
    FILE* f = open_write(filename);
    if (f) {
        pbc_param_out_str(f, para);
        fclose(f);
        return 1;
    }
    return 0;
}

char* read_pairing_desc(char* filename, int rem) {
    char* pairingDesc;
    GByteArray* parStr = read_bytes_from_file(filename);
    if (parStr) {
        pairingDesc = to_string(parStr)->data;
        if (rem)
            remove(filename);
        return pairingDesc;
    }
    return NULL;
}

char* abe_parse_policy(char* policy){
    return parse_policy_lang(policy);
} 

void write_element(GByteArray* b, element_t e) {
    int len = element_length_in_bytes(e);
    write_int(b, len);
    unsigned char* buf = (unsigned char*) malloc(len);
    element_to_bytes(buf, e);
    g_byte_array_append(b, buf, len);
    free(buf);
}

void copy_element(element_t dest, element_t src) {
    int len = element_length_in_bytes(src);
    unsigned char* buf = (unsigned char*) malloc(len);
    element_to_bytes(buf, src);
    element_init(dest, src->field);
    element_from_bytes(dest, buf);
    free(buf);
}

void read_element(GByteArray* b, int* offset, element_t e) {
    int ooff = *offset;
    int len = read_int(b, offset);
    if(*offset > b->len || *offset + len > b->len){
        *offset = ooff;
        return;
    }

    unsigned char* buf = (unsigned char*) malloc(len);
    memcpy(buf, b->data + *offset, len);
    *offset += len;
    element_from_bytes(e, buf);
    free(buf);
}

void ele_set_by_hash256_bytes(element_t e, GByteArray* data){

    char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, data->data, data->len);
    SHA256_Final(hash, &sha256);

    element_from_hash(e, hash, SHA256_DIGEST_LENGTH);
}

GByteArray* hash256_ele_len(element_t e, size_t len){
    int elen = element_length_in_bytes(e);
    unsigned char buf[elen];
    element_to_bytes(buf, e);
    GByteArray* hashsrc = g_byte_array_new();
    g_byte_array_append(hashsrc, buf, elen);
    GByteArray* res = hash256_len(hashsrc, len);
    g_byte_array_free(hashsrc, 1);
    return res;
}

void ele_set_by_hash256_ele(element_t e1, element_t e2){
    int len = element_length_in_bytes(e2);
    unsigned char buf[len];
    element_to_bytes(buf, e2);

    char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, buf, len);
    SHA256_Final(hash, &sha256);

    element_from_hash(e1, buf, SHA256_DIGEST_LENGTH);
}

void ele_set_by_hash256_eles(element_t e1, element_t* e2, size_t num){
    int len = 0;
    int llen = -1;
    unsigned char* buf;

    char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    buf = (unsigned char*) malloc(1);
    for(int i=0; i< num; i++){
        len = element_length_in_bytes(e2[i]);
        if(llen < len){
            llen = len;
            free(buf);
            buf = (unsigned char*) malloc(llen);
        }
        element_to_bytes(buf, e2[i]);
        SHA256_Update(&sha256, buf, len);
    }
    SHA256_Final(hash, &sha256);
    element_from_hash(e1, buf, SHA256_DIGEST_LENGTH);
    free(buf);
}
