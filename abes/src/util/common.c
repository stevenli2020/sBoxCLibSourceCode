
/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "glib.h"
#include "openssl/sha.h"
#include "util/io.h"
#include "util/common.h"

void die(char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
    exit(1);
}

int is_numeric(const char* value) {
    int size = strlen(value);
    for (int i = 0; i < size; i++) {
        if (value[0] == '-')
            continue;
        if (!isdigit(value[i]))
            return -1;
    }
    return 1;
}

GByteArray* append_null(GByteArray* array) {
    unsigned char zero = 0;
    g_byte_array_append(array, &zero, 1);
    return array;
}

GByteArray* to_string(GByteArray* array) {
    return append_null(array);
}

GByteArray* encode64(GByteArray* array) {
    char* newChar = g_base64_encode(array->data, array->len);
    GByteArray* res = g_byte_array_new();
    g_byte_array_append(res, newChar, strlen(newChar));
    free(newChar);
    return res;
}

char* encode64_str(GByteArray* array){
    return to_string(encode64(array))->data;
}

GByteArray* decode64(GByteArray* array) {
    gsize size;
    char* newChar = g_base64_decode(array->data, &size);
    GByteArray* res = g_byte_array_new();
    g_byte_array_append(res, newChar, size);
    free(newChar);
    return res;
}

GByteArray* decode64_str(char* src){
    GByteArray* srcb = g_byte_array_new();
    g_byte_array_append(srcb, src, strlen(src));
    GByteArray* res = decode64(srcb);
    g_byte_array_free(srcb, 1);
    return res;
}

GByteArray* encodeToHex(char* val, int len) {
    GString* str = g_string_new("");
    for (int i = 0; i < len; i++) {
        g_string_append_printf(str, "%02X", (unsigned char) val[i]);
    }
    GByteArray* res = g_byte_array_new();
    g_byte_array_append(res, str->str, str->len);
    g_string_free(str, 1);
    return res;
}

GByteArray* encodeHex(GByteArray* array) {
    return encodeToHex(array->data, array->len);
}

GByteArray* decodeHex(GByteArray* array) {
    GString* str = g_string_new("");
    char* val = array->data;
    unsigned char c = 0;
    for (int i = 0; i < array->len; i++, val += 2) {
        if (sscanf(val, "%02X", (unsigned int*) &c) != 1) {
            break;
        }
        g_string_append_printf(str, "%c", c);
    }
    GByteArray* res = g_byte_array_new();
    g_byte_array_append(res, str->str, str->len);
    g_string_free(str, 1);
    return res;
}

GByteArray* hash256(GByteArray* data) {
    char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, data->data, data->len);
    SHA256_Final(hash, &sha256);
    return encodeToHex(hash, SHA256_DIGEST_LENGTH);
}

GByteArray* hash256_len(GByteArray* data, size_t len) {
    GByteArray* res = g_byte_array_sized_new(len);
    int round = len / SHA256_DIGEST_LENGTH;
    int offset = len % SHA256_DIGEST_LENGTH;
    if(offset > 0)
        round += 1;

    char* buf = data->data;
    int srcLen = data->len;

    char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    for(int i = 0; i < round; i++){
        SHA256_Init(&sha256);
        SHA256_Update(&sha256, buf, srcLen);
        SHA256_Final(hash, &sha256);
        if(len > SHA256_DIGEST_LENGTH){
	    g_byte_array_append(res, hash, SHA256_DIGEST_LENGTH);
	    len -= SHA256_DIGEST_LENGTH;
        }else{
	    g_byte_array_append(res, hash, len);
        }  
    }
    return res;
}

GByteArray* xor_a_b(unsigned char* a, unsigned char* b, size_t len){
    unsigned char *buf = (char*) malloc(len);
    for(int i = 0; i < len; i++){
        buf[i] = a[i]^b[i];
    }
    GByteArray* res = g_byte_array_new();
    g_byte_array_append(res, buf, len);
    free(buf);
    return res;
}

GString* trim(GString* s) {
    int i = 0;
    while (isspace(s->str[i]))
        i++;
    int length = s->len;
    while (isspace(s->str[length - 1]))
        length--;
    int newLength = s->len - length;
    if (newLength > 0)
        g_string_erase(s, length, newLength);
    if (i > 0)
        g_string_erase(s, 0, i);
    return s;
}
