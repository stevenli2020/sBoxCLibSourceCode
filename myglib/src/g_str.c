#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "glib.h"


gchar* g_strnfill(gsize length,  gchar fill_char){
	gchar* res = malloc(sizeof(gchar)*(length+1));
	memset(res, (guchar)fill_char, sizeof(gchar)*length);
	res[length] = '\0';
	return res;
}

gchar* g_strdup_vprintf(const gchar *format, va_list args){
	gchar* buf = malloc(sizeof(gchar) * MAX_PRINT_BUFF_SIZE);
	gchar* resb = NULL;
	int formatted_length = vsnprintf(buf, MAX_PRINT_BUFF_SIZE, format, args);
	if(formatted_length >= MAX_PRINT_BUFF_SIZE){
		free(buf);
		printf("Error: Too large formatted length %d!\n", formatted_length);
		assert(formatted_length < MAX_PRINT_BUFF_SIZE);
	}else{
		resb = malloc(sizeof(gchar)*(formatted_length+1));
		memcpy(resb, buf, formatted_length);
		resb[formatted_length] = '\0';
		free(buf);
	}
	return resb;
}

gchar* g_strdup_printf(const gchar *format, ...){
	va_list ap;
	va_start(ap,format);
	gchar* resb = g_strdup_vprintf(format, ap);
	va_end(ap);
	return resb;
}

guint g_str_hash(gconstpointer v){
	const signed char *p;
	guint h = 5381;
	for (p = v; *p != '\0'; p++)
		h = (h << 5) + h + *p;
	return h;
}

gboolean g_str_equal(gconstpointer v1, gconstpointer v2){
	const gchar *string1 = v1;
	const gchar *string2 = v2;
	return strcmp(string1, string2) == 0;
}

gchar **g_strsplit (const gchar *string, const gchar *delimiter, gint max_tokens){
	GSList *string_list = NULL, *slist;
	gchar **str_array, *s;
	guint n = 0;
	const gchar *remainder;

	if(string == NULL || delimiter == NULL || delimiter[0] == '\0')
		return NULL;

	if (max_tokens < 1)
		max_tokens = G_MAXINT;
	remainder = string;
	s = strstr (remainder, delimiter);
	if (s){
		gsize delimiter_len = strlen (delimiter);

		while (--max_tokens && s){
			gsize len;

			len = s - remainder;
			string_list = g_slist_prepend (string_list,
				g_strndup (remainder, len));
			n++;
			remainder = s + delimiter_len;
			s = strstr (remainder, delimiter);
		}
	}
	if (*string){
		n++;
		string_list = g_slist_prepend (string_list, g_strdup (remainder));
	}

	str_array = malloc(sizeof(gchar*)*(n + 1));

	str_array[n--] = NULL;
	for (slist = string_list; slist->next; slist = slist->next){
		str_array[n--] = slist->data;
	}

	g_slist_free (string_list);

	return str_array;
}

gchar* g_strndup (const gchar *str, gsize n){
	gchar *new_str;

	if (str){
		new_str = malloc(sizeof(gchar)*(n + 1));
		strncpy (new_str, str, n);
		new_str[n] = '\0';
	}else
		new_str = NULL;

	return new_str;
}

gchar* g_strdup (const gchar *str){
	gchar *new_str;
	gsize length;

	if (str){
		length = strlen (str) + 1;
		new_str = malloc(sizeof(gchar)*length);
		memcpy (new_str, str, length);
	}else
		new_str = NULL;

	return new_str;
}

gchar* g_strjoin (const gchar *separator, ...){
	gchar *string, *s;
	va_list args;
	gsize len;
	gsize separator_len;
	gchar *ptr;

	if (separator == NULL)
		separator = "";

	separator_len = strlen (separator);

	va_start (args, separator);

	s = va_arg (args, gchar*);

	if (s){
      /* First part, getting length */
		len = 1 + strlen (s);

		s = va_arg (args, gchar*);
		while (s){
			len += separator_len + strlen (s);
			s = va_arg (args, gchar*);
		}
		va_end (args);

      /* Second part, building string */
		string = malloc (sizeof(gchar) * len);

		va_start (args, separator);

		s = va_arg (args, gchar*);
		ptr = stpcpy (string, s);

		s = va_arg (args, gchar*);
		while (s){
			ptr = stpcpy (ptr, separator);
			ptr = stpcpy (ptr, s);
			s = va_arg (args, gchar*);
		}
	}else
		string = g_strdup ("");

	va_end (args);

	return string;
}

void g_strfreev (gchar **str_array){
	if (str_array) {
		int i;

		for (i = 0; str_array[i] != NULL; i++)
			free (str_array[i]);

		free (str_array);
	}
}

