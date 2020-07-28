#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "glib.h"


//////////////////////// *******G SList Operations******* ///////////////////////////


GSList* g_slist_alloc(void){
	GSList* list = malloc(sizeof(GSList));
	list->data = NULL;
	list->next = NULL;
	return list;
}

GSList* g_slist_append(GSList *list, gpointer data){
	GSList* nlist = g_slist_alloc();
	nlist->data = data;
	nlist->next = NULL;

	if(!list){
		list = g_slist_alloc();
	}
	GSList* last = g_slist_last(list);
	GSList* end = NULL;
	if(last){
		end = last->next;
		last->next = nlist;
		nlist->next = end;	
	}else{
		// insert 0 position
		end = list;
		nlist->next = end;
		list = nlist;
	}
	return list;
}


GSList* g_slist_prepend(GSList *list, gpointer data){
	GSList* nlist = g_slist_alloc();
	nlist->data = data;
	nlist->next = NULL;

	if(!list){
		list = g_slist_alloc();
	}
	nlist->next = list;
	list = nlist;
	return list;
}

gsize g_slist_length(GSList *list){
	gsize len = 0;
	if(list){
		while(list->next){
			len ++;
			list = list->next;
		}
	}
	return len;
}

GSList* g_slist_nth(GSList *list, guint n){
	while (n-- > 0 && list)
		list = list->next;

	// get not the end node
	if(list && list->next)
		return list;
	else
		return NULL;
}

GSList* g_slist_reverse(GSList *list){
	GSList *prev = NULL;
	if(list){
		GSList *end = list;
		GSList *next = NULL;
		while (list->next){
			next = list->next;
			list->next = prev;
			prev = list;
			list = next;
		}
		if(prev)
			end->next = list;
		else
			prev = end;
	}
	return prev;
}

GSList* g_slist_last(GSList *list){
	GSList* last = NULL;
	if (list){
		while (list->next){
			last = list;
			list = list->next;
		}
    }
    return last;
}

void g_slist_free(GSList *list){
	GSList* next = NULL;
	while(list){
		next = list->next;
		list->data = 0;
		list->next = NULL;
		free(list);
		list = next;
	}
	return;
}


