/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cpabe.h"
#include "sbox.h"

GByteArray* decode64_from(char* src){
    gsize size = 0;
    unsigned char * buf = g_base64_decode(src, &size);
    GByteArray* res = g_byte_array_new();
    g_byte_array_append(res, buf, size);
    free(buf);
    return res;
}

void print_g_array(GByteArray* data){
    for(int i = 0; i < data->len; i++){
        printf("%02X ", data->data[i]);
        if(i % 10 == 9)printf("\n");
    }printf("\n");
}

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



PairingGroupPtr get_pairing_from_token(char* token){
    if(token == NULL)
        return NULL;
    char* prop_f;
    GHashTable * t = g_hash_table_new(g_str_hash, g_str_equal);
    if(strcmp(token, "SS512") == 0){
        prop_f = "data/a512.properties";
        g_hash_table_insert(t, PAIRING_TYPE, PAIRING_NAMES[TYPE_A]);
    } else if(strcmp(token, "MNT159") == 0){
        prop_f = "data/d159.properties";
        g_hash_table_insert(t, PAIRING_TYPE, PAIRING_NAMES[TYPE_D]);
    } else if(strcmp(token, "MNT202") == 0){
        prop_f = "data/d202.properties";
        g_hash_table_insert(t, PAIRING_TYPE, PAIRING_NAMES[TYPE_D]);
    } else if(strcmp(token, "MNT224") == 0){
        prop_f = "data/d224.properties";
        g_hash_table_insert(t, PAIRING_TYPE, PAIRING_NAMES[TYPE_D]);
    } else if(strcmp(token, "MNT359") == 0){
        prop_f = "data/d359.properties";
        g_hash_table_insert(t, PAIRING_TYPE, PAIRING_NAMES[TYPE_D]);
    } else if(strcmp(token, "BN160") == 0){
        prop_f = "data/f160.properties";
        g_hash_table_insert(t, PAIRING_TYPE, PAIRING_NAMES[TYPE_F]);
    } else if(strcmp(token, "BN256") == 0){
        prop_f = "data/f256.properties";
        g_hash_table_insert(t, PAIRING_TYPE, PAIRING_NAMES[TYPE_F]);
    } else if(strcmp(token, "BN512") == 0){
        prop_f = "data/f512.properties";
        g_hash_table_insert(t, PAIRING_TYPE, PAIRING_NAMES[TYPE_F]);
    } else {
        return NULL;
    }
    g_hash_table_insert(t, PAIRING_CONFIG, prop_f);
    GroupContextPtr context = new_GroupContext(t);

    CPABECryptPtr cpabe;
    PairingFactoryPtr factory = new_PairingFactory();
    PairingGroupPtr pairingGrp = factory->createPairing(context);
    delete_GroupContext(context);
    delete_PairingFactory(factory);
    g_hash_table_remove_all(t);
    g_hash_table_destroy(t);
    return pairingGrp;
}

int sbox_init(char* token, char** pp, char** msk){
    int res = 0;
    PairingGroupPtr pairingGrp = get_pairing_from_token(token);
    if(pairingGrp == NULL){
        return res;
    }

    CPABEKeyGenPtr cpabeKG = new_CPABEODKeyGen(pairingGrp);

    MasterKeyPairPtr mkp = NULL;
    mkp = cpabeKG->setup(cpabeKG);
    if(mkp != NULL){
        res = 1;
        //strncpy(((CPABEMasterPKPtr)mkp->mpk->derivedMPK)->pub->pairing_desc, token, strlen(token)+1);
        GByteArray* mskb = mkp->msk->serialize(mkp->msk);  
        GByteArray* mpkb = mkp->mpk->serialize(mkp->mpk);

        GByteArray* ppb = g_byte_array_new();
        write_string(ppb, token);
        write_byte_array(ppb, mpkb);

        *msk = g_base64_encode(mskb->data, mskb->len);
        *pp = g_base64_encode(ppb->data, ppb->len);
        g_byte_array_free(mskb, 1);
        g_byte_array_free(mpkb, 1);
        g_byte_array_free(ppb, 1);
        //msk first , then mpk. because mpk contains the pairing
        delete_CPABEMasterSK(mkp->msk);
        delete_CPABEMasterPK(mkp->mpk);
        delete_MasterKeyPair(mkp);
    }
    delete_CPABEKeyGen(cpabeKG);
    delete_PairingGroup(pairingGrp);
    return res;
    
}

int get_pp_struct(char* pp, PairingGroupPtr* pairingGrp, MasterPKPtr* mpk){
    int res = 0;
    GByteArray* ppb1 = decode64_from(pp);

    int offset = 0;

    char* token = read_string(ppb1, &offset);

    GByteArray* ppb2 = g_byte_array_new();
    read_byte_array(ppb1, &offset, &ppb2);

    res = 1;
    if(pairingGrp != NULL){
        *pairingGrp = get_pairing_from_token(token);
    }
    if(mpk != NULL){ 
        *mpk = new_CPABEMasterPKDef();
        // releaf pointer ppb2 in unserialize
        (*mpk)->unserialize(*mpk, ppb2);
    }
    free(token);
    g_byte_array_free(ppb1, 1);
    return res;
}

