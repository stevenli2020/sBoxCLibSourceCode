#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "glib.h"


//////////////////////// *******G Hash Table Operations******* ///////////////////////////


GHashTable* g_hash_table_new(GHashFunc hash_func, GEqualFunc key_equal_func){
	GHashTable* hash_table = malloc(sizeof(GHashTable));
	hash_table->hash_func = hash_func;
	hash_table->key_equal_func = key_equal_func;
	hash_table->table = malloc(sizeof(Slot));
	hash_table->table->next = NULL;
	hash_table->value_destroy_func = NULL;
	hash_table->key_destroy_func = NULL;
	return hash_table;
}

Slot* g_hash_table_lookup_node(GHashTable *hash_table, gconstpointer key){
	if(hash_table){
		guint key_hash = hash_table->hash_func(key);
		Slot* slot = hash_table->table;
		while(slot->next){
			if(hash_table->key_equal_func(slot->key, key) || slot->hash == key_hash ){
				return slot;
			}else{
				slot = slot->next;
			}
		}
	}
	return NULL;
}

gboolean g_hash_table_insert(GHashTable *hash_table, gpointer key, gpointer value){
	if(hash_table){
		gboolean flag = 1;
		Slot* slot = g_hash_table_lookup_node(hash_table, key);
		if(slot){
			if(hash_table->key_destroy_func)
				hash_table->key_destroy_func(slot->key);
			if(hash_table->value_destroy_func)
				hash_table->value_destroy_func(slot->value);
			slot->key = key;
			slot->value = value;
			flag = 0;
		}else{
			slot = malloc(sizeof(Slot));
			slot->key = key;
			slot->value = value;
			slot->hash = hash_table->hash_func(key);
			slot->next = hash_table->table;
			hash_table->table = slot;
		}
		return flag;
	}
	return 0;
}

gboolean g_hash_table_replace(GHashTable *hash_table, gpointer key, gpointer value){
	return g_hash_table_insert(hash_table, key, value);
}

gpointer g_hash_table_lookup(GHashTable *hash_table, gconstpointer key){
	Slot* slot = g_hash_table_lookup_node(hash_table, key);
	if(slot)
		return slot->value;
	return NULL;
}

gboolean g_hash_table_contains(GHashTable *hash_table, gconstpointer key){
	Slot* slot = g_hash_table_lookup_node(hash_table, key);
	return slot != NULL;
}

GList* g_hash_table_get_keys(GHashTable *hash_table){
	GList* keys = g_list_alloc();
	Slot* slot = hash_table->table;
	while(slot->next){
		keys = g_list_prepend(keys, slot->key);
		slot = slot->next;
	}
	return keys;
}

void g_hash_table_remove_all_clear(GHashTable *hash_table, gboolean flag){
	Slot* slot = hash_table->table;
	Slot* next = NULL;
	while(slot->next){
		if(flag){
			if(hash_table->key_destroy_func)
				hash_table->key_destroy_func(slot->key);
			if(hash_table->value_destroy_func)
				hash_table->value_destroy_func(slot->value);
		}
		next = slot->next;
		slot->key = NULL;
		slot->value = NULL;
		slot->hash = 0;
		slot->next = NULL;
		free(slot);
		slot = next;
	}
	hash_table->table = slot;
}

void g_hash_table_remove_all(GHashTable *hash_table){
	g_hash_table_remove_all_clear(hash_table, 0);
}

void g_hash_table_destroy(GHashTable *hash_table){
	g_hash_table_remove_all_clear(hash_table, 1);
	free(hash_table->table);
	hash_table->table = NULL;
	hash_table->hash_func = NULL;
	hash_table->key_equal_func = NULL;
	hash_table->value_destroy_func = NULL;
	hash_table->key_destroy_func = NULL;
	free(hash_table);
}