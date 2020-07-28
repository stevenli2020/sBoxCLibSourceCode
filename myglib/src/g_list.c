#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "glib.h"


//////////////////////// *******G List Operations******* ///////////////////////////

// 0 -> 1 -> 2 -> null
GList* g_list_alloc(void){
	GList* list = malloc(sizeof(GList));
	list->data = NULL;
	list->prev = list;
	list->next = NULL;
	return list;
}

GList* g_list_append(GList *list, gpointer data){
	GList* new = g_list_alloc();
	new->data = data;
	new->prev = NULL;
	new->next = NULL;
	if(!list){
		list = g_list_alloc();
	}
	GList* end = list->prev;
	GList* last = end->prev->next? end->prev: NULL;
	if(last){
		last->next = new;
		new->next = end;
		end->prev = new;
		new->prev = last;
	}else{
		new->next = end;
		new->prev = end;
		end->prev = new;
		list = new;
	}

	return list;
}

GList* g_list_prepend(GList* list, gpointer data){
	GList* new = g_list_alloc();
	new->data = data;
	new->prev = NULL;
	new->next = NULL;
	if(!list){
		list = g_list_alloc();
	}
	GList* end = list->prev;
	GList* first = list->next? list: NULL;
	if(first){
		new->next = first;
		first->prev = new;
		new->prev = end;
	}else{
		new->next = end;
		new->prev = end;
		end->prev = new;
	}
	list = new;
	return list;
}

gsize g_list_length(GList *list){
	int len = 0;
	if(list){
		while(list->next){
			len++;
			list = list->next;
		}
	}
	return len;
}

GList* g_list_nth(GList *list, guint n){
	while (n-- > 0 && list)
		list = list->next;

	// get not the end node
	if(list && list->next)
		return list;
	else
		return NULL;
}

void g_list_free(GList *list){
	GList* next = NULL;
	while(list){
		next = list->next;
		list->next = NULL;
		list->prev = NULL;
		list->data = NULL;
		free(list);
		list = next;
	}
	return;
}