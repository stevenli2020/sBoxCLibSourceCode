/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "abe.h"
#include "sbox.h"

char* trim_str(char * src){
    char *res = src;
    int flag = 1;
    while(1){
        if(res[0] == ' '){
            res += 1;
        }else{
            break;
        }
    }
    char *end = res + strlen(res) - 1;
    while(end != res){
        if(end[0] == ' '){
            end[0] = 0;
            end -= 1;
        }else{
            break;
        }
    }
    return res;
}
typedef struct {
    char p[100];
    PairingScheme s;
}_Scheme;
typedef _Scheme *Scheme;

Scheme get_scheme(char* token){
    if(token == NULL)
        return NULL;
    char* p;
    Scheme s = malloc(sizeof(_Scheme));
    if(!strcmp(token, "SS512")){
        p = "data/a512.properties";
        s->s = SS512;
    }else if(!strcmp(token, "MNT159")){
        p = "data/d159.properties";
        s->s = MNT159;
    }else if(!strcmp(token, "MNT202")){
        p = "data/d202.properties";
        s->s = MNT202;
    }else if(!strcmp(token, "MNT224")){
        p = "data/d224.properties";
        s->s = MNT224;
    }else if(!strcmp(token, "MNT359")){
        p = "data/d359.properties";
        s->s = MNT359;
    }else if(!strcmp(token, "BN160")){
        p = "data/f160.properties";
        s->s = BN160;
    }else if(!strcmp(token, "BN256")){
        p = "data/f256.properties";
        s->s = BN256;
    }else if(!strcmp(token, "BN512")){
        p = "data/f512.properties";
        s->s = BN512;
    }else
        return NULL;
    strncpy(s->p, p, strlen(p)+1);
    return s;
}

PairingGroupPtr get_group_from_token(char* token){
    Scheme s = get_scheme(token);
    if(token == NULL)
        return NULL;
    
    PairingFactoryPtr factory = new_PairingFactory();
    PairingGroupPtr group;
    if(!access(s->p, 0))
        group = factory->loadPairing(s->p);
    else{
        group = factory->createScheme(s->s);
        group->save(group, s->p);
    }
    delete_PairingFactory(factory);
    return group;
}

int sbox_init(char* token, char** pp, char** msk){
    int res = 0;
    PairingGroupPtr group = get_group_from_token(token);
    if(group == NULL){
        return res;
    }
    cca_eodabe_msk_t* rmsk;
    cca_eodabe_pub_t* rpub;
    res = cca_eodabe_setup_1(group->pairing, group->pairingDesc, &rpub, &rmsk);
    if(res){
        *pp = encode64_str(cca_eodabe_pub_serialize(rpub));
        *msk = encode64_str(cca_eodabe_msk_serialize(rmsk));
    }
    cca_eodabe_msk_free(rmsk);
    cca_eodabe_pub_free(rpub);
    return res;
    
}

char* sbox_genABEUUID(char* pp){
    pairing_t pairing;
    int res_flag = extract_pairing_from_pub_bytes(decode64_str(pp), 1, pairing);
    if(!res_flag)
        return NULL;

    element_t a;
    element_init_Zr(a, pairing);
    element_random(a);
    int len = element_length_in_bytes(a);
    unsigned char buf[len];
    element_to_bytes(buf, a);
    char hexbuf[3];
    char * res = malloc((2*len+1)*sizeof(char));
    for(int i = 0; i < len; i++){
        sprintf(hexbuf, "%02X", buf[i]);
        res[i*2] = hexbuf[0];
        res[i*2+1] = hexbuf[1];
    }
    res[2*len] = 0;

    element_clear(a);
    pairing_clear(pairing);
    return res;    
}

int sbox_gen_uks(char* pp, char* imsk, char** attrs, int len, char** tk_s, char** dk_s){
    int res = 0;
    cca_eodabe_pub_t* pub = cca_eodabe_pub_unserialize(decode64_str(pp), 1);
    cca_eodabe_msk_t* msk = cca_eodabe_msk_unserialize(pub->p, decode64_str(imsk), 1);
    cca_eodabe_tk_t* tk;
    cca_eodabe_dk_t* dk;
    res = cca_eodabe_keygen(pub, msk, attrs, &tk, &dk);
    if(res){
        *tk_s = encode64_str(cca_eodabe_tk_serialize(tk));
        *dk_s = encode64_str(cca_eodabe_dk_serialize(dk));
        cca_eodabe_tk_free(tk);
        cca_eodabe_dk_free(dk);
    }
    cca_eodabe_msk_free(msk);
    cca_eodabe_pub_free(pub);
    return res;
}

