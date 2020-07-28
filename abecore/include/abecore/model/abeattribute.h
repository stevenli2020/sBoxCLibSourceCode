/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include <stdlib.h>
#ifndef ABEATTRIBUTE_H
#define ABEATTRIBUTE_H


#ifdef __cplusplus
extern "C" {
#endif
    typedef struct ABEAttributeS *ABEAttributePtr;

    typedef char* (*getKeyPtr) (ABEAttributePtr);
    typedef char* (*getNamePtr) (ABEAttributePtr);
    typedef char* (*getIDPtr) (ABEAttributePtr);

    enum ABEAttributeValue {
        ABE_ATTRIBUTE_KEY,
        ABE_ATTRIBUTE_NAME,
    };


    struct ABEAttributeS {
        void* derivedEntry;
        char* key;
        char* name;
        char* id;
        getKeyPtr getKey;
        getNamePtr getName;
        getIDPtr getID;
    } ABEAttributeS;

    typedef struct ABEAttributeS ABEAttribute[1];

    ABEAttributePtr new_ABEAttribute(char* id, char* key, char* name);
    void delete_ABEAttribute(ABEAttributePtr attribute);
    char* abeAttribute_get_key(const ABEAttributePtr const attribute);
    char* abeAttribute_get_name(const ABEAttributePtr const attribute);
    char* abeAttribute_get_id(const ABEAttributePtr const attribute);
#ifdef __cplusplus
}
#endif

#endif /* ABEATTRIBUTE_H */

