/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#ifndef CPABE_AES_H
#define CPABE_AES_H

#ifdef __cplusplus
extern "C" {
#endif

#define CPABE_AES_BLOCK 4096
#define CPABE_AES_CBLOCK 4128
#define CPABE_AES_BUF 4128

struct _AES_CCM{
    uint8_t* key;
    size_t key_bits;
    uint8_t buf[CPABE_AES_BUF];
    size_t buf_len;
    char flag;
};

typedef struct _AES_CCM cpabe_aes_t[1];
typedef struct _AES_CCM *cpabe_aes_ptr;

void init_cpabe_aes(cpabe_aes_ptr aes, unsigned char* key, int bits, char flag);

size_t cpabe_aes_update(cpabe_aes_ptr aes, unsigned char* in, size_t in_len, unsigned char* out);

size_t cpabe_aes_dofinal1(cpabe_aes_ptr aes, unsigned char* in, size_t in_len, unsigned char* out);

size_t cpabe_aes_dofinal0(cpabe_aes_ptr aes, unsigned char* out);

size_t cpabe_aes_execute_file(cpabe_aes_ptr aes, FILE* inf, FILE* outf);

void clear_cpabe_aes(cpabe_aes_ptr aes);

size_t cpabe_aes_get_out_len_update(cpabe_aes_ptr aes, size_t in_len);

size_t cpabe_aes_get_out_len_dofinal(cpabe_aes_ptr aes, size_t in_len);
    
#ifdef __cplusplus
}
#endif

#endif /* CPABE_AES_H */