char* sbox_genABEUUID(char* pp){
    PairingGroupPtr p;
    get_pp_struct(pp, &p, NULL);
    element_t a;
    element_init_Zr(a, p->pairing);
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
    delete_PairingGroup(p);
    return res;    
}

int sbox_gen_uks(char* pp, char* msk, char** attrs, int len, char** tk, char** dk){
    int res = 0;
    MasterPKPtr mpk;
    MasterSKPtr mssk;

    get_pp_struct(pp, NULL, &mpk);

    GByteArray* mskb1 = decode64_from(msk);
    mssk = new_CPABEMasterSK(mpk->pairing, NULL); 
    // mskb1 will be free
    mssk->unserialize(mssk, mskb1);


    MasterKeyPairPtr mkp = new_MasterKeyPair(mpk, mssk);

    CPABEKeyGenPtr cpabeKG = new_CPABEODKeyGenDef();
    UserSKPtr userSK = cpabeKG->keygen(mkp, attrs);
    if(userSK != NULL){
        res = 1;
        GByteArray* dkKey = userSK->serialize(userSK);
        UserSKPtr tkKey = userSK->getOutsourcedKey(userSK);
        GByteArray* tkbuf = tkKey->serialize(tkKey);
    
        *tk = g_base64_encode(tkbuf->data, tkbuf->len);
        *dk = g_base64_encode(dkKey->data, dkKey->len);
        delete_CPABEODUserSK(userSK);
        // get_pp_struct(pp, NULL, &mpk);
        g_byte_array_free(dkKey, 1);
        g_byte_array_free(tkbuf, 1);
    }

    delete_CPABEMasterSK(mssk);
    delete_CPABEMasterPK(mpk);
    delete_MasterKeyPair(mkp);
    delete_CPABEKeyGen(cpabeKG);
    return res;
}

int sbox_transform(char* pp, char* tk, char* abec, char** abetc){
    int res = 0;
    MasterPKPtr mpk;
    get_pp_struct(pp, NULL, &mpk);
   
    CPABEODTransCryptPtr cpabeod = new_CPABEOD_CCA_TransCryptDef();
    
    GByteArray* tkb1 = decode64_from(tk);
    // hash released the memory of tkb1
    UserSKPtr tkKey = cpabeod->parseUserTKey(mpk, tkb1);

    // forge a ciphertext without aes ciphertext
    int size = 0;
    GByteArray * abecb = decode64_from(abec);    
    CipherTextPtr c1 = cpabeod->unserializeCT(abecb);
    
    CipherTextPtr c2 = cpabeod->transform(tkKey, c1);
    if(c2 != NULL){
        res = 1;
        GByteArray* tctb = c2->serialize(c2);
        *abetc = g_base64_encode(tctb->data, tctb->len);

        g_byte_array_free(tctb, 1);
        delete_CipherTextOD(c2);
    }else{
        delete_CipherText(c1);
        res = 0;
    }
    delete_CPABEODUserSK(tkKey);
    delete_CPABEMasterPK(mpk);
    delete_CPABEODTransCrypt(cpabeod);
    g_byte_array_free(abecb, 1);
    return res;
}
/*int sbox_dec(char* pp, char* dk, char* abetc, char* aesc, char** m, size_t *mlen){
    int res = 0;
    MasterPKPtr mpk;
    get_pp_struct(pp, NULL, &mpk);
    CPABECryptPtr cpabe = new_CPABEOD_CCA_CryptDef();

    GByteArray* dkb1 = decode64_from(dk);
    // hash released the memory of dkb1
    UserSKPtr dkKey = cpabe->parseUserKey(mpk, dkb1);
    //UserSKPtr dkKey = cpabeodCrypt_parseUSK(mpk, dkb1);

    // forge a trans ciphertext with aes ciphertext
    GByteArray* tctb1 = decode64_from(abetc);
    CipherTextPtr c1 = cpabe->unserializeCT(tctb1);
    GByteArray* aescb1 = decode64_from(aesc);
    GByteArray* abebuf;
    int file_len;
    unserializeCT(c1->data, &abebuf, &file_len, NULL);
    GByteArray* baseCTb = serializeCT(abebuf, file_len, aescb1);
    g_byte_array_free(c1->data, 1);
    c1->data = baseCTb;

    GByteArray* rm = cpabe->decrypt(dkKey, c1);
    if(rm != NULL){
        res = 1;
        *m = rm->data;
        *mlen = rm->len;

        //g_byte_array_free(rm, 1);
    }

    // released c1->data
    delete_CipherTextOD(c1);
    delete_CPABEMasterPK(mpk);
    delete_CPABEODUserSK(dkKey);
    delete_CPABECrypt(cpabe);
    g_byte_array_free(aescb1, 1);
    g_byte_array_free(tctb1, 1);
    return res;
}*/
int sbox_enc(char* pp, int aesbits, char* policy, unsigned char* m, size_t mlen, char** abec, unsigned char** aesc){
    sbox_aes_t aes;
    int res = sbox_enc_init(pp, aesbits, policy, abec, aes);
    if(res){
        int pc_len = sbox_get_dofinal_out_len(aes, mlen);
        *aesc = malloc(pc_len);
        res = sbox_aes_dofinal1(aes, m, mlen, *aesc);
        if(res != pc_len){
            printf("Predicted len is not equals to output len!\n");
            free(*abec);
            *abec = NULL;
            free(*aesc);
            *aesc = NULL;
            return 0;
        }
    }else{
        free(*abec);
        *abec = NULL;
    }
    return res;
}

