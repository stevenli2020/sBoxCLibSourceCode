/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include <glib.h>

#ifndef GROUPCONTEXT_H
#define GROUPCONTEXT_H

#ifdef __cplusplus
extern "C" {
#endif
    typedef struct _GroupContext *GroupContextPtr;

    typedef char* (*tpPtr)(const GroupContextPtr, const char*);
    typedef GHashTable* (*tpsPtr)(const GroupContextPtr);

    typedef struct _GroupContext {
        GHashTable* parameters;
        tpPtr getParameter;
        tpsPtr getParameters;
    }GroupContextS;
    typedef struct _GroupContext GroupContext[1];

    GroupContextPtr new_GroupContext(GHashTable* table);
    void delete_GroupContext(GroupContextPtr context);
    char* groupContext_get_parameter(const GroupContextPtr const context, const char* const paraName);
    GHashTable* groupContext_get_parameters(const GroupContextPtr const context);


#ifdef __cplusplus
}
#endif

#endif /* GROUPCONTEXT_H */