int sbox_transform(char* pp, char* itk, char* abec, char** abetc){
    int res = 0;

    pairing_t pairing;
    res = extract_pairing_from_pub_bytes(decode64_str(pp), 1, pairing);
    if(!res)
        return 0;

    cca_eodabe_tk_t* tk = cca_eodabe_tk_unserialize(pairing, decode64_str(itk), 1);
    GByteArray* cph_buf = decode64_str(abec);
    cca_eodabe_cph_t* cph = cca_eodabe_cph_partial_unserialize(pairing, cph_buf);
    cca_eodabe_tcph_t* trans;
    
    res = cca_eodabe_trans(tk, cph, &trans);
    if(res){
        *abetc = encode64_str(cca_eodabe_cph_recompress_c2(pairing, trans, cph_buf, 1));
        cca_eodabe_tcph_free(trans);
    }
    cca_eodabe_cph_free(cph);
    cca_eodabe_tk_free(tk);
    pairing_clear(pairing);
    return res;
}

// output m_b , the encrypted m; output int, m_b length; output abec, abe ciphertext
int sbox_enc(char* pp, char* policy, char** abec, unsigned char** m_b){
    int res = 0;
    cca_eodabe_pub_t* pub = cca_eodabe_pub_unserialize(decode64_str(pp), 1);
    cca_eodabe_cph_t* cph;

    element_t m;
    element_init_GT(m, pub->p);
    element_random(m);
    res = cca_eodabe_enc(pub, m, policy, &cph);
    if(res){
        *abec = encode64_str(cca_eodabe_cph_serialize(cph));
        res = element_length_in_bytes(m);
        *m_b = malloc(res);
        element_to_bytes(*m_b, m);
        cca_eodabe_cph_free(cph);
    }
    element_clear(m);
    cca_eodabe_pub_free(pub);
    return res;
}

int sbox_dec(char* pp, char* idk, char* abetc, unsigned char** m_b){
    int res = 0;
    cca_eodabe_pub_t* pub = cca_eodabe_pub_unserialize(decode64_str(pp), 1);
    cca_eodabe_dk_t* dk = cca_eodabe_dk_unserialize(pub->p, decode64_str(idk), 1);
    cca_eodabe_tcph_t* tcph = cca_eodabe_tcph_unserialize(pub->p, decode64_str(abetc), 1);
    element_t m;
    res = cca_eodabe_dec(pub, dk, tcph, m);
    if(res){
        res = element_length_in_bytes(m);
        *m_b = malloc(res);
        element_to_bytes(*m_b, m);
        element_clear(m);
    }

    cca_eodabe_tcph_free(tcph);
    cca_eodabe_dk_free(dk);
    cca_eodabe_pub_free(pub);
    return res;
}

int sbox_re_enc(char* pp, char* idk, char* abetc, char* npolicy, char** abec){
    int res = 0;
    cca_eodabe_pub_t* pub = cca_eodabe_pub_unserialize(decode64_str(pp), 1);
    cca_eodabe_dk_t* dk = cca_eodabe_dk_unserialize(pub->p, decode64_str(idk), 1);
    cca_eodabe_tcph_t* tcph = cca_eodabe_tcph_unserialize(pub->p, decode64_str(abetc), 1);
    cca_eodabe_cph_t* cph;

    element_t m;
    res = cca_eodabe_dec(pub, dk, tcph, m);
    if(res){
        res = cca_eodabe_enc(pub, m, npolicy, &cph);
        if(res){
            *abec = encode64_str(cca_eodabe_cph_serialize(cph));
            cca_eodabe_cph_free(cph);
        }
        element_clear(m);
    }
    cca_eodabe_tcph_free(tcph);
    cca_eodabe_dk_free(dk);
    cca_eodabe_pub_free(pub);
    return res;
}

#ifndef TEST
#ifdef EXE

