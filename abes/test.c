
#include "glib.h"
#include "pbc.h"
#include "abe.h"

void test_group(){
	PairingFactoryPtr factory = new_PairingFactory();
	PairingGroupPtr group = factory->createScheme(SS512);
	printf("pairing->name: %s\n", group->groupName);
	printf("pairing->desc: %s\n", group->pairingDesc);
	element_t e1, e2, e3;
	element_init_G1(e1, group->pairing);
	element_init_G2(e2, group->pairing);
	element_init_GT(e3, group->pairing);
    element_random(e1);
    element_random(e2);
    element_random(e3);
	pairing_apply(e3, e1, e2, group->pairing);
    element_printf("pairing_apply --> %B\n", e3);
}

void test_init(){
	PairingFactoryPtr factory = new_PairingFactory();
	PairingGroupPtr group = factory->createScheme(SS512);
	cpabe_msk_t* msk;
	cpabe_pub_t* pub;
    cpabe_setup_1(group->pairing, group->pairingDesc, &pub, &msk);
    GByteArray* buf1 = cpabe_pub_serialize(pub);
    GByteArray* buf2 = cpabe_msk_serialize(msk);
    printf("%s\n", encode64_str(buf1));
    printf("%s\n", encode64_str(buf2));
}

void test_keygen(){
	PairingFactoryPtr factory = new_PairingFactory();
	PairingGroupPtr group = factory->createScheme(SS512);
	cpabe_msk_t* msk;
	cpabe_pub_t* pub;
    cpabe_setup_1(group->pairing, group->pairingDesc, &pub, &msk);

    int len = 1;
    char** attrs = malloc(sizeof(char*)*(len+1));
    for(int i = 0; i < len; i++){
        attrs[i] = malloc(10);
        sprintf(attrs[i], "Attr%d", i);
    }
    attrs[len] = NULL;
    cpabe_sk_t * sk;
    int res = cpabe_keygen(pub, msk, attrs, &sk);
    printf("Rest -> %d\n", res);
    GByteArray* buf1 = cpabe_sk_serialize(sk);
    printf("%s\n", encode64_str(buf1));
}

void test_crypto(){
    GByteArray* buf1 ;
    int res;

	PairingFactoryPtr factory = new_PairingFactory();
	PairingGroupPtr group = factory->createScheme(SS512);
	cpabe_msk_t* msk;
	cpabe_pub_t* pub;
    res = cpabe_setup_1(group->pairing, group->pairingDesc, &pub, &msk);
    printf("cpabe_setup_1 Rest -> %d\n", res);
    buf1 = cpabe_pub_serialize(pub);
    pub = NULL;
    pub = cpabe_pub_unserialize(buf1, 0);
    buf1 = cpabe_pub_serialize(pub);
    printf("pp --> %s\n", encode64_str(buf1));
    buf1 = cpabe_msk_serialize(msk);
    msk = NULL;
    msk = cpabe_msk_unserialize(pub->p, buf1, 0);
    buf1 = cpabe_msk_serialize(msk);
    printf("msk --> %s\n", encode64_str(buf1));

    int len = 4;
    char** attrs = malloc(sizeof(char*)*(len+1));
    for(int i = 0; i < len; i++){
        attrs[i] = malloc(10);
        sprintf(attrs[i], "Attr%d", i);
        printf("%s\n", attrs[i]);
    }
    attrs[len] = NULL;
    cpabe_sk_t * sk;
    res = cpabe_keygen(pub, msk, attrs, &sk);
    printf("cpabe_keygen Rest -> %d\n", res);
    buf1 = cpabe_sk_serialize(sk);
    sk = NULL;
    sk = cpabe_sk_unserialize(pub->p, buf1, 0);
    buf1 = cpabe_sk_serialize(sk);
    printf("sk --> %s\n", encode64_str(buf1));

    element_t m;
    element_init_GT(m, pub->p);
    element_random(m);
    element_printf("M -> %B\n", m);
    char* policy = "Attr0&Attr3";
    printf("policy --> %s\n", policy);
    cpabe_cph_t* cph;

    res = cpabe_enc(pub, m, policy, &cph);
    printf("cpabe_enc Rest -> %d\n", res);
    buf1 = cpabe_cph_serialize(cph);
    cph = NULL;
    cph = cpabe_cph_unserialize(pub->p, buf1, 0);
    buf1 = cpabe_cph_serialize(cph);
    printf("abec -> %s\n", encode64_str(buf1));

    element_t rm;
    res = cpabe_dec(pub, sk, cph, rm);
    printf("cpabe_dec Rest -> %d\n", res);
    element_printf("res M -> %B\n", rm);
}

