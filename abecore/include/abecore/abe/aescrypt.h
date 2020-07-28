
#ifndef AESCRYPT_H
#define AESCRYPT_H

#ifdef __cplusplus
extern "C" {
#endif


    GByteArray* aes_128_cbc_encrypt(GByteArray* pt, element_t k);
    GByteArray* aes_128_cbc_decrypt(GByteArray* ct, element_t k);
    GByteArray* aes_ccm_encrypt_ele(GByteArray* pt, element_t k, int bits);
    GByteArray* aes_ccm_decrypt_ele(GByteArray* ct, element_t k, int bits);
    GByteArray* aes_ccm_encrypt(GByteArray* pt, const unsigned char* k, int bits);
    GByteArray* aes_ccm_decrypt(GByteArray* ct, const unsigned char* k, int bits);
    GByteArray* aes_256_ccm_encrypt(GByteArray* pt, element_t k);
    GByteArray* aes_256_ccm_decrypt(GByteArray* ct, element_t k);


#ifdef __cplusplus
}
#endif

#endif /* AESCRYPT_H */
