#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>
#include	"config.h"


int main (int argc, char *argv []){

	char* f1 = strdup(argv[1]);
	config_stc* conf = calloc(sizeof(config_stc),1);
	int ret = paser_config(f1,conf);
	printf("%s %d\n","conf->vad",conf->vad);

	return 0 ;
} /* main */