int sbox_enc_file(char* pp, int aesbits, char* policy, char* mf, char* aescf, char** abec){
    sbox_aes_t aes;
    int res = sbox_enc_init(pp, aesbits, policy, abec, aes);
    if(res){
        res = sbox_aes_file(aes, mf, aescf);
    }
    return res;
}

int sbox_dec(char* pp, char* dk, char* abetc, unsigned char* aesc, size_t c_len, unsigned char** m){
    sbox_aes_t aes;
    int res = sbox_dec_init(pp, dk, abetc, aes);
    if(res){
        int pc_len = sbox_get_dofinal_out_len(aes, c_len);
        *m = malloc(pc_len);
        res = sbox_aes_dofinal1(aes, aesc, c_len, *m);
        if(res != pc_len){
            printf("Predicted len is not equals to output len!\n");
            free(*m);
            *m = NULL;
            return 0;
        }
    }
    return res;
}

int sbox_dec_file(char* pp, char* dk, char* abetc, char* aescf, char* mf){
    sbox_aes_t aes;
    int res = sbox_dec_init(pp, dk, abetc, aes);
    if(res){
        res = sbox_aes_file(aes, aescf, mf);
    }
    return res;
}

int sbox_re_enc(char* pp, char* dk, char* abetc, char* npolicy, char** abec){
    int res = 0;
    MasterPKPtr mpk;
    get_pp_struct(pp, NULL, &mpk);
    CPABECryptPtr cpabe = new_CPABEOD_CCA_CryptDef();

    GByteArray* dkb1 = decode64_from(dk);
    // has released the memory of dkb1
    UserSKPtr dkKey = cpabe->parseUserKey(mpk, dkb1);
    //UserSKPtr dkKey = cpabeodCrypt_parseUSK(mpk, dkb1);

    // forge a trans ciphertext with aes ciphertext
    GByteArray* tctb1 = decode64_from(abetc);
    CipherTextPtr c1 = cpabe->unserializeCT(tctb1);

    CipherTextPtr c = cpabeCrypt_cca_reencrypt(mpk, dkKey, npolicy, c1);
    if(c != NULL){
        res = 1;
        GByteArray* cph_buf = c->serialize(c);
        *abec = g_base64_encode(cph_buf->data, cph_buf->len);
        g_byte_array_free(cph_buf, 1);
        delete_CipherText(c);
    }

    delete_CipherTextOD(c1);
    delete_CPABEMasterPK(mpk);
    delete_CPABEODUserSK(dkKey);
    delete_CPABECrypt(cpabe);
    return res;
}

int sbox_enc_init(char* pp, int aesbits, char* policy, char** abec, sbox_aes aes){
    int res = 0;
    MasterPKPtr mpk;
    get_pp_struct(pp, NULL, &mpk);

    CPABECryptPtr cpabe = new_CPABEOD_CCA_CryptDef();
    CipherTextPtr c = cpabe->encrypt1(mpk, policy, aesbits, aes);
    if(c != NULL){
        res = 1;
        GByteArray* cph_buf = c->serialize(c);
        *abec = g_base64_encode(cph_buf->data, cph_buf->len);
        g_byte_array_free(cph_buf, 1);
        delete_CipherText(c);
    }

    delete_CPABEMasterPK(mpk);
    delete_CPABECrypt(cpabe);
    return res;
}

