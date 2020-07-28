#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "glib.h"


void print_array(GByteArray* array, const char* tag){
	char buf[array->len + 1];
	memcpy(buf, array->data, array->len);
	buf[array->len] = 0;
	printf("%s --> %d, %d, %s\n", tag, array->len, array->cap, buf);
}
void print_string(GString* string, const char* tag){
	printf("%s --> %d, %d, %d, %s\n", tag, string->len, strlen(string->str), string->allocated_len, string->str);
}

void test_g_string(){
	char buf1[10] = "Hello Wo!";
	char buf2[110];
	memset(buf2, (int)'c', 109);
	buf2[109] = '\0';
	GString* string = g_string_new(buf1);
	print_string(string, "g_string_new");

	string = g_string_append_c(string, 'a');
	print_string(string, "g_string_append_c");
	string = g_string_append_c(string, 'a');
	print_string(string, "g_string_append_c");
	string = g_string_append(string, buf2);
	print_string(string, "g_string_append");
	string = g_string_insert_c (string, 3, '-');
	print_string(string, "g_string_insert_c");
	string = g_string_insert_c (string, -1, '-');
	print_string(string, "g_string_insert_c");
	
	string = g_string_erase(string, 30, 70);
	print_string(string, "g_string_erase");
	g_string_append_printf(string, "%d,%s", strlen(buf2),buf1, buf1, buf1, buf1);
	print_string(string, "g_string_append_printf");
	string = g_string_set_size(string, 30);
	print_string(string, "g_string_set_size");


	char c = g_array_index((GArray*)string, char, 2);
	printf("g_array_index --> %s\n", &c);
	
	g_string_free(string, 1);
	printf("g_string_free\n");
	// print_string(string, "g_string_free");
	
}


typedef struct _T{
	int a;
	int b;
	int c;
}T;
void test_g_array(){
	int size = sizeof(T);
	GArray* array = g_array_new(0,0,size);
	T t1, t2, t3;
	t1.a = 10;
	t1.b = 11;
	t1.c = 12;
	t2.a = 20;
	t2.b = 21;
	t2.c = 22;
	t3.a = 30;
	t3.b = 31;
	t3.c = 32;
	array = g_array_append_val(array, t1);
	array = g_array_append_val(array, t2);
	array = g_array_append_val(array, t3);
	T t4 = g_array_index(array, T, 1);
	printf("g_array_index -> %d, %d, %d\n", t4.a, t4.b, t4.c);

}

void test_g_str(){
	char c1[] = "Hello World!";
	char c11[] = "Hello World!";
	char c2[] = "I am fine! HAHA!";
	char c3[] = "How are you!";

	char* c4 = g_strdup_printf("%s -- %s -- %s", c1, c2, c3);
	printf("g_strdup_printf --> %s\n", c4);
	gboolean flag = g_str_equal(c1, c11);
	printf("g_str_equal --> %d\n", flag);
	flag = g_str_equal(c1, c2);
	printf("g_str_equal --> %d\n", flag);
	guint hash =  g_str_hash(c4);
	printf("g_str_hash --> %d\n", hash);
	char* c5 = g_strnfill(10,  'L');
	printf("g_strnfill --> %s\n", c5);
	gchar ** ts = g_strsplit (c1, " ", -1);
	gchar ** ps = ts;
	printf("g_strsplit --> ");
	while(*ps){
		printf(" %s, ", *(ps++));
	}
	printf("\n");
	ts = g_strsplit (c2, " ", -1);
	ps = ts;
	printf("g_strsplit --> ");
	while(*ps){
		printf(" %s, ", *(ps++));
	}
	printf("\n");

	c5 = g_strndup (c1, strlen(c1) - 1);
	printf("g_strndup --> %s\n", c5);
	c5 = g_strdup (c1);
	printf("g_strdup --> %s\n", c5);
	c5 = g_strjoin ("--", *ts, *(ts+1));
	printf("g_strjoin --> %s\n", c5);
	g_strfreev (ts);
}

