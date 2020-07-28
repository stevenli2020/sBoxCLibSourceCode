/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#ifdef EXE
#ifdef TEST
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "abe.h"
#include "sbox.h"
#include "aes.h"

void sbox_test(){
    char *pp, *msk, *abec, *abetc, *dk, *tk;
    unsigned char *m;
    char** attrs = malloc(4*sizeof(char*));
    attrs[0] = "attr1";
    attrs[1] = "attr5";
    attrs[2] = "attr6";
    attrs[3] = NULL;
    char* policy = "(attr1|attr2|attr3|attr4)&((attr5&attr6)|(attr5|attr6))";

    int res;
    int mlen;

    pp = NULL;
    msk = NULL;
    res = sbox_init("SS512", &pp, &msk);
    printf("pp = %s\n", pp);
    printf("msk = %s\n", msk);

    dk = NULL;
    tk = NULL;
    res = sbox_gen_uks(pp, msk, attrs, 3, &tk, &dk);
    printf("tk = %s\n", tk);
    printf("dk = %s\n", dk);

    abec = NULL;
    m = NULL;
    mlen = sbox_enc(pp, policy, &abec, &m);
    printf("abec = %s\n", abec);
    printf("m    = %s\n", g_base64_encode(m, mlen));

    abetc = NULL;
    res = sbox_transform(pp, tk, abec, &abetc);
    printf("abetc = %s\n", abetc);

    unsigned char *rm;
    res = sbox_dec(pp, dk, abetc, &rm);
    printf("res --> %d, %d\n", res, 
        strcmp(g_base64_encode(m, mlen), g_base64_encode(rm, res)));
    printf("rm = %s\n", g_base64_encode(rm, res));

    res = sbox_re_enc(pp, dk, abetc, policy, &abec);
    printf("abec = %s\n", abec);

    abetc = NULL;
    res = sbox_transform(pp, tk, abec, &abetc);
    printf("abetc = %s\n", abetc);

    rm = NULL;
    res = sbox_dec(pp, dk, abetc, &rm);
    printf("res --> %d, %d\n", res, 
        strcmp(g_base64_encode(m, mlen), g_base64_encode(rm, res)));
    printf("rm = %s\n", g_base64_encode(rm, res));
}

void aes_test(){
    AES_CCM aes_t;
    int len = 0;
    char key[20] = "Hello World!";
    init_aes(aes_t, 256, key, strlen(key), 1);

    char m[5000];
    char c[20000];
    char r[20000];
    char *t = c;
    memset(m, 'k', 5000);
    len += aes_update(aes_t, m, 5000, c + len);
    len += aes_update(aes_t, m, 5000, c + len);
    len += aes_dofinal1(aes_t, m, 5000, c + len);
    printf("%d\n", len);

    int rlen = 0;
    clear_aes(aes_t);
    init_aes(aes_t, 256, key, strlen(key), 0);
    rlen += aes_update(aes_t, c, 5000, r + rlen);
    rlen += aes_update(aes_t, c + 5000, 5000, r + rlen);
    rlen += aes_dofinal1(aes_t, c + 10000, len - 10000, r + rlen);
    printf("%d\n", rlen);
    r[rlen] = 0;
    printf("%s\n", r);
}

void main(){
    printf("TEST SBOX ----------------------------------\n");
    sbox_test();
    printf("TEST AES ----------------------------------\n");
    aes_test();
}
#endif
#endif