int sbox_dec_init(char* pp, char* dk, char* abetc, sbox_aes aes){
    int res = 0;
    MasterPKPtr mpk;
    get_pp_struct(pp, NULL, &mpk);
    CPABECryptPtr cpabe = new_CPABEOD_CCA_CryptDef();

    GByteArray* dkb1 = decode64_from(dk);
    // has released the memory of dkb1
    UserSKPtr dkKey = cpabe->parseUserKey(mpk, dkb1);
    //UserSKPtr dkKey = cpabeodCrypt_parseUSK(mpk, dkb1);

    // forge a trans ciphertext with aes ciphertext
    GByteArray* tctb1 = decode64_from(abetc);
    CipherTextPtr c1 = cpabe->unserializeCT(tctb1);

    res = cpabeodCrypt_cca_decrypt2(mpk, dkKey, c1, aes);
    // res = cpabe->decrypt1(dkKey, c1, aes);

    // released c1->data
    delete_CipherTextOD(c1);
    delete_CPABEMasterPK(mpk);
    delete_CPABEODUserSK(dkKey);
    delete_CPABECrypt(cpabe);
    return res;
}

int sbox_aes_update(sbox_aes aes, unsigned char* in, size_t in_len, unsigned char* out){
    return cpabe_aes_update(aes, in, in_len, out);
}

int sbox_aes_dofinal1(sbox_aes aes, unsigned char* in, size_t in_len, unsigned char* out){
    int pred = sbox_get_dofinal_out_len(aes, in_len);
    int res = cpabe_aes_dofinal1(aes, in, in_len, out);
    clear_cpabe_aes(aes);
    return res;
}

int sbox_aes_dofinal0(sbox_aes aes, unsigned char* out){
    int res = cpabe_aes_dofinal0(aes, out);
    clear_cpabe_aes(aes);
    return res;
}

int sbox_aes_file(sbox_aes aes, char* fin, char* fout){
    FILE *inf, *outf;
    inf = fopen(fin, "rb");
    outf = fopen(fout, "wb");
    if(inf && outf){
        int res = cpabe_aes_execute_file(aes, inf, outf);
        clear_cpabe_aes(aes);
        fclose(inf);
        fclose(outf);
        return res;
    }
    return 0;
}

size_t sbox_get_update_out_len(sbox_aes aes, size_t in_len){
    return cpabe_aes_get_out_len_update(aes, in_len);
}

size_t sbox_get_dofinal_out_len(sbox_aes aes, size_t in_len){
    return cpabe_aes_get_out_len_dofinal(aes, in_len);
}

#ifdef sbox_exe

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
        if(argc < 5){
            printf("The enc function need 3 arguements but %d given! You may use \"sbox --help\" to get help!\n", argc - 2);
        } else{
            char *pp = trim_str(argv[2]);
            char *policy = trim_str(argv[3]);
            char *m = trim_str(argv[4]);
            char * abec, * aesc, * aesct;
            res = sbox_enc(pp, 128, policy, m, strlen(m) + 1, &abec, (unsigned char **)&aesct);
            if(res){
                aesc = g_base64_encode(aesct, res);
                printf("%s  ,  %s\n", abec, aesc);
                free(aesct);
                free(aesc);
                free(abec);
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
            res = sbox_enc_file(pp, 128, policy, mf, cf, &abec);
            if(res){
                printf("%s\n", abec);
                free(abec);
            }else
                printf("Error\n");
        }
    }else if(strcmp(argv[1], "dec") == 0){
        if(argc < 6)
            printf("The dec function need 4 arguements but %d given! You may use \"sbox --help\" to get help!\n", argc - 2);
        else{
            char *pp = trim_str(argv[2]);
            char *dk = trim_str(argv[3]);
            char *abetc = trim_str(argv[4]);
            GByteArray *aesc = decode64_from(trim_str(argv[5]));
            char *m;
            res = sbox_dec(pp, dk, abetc, aesc->data, aesc->len, (unsigned char**)&m);
            if(res){
                printf("%s\n", m);
                free(m);
            }else
                printf("DecFailed\n");
            g_byte_array_free(aesc, 1);
        }
    }else if(strcmp(argv[1], "decf") == 0){
        if(argc < 7)
            printf("The decf function need 5 arguements but %d given! You may use \"sbox --help\" to get help!\n", argc - 2);
        else{
            char *pp = trim_str(argv[2]);
            char *dk = trim_str(argv[3]);
            char *abetc = trim_str(argv[4]);
            char *cf = trim_str(argv[5]);
            char *pf = trim_str(argv[6]);
            res = sbox_dec_file(pp, dk, abetc, cf, pf);
            if(res){
                printf("Success decryption  --> %s\n", pf);
            }else
                printf("DecFailed\n");
        }
    }else if(strcmp(argv[1], "reEnc") == 0){
        if(argc < 6)
            printf("The decf function need 5 arguements but %d given! You may use \"sbox --help\" to get help!\n", argc - 2);
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
        printf("enc;        Encryption;                 Encrypt message : pp, policystr, m --> abeCipher, aesCipher\n");
        printf("encf;       Encryption;                 Encrypt file : pp, policystr, message filename, output filename --> abeCipher\n");
        printf("trans;      Transformation decryption;  pp, tk, abeCipher --> abetCipher\n");
        printf("dec;        Decrypt message;            Decrypt message : pp, dk, abetCipher, aesCipher  --> m\n");
        printf("decf;       final decryption;           Decrypt file : pp, dk, abetCipher, aes cipher filename, output file name\n");
        printf("reEnc;      re-encrypt abet;            Re-encrypt data without aesc : pp, dk, abetCipher, new policy\n");
    }
}

