/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/aes.h>
#include <openssl/evp.h>
#include <openssl/sha.h>
#include "glib.h"
#include "util/common.h"
#include "aes.h"

static int rand_seed = 0;

const EVP_CIPHER* get_ase_ccm(int bits){
    const EVP_CIPHER* aes_ccm = NULL;
    if(bits == 128)
        aes_ccm = EVP_aes_128_ccm();
    else if(bits == 192)
        aes_ccm = EVP_aes_192_ccm();
    else if(bits == 256)
        aes_ccm = EVP_aes_256_ccm();
    return aes_ccm;
}

GByteArray* aes_ccm_enc(GByteArray* pt, const unsigned char* key, const EVP_CIPHER* aes_ccm){
    int tag_len = 16;
    int n_len = 16;

    unsigned char ccm_n[n_len];
    srand((unsigned)time(NULL) + rand_seed++);
    for(int i = 0; i < n_len; i++){
        ccm_n[i] = (unsigned char) rand();
    }

    EVP_CIPHER_CTX *ctx;
    int outlen, tmplen;
    ctx = EVP_CIPHER_CTX_new();
    /* Set cipher type and mode */
    EVP_EncryptInit_ex(ctx, aes_ccm, NULL, NULL, NULL);
    /* Set nonce length if default 96 bits is not appropriate */
    EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_AEAD_SET_IVLEN, n_len,
                        NULL);
    /* Set tag length */
    EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_AEAD_SET_TAG, tag_len, NULL);
    /* Initialise key and IV */
    EVP_EncryptInit_ex(ctx, NULL, NULL, key, ccm_n);
    /* Set plaintext length: only needed if AAD is used */
    EVP_EncryptUpdate(ctx, NULL, &outlen, NULL, pt->len);

    int outlens = outlen;
    unsigned char outbuf[outlen];

    /* Zero or one call to specify any AAD */
    /* EVP_EncryptUpdate(ctx, NULL, &outlen, ccm_adata, sizeof(ccm_adata)); */
    /* Encrypt plaintext: can only be called once */
    EVP_EncryptUpdate(ctx, outbuf, &outlen, pt->data, pt->len);
    int outlenr = outlen;
    /* Output encrypted block */
    /* Finalise: note get no output for CCM */
    EVP_EncryptFinal_ex(ctx, outbuf, &outlen);

    /* Get tag */
    unsigned char tagbuf[tag_len];
    EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_AEAD_GET_TAG, tag_len, tagbuf);

    GByteArray* res = NULL;
    if(outlenr != outlens){
        printf("the out put len is not equal to expected!! %d - %d\n", outlenr, outlens);
    }else{
        res = g_byte_array_new();
        g_byte_array_append(res, ccm_n, n_len);
        g_byte_array_append(res, tagbuf, tag_len);
        g_byte_array_append(res, outbuf, outlens);
    }

    EVP_CIPHER_CTX_free(ctx);

    return res;
}

GByteArray* aes_ccm_dec(GByteArray* ct, const unsigned char* key, const EVP_CIPHER* aes_ccm){
    int tag_len = 16;
    int n_len = 16;


    EVP_CIPHER_CTX *ctx;
    int outlen, tmplen, rv;
    ctx = EVP_CIPHER_CTX_new();
    /* Select cipher */
    EVP_DecryptInit_ex(ctx, aes_ccm, NULL, NULL, NULL);
    /* Set nonce length, omit for 96 bits */
    EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_AEAD_SET_IVLEN, n_len,
                        NULL);

    /* Set expected tag value */
    EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_AEAD_SET_TAG,
                        tag_len, ct->data + n_len);
    /* Specify key and IV */
    EVP_DecryptInit_ex(ctx, NULL, NULL, key, ct->data);
    /* Set ciphertext length: only needed if we have AAD */
    EVP_DecryptUpdate(ctx, NULL, &outlen, NULL, ct->len - n_len - tag_len);

    int outlens = outlen;
    unsigned char outbuf[outlens];

    /* Zero or one call to specify any AAD */
    //EVP_DecryptUpdate(ctx, NULL, &outlen, ccm_adata, sizeof(ccm_adata));
    rv = EVP_DecryptUpdate(ctx, outbuf, &outlen, ct->data + n_len + tag_len, ct->len - n_len - tag_len);
    /* Output decrypted block: if tag verify failed we get nothing */
    GByteArray* res = NULL;
    if (rv > 0) {

        res = g_byte_array_new();
        g_byte_array_append(res, outbuf,outlen);
    } else{
        printf("Plaintext not available: tag verify failed.\n");
    }

    EVP_CIPHER_CTX_free(ctx);
    return res;
}
    
