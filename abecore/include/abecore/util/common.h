/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <inttypes.h>
#include <ctype.h>
#include <glib.h>

#ifndef COMMON_H
#define COMMON_H

#ifdef __cplusplus
extern "C" {
#endif
    int is_numeric(const char* value);
    GByteArray* to_string(GByteArray* array);
    GByteArray* append_null(GByteArray* array);
    void die(char* fmt, ...);
    GByteArray* encode64(GByteArray* array);
    GByteArray* decode64(GByteArray* array);
    GByteArray* hash256(GByteArray* array);

    GByteArray* encodeToHex(char* val, int len);
    GByteArray* encodeHex(GByteArray* array);
    GByteArray* decodeHex(GByteArray* array);
    
    GString* trim(GString* s);

    GByteArray* hash256_len(GByteArray* data, size_t len);
    GByteArray* xor_a_b(unsigned char* a, unsigned char* b, size_t len);
#ifdef __cplusplus
}
#endif

#endif /* COMMON_H */

