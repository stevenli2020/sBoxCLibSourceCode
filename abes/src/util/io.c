/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>
#include "glib.h"
#include "util/io.h"
#include "util/common.h"

FILE* open_file(const char* file, char* mode) {
    FILE* f;
    if (!(f = fopen(file, mode)))
        return NULL;
    return f;
}

FILE* open_read(const char* const file) {
    return open_file(file, "rb");
}

FILE* open_write(const char* const file) {
    return open_file(file, "wb+");
}

int write_bytes_to_file(const char* filename, GByteArray* byteArray, int free) {
    return write_bytes_to_file_ex(filename, byteArray, free, 1);
}

int overwrite_bytes_to_file(const char* filename, GByteArray* byteArray, int free) {
    return write_bytes_to_file_ex(filename, byteArray, free, 0);
}

int write_bytes_to_file_ex(const char* filename, GByteArray* byteArray, int free, int backup) {
    FILE* cfgFile;
    if (backup && access(filename, F_OK) != 1) {
        GString* v = g_string_new(filename);
        g_string_append_printf(v, "%ld", time(NULL));
        rename(filename, v->str);
        g_string_free(v, 1);
    }
    if (cfgFile = open_write(filename)) {
        fwrite(byteArray->data, 1, byteArray->len, cfgFile);
        fclose(cfgFile);
        if (free)
            g_byte_array_free(byteArray, 1);
        return 1;
    }
    return 0;
}

GByteArray* read_string_from_file(const char* file) {
    GByteArray * array = read_bytes_from_file(file);
    return append_null(array);
}

GByteArray* read_bytes_from_file(const char* file) {
    FILE* f;
    struct stat s;
    stat(file, &s);
    f = open_read(file);
    if (f) {
        unsigned char buf[s.st_size];
        fread(buf, 1, s.st_size, f);
        fclose(f);
        GByteArray* a = g_byte_array_new();
        g_byte_array_append(a, buf, s.st_size);
        return a;
    }
    return NULL;
}

void write_int(GByteArray* byteArray, const int value) {
    size_t len = sizeof(value);
    guint8 a[len];
    for (int i = 0; i < len ; i++) {
        a[i] = (value >> (8 * (3-i))) & 0xff;
    }
    g_byte_array_append(byteArray, a, len);
}

int read_int(GByteArray* byteArray, int* offset) {
    if(*offset >= byteArray->len)
        return -1;
    int res = 0;
    size_t len = sizeof(res);
    unsigned char * buf = byteArray->data + *offset;
    for (int i = 0; i < len ; i++) {
        res |= buf[i] << (8 * (3-i));
    }
    *offset = *offset + len;
    return res;
}

void write_string(GByteArray* byteArray, const char* value) {
    int len = strlen(value);
    write_int(byteArray, len);
    g_byte_array_append(byteArray, (unsigned char*) value, len);
}

char* read_string(GByteArray* byteArray, int* offset) {
    int size = read_int(byteArray, offset);
    char* value = malloc(sizeof (unsigned char)*(size + 1));
    memcpy(value, byteArray->data + (*offset), size);
    (*offset) += size;
    value[size] = '\0';
    return value;
}

void write_byte_array(GByteArray* b, GByteArray* newData) {
    int len = newData->len;
    write_int(b, len);
    g_byte_array_append(b, newData->data, len);
}

void read_byte_array(GByteArray* b, int* offset, GByteArray** newData) {
    int orglen = read_int(b, offset);
    int len = orglen;
    int rlen = (b->len - *offset);
    if (rlen < len)
        len = rlen;
    unsigned char* buf = b->data + *offset;
    *offset += len;
    *newData = g_byte_array_new();
    g_byte_array_append(*newData, buf, len);
}

void read_to_size(int connfd, int remainSize, GByteArray* newData) {
    char buff_data[4096];
    int readSize = sizeof (buff_data) - 1;
    while (remainSize) {
        int rSize = readSize;
        if (remainSize < readSize) {
            rSize = remainSize;
        }
        int nread = read(connfd, buff_data, rSize);
        g_byte_array_append(newData, buff_data, nread);
        remainSize -= nread;
    }
}

GByteArray* read_byte_array_data(GByteArray* data, int* rlen, int* offset, int datasize, int connfd) {
    GByteArray * dataByteArray = g_byte_array_new();
    if (*offset == *rlen) {
        char buff_data[4096];
        int readSize = sizeof (buff_data) - 1;
        int nread = read(connfd, buff_data, readSize);
        *rlen = nread;
        *offset = 0;
        g_byte_array_set_size(data, 0);
        g_byte_array_append(data, buff_data, nread);
    }
    if (*offset < *rlen)
        read_byte_array(data, offset, &dataByteArray);

    int remainSize = datasize - dataByteArray->len;
    if (remainSize) {
        read_to_size(connfd, remainSize, dataByteArray);
    }
    return dataByteArray;
}
