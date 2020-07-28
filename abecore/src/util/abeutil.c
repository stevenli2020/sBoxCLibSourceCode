/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "util/abeutil.h"
#include "openssl/sha.h"
#include "abe/policyparser.h"
#include "model/abeattribute.h"

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
    int len = read_int(b, offset);
    unsigned char* buf = (unsigned char*) malloc(len);
    memcpy(buf, b->data + *offset, len);
    *offset += len;
    element_from_bytes(e, buf);
    free(buf);
}

char* abe_parse_policy(char* s) {
    return parse_policy_lang(s);
}

GSList* read_attrlist(char* attrF) {
    GByteArray* attrs = read_bytes_from_file(attrF);
    int offset = 0;
    int len = attrs->len;
    char* rattr;
    char* key;
    char* val;
    GSList* attributes = NULL;
    while (offset < len) {
        rattr = read_string(attrs, &offset);
        key = strtok(rattr, "=|");
        val = strtok(NULL, "=|");
        attributes = g_slist_append(attributes, new_ABEAttribute(key, val, val));
    }
    g_byte_array_free(attrs, 1);
    return attributes;
}

void add_attribute_token(GString* token, GString* parsed, GHashTable* attributeMaps) {
    if (token->len > 0) {
        char* value = trim(token)->str;
        if (g_hash_table_contains(attributeMaps, value)) {
            ABEAttributePtr attr = g_hash_table_lookup(attributeMaps, value);
            g_string_append(parsed, attr->getID(attr));
        } else
            g_string_append(parsed, value);
    }
}

void revert_attribute_token(GString* token, GString* parsed, GHashTable* attributeMaps) {
    if (token->len > 0) {
        char* value = trim(token)->str;
        if (g_hash_table_contains(attributeMaps, value)) {
            ABEAttributePtr attr = g_hash_table_lookup(attributeMaps, value);
            g_string_append(parsed, attr->getKey(attr));
        } else
            g_string_append(parsed, value);
    }
}
typedef void (*addTokenPtr)(GString* token, GString* parsed, GHashTable* attributeMaps);

char* map_attribute_policy(GHashTable* attributeMaps, const char* policyStr, void (*addTokenPtr)(GString*, GString*, GHashTable*)) {
    int i = 0;
    int size = strlen(policyStr);
    GString* parsed = g_string_new("");
    GString* token = g_string_new("");
    while (i < size) {
        if (policyStr[i] == '&' || policyStr[i] == '|' || policyStr[i] == '(' || policyStr[i] == ')') {
            (*addTokenPtr)(token, parsed, attributeMaps);
            g_string_set_size(token, 0);
            g_string_append_c(parsed, policyStr[i]);
        } else {
            g_string_append_c(token, policyStr[i]);
        }
        i++;
    }
    if (token->len > 0)
        (*addTokenPtr)(token, parsed, attributeMaps);
    char* val = parsed->str;
    g_string_free(token, 1);
    g_string_free(parsed, 0);
    return val;
}

char* map_policy(GHashTable* attributeMaps, const char* policyStr) {
    return map_attribute_policy(attributeMaps, policyStr, &add_attribute_token);
}

char* unmap_policy(GHashTable* attributeMaps, const char* policyStr) {
    return map_attribute_policy(attributeMaps, policyStr, &revert_attribute_token);
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
