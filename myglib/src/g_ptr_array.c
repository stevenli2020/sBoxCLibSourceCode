#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "glib.h"


//////////////////////// *******G Pointer Operations******* ///////////////////////////

GPtrArray* g_ptr_array_new (){
	gsize wsize = sizeof(gpointer);
	GPtrArray* gparray = (GPtrArray*) g_array_sized_new(0, 0, wsize, 0);
	return gparray;
}

GPtrArray* g_ptr_array_add (GPtrArray* array, gpointer data){
	array = (GPtrArray*) g_array_append_val((GArray*) array, data);
	return array;
}

gpointer g_ptr_array_index (GPtrArray* array, gsize pos){
	if(pos < 0 || pos >= array->len)
		return NULL;
	return array->pdata[pos];
}

gpointer g_ptr_array_remove_index (GPtrArray* array, gsize pos){
	if(array){
		gpointer res = g_ptr_array_index(array, pos);
		// array = (GByteArray*) g_array_remove_index((GArray*) array, WORDSIZE, pos);
		g_array_remove_index((GArray*) array, pos);
		return res;
	}
	return NULL;
}


gpointer g_ptr_array_remove_index_fast(GPtrArray *array, guint pos){
	if(array){
		if(pos < 0 || pos >= array->len)
			return NULL;
		gpointer res = array->pdata[pos];
		gpointer last = array->pdata[array->len - 1];
		array->pdata[pos] = last;
		array->pdata[array->len - 1] = NULL;
		array->len -= 1;
		array->cap += 1;
		return res;
	}
	return NULL;
}

void g_ptr_array_set_size (GPtrArray *array, gint length){
	array = (GPtrArray*) g_array_set_size((GArray*) array, length);
}

void g_ptr_array_free (GPtrArray* array, gboolean flag){
	g_array_free((GArray*) array, flag);
}

