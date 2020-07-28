/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include <stdio.h>
#include <stdlib.h>
#include "cpabe.h"

void testContext() {

    GHashTable* t = g_hash_table_new(g_str_hash, g_str_equal);
    g_hash_table_insert(t, PAIRING_TYPE, PAIRING_NAMES[TYPE_A]);
    g_hash_table_insert(t, TYPE_A_QBITS, "512");
    g_hash_table_insert(t, TYPE_A_RBITS, "160");
    GroupContextPtr context = new_GroupContext(t);
    char* val = context->getParameter(context, TYPE_A_QBITS);
    char* val2 = context->getParameter(context, TYPE_A_RBITS);
    printf("%s:%s\n", val, val2);
    delete_GroupContext(context);
    free(val);
    free(val2);
}

void testAESCCM(){
    PairingFactoryPtr factory = new_PairingFactory();
    GHashTable* t = g_hash_table_new(g_str_hash, g_str_equal);
    g_hash_table_insert(t, PAIRING_TYPE, PAIRING_NAMES[TYPE_F]);
    g_hash_table_insert(t, PAIRING_CONFIG, "configF.prop");
    GroupContextPtr context = new_GroupContext(t);
    PairingGroupPtr pairingGrp = factory->createPairing(context);

    pairing_ptr p = pairingGrp->pairing;
    element_t a;
    element_init_Zr(a, pairingGrp->pairing);
    element_random(a);
    
    char* m = "Hello World!!!";
    GByteArray* mb = g_byte_array_new_take(m, strlen(m)+1);
    GByteArray* cb = aes_256_ccm_encrypt(mb, a);
    GByteArray* rmb = aes_256_ccm_decrypt(cb, a);

    printf("%s\n", rmb->data);
}

void testPairing() {
    PairingFactoryPtr factory = new_PairingFactory();
    //PairingGroupPtr pairingGrp = factory->createScheme(BN160);
    /*
     // TYPE_A
     GHashTable* t = g_hash_table_new(g_str_hash, g_str_equal);
     g_hash_table_insert(t, PAIRING_TYPE, PAIRING_NAMES[TYPE_A]);
     g_hash_table_insert(t, PAIRING_CONFIG, "configA.prop");
     g_hash_table_insert(t, TYPE_A_QBITS, "512");
     g_hash_table_insert(t, TYPE_A_RBITS, "160");
     */

    /*
    // TYPE_D
    GHashTable* t = g_hash_table_new(g_str_hash, g_str_equal);
    g_hash_table_insert(t, PAIRING_TYPE, PAIRING_NAMES[TYPE_D]);
    g_hash_table_insert(t, PAIRING_CONFIG, "configD.prop");
    g_hash_table_insert(t, TYPE_D_DVALUE, "496659");
    g_hash_table_insert(t, TYPE_D_BITLIMIT, "500");
     */

    
    //TYPE_F
    GHashTable* t = g_hash_table_new(g_str_hash, g_str_equal);
    g_hash_table_insert(t, PAIRING_TYPE, PAIRING_NAMES[TYPE_F]);
    g_hash_table_insert(t, PAIRING_CONFIG, "configF.prop");
    g_hash_table_insert(t, TYPE_F_BITS, "256");
    


    GroupContextPtr context = new_GroupContext(t);
    PairingGroupPtr pairingGrp = factory->createPairing(context);

    pairing_ptr p = pairingGrp->pairing;
    element_t a;
    element_init_G1(a, pairingGrp->pairing);
    element_random(a);
    element_printf("%B\n", a);
    element_t b;
    element_init_G1(b, p);
    element_random(b);

    delete_PairingGroup(pairingGrp);
    delete_PairingFactory(factory);
    delete_GroupContext(context);
    g_hash_table_destroy(t);
}

