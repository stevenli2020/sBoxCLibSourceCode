/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#ifndef SBOX_H
#define SBOX_H

#ifdef __cplusplus
extern "C" {
#endif 

/**
input : 	token 	-> chosen from {SS512, MNT159, MNT202, MNT224, BN359, BN160, BN256, BN512}
output:		pp 		-> public parameters
			msk 	-> master secret key
*/
int sbox_init(char* token, char** pp, char** msk);

/**
input : 	pp 		-> public parameters
return:		 		-> a UUID string
*/
char* sbox_genABEUUID(char* pp);

/**
input : 	pp 		-> public parameters
			msk		-> master secret key
			attrs	-> a attribute name array, the last one must be NULL. for example, char *attrs[10]; attrs[9] = NULL
			len 	-> attrs length
output:		tk 		-> user tk
			dk	 	-> user sk
*/
int sbox_gen_uks(char* pp, char* msk, char** attrs, int len, char** tk, char** dk);

/**
input : 	pp 		-> public parameters
			policy	-> policy is combined with UUIDs generated by sbox_genABEUUID and AND symbol('&'), OR symbol('|'), and '(' ')'
output:		abec 	-> abe ciphertext
			m_b		-> plain data encrypted
return:		int 	-> the length of m_b
*/
int sbox_enc(char* pp, char* policy, char** abec, unsigned char** m_b);

/**
input : 	pp 		-> public parameters
			tk		-> user tk
output:		abec 	-> abe ciphertext
			abetc	-> tranformed abe ciphertext
*/
int sbox_transform(char* pp, char* tk, char* abec, char** abetc);

/**
input : 	pp 		-> public parameters
			dk		-> user dk
			abetc	-> tranformed abe ciphertext
output:		m_b 	-> decrypted data
return:		int 	-> the length of m_b
*/
int sbox_dec(char* pp, char* dk, char* abetc, unsigned char** m_b);

/**
input : 	pp 		-> public parameters
			dk		-> user dk
			abetc	-> tranformed abe ciphertext
			npolicy	-> new policy
output:		abec 	-> new abe ciphertext
*/
int sbox_re_enc(char* pp, char* dk, char* abetc, char* npolicy, char** abec);

#ifdef __cplusplus
}
#endif

#endif /* SBOX_H */