int exe_test() {


    int res;
    char token[20] = "SS512";
    char m[300] = "1231asdfqwsf23423123";
    char * pp, * msk;
    res = sbox_init(token, &pp, &msk);
    // printf("pp   -------------------------\n%s\n", pp);
    // printf("msk  -------------------------\n%s\n", msk);
    char* gattrs[5];
    for(int i = 0; i < 4; i++){
        gattrs[i] = sbox_genABEUUID(pp);
        printf("attr %d -------------------------\n%s\n", i, gattrs[i]);
    }
    gattrs[4] = NULL;
    
    char *tk, *dk;
    res = sbox_gen_uks(pp, msk, gattrs, 2, &tk, &dk);
    printf("tk  -------------------------\n%s\n", tk);
    printf("dk  -------------------------\n%s\n", dk);

    size_t attrVlen = strlen(gattrs[0]);
    char *policy = malloc(attrVlen*3*sizeof(char));
    policy[0] = 0;
    for(int i = 0; i < 2; i++){
        strcat(policy, gattrs[i]);
        if(i < 1)
            strcat(policy, " & ");
    } 
    // policy = "attr1&attr2&attr3&attr4";
    printf("policy is --> %s\n",policy);

    char *abec;
    unsigned char *aesc;
    int c_len = sbox_enc(pp, 128, policy, m, strlen(m)+1, &abec, &aesc);
    printf("abec  -------------------------\n%s\n", abec);
    printf("aesc len  ---------------------%d\n", c_len);
    char *abetc;
    abetc = NULL;
    res = sbox_transform(pp, tk, abec, &abetc);
    printf("sbox_transform  -------------------------%d\n", res);
    char *rm;
    size_t mlen;
    res = sbox_dec(pp, dk, abetc, aesc, c_len, (unsigned char **)&rm);
    printf("%s\n",rm);

    // encrypt large-size data
    abetc = NULL;
    abec = NULL;
    aesc = NULL;
    sbox_aes_t aes;
    printf("------------------------------------------------------------\n");
    res = sbox_enc_init(pp, 128, policy, &abec, aes);
    char *aesk = g_base64_encode(aes->key, aes->key_bits);
    // out_len = sbox_get_update_out_len(aes, strlen(m) + 1);
    // out_len = sbox_get_dofinal_out_len(aes, strlen(m) + 1);
    aesc = malloc(1024);
    // char* tmp = aesc;
    // c_len = sbox_aes_update(aes, m, strlen(m) + 1, tmp);
    // tmp += c_len
    // c_len += sbox_aes_dofinal(aes, tmp);
    c_len = sbox_aes_dofinal1(aes, m, strlen(m) + 1, aesc);

    res = sbox_transform(pp, tk, abec, &abetc);
    printf("sbox_transform  -------------------------%d\n", res);

    res = sbox_dec_init(pp, dk, abetc, aes);
    // out_len = sbox_get_update_out_len(aes, c_len);
    // out_len = sbox_get_dofinal_out_len(aes, c_len);
    rm = malloc(1024);
    // tmp = rm;
    // c_len = sbox_aes_update(aes, m, strlen(m) + 1, tmp);
    // tmp += c_len
    // c_len += sbox_aes_dofinal(aes, tmp);
    aesk = g_base64_encode(aes->key, aes->key_bits);
    res = sbox_aes_dofinal1(aes, aesc, c_len, rm);
    printf("%s\n",rm);
    printf("------------------------------------------------------------\n");

    abec = NULL;
    //re encrypt
    res = sbox_re_enc(pp, dk, abetc, policy, &abec);
    printf("sbox_re_enc -----------------------------%d\n", res);
    res = sbox_transform(pp, tk, abec, &abetc);
    printf("sbox_transform  -------------------------%d\n", res);
    res = sbox_dec_init(pp, dk, abetc, aes);
    res = sbox_aes_dofinal1(aes, aesc, c_len, rm);
    printf("%s\n",rm);
    printf("------------------------------------------------------------\n");


    // encrypts file
    char* f1 = "fp.txt";
    char* f2 = "fc.cipher";
    char* f3 = "fp.plain";
    abetc = NULL;
    abec = NULL;
    res = sbox_enc_init(pp, 128, policy, &abec, aes);
    res = sbox_aes_file(aes, f1, f2);

    abetc = NULL;
    res = sbox_transform(pp, tk, abec, &abetc);
    printf("sbox_transform  -------------------------%d\n", res);

    res = sbox_dec_init(pp, dk, abetc, aes);
    res = sbox_aes_file(aes, f2, f3);


    return (EXIT_SUCCESS);
}