void test_g_byte_array(){
	int l1 = 10;
	int l2 = 110;
	char buf1[10] = "Hello Wor!";
	char buf2[110];
	memset(buf2, (int)'c', 110);

	GByteArray* array = g_byte_array_new();
	print_array(array, "g_byte_array_new");
	// array = g_byte_array_append(array, buf2, l2);
	// print_array(array, "g_byte_array_append");
	g_byte_array_free(array, 1);
	array = g_byte_array_sized_new (100);
	print_array(array, "g_byte_array_sized_new");
	g_byte_array_free(array, 1);	

	array = g_byte_array_new_take(buf1, l1);
	print_array(array, "g_byte_array_new_take");
	array = g_byte_array_remove_index(array, l1/2);
	print_array(array, "g_byte_array_remove_index");
	array = g_byte_array_append(array, buf2, l2);
	print_array(array, "g_byte_array_append");
	array = g_byte_array_prepend(array, buf2, l1/2);
	print_array(array, "g_byte_array_prepend");

	array = g_byte_array_set_size(array, l2/2);
	print_array(array, "g_byte_array_set_size");

	g_byte_array_free(array, 1);
}

void test_g_utils(){
	char src1[20] = "Hello World!";
	char src2[110];
	memset(src2, (int)'c', 109);

	guint hash = g_str_hash(src1);
	printf("g_str_hash --> %d\n", hash);

	int len = strlen(src2) + 1;
	gchar* base64 = g_base64_encode(src2, len);
	printf("g_base64_encode --> %s\n", base64);
	char* m = g_base64_decode(base64, &len);
	printf("g_base64_decode --> %s\n", m);
	gboolean flag = g_str_equal(src2, m);
	printf("g_str_equal --> %d\n", flag);

}

void test_g_list(){
	GList* list = g_list_alloc();
	char str1[20] = "hello World1";
	char str2[20] = "hello World2";
	char str3[20] = "hello World3";
	char str0[20] = "hello World0";

	printf("g_list_alloc\n");
	list = g_list_append(list, str1);
	printf("g_list_append\n");
	printf("g_list_nth %d %p - %p--> %s\n", 0, str1, list->data, list->data);
	list = g_list_append(list, str2);
	printf("g_list_append\n");
	printf("g_list_nth %d %p - %p--> %s\n", 1, str2, list->next->data, list->next->data);
	list = g_list_append(list, str3);
	printf("g_list_append\n");
	printf("g_list_nth %d %p - %p--> %s\n", 2, str3, list->next->next->data, list->next->next->data);
	list = g_list_prepend(list, str0);
	printf("g_list_prepend\n");
	printf("g_list_nth %d %p - %p--> %s\n", 0, str3, list->data, list->data);
	int len = g_list_length(list);
	printf("g_list_length --> %d\n", len);
	GList * nc = g_list_nth(list, 0);
	printf("g_list_nth %d --> %s\n", 0, nc->data);
	nc = g_list_nth(list, 1);
	printf("g_list_nth %d --> %s\n", 1, nc->data);
	nc = g_list_nth(list, 2);
	printf("g_list_nth %d --> %s\n", 2, nc->data);
	nc = g_list_nth(list, 3);
	printf("g_list_nth %d --> %s\n", 3, nc->data);
	nc = g_list_nth(list, 4);
	printf("g_list_nth %d --> %p\n", 4, nc);

	g_list_free(list);
	printf("g_list_free --> %p\n", list);
	nc = g_list_nth(list, 3);
	printf("g_list_nth %d --> %p\n", 3, nc);
}

