#ifndef CONFIG_HEADER
#define CONFIG_HEADER
typedef struct config{
	int vad;
	int vad_base;
	float vad_fac;
	int vad_start_append;
	int vad_end_append;
	int sample_rate;
	int sample_bit;
	int frame_size;
	int frame_move;
	int bank_num;
	int fbank_num;
	int mfcc_size;
}config_stc;

int paser_config(char* filename,config_stc* conf);

#endif