void data_test(){
    char *pp, *msk, *abec, *abetc, *dk, *tk;
    char* f1 = "fp.txt";
    char* f2="fp.txt.cipher";
    char** attrs = malloc(4*sizeof(char*));
    attrs[0] = "attr1";
    attrs[1] = "attr5";
    attrs[2] = "attr6";
    attrs[3] = NULL;
    // policy = "(57B54625920696F3134ADC9CBEBB6DEDACCB1B2F|29002062747382149351890646731401|68659764785350732284753166670055|49553758751755278589616480501646)&((41286359289877250598161890933730&49337842150453473592898772505981)|(41286359289877250598161890933730|49337842150453473592898772505981))";
    char* policy = "(attr1|attr2|attr3|attr4)&((attr5&attr6)|(attr5|attr6))";
    char* f3 = "fp.txt.plain";

    int res;
    sbox_aes_t aes;

    pp = NULL;
    msk = NULL;
    res = sbox_init("SS512", &pp, &msk);
    printf("pp = %s\n", pp);
    printf("msk = %s\n", msk);

    dk = NULL;
    tk = NULL;
    res = sbox_gen_uks(pp, msk, attrs, 1, &tk, &dk);
    printf("tk = %s\n", tk);
    printf("dk = %s\n", dk);

    abec = NULL;
    res = sbox_enc_init(pp, 128, policy, &abec, aes);
    printf("abec = %s\n", abec);
    res = sbox_aes_file(aes, f1, f2);

    abetc = NULL;
    pp = "AAAABVNTNTEyAAADewAAAWd0eXBlIGEKcSA0ODYyODE3MzM0Mjc1MzcyNTA3MDQ1MjAwODIzODk2MjM1NjE1MDI1MTkyNzM1ODI2MDQyNTk5NDA2Nzg0NzU1Nzc2NjYzMDU3MTk0Mzk4Mjk0MjIyMjUxMjE3Nzg3NTYzMjU3NzEwNDE3ODExMDE5NTU3NjE4NjM5MTg2OTM5NzE2MjgxMDAzMzYzNTgzMzM1MjMwMDcxMTc5CmggNjY1NDU0NjEwNjk1NTE2NDg4NjEzNzgyMjEwMzgyNDI0NTU2NzEzMTE0NjA0MTg5ODAzMDE5NzI3NDQzNTgwMjk3NjAxNjAzMjYyNzAxNjQ0MDMwNzY1MzE4MTAzNDA2NTYyMTQyNTc4MApyIDczMDc1MTE2NzExNDU5NTE4NjE0MjgyOTAwMjg1MzczOTUxOTk1ODYxNDgwMjQzMQpleHAyIDE1OQpleHAxIDEzOApzaWduMSAxCnNpZ24wIC0xCgAAAIAvPBbzpnbQ87Wkf4EiBoEw5mqaI9bAhkWy1OUNx9wUQxvfEiTaSRyFlKUlI2k33ORsGWvf9ggBzEgOhKTqMImEUU0VV1zgNXGzq+lFQteHS+g91Zfp2M8m+wLQzEVK4CMuXISiuAGW0pJd6Vf+LehpxYjD8umVyx6KP6KCpcsn0wAAAIAJtnl3Ob1N9CwvOyzb0NZUd58qx4hfhEQKwYfjL93UCUJzRAw4zOip3WHNwIAwaoCJ2Pjv5VHN9qusyQOM0LfpQ72NaNlGbaxq3WowktkwnPOO6Qesr7tH864Dpju/6+HAHTi84pR1DARZmXcwbccOJwauzeGNGGoUbD+DVfxkPAAAAIAeEDSwXSUzGNxztoISJAYHGue+a2qLX3/Up/R/REZbTfPWdFUiiaJghEkSH3MbkKQFFUimXoF9NcRoHh5JPa7+B4Iya4LEH4BevLvCfSIztUbAGbP48aZRRW1H2sJJSOJ8N8N9fSorgZvWKuw1c7nzdrN9vT1HJbosoAOg0uqGCQAAAIATZvnBG1rrn3toN7WvWLai17190s5CT6pr1PHNG4ljvtTMXHij28xIRzkFXRSIzCet/cUZFrVJdUn4tfRMIxaPWy3OIKNYgM3giVkDj2CztM4VR8+bGK1FcEqoG04lDx+eLPkFZU133bXhmZRTm0scRZNp7zeUBRxWSILWU84yAg==";
    tk = "AAAAAgAAAIAMSl9JDhHFsj0NXyU4B8e8V/GtUtn4sYT7GK652J2pEk/Y38KpKIpBQX5snQoOdbqYjfV4sMdF1CCckFL6SQ60O8cwgsRTCtgN5q4NuqXNO2ECT0LL6c/Y9aIdu+udxMTJ4qsdcL0q577C4iRKwzccy0OFo7jqHyCXWdtPbMYSHAAAAAIAAAAJdW5kZWZpbmVkAAAAgC1Y03PQVTaCGZ6lCUqv4+eNuVzmNCHdAbVShMZHKNFzlsZKidCI98CJAohJT6gwDIu/gDB6N1494LTqR3gfvcE002i1cfimaiUvZYXc85utNI3Ia5QscxAX6zHuCJ5dUxE3+iNkvO0HiX5o+RKK+EtQWqvwE+aOKHRX0SWKilQmAAAAgFXRa6Ys5RQ8f2QGEpYCTvY0t3henw+sw09cnorcbGBY5bg0ABYnENFehwT8v2b5p6M96KbCL4cmlVyTG6kr/bsozRQaaLMAJSlxXvKjlJwLMA7AKHyHun40rGxw+Cxq4bA7sFiUEXRJp/wFHkIlKDZiJcAY9Ncx3ojpW9K9Tx1ZAAAACXVuZGVmaW5lZAAAAIAUIgIinMCDvvpk5HKi1WSIu/mwAujf8C0goH9YgyQqqx8yKK+AT+Ww2Xue6sjYm18y3Dz5KAUVEg+pI1vR/lX2LPogBI0fxux9VWI6Hr+VRmBIAlxRk0aiemm5+MDJEQaVR6ExV+d+HdY/U8drViGIKBXYz9ymyLg0x8LTq1uY7QAAAIAfHHusVwa5aTTOHkyIWuIWLmvgKLJw4c6r6eMYcKXkceHp+jVXpBUwFts5Pp5k3OHgk3SeEeB5U3D8xgx2SDmFWiNA6axZQsm0eG/jkUgrjuexd4Mhk9k/zkS5EsLw9lxcUQ+PBwhmweHPUS9F3IaYu6C7idHsBJ05clEb5IjuWg==";
    abec = "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";
    
    res = sbox_transform(pp, tk, abec, &abetc);
    printf("abetc = %s\n", abetc);

    res = sbox_dec_init(pp, dk, abetc, aes);
    res = sbox_aes_file(aes, f2, f3);
}

