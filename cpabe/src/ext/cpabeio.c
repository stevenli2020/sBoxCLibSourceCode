/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include "stdio.h"
#include "glib.h"
#include "util/abeutil.h"

void read_cpabe_file(char* file, GByteArray** cph_buf,
        int* file_len, GByteArray** aes_buf) {
    FILE* f;
    int i;
    int len;

    *cph_buf = g_byte_array_new();
    *aes_buf = g_byte_array_new();

    f = open_read(file);

    /* read real file len as 32-bit big endian int */
    *file_len = 0;
    for (i = 3; i >= 0; i--)
        *file_len |= fgetc(f) << (i * 8);

    /* read aes buf */
    len = 0;
    for (i = 3; i >= 0; i--)
        len |= fgetc(f) << (i * 8);
    g_byte_array_set_size(*aes_buf, len);
    fread((*aes_buf)->data, 1, len, f);

    /* read cph buf */
    len = 0;
    for (i = 3; i >= 0; i--)
        len |= fgetc(f) << (i * 8);
    g_byte_array_set_size(*cph_buf, len);
    fread((*cph_buf)->data, 1, len, f);

    fclose(f);
}

GByteArray* serializeCT(GByteArray* cph_buf,
        int file_len, GByteArray* aes_buf) {
    GByteArray* ct = g_byte_array_new();
    unsigned char flag = 0;
    g_byte_array_append(ct, &flag, 1);
    write_int(ct, file_len);
    write_byte_array(ct, cph_buf);
    if(aes_buf == NULL)
        write_int(ct, 0);
    else
        write_byte_array(ct, aes_buf);
    return ct;
}

void unserializeCT(GByteArray* ct, GByteArray** cph_buf,
        int* file_len, GByteArray** aes_buf) {
    if(ct->data[0] != 0)
        return;
    int offset = 1;
    int len = read_int(ct, &offset);
    GByteArray* cph_buf1 = g_byte_array_new();
    GByteArray* aes_buf1 = g_byte_array_new();
    read_byte_array(ct, &offset, &cph_buf1);
    read_byte_array(ct, &offset, &aes_buf1);
    if(file_len != NULL)
        *file_len = len;
    if(cph_buf != NULL)
        *cph_buf = cph_buf1;
    else
        g_byte_array_free(cph_buf1, 1);
    if(aes_buf != NULL)
        *aes_buf = aes_buf1;
    else
        g_byte_array_free(aes_buf1, 1);
}

GByteArray* serializeCT1(GByteArray* cph_buf, int file_len, int aes_bits){
    GByteArray* ct = g_byte_array_new();
    unsigned char flag = 1;
    g_byte_array_append(ct, &flag, 1);
    write_int(ct, file_len);
    write_int(ct, aes_bits);
    write_byte_array(ct, cph_buf);
    return ct;
}

void unserializeCT1(GByteArray* ct, GByteArray** cph_buf, int* file_len, int* aes_bits){

    if(ct->data[0] != 1)
        return;
    int offset = 1;
    int len1 = read_int(ct, &offset);
    int len2 = read_int(ct, &offset);
    GByteArray* cph_buf1 = g_byte_array_new();
    read_byte_array(ct, &offset, &cph_buf1);
    if(file_len != NULL)
        *file_len = len1;
    if(aes_bits != NULL)
        *aes_bits = len2;
    if(cph_buf != NULL)
        *cph_buf = cph_buf1;
    else
        g_byte_array_free(cph_buf1, 1);
}

void
write_cpabe_file(char* file, GByteArray* cph_buf,
        int file_len, GByteArray* aes_buf) {
    FILE* f;
    int i;

    f = open_write(file);

    /* write real file len as 32-bit big endian int */
    for (i = 3; i >= 0; i--)
        fputc((file_len & 0xff << (i * 8))>>(i * 8), f);

    /* write aes_buf */
    for (i = 3; i >= 0; i--)
        fputc((aes_buf->len & 0xff << (i * 8))>>(i * 8), f);
    fwrite(aes_buf->data, 1, aes_buf->len, f);

    /* write cph_buf */
    for (i = 3; i >= 0; i--)
        fputc((cph_buf->len & 0xff << (i * 8))>>(i * 8), f);
    fwrite(cph_buf->data, 1, cph_buf->len, f);

    fclose(f);
}

void
write_cpabetrans_file(char* file, GByteArray* cph_buf,
        int file_len, GByteArray* aes_buf, GByteArray* m) {
    FILE* f;
    int i;

    f = open_write(file);

    /* write real file len as 32-bit big endian int */
    for (i = 3; i >= 0; i--)
        fputc((file_len & 0xff << (i * 8))>>(i * 8), f);

    /* write aes_buf */
    for (i = 3; i >= 0; i--)
        fputc((aes_buf->len & 0xff << (i * 8))>>(i * 8), f);
    fwrite(aes_buf->data, 1, aes_buf->len, f);

    /* write cph_buf */
    for (i = 3; i >= 0; i--)
        fputc((cph_buf->len & 0xff << (i * 8))>>(i * 8), f);
    fwrite(cph_buf->data, 1, cph_buf->len, f);

    /* write trans_buf*/
    for (i = 3; i >= 0; i--)
        fputc((m->len & 0xff << (i * 8))>>(i * 8), f);
    fwrite(m->data, 1, m->len, f);

    fclose(f);
}

void read_cpabetrans_file(char* file, GByteArray** cph_buf,
        int* file_len, GByteArray** aes_buf, GByteArray** trans_buf) {
    FILE* f;
    int i;
    int len;

    *cph_buf = g_byte_array_new();
    *aes_buf = g_byte_array_new();
    *trans_buf = g_byte_array_new();

    f = open_read(file);
    
    /* read real file len as 32-bit big endian int */
    *file_len = 0;
    for (i = 3; i >= 0; i--)
        *file_len |= fgetc(f) << (i * 8);

    /* read aes buf */
    len = 0;
    for (i = 3; i >= 0; i--)
        len |= fgetc(f) << (i * 8);
    g_byte_array_set_size(*aes_buf, len);
    fread((*aes_buf)->data, 1, len, f);

    /* read cph buf */
    len = 0;
    for (i = 3; i >= 0; i--)
        len |= fgetc(f) << (i * 8);
    g_byte_array_set_size(*cph_buf, len);
    fread((*cph_buf)->data, 1, len, f);

    /* read trans buf */
    len = 0;
    for (i = 3; i >= 0; i--)
        len |= fgetc(f) << (i * 8);
    g_byte_array_set_size(*trans_buf, len);
    fread((*trans_buf)->data, 1, len, f);

    fclose(f);
}
