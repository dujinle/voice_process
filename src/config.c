#include "config.h"
#include <stdio.h>


int paser_config(char* filename,config_stc* conf){

	FILE* fp = fopen(filename,"r");
	if(fp == NULL){
		return -1;
	}
	float value = 0.0;
	char line[1024],name[48];
	while(fgets(line,1024,fp) != NULL){
		int len = strlen(line);
		line[len - 1] = '\0';
		if(strlen(line) == 0 || line[0] == '#'){
			continue;
		}
		int r = sscanf(line, "%[^ \t]%*[ \t]=%*[ \t]%f;",name,&value);
		if(r != 2){
			return -1;
		}
		if(strcmp(name,"vad") == 0){
			conf->vad = value;
		}else if(strcmp(name,"vad_base") == 0){
			conf->vad_base = value;
		}else if(strcmp(name,"vad_start_append") == 0){
			conf->vad_start_append = value;
		}else if(strcmp(name,"vad_end_append") == 0){
			conf->vad_end_append = value;
		}else if(strcmp(name,"sample_rate") == 0){
			conf->sample_rate = value;
		}else if(strcmp(name,"sample_bit") == 0){
			conf->sample_bit = value;
		}else if(strcmp(name,"frame_size") == 0){
			conf->frame_size = value;
		}else if(strcmp(name,"frame_move") == 0){
			conf->frame_move = value;
		}else if(strcmp(name,"bank_num") == 0){
			conf->bank_num = value;
		}else if(strcmp(name,"fbank_num") == 0){
			conf->fbank_num = value;
		}else if(strcmp(name,"vad_fac") == 0){
			conf->vad_fac = value;
		}else if(strcmp(name,"mfcc_size") == 0){
			conf->mfcc_size = value;
		}
	}
	printf("%s:%d\n","vad",conf->vad);
	printf("%s:%f\n","vad_fac",conf->vad_fac);
	printf("%s:%d\n","sample_rate",conf->sample_rate);
	printf("%s:%d\n","sample_bit",conf->sample_bit);
	return 0;
}
