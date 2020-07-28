
#ifndef GLIB_H
#define GLIB_H

#include <stdio.h>
#include <stdint.h>
#include <limits.h>
#include <stdarg.h>

#define DARRAYSIZE 0xFF
#define GSIZE_MAX SIZE_MAX
#define G_MAXINT INT_MAX
#define MAX_PRINT_BUFF_SIZE 10240
/*#define DARRAYSIZE 10
#define GSIZE_MAX 100
#define G_MAXINT 10
#define MAX_PRINT_BUFF_SIZE 100*/

#ifdef __cplusplus
extern "C" {
#endif

	typedef size_t gsize;
	typedef signed long gssize;
	typedef void* gpointer;
	typedef const void* gconstpointer;
	typedef char gchar;
	typedef unsigned char guchar;
	typedef unsigned int guint;
	typedef int gint;
	typedef unsigned char guint8;
	typedef uint8_t gbyte;
	typedef uint8_t gboolean;

	typedef void (*ELEClearFunc)(gpointer);
	typedef struct _g_array{
		gpointer data;
		gsize len;
		gsize cap;
		gsize el_size;
		gsize zero_terminated;
		gsize clear;
		ELEClearFunc clear_func;
	}GArray;

	GArray* g_array_new(gboolean zero_terminated, gboolean clear, guint el_size);
	GArray* g_array_sized_new (gboolean zero_terminated, gboolean clear, guint el_size, guint reserved_size);
	GArray* g_array_set_size(GArray* array, gsize len);
	GArray* g_array_append_vals(GArray* array, gconstpointer data, gsize len);
	GArray* g_array_prepend_vals(GArray* array, gconstpointer data, gsize len);
#define g_array_append_val(a,v)	  g_array_append_vals (a, &(v), 1)
#define g_array_prepend_val(a,v)  g_array_prepend_vals (a, &(v), 1)
#define g_array_index(a,t,i)      (((t*) (void *) (a)->data) [(i)])
	GArray* g_array_remove_index(GArray* array, gsize pos);
	void g_array_free(GArray* array, gboolean flag);
	GArray* g_array_if_expand(GArray* array, gsize len);

	typedef struct _g_byte_array{
		gchar* data;
		gsize len;
		gsize cap;
		gsize el_size;
		gsize zero_terminated;
		gsize clear;
		ELEClearFunc clear_func;
	}GByteArray;

	GByteArray* g_byte_array_new();
	GByteArray* g_byte_array_new_take(gbyte* data, gsize len);
	GByteArray* g_byte_array_sized_new (gsize size);
	GByteArray* g_byte_array_set_size(GByteArray* array, gsize len);
	GByteArray* g_byte_array_remove_index(GByteArray* array, gsize len);
	GByteArray* g_byte_array_append(GByteArray* array, const gbyte* data, gsize len);
	GByteArray* g_byte_array_prepend(GByteArray* array, const gbyte* data, gsize len);
	void g_byte_array_free(GByteArray* array, gboolean flag);


	typedef struct _g_ptr_array{
		gpointer* pdata;
		gsize len;
		gsize cap;
		gsize el_size;
		gsize zero_terminated;
		gsize clear;
		ELEClearFunc clear_func;
	}GPtrArray;

	GPtrArray* g_ptr_array_new();
	void g_ptr_array_set_size(GPtrArray *array, gint length);
	GPtrArray* g_ptr_array_add(GPtrArray* array, gpointer data);
	gpointer g_ptr_array_index(GPtrArray* array, gsize pos);
	gpointer g_ptr_array_remove_index(GPtrArray* array, gsize pos);
	gpointer g_ptr_array_remove_index_fast(GPtrArray *array, guint index);
	void g_ptr_array_free(GPtrArray* array, gboolean flag);

	typedef struct _g_string{
		gchar* str;
		gsize len;
		gsize allocated_len;
		gsize el_size;
		gsize zero_terminated;
		gsize clear;
		ELEClearFunc clear_func;
	}GString;

	GString* g_string_new(const gchar* init);
	GString* g_string_append_c(GString *string, gchar c);
	GString* g_string_append (GString *string, const gchar *val);
	GString *g_string_insert_c (GString *string, gssize pos, gchar c);
	GString* g_string_set_size(GString *string, gsize len);
	void g_string_append_printf(GString *string, const gchar *format, ...);
	GString* g_string_erase(GString* string, gsize pos, gsize len);
	void g_string_free(GString* string, gboolean flag);
	GString* g_string_if_expand(GString* array, gsize len);

	typedef struct _g_slist {
		gpointer data;
		struct _g_slist *next;
	}GSList;

	GSList* g_slist_alloc(void);
	GSList* g_slist_append(GSList *list, gpointer data);
	GSList* g_slist_prepend(GSList *list, gpointer data);
	gsize g_slist_length(GSList *list);
	GSList* g_slist_nth(GSList *list, guint n);
	GSList* g_slist_reverse(GSList *list);
	GSList* g_slist_last (GSList *list);
	void g_slist_free(GSList *list);

	typedef struct _g_list {
		gpointer data;
		struct _g_list *next;
		struct _g_list *prev;
	}GList;

	GList* g_list_alloc(void);
	GList* g_list_append(GList *list, gpointer data);
	GList* g_list_prepend(GList* list, gpointer data);
	gsize g_list_length(GList *list);
	GList* g_list_nth(GList *list, guint n);
	GList* g_list_last (GList *list);
	void g_list_free(GList *list);


	typedef struct _Slot {
		gpointer key;
		gpointer value;
		guint hash;
		struct _Slot *next;
	}Slot;

	typedef guint (*GHashFunc)(gconstpointer);
	typedef gboolean (*GEqualFunc)(gconstpointer, gconstpointer);
	typedef void (*GDestroyNotify)(gpointer);

	typedef struct _g_hash_table {
		GHashFunc hash_func;
		GEqualFunc key_equal_func;
		Slot *table;
		GDestroyNotify value_destroy_func, key_destroy_func;

	}GHashTable;

	GHashTable* g_hash_table_new(GHashFunc hash_func, GEqualFunc key_equal_func);
	gboolean g_hash_table_insert(GHashTable *hash_table, gpointer key, gpointer value);
	gboolean g_hash_table_replace(GHashTable *hash_table, gpointer key, gpointer value);
	gpointer g_hash_table_lookup(GHashTable *hash_table, gconstpointer key);
	gboolean g_hash_table_contains(GHashTable *hash_table, gconstpointer key);
	GList* g_hash_table_get_keys(GHashTable *hash_table);
	void g_hash_table_remove_all(GHashTable *hash_table);
	void g_hash_table_destroy(GHashTable *hash_table);


	gchar* g_strdup_vprintf(const gchar *format, va_list args);
	gchar* g_strdup_printf(const gchar *format, ...);
	gboolean g_str_equal(gconstpointer v1, gconstpointer v2);
	guint g_str_hash(gconstpointer v);
	gchar* g_strnfill(gsize length,  gchar fill_char);
	gchar **g_strsplit (const gchar *string, const gchar *delimiter, gint max_tokens);
	gchar *g_strndup (const gchar *str, gsize n);
	gchar* g_strdup (const gchar *str);
	gchar* g_strjoin (const gchar *separator, ...);
	void g_strfreev (gchar **str_array);


	gpointer g_malloc(gsize wsize, gsize len);
	gpointer g_realloc(gpointer src, gsize wsize, gsize len);
	gchar* g_base64_encode(const guchar *data, gsize len);
	guchar* g_base64_decode(const gchar *text, gsize *out_len);

#ifdef __cplusplus
}
#endif

#endif /* GLIB_H */