void test_odcrypto(){
    GByteArray* buf1 ;
    int res;

    PairingFactoryPtr factory = new_PairingFactory();
    PairingGroupPtr group = factory->createScheme(SS512);
    cpabe_msk_t* msk;
    cpabe_pub_t* pub;
    res = odabe_setup_1(group->pairing, group->pairingDesc, &pub, &msk);
    printf("odabe_setup_1 Rest -> %d\n", res);
    buf1 = odabe_pub_serialize(pub);
    pub = NULL;
    pub = odabe_pub_unserialize(buf1, 0);
    buf1 = odabe_pub_serialize(pub);
    printf("pp --> %s\n", encode64_str(buf1));
    buf1 = odabe_msk_serialize(msk);
    msk = NULL;
    msk = odabe_msk_unserialize(pub->p, buf1, 0);
    buf1 = odabe_msk_serialize(msk);
    printf("msk --> %s\n", encode64_str(buf1));

    int len = 4;
    char** attrs = malloc(sizeof(char*)*(len+1));
    for(int i = 0; i < len; i++){
        attrs[i] = malloc(10);
        sprintf(attrs[i], "Attr%d", i);
        printf("%s\n", attrs[i]);
    }
    attrs[len] = NULL;
    odabe_tk_t * tk;
    odabe_dk_t* dk;
    res = odabe_keygen(pub, msk, attrs, &tk, &dk);
    printf("odabe_keygen Rest -> %d\n", res);
    buf1 = odabe_tk_serialize(tk);
    tk = NULL;
    tk = odabe_tk_unserialize(pub->p, buf1, 0);
    buf1 = odabe_tk_serialize(tk);
    printf("tk --> %s\n", encode64_str(buf1));
    buf1 = odabe_dk_serialize(dk);
    dk = NULL;
    dk = odabe_dk_unserialize(pub->p, buf1, 0);
    buf1 = odabe_dk_serialize(dk);
    printf("dk --> %s\n", encode64_str(buf1));

    element_t m;
    element_init_GT(m, pub->p);
    element_random(m);
    element_printf("M -> %B\n", m);
    char* policy = "Attr0&Attr3";
    printf("policy --> %s\n", policy);
    odabe_cph_t* cph;

    res = odabe_enc(pub, m, policy, &cph);
    printf("odabe_enc Rest -> %d\n", res);
    buf1 = odabe_cph_serialize(cph);
    cph = NULL;
    cph = odabe_cph_unserialize(pub->p, buf1, 0);
    buf1 = odabe_cph_serialize(cph);
    printf("abec -> %s\n", encode64_str(buf1));

    odabe_tcph_t* trans;
    res = odabe_trans(pub, tk, cph, &trans);
    printf("odabe_trans Rest -> %d\n", res);
    buf1 = odabe_tcph_serialize(trans);
    trans = NULL;
    trans = odabe_tcph_unserialize(pub->p, buf1, 0);
    buf1 = odabe_tcph_serialize(trans);
    printf("abetc -> %s\n", encode64_str(buf1));

    element_t rm;
    res = odabe_dec(pub, dk, trans, rm);
    printf("odabe_dec Rest -> %d\n", res);
    element_printf("res M -> %B\n", rm);
}

void gen_props_files(){

    PairingFactoryPtr factory = new_PairingFactory();
    PairingGroupPtr group = factory->createScheme(SS512);
    group->save(group, "props/a512.properties");

    group = factory->createScheme(MNT159);
    group->save(group, "props/d159.properties");

    group = factory->createScheme(MNT202);
    group->save(group, "props/d202.properties");

    group = factory->createScheme(MNT224);
    group->save(group, "props/d224.properties");

    group = factory->createScheme(MNT359);
    group->save(group, "props/d359.properties");

    group = factory->createScheme(BN160);
    group->save(group, "props/f160.properties");

    group = factory->createScheme(BN256);
    group->save(group, "props/f256.properties");

    group = factory->createScheme(BN512);
    group->save(group, "props/f512.properties");

    group = factory->loadPairing("props/f160.properties");

    printf("pairing->name: %s\n", group->groupName);
    printf("pairing->desc: %s\n", group->pairingDesc);
    element_t e1, e2, e3;
    element_init_G1(e1, group->pairing);
    element_init_G2(e2, group->pairing);
    element_init_GT(e3, group->pairing);
    element_random(e1);
    element_random(e2);
    element_random(e3);
    pairing_apply(e3, e1, e2, group->pairing);
    element_printf("pairing_apply --> %B\n", e3);
}


