#include <string.h>
#include <time.h>
#include <glib.h>
#include <openssl/aes.h>
#include <openssl/evp.h>
#include <openssl/sha.h>
#include <pbc.h>
#include "abe/aescrypt.h"
#include "util/abeutil.h"

void 
init_aes(element_t k, int enc, AES_KEY* key, unsigned char* iv) {
    int key_len;
    unsigned char* key_buf;

    key_len = element_length_in_bytes(k) < 17 ? 17 : element_length_in_bytes(k);
    key_buf = (unsigned char*) malloc(key_len);
    element_to_bytes(key_buf, k);

    if (enc)
        AES_set_encrypt_key(key_buf + 1, 128, key);
    else
        AES_set_decrypt_key(key_buf + 1, 128, key);
    free(key_buf);

    memset(iv, 0, 16);
}

GByteArray* aes_128_cbc_encrypt(GByteArray* pt, element_t k) {
    AES_KEY key;
    unsigned char iv[16];
    GByteArray* ct;
    guint8 len[4];
    guint8 zero;

    init_aes(k, 1, &key, iv);

    /* TODO make less crufty */

    /* stuff in real length (big endian) before padding */
    len[0] = (pt->len & 0xff000000) >> 24;
    len[1] = (pt->len & 0xff0000) >> 16;
    len[2] = (pt->len & 0xff00) >> 8;
    len[3] = (pt->len & 0xff) >> 0;
    g_byte_array_prepend(pt, len, 4);

    /* pad out to multiple of 128 bit (16 byte) blocks */
    zero = 0;
    while (pt->len % 16)
        g_byte_array_append(pt, &zero, 1);

    ct = g_byte_array_new();
    g_byte_array_set_size(ct, pt->len);

    AES_cbc_encrypt(pt->data, ct->data, pt->len, &key, iv, AES_ENCRYPT);

    return ct;
}

GByteArray* aes_128_cbc_decrypt(GByteArray* ct, element_t k) {
    AES_KEY key;
    unsigned char iv[16];
    GByteArray* pt;
    unsigned int len;

    init_aes(k, 0, &key, iv);

    pt = g_byte_array_new();
    g_byte_array_set_size(pt, ct->len);

    AES_cbc_encrypt(ct->data, pt->data, ct->len, &key, iv, AES_DECRYPT);

    /* TODO make less crufty */

    /* get real length */
    len = 0;
    len = len
            | ((pt->data[0]) << 24) | ((pt->data[1]) << 16)
            | ((pt->data[2]) << 8) | ((pt->data[3]) << 0);
    g_byte_array_remove_index(pt, 0);
    g_byte_array_remove_index(pt, 0);
    g_byte_array_remove_index(pt, 0);
    g_byte_array_remove_index(pt, 0);

    /* truncate any garbage from the padding */
    g_byte_array_set_size(pt, len);

    return pt;
}

static rand_seed = 0;

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
        /*
        printf("Plaintext: %d- %d\n", outlen, outlens);
        printf("is ---> ");
        for (int i = 0; i < outlen; i ++) {
            printf(" %2x", outbuf[i]);
        } 
        printf("\n"); */

        res = g_byte_array_new();
        g_byte_array_append(res, outbuf,outlen);
    } else{
        printf("Plaintext not available: tag verify failed.\n");
    }

    EVP_CIPHER_CTX_free(ctx);
    return res;
}

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

GByteArray* aes_ccm_encrypt_ele(GByteArray* pt, element_t k, int bits){
    const EVP_CIPHER* aes_ccm = get_ase_ccm(bits);
    if(aes_ccm == NULL)
        return NULL;
    GByteArray* ccm_k = hash256_ele_len(k, bits);
    return aes_ccm_enc(pt, ccm_k->data, aes_ccm);
}

GByteArray* aes_ccm_decrypt_ele(GByteArray* ct, element_t k, int bits){
    const EVP_CIPHER* aes_ccm = get_ase_ccm(bits);
    if(aes_ccm == NULL)
        return NULL;
    GByteArray* ccm_k = hash256_ele_len(k, bits);
    return aes_ccm_dec(ct, ccm_k->data, aes_ccm);
}
GByteArray* aes_256_ccm_encrypt(GByteArray* pt, element_t k){
    return aes_ccm_encrypt_ele(pt, k, 256);
}
GByteArray* aes_256_ccm_decrypt(GByteArray* ct, element_t k){
    return aes_ccm_decrypt_ele(ct, k, 256);
}


