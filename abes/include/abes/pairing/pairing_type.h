/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */



#ifndef PAIRINGTYPE_H
#define PAIRINGTYPE_H

#ifdef __cplusplus
extern "C" {
#endif


#define TYPE_A_QBITS "TYPE_A_QBITS"
#define TYPE_A_RBITS "TYPE_A_RBITS"

#define TYPE_D_DVALUE "TYPE_D_DVALUE"
#define TYPE_D_BITLIMIT "TYPE_D_BITLIMIT"

#define TYPE_F_BITS "TYPE_F_BITS"

#define TYPE_D_PARAMS "d.properties"

	typedef enum {
		SS512,
		MNT159,
		MNT202,
		MNT224,
		MNT359,
		BN160,
		BN256,
		BN512
	} PairingScheme;

	typedef enum{
		TYPE_A,
		TYPE_D,
		TYPE_F
	} PairingType;

    static char* PAIRING_NAMES[] = {
        "A", "D", "F"
    };

#ifdef __cplusplus
}
#endif

#endif /* PAIRINGTYPE_H */