int sbox(int argc, char** argv){
    int res;
    if(argc == 1){
        printf("Some arguements must be given! You may use \"sbox --help\" to get help!\n");
    }else if(strcmp(argv[1], "init") == 0){
        if(argc < 3)
            printf("The init function need 1 arguements but %d given! You may use \"sbox --help\" to get help!\n", argc - 2);
        else{
            char *token = trim_str(argv[2]);
            char * pp, * msk;
            res = sbox_init(token, &pp, &msk);
            if(res)
                printf("%s  ,  %s\n", pp, msk);
            else
                printf("Error\n");
        }
    }else if(strcmp(argv[1], "genUUID") == 0){
        if(argc < 3)
            printf("The genUUID function need 1 arguements but %d given! You may use \"sbox --help\" to get help!\n", argc - 2);
        else{
            char *pp = trim_str(argv[2]);
            char * rest = sbox_genABEUUID(pp);
            if(rest != NULL)
                printf("%s\n", rest);
            else
                printf("Error\n");
        }
    }else if(strcmp(argv[1], "genUKs") == 0){
        if(argc < 5)
            printf("The genUKs function need 3 arguements but %d given! You may use \"sbox --help\" to get help!\n", argc - 2);
        else{
            char *pp = trim_str(argv[2]);
            char *msk = trim_str(argv[3]);
            char *tmp = strtok(argv[4], ",");
            // char **attrsbuf = (char**)malloc(sizeof(char*)*20);
            GPtrArray* attrsbuf = g_ptr_array_new();
            char *tk, *dk;
            int i = 0;
            while(tmp != NULL){
                g_ptr_array_add(attrsbuf, trim_str(tmp));
                // attrsbuf[i] = trim_str(tmp);
                tmp = strtok(NULL, ",");
                i++;
            }
            // attrsbuf[i] = NULL;
            g_ptr_array_add(attrsbuf, NULL);
            if(tmp != NULL){
                printf("Error-Too Many Attributes\n");
            }else{
                // res = sbox_gen_uks(pp, msk, attrsbuf, i, &tk, &dk);
                res = sbox_gen_uks(pp, msk, (char**)(attrsbuf->pdata), i, &tk, &dk);
                if(res)
                    printf("%s  ,  %s\n", dk, tk);
                else
                    printf("Error\n");
            }
            
        }
    }else if(strcmp(argv[1], "trans") == 0){
        if(argc < 5)
            printf("The trans function need 3 arguements but %d given! You may use \"sbox --help\" to get help!\n", argc - 2);
        else{
            char *pp = trim_str(argv[2]);
            char *tk = trim_str(argv[3]);
            char *abec = trim_str(argv[4]);
            char * abetc;
            res = sbox_transform(pp, tk, abec, &abetc);
            if(res)
                printf("%s\n", abetc);
            else
                printf("TransFailed\n");
        }
    }else if(strcmp(argv[1], "enc") == 0){
        if(argc < 4){
            printf("The enc function need at least 2 arguements but %d given! You may use \"sbox --help\" to get help!\n", argc - 2);
        } else{
            char *pp = trim_str(argv[2]);
            char *policy = trim_str(argv[3]);
            char * abec, *ms;
            unsigned char *m;
            res = sbox_enc(pp, policy, &abec, &m);
            if(res){
                ms = g_base64_encode(m, res);
                printf("%s  ,  %s\n", abec, ms);
            }else
                printf("Error\n");
        }
    }else if(strcmp(argv[1], "encf") == 0){
        if(argc < 6){
            printf("The encf function need 4 arguements but %d given! You may use \"sbox --help\" to get help!\n", argc - 2);
        } else{
            char *pp = trim_str(argv[2]);
            char *policy = trim_str(argv[3]);
            char *mf = trim_str(argv[4]);
            char *cf = trim_str(argv[5]);
            char * abec;
            // res = sbox_enc_file(pp, 128, policy, mf, cf, &abec);
            // if(res){
            //     printf("%s\n", abec);
            //     free(abec);
            // }else
            //     printf("Error\n");
        }
    }else if(strcmp(argv[1], "dec") == 0){
        if(argc < 5)
            printf("The dec function need 3 arguements but %d given! You may use \"sbox --help\" to get help!\n", argc - 2);
        else{
            char *pp = trim_str(argv[2]);
            char *dk = trim_str(argv[3]);
            char *abetc = trim_str(argv[4]);
            char *ms;
            unsigned char *m;
            res = sbox_dec(pp, dk, abetc, &m);
            if(res){
                ms = g_base64_encode(m, res);
                printf("%s\n", ms);
            }else
                printf("DecFailed\n");
        }
    }else if(strcmp(argv[1], "reEnc") == 0){
        if(argc < 6)
            printf("The reEnc function need 5 arguements but %d given! You may use \"sbox --help\" to get help!\n", argc - 2);
        else{
            char *pp = trim_str(argv[2]);
            char *dk = trim_str(argv[3]);
            char *abetc = trim_str(argv[4]);
            char *npolicy = trim_str(argv[5]);
            char * nabec;
            res = sbox_re_enc(pp, dk, abetc, npolicy, &nabec);
            if(res){
                printf("%s\n", nabec);
            }else
                printf("ReEncFailed\n");
        }
    }else if(strcmp(argv[1], "--help") == 0){
        printf("--help;     Commands instruction;\n");
        printf("init;       Init public parameters;     propName --> pp, msk\n");
        printf("genUUID;    Generate ABE UUID;          pp --> uuid\n");
        printf("genUKs;     Generate user keys;         pp, attrs --> dk, tk\n");
        printf("%s\n%s", 
               "enc;        Encryption;                 ABE Encrypt, produce a message and its ciphertext abec :",
               "                                        pp, policystr --> abeCipher, m\n");
        printf("trans;      Transformation decryption;  pp, tk, abeCipher --> abetCipher\n");
        printf("dec;        Decrypt message;            Decrypt message : pp, dk, abetCipher --> m\n");
        printf("reEnc;      re-encrypt abet;            Re-encrypt data without aesc : pp, dk, abetCipher, new policy --> new abeCipher\n");
    }
}


int main(int argc, char** argv){
    sbox(argc, argv);
}
#endif
#endif
