#include <string.h>
#include <time.h>
#include <glib.h>
#include <openssl/evp.h>
#include <pbc.h>
#include "abe/aescrypt.h"
#include "util/abeutil.h"
#include "abe/cpabe/cpabe_aes.h"


/* flag = 1, encryption; flag = 0, decryption*/
void init_cpabe_aes(cpabe_aes_ptr aes, unsigned char* key, int bits, char flag){
    aes->key = malloc(bits*sizeof(unsigned char));
    memcpy(aes->key, key, bits);
    aes->key_bits = bits;
    memset(aes->buf, 0, CPABE_AES_BUF*sizeof(uint8_t));
    aes->buf_len = 0;
    if(flag > 0)
        aes->flag = 1;
    else
        aes->flag = 0;
}

size_t cpabe_aes_update(cpabe_aes_ptr aes, unsigned char* in, size_t in_len, unsigned char* out){

    size_t sin_len, sout_len;
    if(aes->flag){
        sin_len = CPABE_AES_BLOCK;
        sout_len = CPABE_AES_CBLOCK;
    }else{
        sin_len = CPABE_AES_CBLOCK;
        sout_len = CPABE_AES_BLOCK;
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

size_t cpabe_aes_dofinal1(cpabe_aes_ptr aes, unsigned char* in, size_t in_len, unsigned char* out){
    int res = cpabe_aes_update(aes, in, in_len, out);
    res += cpabe_aes_dofinal0(aes, out + res);
    return res;
}

size_t cpabe_aes_dofinal0(cpabe_aes_ptr aes, unsigned char* out){
    size_t sin_len, sout_len;
    if(aes->flag){
        sin_len = CPABE_AES_BLOCK;
        sout_len = CPABE_AES_CBLOCK;
    }else{
        sin_len = CPABE_AES_CBLOCK;
        sout_len = CPABE_AES_BLOCK;
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

size_t cpabe_aes_execute_file(cpabe_aes_ptr aes, FILE* inf, FILE* outf){
    int il, ol;
    int res = 0;
    if(aes->flag){
        il = CPABE_AES_BLOCK;
        ol = CPABE_AES_CBLOCK;
    }else{
        il = CPABE_AES_CBLOCK;
        ol = CPABE_AES_BLOCK;
    }
    char in_buf[il];
    char out_buf[ol];
    int rl, wl;
    while(rl = fread(in_buf, 1, il, inf)){
        wl = cpabe_aes_update(aes, in_buf, rl, out_buf);
        if(wl > 0){
            res += wl;
            fwrite(out_buf, 1, wl, outf);
        }
    }
    wl = cpabe_aes_dofinal0(aes, out_buf);
    if(wl > 0){
        res += wl;
        fwrite(out_buf, 1, wl, outf);
    }
    return res;
}

void clear_cpabe_aes(cpabe_aes_ptr aes){
    free(aes->key);
    aes->key_bits = 0;
    memset(aes->buf, 0, CPABE_AES_BUF*sizeof(uint8_t));
    aes->buf_len = 0;
    aes->flag = -1;
}

size_t cpabe_aes_get_out_len_update(cpabe_aes_ptr aes, size_t in_len){
    size_t t_len = aes->buf_len + in_len;
    int round, res, s1_len, s2_len;
    if(aes->flag){
        s1_len = CPABE_AES_BLOCK;
        s2_len = CPABE_AES_CBLOCK;
    }else {
        s1_len = CPABE_AES_CBLOCK;
        s2_len = CPABE_AES_BLOCK;
    }
    round = t_len / s1_len;
    res = round * s2_len;
    return res;
}

size_t cpabe_aes_get_out_len_dofinal(cpabe_aes_ptr aes, size_t in_len){
    size_t t_len = aes->buf_len + in_len;
    int round, res, s1_len, s2_len;
    if(aes->flag){
        s1_len = CPABE_AES_BLOCK;
        s2_len = CPABE_AES_CBLOCK;
    }else {
        s1_len = CPABE_AES_CBLOCK;
        s2_len = CPABE_AES_BLOCK;
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

    