void data_test1(){
    char* pp = "AAAABVNTNTEyAAADewAAAWd0eXBlIGEKcSA0ODYyODE3MzM0Mjc1MzcyNTA3MDQ1MjAwODIzODk2MjM1NjE1MDI1MTkyNzM1ODI2MDQyNTk5NDA2Nzg0NzU1Nzc2NjYzMDU3MTk0Mzk4Mjk0MjIyMjUxMjE3Nzg3NTYzMjU3NzEwNDE3ODExMDE5NTU3NjE4NjM5MTg2OTM5NzE2MjgxMDAzMzYzNTgzMzM1MjMwMDcxMTc5CmggNjY1NDU0NjEwNjk1NTE2NDg4NjEzNzgyMjEwMzgyNDI0NTU2NzEzMTE0NjA0MTg5ODAzMDE5NzI3NDQzNTgwMjk3NjAxNjAzMjYyNzAxNjQ0MDMwNzY1MzE4MTAzNDA2NTYyMTQyNTc4MApyIDczMDc1MTE2NzExNDU5NTE4NjE0MjgyOTAwMjg1MzczOTUxOTk1ODYxNDgwMjQzMQpleHAyIDE1OQpleHAxIDEzOApzaWduMSAxCnNpZ24wIC0xCgAAAIAKSbRy8Q74xRh9VZZBdFwM8gqjd6cMfepPC1py663jwLwAEa7Le6dXj+yNe5Xmaw2CU6ZjMxgS4raDEYrI5+VLFicDLLxBnD+AQ+WVYArpUO6Yrp9IGQWgW2Ul/wnyN/PQkW9xwL0jCNVsyypYFpGIRwbD43DlPz7Wl6rx7lD0DQAAAIBHcqrMslW9e/1249j8AMDqxQyJ5YURVizkBcADPIG68gCYrwMDfKNNq8hH4XBKTUnA1E+hPVYkpsJlUQt3YobNGcOGM0AYhuJvb5yxe3l90m8g34ySamAtpmlHmcySQrBu3GdaKhCYnFZ+f1qRHF7lMFTjfYwpnszF24frbfJqYAAAAIAoIOLna0f/MW2YuPyQImpPsV181F5wlEfG/9ZsPFXAP1y61B9RPr3uzUgzwR8zZWj7nMAmO3iKCHpskiIJpkJmDO9NvG5tsqlg2V39gxGmSCi2GUY5YorWemOZUZDlmIYPTqL4OEtsMdzgDEYBVnP3cztJuW7N1L4z9udA06XVMgAAAIAnpNICKJhBpy2oIfum/97z1hQlHhIP1B/7D8SymNhrvc0mNA5od9lt4kp/gON7WTAFXOJV98/Nf6bhrySDRqP2IQ8ZFb7IMXDqglGVlhv/BWv4615R6NS91pbvw6r4s+Xg5df/4UO/GeKiO/119GMOXRG1gRH0CV75IplKSSka3g==";
    char* dk = "AAAAAwAAABQKxvxL5PtN9eiyJp34aj3FN9fdNA==";
    char* abetc = "AAAAAgAABBkBAAAAAAAAAIAAAAQMAAAAgCarKeRi1INCq7feuOxbBFQVyahfhnx4lUzB24jUXvSVI5lCrYQs37QcBCR9CTYxfg1ZxLqEzc99unAJhCkxmNJB8LkLEVFEtI3yS42SuVkuf6Csh7rxaYyS3SrsL0zditqcGP5zFQh2eSsHWwZAzeI35k+7Ssf8K/zXAtUrj39zAAAAgBt+O5t+2C425lBMwLgbz5Y9yHydDC3jrMMtPmngHCF4rsh3mJjUoOcrq82NiOT0sbqKzINsjJ9RrJmsY3pcE1hYnqzDXp3n9rwdI8y3m5DkpG4k4ZpXOHYpp+VBL/NsLVyqpQs2pOTZnivB4smym0CIR2498ymF3AhuG/b4nE+LAAAAgB3JVFPnSzi/gVh0XplIkxwQ4NFyxsXXtoUn1VNWM5gfVh0bxes02b5DVRa0GCA0lGjpcpIGaJAojf5ykVtcLVRsulOnDcEO+RlydWO3/puRPxexgDDqDcUWx4SIgmhLDscgNuATbNOmD7+gAMO0BQLVtZZkHzLmPo7PMOz3f3lZAAAAAgAAAAIAAAABAAAAAAAAACgzMzE3MjZDOUY0NjNCMjZBMTUzMTQ0RjQ2ODFGN0E3NTgyMDA3QjU2AAAAgENSoQH5RKYfjjkrgCbEdfkWecUV2+1wMeEH6GqMVwxkRpQ5F2DFbBwCqgPtqnqWEXe96YnARZ8d/6sbUqzD3ZNBsKUyvQ5xq1vu8A/NWCmXPFesX8kJzksp8jNUDWve1ylTrbGJ3o56SdLpSRgNgYmGXuqT826v5U/DvNu5u84lAAAAgCHXag6AYYPmf2W7wSnfmRxvSgPOY8rhri6YJqsyUqB3qSLxZidOHQAJdTpJRgQABP7bK++617mlNL85CiVHl0ErQohPfjlPHXx9s/qluB51TFapEl325VUTlGp9viyDi2eGfJBXHX1FdCz6SeCiXMibkGSmSv5j6/a6C6tEhhNGAAAAAQAAAAAAAAAoNjIxNDQyNDY1OTA5REQ5MEUzRjM4QkI2Mzk3OEQyNUExQzQ4MUJDMQAAAIADIo0jmz7Zdh2yNzRAPuCGDnRHo3tzmCX/Go4bx7+ldoAIiRkULOSq6mxV0dezp80hRwsaUjLiyflsPyROWZUmXHFjfiyDuRyF9t/E/QRaq/dX8HSde6Z3r33FyYo9neull78nG/hZgMUJTytIJ/G+GBamqzecGdqBYG/jySVBHgAAAIBAM11S1/AmDOINAwqJFLrbxxx2Tz3BzAb5Jvcji2eV9uWUCRRV4b0Hdag1a7dHB3vF0MAJXUcjD90fc5W3d/6mMWjur4n7M5ifMpTOr8n6ji0E/GWvYrpQdXPl6SuKoVNppy+S0hEs+02N4ED9QXPOnVGlTXucfprZtvwbnb5xtgAAAIQAAACAJ+y0yG8gjoLLCCFsU0TAHbC51YHfEw5nYnm6YD6RWctIv/KWImI806p/h3svlkuU0VEyhQhfpDy+WMT+iTiUpAiOLRIWb8RT96KQX2jZuAnFhdfXHIVaYuyZcjJHvMvbkxCJw+bby6vJLu5F64HyM4dWOb6EIoirvvSs1x6NxkA=";
    char* f2 = "test_data/fp.txt.cipher";
    char* f3 = "test_data/fp.plain";

    int res = sbox_dec_file(pp, dk, abetc, f2, f3);
    printf("sbox_dec_file ------------ > %d\n", res);

}


int main(int argc, char** argv){
#ifndef TEST
    sbox(argc, argv);
#else
    // exe_test();
    // data_test();
    data_test1();
#endif
}

#endif