void test_cca_odcrypto(){
    GByteArray* buf1 ;
    int res;

    PairingFactoryPtr factory = new_PairingFactory();
    PairingGroupPtr group = factory->createScheme(SS512);
    cpabe_msk_t* msk;
    cpabe_pub_t* pub;
    res = cca_odabe_setup_1(group->pairing, group->pairingDesc, &pub, &msk);
    printf("cca_odabe_setup_1 Rest -> %d\n", res);
    buf1 = cca_odabe_pub_serialize(pub);
    pub = NULL;
    pub = cca_odabe_pub_unserialize(buf1, 0);
    buf1 = cca_odabe_pub_serialize(pub);
    printf("pp --> %s\n", encode64_str(buf1));
    buf1 = cca_odabe_msk_serialize(msk);
    msk = NULL;
    msk = cca_odabe_msk_unserialize(pub->p, buf1, 0);
    buf1 = cca_odabe_msk_serialize(msk);
    printf("msk --> %s\n", encode64_str(buf1));

    int len = 4;
    char** attrs = malloc(sizeof(char*)*(len+1));
    for(int i = 0; i < len; i++){
        attrs[i] = malloc(10);
        sprintf(attrs[i], "Attr%d", i);
        printf("%s\n", attrs[i]);
    }
    attrs[len] = NULL;
    cca_odabe_tk_t * tk;
    cca_odabe_dk_t* dk;
    res = cca_odabe_keygen(pub, msk, attrs, &tk, &dk);
    printf("cca_odabe_keygen Rest -> %d\n", res);
    buf1 = cca_odabe_tk_serialize(tk);
    tk = NULL;
    tk = cca_odabe_tk_unserialize(pub->p, buf1, 0);
    buf1 = cca_odabe_tk_serialize(tk);
    printf("tk --> %s\n", encode64_str(buf1));
    buf1 = cca_odabe_dk_serialize(dk);
    dk = NULL;
    dk = cca_odabe_dk_unserialize(pub->p, buf1, 0);
    buf1 = cca_odabe_dk_serialize(dk);
    printf("dk --> %s\n", encode64_str(buf1));

    element_t m;
    element_init_GT(m, pub->p);
    element_random(m);
    element_printf("M -> %B\n", m);
    char* policy = "Attr0&Attr3";
    printf("policy --> %s\n", policy);
    cca_odabe_cph_t* cph;

    res = cca_odabe_enc(pub, m, policy, &cph);
    printf("cca_odabe_enc Rest -> %d\n", res);
    buf1 = cca_odabe_cph_serialize(cph);
    cph = NULL;
    cph = cca_odabe_cph_unserialize(pub->p, buf1, 0);
    buf1 = cca_odabe_cph_serialize(cph);
    printf("abec -> %s\n", encode64_str(buf1));

    cca_odabe_tcph_t* trans;
    res = cca_odabe_trans(pub, tk, cph, &trans);
    printf("cca_odabe_trans Rest -> %d\n", res);
    buf1 = cca_odabe_tcph_serialize(trans);
    trans = NULL;
    trans = cca_odabe_tcph_unserialize(pub->p, buf1, 0);
    buf1 = cca_odabe_tcph_serialize(trans);
    printf("abetc -> %s\n", encode64_str(buf1));

    element_t rm;
    res = cca_odabe_dec(pub, dk, trans, rm);
    printf("cca_odabe_dec Rest -> %d\n", res);
    element_printf("res M -> %B\n", rm);
}

