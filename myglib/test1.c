#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "glib.h"

struct tt{
	void** ppp;
	int len;
};

typedef struct tt TT;

void test1(){
	int  len = 10;
	TT* tt = malloc(sizeof(TT));
	void** ppp = malloc(sizeof(void*)*10);
	char s[] = "Hello!%d";
	for(int i = 0; i < len; i++){
		char buf[10];
		sprintf(buf, s, i);
		ppp[i] = buf;
	}

	GByteArray* a = malloc(sizeof(GByteArray));
}

void main(){
	test1();
}