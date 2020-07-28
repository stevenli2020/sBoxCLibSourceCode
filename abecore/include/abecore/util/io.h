/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include "glib.h"
#include "common.h"

#ifndef IO_H
#define IO_H

#ifdef __cplusplus
extern "C" {
#endif

    FILE* open_file(const char* file, char* mode);
    FILE* open_read(const char* const file);
    FILE* open_write(const char* const file);

    int write_bytes_to_file(const char* filename, GByteArray* byteArray, int free);
    int overwrite_bytes_to_file(const char* filename, GByteArray* byteArray, int free);
    int write_bytes_to_file_ex(const char* filename, GByteArray* byteArray, int free, int backup);
    GByteArray * read_bytes_from_file(const char* file);
    GByteArray * read_string_from_file(const char* file);

    char* read_string(GByteArray* byteArray, int* offset);
    void write_string(GByteArray* byteArray, const char* value);
    int read_int(GByteArray* byteArray, int* offset);
    void write_int(GByteArray * byteArray, const int value);

    void write_byte_array(GByteArray* b, GByteArray * newData);
    void read_byte_array(GByteArray* b, int* offset, GByteArray** newData);

    void read_to_size(int connfd, int remainSize, GByteArray * newData);
    GByteArray* read_byte_array_data(GByteArray* data, int* rlen, int* offset, int datasize, int connfd);
#ifdef __cplusplus
}
#endif

#endif /* IO_H */