GByteArray* aes_ccm_encrypt(GByteArray* pt, const unsigned char* k, int bits){
    if(k == NULL || pt == NULL)
        return NULL;
    const EVP_CIPHER* aes_ccm = get_ase_ccm(bits);
    if(aes_ccm == NULL)
        return NULL;
    return aes_ccm_enc(pt, k, aes_ccm);
}

GByteArray* aes_ccm_decrypt(GByteArray* ct, const unsigned char* k, int bits){
    if(k == NULL || ct == NULL)
        return NULL;
    const EVP_CIPHER* aes_ccm = get_ase_ccm(bits);
    if(aes_ccm == NULL)
        return NULL;
    return aes_ccm_dec(ct, k, aes_ccm);
}

/* flag = 1, encryption; flag = 0, decryption*/
void init_aes(AES_CCM_T aes, int bits, unsigned char* key, int len, char flag){
    int k_len = bits/8;
    aes->key = malloc(k_len);
    GByteArray* hash = g_byte_array_new();
    g_byte_array_append(hash, key, len);
    hash = hash256_len(hash, k_len);
    memcpy(aes->key, hash->data, hash->len);
    aes->key_bits = bits;
    memset(aes->buf, 0, AES_CCM_BUF*sizeof(uint8_t));
    aes->buf_len = 0;
    if(flag > 0)
        aes->flag = 1;
    else
        aes->flag = 0;
    g_byte_array_free(hash, 1);
}

size_t aes_update(AES_CCM_T aes, unsigned char* in, size_t in_len, unsigned char* out){

    size_t sin_len, sout_len;
    if(aes->flag){
        sin_len = AES_CCM_BLOCK;
        sout_len = AES_CCM_CBLOCK;
    }else{
        sin_len = AES_CCM_CBLOCK;
        sout_len = AES_CCM_BLOCK;
    }

    size_t t_len = in_len + aes->buf_len;
    size_t in_off = 0;
    uint8_t* tmp;
    uint8_t* in_tmp;
    GByteArray* pt = g_byte_array_new();
    g_byte_array_append(pt, aes->buf, aes->buf_len);
    GByteArray* output;
    int out_len = 0;

    while(in_off < in_len){
        if(t_len >= sin_len){
            in_tmp = in + in_off;
            tmp = aes->buf + aes->buf_len;
            // memcpy(tmp, in_tmp, sin_len - aes->buf_len);
            // pt->len = sin_len;
            g_byte_array_append(pt, in_tmp, sin_len - aes->buf_len);
            if(aes->flag){
                output = aes_ccm_encrypt(pt, aes->key, aes->key_bits);
            }else{
                output = aes_ccm_decrypt(pt, aes->key, aes->key_bits);
            }
            tmp = out + out_len;
            memcpy(tmp, output->data, output->len);
            out_len += output->len;
            g_byte_array_free(output, 1);

            in_off += sin_len - aes->buf_len;
            aes->buf_len = 0;
            t_len -= sin_len;
        }else{
            in_tmp = in + in_off;
            tmp = aes->buf + aes->buf_len;
            memcpy(tmp, in_tmp, t_len - aes->buf_len);
            in_off += t_len - aes->buf_len;
            aes->buf_len = t_len;
        }
    }
    g_byte_array_free(pt, 1);
    return out_len;
}

