/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "glib.h"
#include "pairing/group_context.h"

GroupContextPtr new_GroupContext(GHashTable* table) {
    if (table == NULL)
        return NULL;
    GroupContextPtr context = malloc(sizeof (GroupContext));
    context->parameters = table;
    context->getParameter = groupContext_get_parameter;
    context->getParameters = groupContext_get_parameters;
    return context;
}

void delete_GroupContext(GroupContextPtr context) {
    if (context){
        context->parameters = NULL;
        free(context);
    }
}

char* groupContext_get_parameter(const GroupContextPtr const context, const char* const paraName) {
    if (!context || !paraName)
        return NULL;
    if (g_hash_table_contains(context->parameters, paraName)) {
        char* val = g_hash_table_lookup(context->parameters, paraName);
        return strdup(val);
    }
    return NULL;
}

GHashTable* groupContext_get_parameters(const GroupContextPtr const context) {
    if (context)
        return context->parameters;
}
