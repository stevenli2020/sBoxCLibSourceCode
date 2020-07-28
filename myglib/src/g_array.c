#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "glib.h"

GArray* g_array_new(gboolean zero_terminated, gboolean clear, guint el_size){
	return g_array_sized_new(zero_terminated, clear, el_size, 0);
}

GArray* g_array_sized_new(gboolean zero_terminated, gboolean clear, guint el_size, guint reserved_size){
	if(el_size <= 0)
		return NULL;

	GArray* array = malloc(sizeof(GArray));
	array->el_size        = el_size;
	if(reserved_size <= 0)
		reserved_size = DARRAYSIZE;
	array->data            = malloc(el_size * reserved_size);
	array->len             = 0;
	array->cap             = reserved_size;
	array->zero_terminated = (zero_terminated ? 1 : 0);
	array->clear           = (clear ? 1 : 0);
	array->clear_func      = NULL;

	return array;
}

GArray* g_array_set_size(GArray* array, gsize len){
	// gsize wsize = array->el_size;
	// if(!array)
	// 	return NULL;
	// gsize clen = array->cap + array->len;
	// if(clen < len){
	// 	array->data = realloc(array->data, wsize*len);
	// }
	// if(array->len > len)
	// 	array->len = len;
	// array->cap = len - array->len;
	// return array;
	gsize wsize = array->el_size;
	if(!array)
		return NULL;
	gsize clen = array->cap + array->len;
	if(clen < len){
		array->data = realloc(array->data, wsize*len);
		clen = len;
	}
	array->len = len;
	array->cap = clen - array->len;
	return array;
}

GArray* g_array_append_vals(GArray* array, gconstpointer data, gsize len){
	if(len == 0)
		return array;
	gsize wsize = array->el_size;
	array = g_array_if_expand(array, len);
	memcpy(array->data + array->len*wsize, data, len*wsize);
	array->len = array->len + len;
	array->cap = array->cap - len;
	return array;
}

GArray* g_array_prepend_vals(GArray* array, gconstpointer data, gsize len){
	gsize wsize = array->el_size;
	gbyte* tmp1 = malloc(wsize*array->len);
	memcpy(tmp1, array->data, array->len*wsize);
	array = g_array_if_expand(array, len);
	memcpy(array->data, data, len*wsize);
	memcpy(array->data + len*wsize, tmp1, array->len*wsize);
	free(tmp1);
	array->len = array->len + len;
	array->cap = array->cap - len;
	return array;
}

GArray* g_array_remove_index(GArray* array, gsize pos){
	if(array){
		if(pos < 0 || pos > array->len)
			return array;
		gsize wsize = array->el_size;
		memcpy(array->data + pos*wsize, array->data + pos*wsize + wsize, (array->len - pos - 1)*wsize);
		array->len = array->len - 1;
		array->cap = array->cap + 1;
	}
	return array;
}

void g_array_free(GArray* array, gboolean flag){
	if(array == NULL)
		return;
	if(array->data == NULL)
		return;
	if(flag != 0){
		free(array->data);
	}
	array->data = NULL;
	array->len = 0;
	array->cap = 0;
	free(array);
}

GArray* g_array_if_expand(GArray* array, gsize len){
	if(!array){
		return NULL;
	}
	gsize wsize = array->el_size;
	gsize cap = array->cap;
	if(cap < len){
		int round = len / DARRAYSIZE;
		int off = len % DARRAYSIZE;
		round = off? round+1 : round;
		cap = round*DARRAYSIZE;
		array->data = realloc(array->data, wsize*(cap + array->len));
		array->cap = cap;
	}
	return array;
}