char** getAttrs() {
    int attrCount = 10;
    char* base = "attr";
    GSList* b1 = g_slist_alloc();

    char** attrs = 0;
    char* temp = 0;
    for (int i = 0; i < attrCount; i++) {
        char ti = (char) (i + '0');
        temp = malloc(sizeof (ti) + sizeof (base));
        sprintf(temp, "%s%c", base, ti);
        b1 = g_slist_prepend(b1, temp);
        //printf("%d %s\n", b1, temp);
    }
    GSList* attrB = g_slist_reverse(b1);
    attrB = attrB->next;
    GSList* ap;
    int n = g_slist_length(attrB);
    attrs = malloc((n + 1) * sizeof (char*));

    int z = 0;
    for (ap = attrB; ap; ap = ap->next) {
        attrs[z++] = ap->data;
    }
    g_slist_free(b1);
    attrs[z] = 0;
    for (int i = 0; i < n; i++) {
        printf("A--------%s\n", attrs[i]);
    }
    return attrs;
}

void cpabeCrypt(char** argv) {
    //    PairingFactoryPtr factory = new_PairingFactory();
    //    PairingGroupPtr pairingGrp = factory->createScheme(SS512);
    //    CPABECryptPtr cpabe = new_CPABECrypt(pairingGrp);
    char* msgStr = "TEST MSG1\nTEST MSG2\n";

    if (argv[1]) {
        msgStr = argv[1];
    }
    GHashTable * t = g_hash_table_new(g_str_hash, g_str_equal);

    /* TYPE_A 
        g_hash_table_insert(t, PAIRING_TYPE, PAIRING_NAMES[TYPE_A]);
        g_hash_table_insert(t, TYPE_A_QBITS, "512");
        g_hash_table_insert(t, TYPE_A_RBITS, "160");
        g_hash_table_insert(t, PAIRING_CONFIG, "config.prop");
     */

    ///* TYPE_D
    g_hash_table_insert(t, PAIRING_TYPE, PAIRING_NAMES[TYPE_D]);
    g_hash_table_insert(t, TYPE_D_DVALUE, "496659");
    g_hash_table_insert(t, TYPE_D_BITLIMIT, "500");
    g_hash_table_insert(t, PAIRING_CONFIG, "configD.prop");
     //*/

    /* TYPE_F
    g_hash_table_insert(t, PAIRING_TYPE, PAIRING_NAMES[TYPE_F]);
    g_hash_table_insert(t, PAIRING_CONFIG, "configF.prop");
    g_hash_table_insert(t, TYPE_F_BITS, "512");
     */
    GroupContextPtr context = new_GroupContext(t);
    int outsourced = 0;

    CPABECryptPtr cpabe;
    PairingFactoryPtr factory = new_PairingFactory();
    PairingGroupPtr pairingGrp = factory->createPairing(context);
    CPABEKeyGenPtr cpabeKG;

    if (outsourced) {
        cpabe = new_CPABEODCryptDef();
        cpabeKG = new_CPABEODKeyGen(pairingGrp);
    } else {
        cpabe = new_CPABECryptDef();
        cpabeKG = new_CPABEKeyGen(pairingGrp);
    }
    CPABEODTransCryptPtr cpabeod = new_CPABEODTransCryptDef();
    char * mpkF = "mpk.key";
    char * mskF = "msk.key";
    FILE* f = open_read(mpkF);
    FILE* f1 = open_read(mskF);
    MasterKeyPairPtr mkp = NULL;
    if (!(f && f1)) {
        mkp = cpabeKG->setup(cpabeKG);
        mkp->msk->save(mkp->msk, mskF);
        mkp->mpk->save(mkp->mpk, mpkF);
    }
    if (f) {
        fclose(f);
    }
    if (f1) {
        fclose(f1);
    }

    MasterPKPtr mpk = cpabeKG->loadMPK(mpkF);
    if (mpk == NULL) {
        printf("No master public key");
    }
    GByteArray* k1 = mpk->serialize(mpk);
    mpk->unserialize(mpk, k1);
    MasterSKPtr msk = cpabeKG->loadMSK(mpk, mskF);
    if (msk == NULL) {
        printf("No master private key");
    }
    GByteArray* k2 = msk->serialize(msk);
    msk->unserialize(msk, k2);
    char** attrs = getAttrs();
    if (mkp)
        delete_MasterKeyPair(mkp);
    mkp = new_MasterKeyPair(mpk, msk);
    char * userSKF = "usersk.key";

    UserSKPtr userSK = cpabeKG->keygen(mkp, attrs);
    userSK->save(userSK, userSKF);
    userSK->load(userSK, userSKF);

    GByteArray* dkKey = userSK->serialize(userSK);
    if (dkKey) {
        write_bytes_to_file("b.test", dkKey, 1);
        printf("DK Key\n");
    }
    UserSKPtr tkKey = userSK->getOutsourcedKey(userSK);
    if (tkKey) {
        GByteArray* tkf = tkKey->serialize(tkKey);
        write_bytes_to_file("a.test", tkf, 1);
        printf("Trans Key\n");
    }
    UserSKPtr userSK1;
    UserSKPtr userTK;
    if (outsourced) {
        userTK = cpabeod->loadUserTKey(mpk, "usersk.key.tk");
        userSK1 = cpabe->loadUserKey(mpk, "usersk.key.dk");
        printf("Load Trans Key\n");
    } else {
        userSK1 = cpabe->loadUserKey(mpk, userSKF);
    }
    int attrCount = 9;
    GString* val = g_string_new("");
    for (int i = 0; i < attrCount; i++) {
        g_string_append_printf(val, "attr%d & ", i);
    }
    g_string_append_printf(val, " attr%d", attrCount);

    char* policy = val->str;
    g_string_free(val, 0);
    GByteArray* msg = g_byte_array_new();
    g_byte_array_append(msg, msgStr, strlen(msgStr));
    clock_t es = clock();
    CipherTextPtr ctPtr = cpabe->encrypt(mpk, msg, policy);
    clock_t et = clock();
    printf("Encryption : %f\n", difftime(et, es) / 1000);
    GByteArray* data = ctPtr->serialize(ctPtr);
    write_bytes_to_file("test.txt.cpabe", data, 1);
    delete_CipherText(ctPtr);
    GByteArray* lct = read_bytes_from_file("test.txt.cpabe");
    CipherTextPtr nctPtr = cpabe->unserializeCT(lct);

    if (outsourced) {
        clock_t ts = clock();
        CipherTextPtr tctPtr = cpabeod->transform(userTK, nctPtr);
        clock_t te = clock();
        printf("Transformation: %f\n", difftime(te, ts) / 1000);
        GByteArray* aaa = tctPtr->serialize(tctPtr);
        write_bytes_to_file("test.txt.tcpabe", aaa, 1);
        delete_CipherTextOD(tctPtr);
        GByteArray* bbb = read_bytes_from_file("test.txt.tcpabe");
        nctPtr = cpabe->unserializeCT(bbb);
        printf("Transformed\n");
        g_byte_array_free(bbb, 1);
    }
    clock_t ts = clock();
    GByteArray* rMsg = cpabe->decrypt(userSK1, nctPtr);

    clock_t te = clock();
    printf("Decryption: %f\n", difftime(te, ts) / 1000);
    printf("Msg: %s\n", to_string(rMsg)->data);

    if (outsourced) {
        delete_CPABEODUserSK(userTK);
        delete_CPABEODUserSK(userSK1);
        //  delete_CPABEODUserSK(userSK);
        delete_CipherTextOD(nctPtr);
    } else {
        delete_CPABEUserSK(userSK1);
        delete_CPABEUserSK(userSK);
        delete_CipherText(nctPtr);
    }
    delete_CPABEMasterSK(msk);
    delete_MasterKeyPair(mkp);
    delete_CPABEMasterPK(mpk);
    delete_CPABECrypt(cpabe);
    delete_CPABEKeyGen(cpabeKG);
    delete_CPABEODTransCrypt(cpabeod);
    delete_GroupContext(context);
    delete_PairingFactory(factory);
    delete_PairingGroup(pairingGrp);
    g_byte_array_free(rMsg, 1);
    g_byte_array_free(lct, 1);
    free(attrs);
    free(policy);
    g_hash_table_remove_all(t);
    g_hash_table_destroy(t);
}

