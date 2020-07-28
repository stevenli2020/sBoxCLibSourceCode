/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include "glib.h"

#ifndef CPABEIO_H
#define CPABEIO_H

#ifdef __cplusplus
extern "C" {
#endif
    GByteArray* serializeCT(GByteArray* cph_buf,
            int file_len, GByteArray* aes_buf);

    void unserializeCT(GByteArray* ct, GByteArray** cph_buf,
            int* file_len, GByteArray** aes_buf);


    GByteArray* serializeCT1(GByteArray* cph_buf,
            int file_len, int aes_bits);

    void unserializeCT1(GByteArray* ct, GByteArray** cph_buf,
            int* file_len, int* aes_bits);


    void read_cpabe_file(char* file, GByteArray** cph_buf,
            int* file_len, GByteArray** aes_buf);

    void write_cpabe_file(char* file, GByteArray* cph_buf,
            int file_len, GByteArray* aes_buf);

    void write_cpabetrans_file(char* file, GByteArray* cph_buf,
            int file_len, GByteArray* aes_buf, GByteArray* m);
    void read_cpabetrans_file(char* file, GByteArray** cph_buf,
            int* file_len, GByteArray** aes_buf, GByteArray** trans_buf);

#ifdef __cplusplus
}
#endif

#endif /* CPABEIO_H */

