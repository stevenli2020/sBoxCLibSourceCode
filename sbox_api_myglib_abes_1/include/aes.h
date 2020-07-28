/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#ifndef AES_H
#define AES_H

#ifdef __cplusplus
extern "C" {
#endif 

#define AES_CCM_BLOCK 4096
#define AES_CCM_CBLOCK 4128
#define AES_CCM_BUF 4128

struct _AES_CCM{
    uint8_t* key;
    size_t key_bits;
    uint8_t buf[AES_CCM_BUF];
    size_t buf_len;
    char flag;
};

typedef struct _AES_CCM AES_CCM[1];
typedef struct _AES_CCM *AES_CCM_T;

/**
input : 	aes 	-> aes ccm mod handle
			bits	-> aes bits
			key		-> key value
			len 	-> key value byte length
			flag	-> 1 - encryption; 0 - decryption
output:		aes 	-> aes ccm mod handle
*/
void init_aes(AES_CCM_T aes, int bits, unsigned char* key, int len, char flag);

/**
input : 	aes 	-> aes ccm mod handle
			in		-> updated message
			in_len	-> updated message length
output:		out 	-> encrypted data
return:		int     -> encrypted data length (for example : 4096 message -> 4131 ciphertext); if < 0, encrypting failed!
*/
size_t aes_update(AES_CCM_T aes, unsigned char* in, size_t in_len, unsigned char* out);

/**
this operation will delete key
input : 	aes 	-> aes ccm mod handle
			in		-> updated message
			in_len	-> updated message length
output:		out 	-> encrypted data
return:		int     -> encrypted data length
*/
size_t aes_dofinal1(AES_CCM_T aes, unsigned char* in, size_t in_len, unsigned char* out);

/**
this operation will delete key
input : 	aes 	-> aes ccm mod handle
output:		out 	-> encrypted data
return:		int     -> encrypted data length
*/
size_t aes_dofinal0(AES_CCM_T aes, unsigned char* out);

/**
encrypt a file fin and output the cipher into fout
input : 	aes 	-> aes ccm mod handle
		 	fin 	-> filename to be encrypted
			fout 	-> encrypted filename
return:		int     -> encrypted data length
*/
size_t aes_execute_file(AES_CCM_T aes, FILE* inf, FILE* outf);

void clear_aes(AES_CCM_T aes);

/**
this operation can get the output len, which could be used to allocate memory
input : 	aes 	-> aes ccm mod handle
			in_len 	-> input length
return:		int     -> predicted output length
*/
size_t aes_get_out_len_update(AES_CCM_T aes, size_t in_len);

/**
this operation can get the output len, which could be used to allocate memory
input : 	aes 	-> aes ccm mod handle
			in_len 	-> input length
return:		int     -> predicted output length
*/
size_t aes_get_out_len_dofinal(AES_CCM_T aes, size_t in_len);

#ifdef __cplusplus
}
#endif

#endif /* AES_H */

