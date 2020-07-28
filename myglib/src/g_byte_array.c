#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "glib.h"


//////////////////////// *******G Byte Array Operations******* ///////////////////////////

GByteArray* g_byte_array_new(){
	gsize wsize = sizeof(gchar);
	return g_array_sized_new(0, 0, wsize, 0);
}

GByteArray* g_byte_array_new_take(gbyte* data, gsize len){
	GByteArray* gbarray = g_byte_array_sized_new(len);
	gbarray = g_byte_array_append(gbarray, data, len);
	return gbarray;
}

GByteArray* g_byte_array_sized_new(gsize size){
	GByteArray* gbarray = (GByteArray*) g_array_sized_new(0, 0, 1, size);
	return gbarray;
}

GByteArray* g_byte_array_set_size(GByteArray* array, gsize len){
	array = (GByteArray*) g_array_set_size((GArray*) array, len);
	return array;
}

GByteArray* g_byte_array_remove_index(GByteArray* array, gsize pos){
	array = (GByteArray*) g_array_remove_index((GArray*) array, pos);
	return array;
}

GByteArray* g_byte_array_append(GByteArray* array, const gbyte* data, gsize len){
	array = (GByteArray*) g_array_append_vals((GArray*) array, data, len);
	return array;
}

GByteArray* g_byte_array_prepend(GByteArray* array, const gbyte* data, gsize len){
	array = (GByteArray*) g_array_prepend_vals((GArray*) array, data, len);
	return array;
}

void g_byte_array_free(GByteArray* array, gboolean flag){
	g_array_free((GArray*) array, flag);
}