void test_g_slist(){
	GSList* list = g_slist_alloc();
	char str1[20] = "hello World1";
	char str2[20] = "hello World2";
	char str3[20] = "hello World3";
	char str0[20] = "hello World0";

	printf("g_slist_alloc\n");
	list = g_slist_append(list, str1);
	printf("g_slist_append\n");
	printf("g_slist_nth %d %p - %p--> %s\n", 0, str1, list->data, list->data);
	list = g_slist_append(list, str2);
	printf("g_slist_append\n");
	printf("g_slist_nth %d %p - %p--> %s\n", 1, str2, list->next->data, list->next->data);
	list = g_slist_append(list, str3);
	printf("g_slist_append\n");
	printf("g_slist_nth %d %p - %p--> %s\n", 1, str3, list->next->next->data, list->next->next->data);
	list = g_slist_prepend(list, str0);
	printf("g_slist_prepend\n");
	printf("g_slist_nth %d %p - %p--> %s\n", 0, str1, list->data, list->data);
	int len = g_slist_length(list);
	printf("g_slist_length --> %d\n", len);
	GSList * nc = g_slist_nth(list, 0);
	printf("g_slist_nth %d --> %s\n", 0, nc->data);
	nc = g_slist_nth(list, 1);
	printf("g_slist_nth %d --> %s\n", 1, nc->data);
	nc = g_slist_nth(list, 2);
	printf("g_slist_nth %d --> %s\n", 2, nc->data);
	nc = g_slist_nth(list, 3);
	printf("g_slist_nth %d --> %s\n", 3, nc->data);
	nc = g_slist_nth(list, 4);
	printf("g_slist_nth %d --> %p\n", 4, nc);

	g_list_free(list);
	printf("g_list_free --> %p\n", list);
	nc = g_list_nth(list, 3);
	printf("g_list_nth %d --> %p\n", 3, nc);
}

void test_g_hash_table(){
	GHashTable* table = g_hash_table_new(g_str_hash, g_str_equal);
	char ks[][10] = {"key1", "key2", "key3", "key4"};
	char vs[][10] = {"value1", "value2", "value3", "value4"};
	char kp[10] = "key2";
	char vp[10] = "valueP";

	gboolean flag = 0;
	for(int i = 0; i < 4; i++){
		flag = g_hash_table_insert(table, ks[i], vs[i]);
		printf("g_hash_table_insert --> %d\n", flag);
	}
	flag = g_hash_table_contains(table, kp);
	printf("g_hash_table_contains --> %d\n", flag);
	flag = g_hash_table_contains(table, "keyp");
	printf("g_hash_table_contains --> %d\n", flag);
	char* value = g_hash_table_lookup(table, kp);
	printf("g_hash_table_lookup --> %s\n", value);
	flag = g_hash_table_replace(table, kp, vp);
	printf("g_hash_table_replace --> %d\n", flag);
	value = g_hash_table_lookup(table, kp);
	printf("g_hash_table_lookup --> %s\n", value);
	GList* keys = g_hash_table_get_keys(table);
	int len = g_list_length(keys);
	printf("g_hash_table_get_keys len--> %d\n", len);
	GList* nv = g_list_nth(keys, 0);
	printf("g_list_nth --> %s\n", nv->data);
	nv = g_list_nth(keys, 1);
	printf("g_list_nth --> %s\n", nv->data);
	nv = g_list_nth(keys, 2);
	printf("g_list_nth --> %s\n", nv->data);
	nv = g_list_nth(keys, 3);
	printf("g_list_nth --> %s\n", nv->data);
	nv = g_list_nth(keys, 4);
	printf("g_list_nth --> %p\n", nv);
	g_hash_table_remove_all(table);
	printf("g_hash_table_remove_all\n");
	flag = g_hash_table_contains(table, kp);
	printf("g_hash_table_contains --> %d\n", flag);
	flag = g_hash_table_insert(table, kp, vp);
	printf("g_hash_table_insert --> %d\n", flag);
	flag = g_hash_table_contains(table, kp);
	printf("g_hash_table_contains --> %d\n", flag);
	g_hash_table_destroy(table);
	printf("g_hash_table_destroy --> %p\n", table);
}


void main(){
	test_g_str();
	printf("-------------------------------------------------------\n");
	test_g_string();
	printf("-------------------------------------------------------\n");
	test_g_byte_array();
	printf("-------------------------------------------------------\n");
	test_g_utils();
	printf("-------------------------------------------------------\n");
	test_g_list();
	printf("-------------------------------------------------------\n");
	test_g_slist();
	printf("-------------------------------------------------------\n");
	test_g_hash_table();
	printf("-------------------------------------------------------\n");
	test_g_array();
}