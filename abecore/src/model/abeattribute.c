/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include  "glib.h"
#include "stdlib.h"
#include "string.h"
#include "model/abeattribute.h"

ABEAttributePtr new_ABEAttribute(char* id, char* key, char* name) {
    if (key == NULL || name == NULL)
        return NULL;
    ABEAttributePtr attr = malloc(sizeof (ABEAttribute));
    attr->id = id;
    attr->key = key;
    attr->name = name;
    attr->getID = abeAttribute_get_id;
    attr->getKey = abeAttribute_get_key;
    attr->getName = abeAttribute_get_name;
    return attr;
}

void delete_ABEAttribute(ABEAttributePtr attr) {
    if (attr) {
        //        if (attr->id)
        //            free(attr->id);
        //        printf("%s\n", attr->key);
        //        if (attr->key)
        //            free(attr->key);
        //        if (attr->name)
        //            free(attr->name);
        free(attr);
    }
}

char* abeAttribute_get_key(const ABEAttributePtr const attr) {
    return attr->key;
}

char* abeAttribute_get_name(const ABEAttributePtr const attr) {
    return attr->name;
}

char* abeAttribute_get_id(const ABEAttributePtr const attr) {
    return attr->id;
}