void test_cca_eodcrypto(){
    GByteArray* buf1 ;
    GByteArray* buf2;
    int res;

    PairingFactoryPtr factory = new_PairingFactory();
    PairingGroupPtr group = factory->createScheme(SS512);
    cca_eodabe_msk_t* msk;
    cca_eodabe_pub_t* pub;
    res = cca_eodabe_setup_1(group->pairing, group->pairingDesc, &pub, &msk);
    printf("cca_eodabe_setup_1 Rest -> %d\n", res);
    buf1 = cca_eodabe_pub_serialize(pub);
    pub = NULL;
    pub = cca_eodabe_pub_unserialize(buf1, 0);
    buf1 = cca_eodabe_pub_serialize(pub);
    printf("pp --> %s\n", encode64_str(buf1));
    buf1 = cca_eodabe_msk_serialize(msk);
    msk = NULL;
    msk = cca_eodabe_msk_unserialize(pub->p, buf1, 0);
    buf1 = cca_eodabe_msk_serialize(msk);
    printf("msk --> %s\n", encode64_str(buf1));

    int len = 4;
    char** attrs = malloc(sizeof(char*)*(len+1));
    for(int i = 0; i < len; i++){
        attrs[i] = malloc(10);
        sprintf(attrs[i], "Attr%d", i);
        printf("%s\n", attrs[i]);
    }
    attrs[len] = NULL;
    cca_eodabe_tk_t * tk;
    cca_eodabe_dk_t* dk;
    res = cca_eodabe_keygen(pub, msk, attrs, &tk, &dk);
    printf("cca_eodabe_keygen Rest -> %d\n", res);
    buf1 = cca_eodabe_tk_serialize(tk);
    tk = NULL;
    tk = cca_eodabe_tk_unserialize(pub->p, buf1, 0);
    buf1 = cca_eodabe_tk_serialize(tk);
    printf("tk --> %s\n", encode64_str(buf1));
    buf1 = cca_eodabe_dk_serialize(dk);
    dk = NULL;
    dk = cca_eodabe_dk_unserialize(pub->p, buf1, 0);
    buf1 = cca_eodabe_dk_serialize(dk);
    printf("dk --> %s\n", encode64_str(buf1));

    element_t m;
    element_init_GT(m, pub->p);
    element_random(m);
    element_printf("M -> %B\n", m);
    char* policy = "Attr0&Attr3";
    printf("policy --> %s\n", policy);
    cca_eodabe_cph_t* cph;

    res = cca_eodabe_enc(pub, m, policy, &cph);
    printf("cca_eodabe_enc Rest -> %d\n", res);
    buf1 = cca_eodabe_cph_serialize(cph);
    cph = NULL;
    cph = cca_eodabe_cph_unserialize(pub->p, buf1, 0);
    buf1 = cca_eodabe_cph_serialize(cph);
    printf("abec -> %s\n", encode64_str(buf1));

    cca_eodabe_tcph_t* trans;
    res = cca_eodabe_trans(tk, cph, &trans);
    printf("cca_eodabe_trans Rest -> %d\n", res);
    buf1 = cca_eodabe_tcph_serialize(trans);
    trans = NULL;
    trans = cca_eodabe_tcph_unserialize(pub->p, buf1, 0);
    buf1 = cca_eodabe_tcph_serialize(trans);
    printf("abetc -> %s\n", encode64_str(buf1));

    element_t rm;
    res = cca_eodabe_dec(pub, dk, trans, rm);
    printf("cca_odabe_dec Rest -> %d, %d\n", res, element_cmp(m, rm));
    element_printf("res M -> %B\n", rm);


    cca_eodabe_cph_t* src = cph;

    buf1 = cca_eodabe_cph_serialize(cph);
    cph = cca_eodabe_cph_partial_unserialize(pub->p, buf1);
    res = cca_eodabe_trans(tk, cph, &trans);
    printf("cca_eodabe_trans where cph is partial Rest -> %d\n", res);
    buf1 = cca_eodabe_cph_recompress_c2(pub->p, trans, buf1, 0);
    printf("abetc -> %s\n", encode64_str(buf1));


}

int main(int argc, char** argv){
    if(argc <= 1){
        // printf("CPABE TEST =============================================\n");
        // test_crypto();
        // printf("CPABE TEST END +++++++++++++++++++++++++++++++++++++++++\n");
        // printf("ODABE TEST =============================================\n");
        // test_odcrypto();
        // printf("ODABE TEST END +++++++++++++++++++++++++++++++++++++++++\n");
        // printf("CCA ODABE TEST =========================================\n");
        // test_cca_odcrypto();
        // printf("CCA ODABE TEST END +++++++++++++++++++++++++++++++++++++\n");
        printf("CCA EODABE TEST =========================================\n");
        test_cca_eodcrypto();
        printf("CCA EODABE TEST END +++++++++++++++++++++++++++++++++++++\n");
    }else{
        gen_props_files();
    }
}