/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */



#ifndef ECSIGGEN_H
#define ECSIGGEN_H

#ifdef __cplusplus
extern "C" {
#endif


    ECSigKeyPairPtr genECSigPair(pairing_ptr p, element_t g);

#ifdef __cplusplus
}
#endif

#endif /* ECSIGGEN_H */