size_t aes_dofinal1(AES_CCM_T aes, unsigned char* in, size_t in_len, unsigned char* out){
    int res = aes_update(aes, in, in_len, out);
    res += aes_dofinal0(aes, out + res);
    return res;
}

size_t aes_dofinal0(AES_CCM_T aes, unsigned char* out){
    size_t sin_len, sout_len;
    if(aes->flag){
        sin_len = AES_CCM_BLOCK;
        sout_len = AES_CCM_CBLOCK;
    }else{
        sin_len = AES_CCM_CBLOCK;
        sout_len = AES_CCM_BLOCK;
    }

    size_t t_len = aes->buf_len;
    if(t_len >= sin_len){
        printf("Wrong block size!!!\n");
        return -1;
    }

    int out_len = 0;
    GByteArray* pt = g_byte_array_new();
    g_byte_array_append(pt, aes->buf, aes->buf_len);
    GByteArray* output;

    if(aes->flag){
        output = aes_ccm_encrypt(pt, aes->key, aes->key_bits);
    }else{
        output = aes_ccm_decrypt(pt, aes->key, aes->key_bits);
    }
    memcpy(out, output->data, output->len);
    out_len = output->len;
    g_byte_array_free(output, 1);
    g_byte_array_free(pt, 1);
    aes->buf_len = 0;

    return out_len;
}

size_t aes_execute_file(AES_CCM_T aes, FILE* inf, FILE* outf){
    int il, ol;
    int res = 0;
    if(aes->flag){
        il = AES_CCM_BLOCK;
        ol = AES_CCM_CBLOCK;
    }else{
        il = AES_CCM_CBLOCK;
        ol = AES_CCM_BLOCK;
    }
    char in_buf[il];
    char out_buf[ol];
    int rl, wl;
    while(rl = fread(in_buf, 1, il, inf)){
        wl = aes_update(aes, in_buf, rl, out_buf);
        if(wl > 0){
            res += wl;
            fwrite(out_buf, 1, wl, outf);
        }
    }
    wl = aes_dofinal0(aes, out_buf);
    if(wl > 0){
        res += wl;
        fwrite(out_buf, 1, wl, outf);
    }
    return res;
}

void clear_aes(AES_CCM_T aes){
    free(aes->key);
    aes->key_bits = 0;
    memset(aes->buf, 0, AES_CCM_BUF*sizeof(uint8_t));
    aes->buf_len = 0;
    aes->flag = -1;
}

size_t aes_get_out_len_update(AES_CCM_T aes, size_t in_len){
    size_t t_len = aes->buf_len + in_len;
    int round, res, s1_len, s2_len;
    if(aes->flag){
        s1_len = AES_CCM_BLOCK;
        s2_len = AES_CCM_CBLOCK;
    }else {
        s1_len = AES_CCM_CBLOCK;
        s2_len = AES_CCM_BLOCK;
    }
    round = t_len / s1_len;
    res = round * s2_len;
    return res;
}

size_t aes_get_out_len_dofinal(AES_CCM_T aes, size_t in_len){
    size_t t_len = aes->buf_len + in_len;
    int round, res, s1_len, s2_len;
    if(aes->flag){
        s1_len = AES_CCM_BLOCK;
        s2_len = AES_CCM_CBLOCK;
    }else {
        s1_len = AES_CCM_CBLOCK;
        s2_len = AES_CCM_BLOCK;
    }
    round = t_len / s1_len;
    res = round * s2_len;
    int off = t_len % s1_len;
    if(off){
        if(aes->flag)
            // tag + nounce
            off += 16 + 16;
        else
            // remove tag + nounce
            off -= 16 + 16;
        res += off;
    }
    return res;
}

    


