#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "glib.h"


//////////////////////// *******G String Operations******* ///////////////////////////

GString* g_string_new (const gchar* init){
	gsize len = 0;
	gsize tlen = 0;
	if(init){
		len = strlen(init) + 1;
		tlen = len;
	}else{
		len = 0;
		tlen = DARRAYSIZE + 1;
	}
	gsize wsize = sizeof(gchar);
	GString* string = (GString*) g_array_sized_new(0, 0, wsize, tlen);
	string = (GString*) g_array_append_vals((GArray*) string, init, len);
	string->len -= 1;
	return string;
}

GString* g_string_append_c (GString *string, gchar c){
	string = g_string_if_expand(string, 1);
	string->str[string->len] = c;
	string->str[string->len + 1] = '\0';
	string->len = string->len + 1;
	string->allocated_len = string->allocated_len - 1;
	return string;
}


GString *g_string_insert_c (GString *string, gssize pos, gchar c){
	gsize pos_unsigned;

	if(string == NULL)
		return NULL;

	string = g_string_if_expand (string, 1);

	if (pos < 0)
		pos = string->len;
	else{
    /* g_return_val_if_fail ((gsize) pos <= string->len, string);*/
		if((gsize)pos > string->len)
			return string;
	}
	pos_unsigned = pos;

  	/* If not just an append, move the old stuff */
	if (pos_unsigned < string->len){
		memmove (string->str + pos_unsigned + 1,
			string->str + pos_unsigned, string->len - pos_unsigned);
	}

	string->str[pos_unsigned] = c;
	string->len += 1;
	string->str[string->len] = 0;

	return string;
}

GString* g_string_append (GString *string, const gchar *data){
	if(data == NULL || strlen(data) == 0)
		return string;
	gsize vlen = strlen(data);
	string = g_string_if_expand(string, vlen);
	memcpy(string->str + string->len, data, vlen + 1);
	string->len = string->len + vlen;
	string->allocated_len = string->allocated_len - vlen;
	return string;
}

GString* g_string_set_size(GString *string, gsize len){
	string->len += 1;
	string = (GString*) g_array_set_size((GArray*) string, len + 1);
	string->len -= 1;
	string->str[string->len] = '\0';
	return string;
/*
	if(!string)
		string = g_string_new(NULL);
	gsize clen = string->allocated_len + string->len;
	if(clen < len){
		string->str = realloc(string->str, len);
	}
	if(string->len > len)
		string->len = len;
	string->str[string->len] = '\0';
	string->allocated_len = len - string->len;
	return string;*/
}

void g_string_append_printf(GString *string, const gchar *format, ...){
	if(string){
		va_list ap;
		va_start(ap,format);
		gchar* resb = g_strdup_vprintf(format, ap);
		va_end(ap);
		if(resb){
			string = g_string_append(string, resb);
			free(resb);
		}
	}
	return;
}

GString* g_string_erase(GString* string, gsize pos, gsize len){
	if(pos < 0 || pos > string->len)
		return string;
	gsize end = pos + len;
	if(end >= string->len){
		memset(string->str + pos, 0, string->len - pos);
		string->len = pos;
		string->allocated_len = string->allocated_len + string->len - pos;
	}else{
		memset(string->str + pos, 0, len);
		memcpy(string->str + pos, string->str + pos + len, string->len - pos - len + 1);
		string->len = string->len - len;
		string->allocated_len = string->allocated_len + len;
	}
	return string;
}

void g_string_free(GString* string, gboolean flag){
	g_array_free((GArray*) string, flag);
}

GString* g_string_if_expand(GString* string, gsize len){
	if(!string){
		string = g_string_new(NULL);
	}
	gsize cap = string->allocated_len;
	if(cap < len){
		cap = (len/DARRAYSIZE + 1)*DARRAYSIZE;
		string->str = realloc(string->str, sizeof(gchar)*(cap + string->len));
		string->allocated_len = cap;
	}
	return string;
}