void cpabeCCACrypt(char** argv) {
    //    PairingFactoryPtr factory = new_PairingFactory();
    //    PairingGroupPtr pairingGrp = factory->createScheme(SS512);
    //    CPABECryptPtr cpabe = new_CPABECrypt(pairingGrp);
    char* msgStr = "TEST MSG1\nTEST MSG2\n";

    if (argv[1]) {
        msgStr = argv[1];
    }
    GHashTable * t = g_hash_table_new(g_str_hash, g_str_equal);

    /*//TYPE_A 
    g_hash_table_insert(t, PAIRING_TYPE, PAIRING_NAMES[TYPE_A]);
    g_hash_table_insert(t, TYPE_A_QBITS, "512");
    g_hash_table_insert(t, TYPE_A_RBITS, "160");
    g_hash_table_insert(t, PAIRING_CONFIG, "configA.prop");
     */

    //TYPE_D
    g_hash_table_insert(t, PAIRING_TYPE, PAIRING_NAMES[TYPE_D]);
    g_hash_table_insert(t, TYPE_D_DVALUE, "496659");
    g_hash_table_insert(t, TYPE_D_BITLIMIT, "500");
    g_hash_table_insert(t, PAIRING_CONFIG, "configD.prop");
    

    /* //TYPE_F
    g_hash_table_insert(t, PAIRING_TYPE, PAIRING_NAMES[TYPE_F]);
    g_hash_table_insert(t, PAIRING_CONFIG, "configF.prop");
    g_hash_table_insert(t, TYPE_F_BITS, "512");
     */
    GroupContextPtr context = new_GroupContext(t);
    int outsourced = 1;

    CPABECryptPtr cpabe;
    PairingFactoryPtr factory = new_PairingFactory();
    PairingGroupPtr pairingGrp = factory->createPairing(context);

    CPABEKeyGenPtr cpabeKG;

    if (outsourced) {
        cpabe = new_CPABEOD_CCA_CryptDef();
        cpabeKG = new_CPABEODKeyGen(pairingGrp);
    } else {
        cpabe = new_CPABE_CCA_CryptDef();
        cpabeKG = new_CPABEKeyGen(pairingGrp);
    }
    CPABEODTransCryptPtr cpabeod = new_CPABEOD_CCA_TransCryptDef();


    //MasterKeyPairPtr mkp = cpabeKG->setup(cpabeKG, context);
    MasterKeyPairPtr mkp = cpabeKG->setup(cpabeKG);
    //MasterPKPtr mpk = mkp->mpk;
    //MasterSKPtr msk = mkp->msk;
    char * mpkF = "keys/mpk.key";
    char * mskF = "keys/msk.key";
    FILE* f = open_read(mpkF);
    FILE* f1 = open_read(mskF);
    mkp->msk->save(mkp->msk, mskF);
    mkp->mpk->save(mkp->mpk, mpkF);
    if (f) {
        fclose(f);
    }
    if (f1) {
        fclose(f1);
    }

    MasterPKPtr mpk = cpabeKG->loadMPK(mpkF);
    if (mpk == NULL) {
        printf("No master public key");
    }
    GByteArray* k1 = mpk->serialize(mpk);
    mpk->unserialize(mpk, k1);

    MasterSKPtr msk = cpabeKG->loadMSK(mpk, mskF);
    if (msk == NULL) {
        printf("No master private key");
    }
    GByteArray* k2 = msk->serialize(msk);
    msk->unserialize(msk, k2);


    char** attrs = getAttrs();
    if (mkp)
        delete_MasterKeyPair(mkp);
    mkp = new_MasterKeyPair(mpk, msk);
    char * userSKF = "keys/usersk.key";

    UserSKPtr userSK = cpabeKG->keygen(mkp, attrs);
    /*// dk = usersk{derivek{userdk, usertk=null}}
    // tk = usersk{derivek{userdk=null, usertk}}
    UserSKPtr userSK1 = userSK;
    UserSKPtr userTK = userSK;*/
    userSK->save(userSK, userSKF);
    userSK->load(userSK, userSKF);

    GByteArray* dkKey = userSK->serialize(userSK);
    if (dkKey) {
       write_bytes_to_file("tmp/b.test", dkKey, 1);
       printf("DK Key\n");
    }
    UserSKPtr tkKey = userSK->getOutsourcedKey(userSK);
    if (tkKey) {
       GByteArray* tkf = tkKey->serialize(tkKey);
        write_bytes_to_file("tmp/a.test", tkf, 1);
        printf("Trans Key\n");
    }
    UserSKPtr userSK1;
    UserSKPtr userTK;
    if (outsourced) {
        userTK = cpabeod->loadUserTKey(mpk, "keys/usersk.key.tk");
        userSK1 = cpabe->loadUserKey(mpk, "keys/usersk.key.dk");
        printf("Load Trans Key\n");
    } else {
        userSK1 = cpabe->loadUserKey(mpk, userSKF);
    }


    int attrCount = 9;
    GString* val = g_string_new("");
    for (int i = 0; i < attrCount; i++) {
        g_string_append_printf(val, "attr%d & ", i);
    }
    g_string_append_printf(val, " attr%d", attrCount);

    char* policy = val->str;
    printf("policy --> %s\n", policy);
    g_string_free(val, 0);
    GByteArray* msg = g_byte_array_new();
    g_byte_array_append(msg, msgStr, strlen(msgStr));
    clock_t es = clock();
    CipherTextPtr ctPtr = cpabe->encrypt(mpk, msg, policy);
    clock_t et = clock();
    printf("Encryption : %f\n", difftime(et, es) / 1000);
    GByteArray* data = ctPtr->serialize(ctPtr);
    write_bytes_to_file("tmp/test.txt.cpabe", data, 1);
    delete_CipherText(ctPtr);
    GByteArray* lct = read_bytes_from_file("tmp/test.txt.cpabe");
    CipherTextPtr nctPtr = cpabe->unserializeCT(lct);
    //CipherTextPtr nctPtr = ctPtr;

    if (outsourced) {
        clock_t ts = clock();
        CipherTextPtr tctPtr = cpabeod->transform(userTK, nctPtr);
        clock_t te = clock();
        printf("Transformation: %f\n", difftime(te, ts) / 1000);
        GByteArray* aaa = tctPtr->serialize(tctPtr);
        write_bytes_to_file("tmp/test.txt.tcpabe", aaa, 1);
        delete_CipherTextOD(tctPtr);
        GByteArray* bbb = read_bytes_from_file("tmp/test.txt.tcpabe");
        nctPtr = cpabe->unserializeCT(bbb);
        g_byte_array_free(bbb, 1);
        //nctPtr = tctPtr;
        printf("Transformed\n");
    }
    clock_t ts = clock();
    GByteArray* rMsg = cpabe->decrypt(userSK1, nctPtr);
    clock_t te = clock();
    printf("Decryption: %f\n", difftime(te, ts) / 1000);
    if(rMsg)
        printf("Msg: %s\n", to_string(rMsg)->data);
    else
        printf("Msg: Decryption failed!!!!\n");


    if (outsourced) {
        delete_CPABEODUserSK(userTK);
        delete_CPABEODUserSK(userSK1);
        //  delete_CPABEODUserSK(userSK);
        delete_CipherTextOD(nctPtr);
    } else {
        delete_CPABEUserSK(userSK1);
        delete_CPABEUserSK(userSK);
        delete_CipherText(nctPtr);
    }
    delete_CPABEMasterSK(msk);
    delete_MasterKeyPair(mkp);
    delete_CPABEMasterPK(mpk);
    delete_CPABECrypt(cpabe);
    delete_CPABEKeyGen(cpabeKG);
    delete_CPABEODTransCrypt(cpabeod);
    delete_GroupContext(context);
    delete_PairingFactory(factory);
    delete_PairingGroup(pairingGrp);
    g_byte_array_free(rMsg, 1);
    g_byte_array_free(lct, 1);
    free(attrs);
    free(policy);
    g_hash_table_remove_all(t);
    g_hash_table_destroy(t);
}

void cpabeCCALCrypt(){
    GHashTable * t = g_hash_table_new(g_str_hash, g_str_equal);

    //TYPE_D
    g_hash_table_insert(t, PAIRING_TYPE, PAIRING_NAMES[TYPE_D]);
    g_hash_table_insert(t, TYPE_D_DVALUE, "496659");
    g_hash_table_insert(t, TYPE_D_BITLIMIT, "500");
    g_hash_table_insert(t, PAIRING_CONFIG, "configD.prop");

    GroupContextPtr context = new_GroupContext(t);
    int outsourced = 1;

    CPABECryptPtr cpabe;
    PairingFactoryPtr factory = new_PairingFactory();
    PairingGroupPtr pairingGrp = factory->createPairing(context);

    CPABEKeyGenPtr cpabeKG;

    if (outsourced) {
        cpabe = new_CPABEOD_CCA_CryptDef();
        cpabeKG = new_CPABEODKeyGen(pairingGrp);
    } else {
        cpabe = new_CPABE_CCA_CryptDef();
        cpabeKG = new_CPABEKeyGen(pairingGrp);
    }
    CPABEODTransCryptPtr cpabeod = new_CPABEOD_CCA_TransCryptDef();


    //MasterKeyPairPtr mkp = cpabeKG->setup(cpabeKG, context);
    MasterKeyPairPtr mkp = cpabeKG->setup(cpabeKG);
    //MasterPKPtr mpk = mkp->mpk;
    //MasterSKPtr msk = mkp->msk;
    char * mpkF = "keys/mpk.key";
    char * mskF = "keys/msk.key";
    FILE* f = open_read(mpkF);
    FILE* f1 = open_read(mskF);
    mkp->msk->save(mkp->msk, mskF);
    mkp->mpk->save(mkp->mpk, mpkF);
    if (f) {
        fclose(f);
    }
    if (f1) {
        fclose(f1);
    }

    MasterPKPtr mpk = cpabeKG->loadMPK(mpkF);
    if (mpk == NULL) {
        printf("No master public key");
    }
    GByteArray* k1 = mpk->serialize(mpk);
    mpk->unserialize(mpk, k1);

    MasterSKPtr msk = cpabeKG->loadMSK(mpk, mskF);
    if (msk == NULL) {
        printf("No master private key");
    }
    GByteArray* k2 = msk->serialize(msk);
    msk->unserialize(msk, k2);


    char** attrs = getAttrs();
    if (mkp)
        delete_MasterKeyPair(mkp);
    mkp = new_MasterKeyPair(mpk, msk);
    char * userSKF = "keys/usersk.key";

    UserSKPtr userSK = cpabeKG->keygen(mkp, attrs);
    /*// dk = usersk{derivek{userdk, usertk=null}}
    // tk = usersk{derivek{userdk=null, usertk}}
    UserSKPtr userSK1 = userSK;
    UserSKPtr userTK = userSK;*/
    userSK->save(userSK, userSKF);
    userSK->load(userSK, userSKF);

    GByteArray* dkKey = userSK->serialize(userSK);
    if (dkKey) {
       write_bytes_to_file("tmp/b.test", dkKey, 1);
       printf("DK Key\n");
    }
    UserSKPtr tkKey = userSK->getOutsourcedKey(userSK);
    if (tkKey) {
       GByteArray* tkf = tkKey->serialize(tkKey);
        write_bytes_to_file("tmp/a.test", tkf, 1);
        printf("Trans Key\n");
    }
    UserSKPtr userSK1;
    UserSKPtr userTK;
    if (outsourced) {
        userTK = cpabeod->loadUserTKey(mpk, "keys/usersk.key.tk");
        userSK1 = cpabe->loadUserKey(mpk, "keys/usersk.key.dk");
        printf("Load Trans Key\n");
    } else {
        userSK1 = cpabe->loadUserKey(mpk, userSKF);
    }


    int attrCount = 9;
    GString* val = g_string_new("");
    for (int i = 0; i < attrCount; i++) {
        g_string_append_printf(val, "attr%d & ", i);
    }
    g_string_append_printf(val, " attr%d", attrCount);

    char* policy = val->str;
    printf("policy --> %s\n", policy);
    //policy = "attr0 & (attr1 | attr2 | attr3) & attr4";
    g_string_free(val, 0);


    int mlen = 17322;
    int buf = 18000;
    char message[mlen];
    unsigned char cipherm[18000];
    char message1[18000];
    memset(message, '*', mlen-2);
    message[mlen-2] = '+';
    message[mlen-1] = '\0';


    cpabe_aes_t aes;
    clock_t es = clock();
    CipherTextPtr ctPtr = cpabe->encrypt1(mpk, policy, 128, aes);
    clock_t et = clock();
    int res_enc = cpabe_aes_dofinal1(aes, message, mlen, cipherm);

            printf("cipherm --> ");
            for(int i = 0; i < 10; i++){
                printf(" %d ", cipherm[i]);
            }
            printf("\n");

    clock_t eta = clock();

    printf("Encryption : %f  ---  %f\n", difftime(et, es) / 1000, difftime(eta, et) / 1000);
    printf("Encryption : output size ---> %d\n", res_enc);

    GByteArray* data = ctPtr->serialize(ctPtr);
    write_bytes_to_file("tmp/test.txt.cpabe", data, 1);
    delete_CipherText(ctPtr);
    GByteArray* lct = read_bytes_from_file("tmp/test.txt.cpabe");
    CipherTextPtr nctPtr = cpabe->unserializeCT(lct);
    //CipherTextPtr nctPtr = ctPtr;

    if (outsourced) {
        clock_t ts = clock();
        CipherTextPtr tctPtr = cpabeod->transform(userTK, nctPtr);
        clock_t te = clock();
        printf("Transformation: %f\n", difftime(te, ts) / 1000);
        GByteArray* aaa = tctPtr->serialize(tctPtr);
        write_bytes_to_file("tmp/test.txt.tcpabe", aaa, 1);
        delete_CipherTextOD(tctPtr);
        GByteArray* bbb = read_bytes_from_file("tmp/test.txt.tcpabe");
        nctPtr = cpabe->unserializeCT(bbb);
        g_byte_array_free(bbb, 1);
        //nctPtr = tctPtr;
        printf("Transformed\n");
    }


    clear_cpabe_aes(aes);
    clock_t ts = clock();
    int res_dec = cpabe->decrypt1(userSK1, nctPtr, aes);
    clock_t te = clock();
    res_dec = cpabe_aes_dofinal1(aes, cipherm, res_enc, message1);
    clock_t tea = clock();
    printf("Decryption: %f  ---  %f\n", difftime(te, ts) / 1000, difftime(tea, te) / 1000);
    printf("Decryption : output size ---> %d\n", res_dec);

    if(res_dec > 0)
        printf("Msg: %s\n", message1);
    else
        printf("Msg: Decryption failed!!!!\n");

    clear_cpabe_aes(aes);

    if (outsourced) {
        delete_CPABEODUserSK(userTK);
        delete_CPABEODUserSK(userSK1);
        //  delete_CPABEODUserSK(userSK);
        delete_CipherTextOD(nctPtr);
    } else {
        delete_CPABEUserSK(userSK1);
        delete_CPABEUserSK(userSK);
        delete_CipherText(nctPtr);
    }
    delete_CPABEMasterSK(msk);
    delete_MasterKeyPair(mkp);
    delete_CPABEMasterPK(mpk);
    delete_CPABECrypt(cpabe);
    delete_CPABEKeyGen(cpabeKG);
    delete_CPABEODTransCrypt(cpabeod);
    delete_GroupContext(context);
    delete_PairingFactory(factory);
    delete_PairingGroup(pairingGrp);
    g_byte_array_free(lct, 1);
    free(attrs);
    free(policy);
    g_hash_table_remove_all(t);
    g_hash_table_destroy(t);
}

int main(int argc, char** argv) {
    //testContext();
    //testPairing();
    //testAESCCM();
    //cpabeCrypt(argv);
    //cpabeCCACrypt(argv);
    cpabeCCALCrypt();

    return (EXIT_SUCCESS);
}
