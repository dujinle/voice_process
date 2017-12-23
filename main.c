#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>
#include	<inttypes.h>
#include	<ctype.h>

#include	"core/compare.h"
#include	"sndfile/sndfile.h"
#include	"process.h"

int main (int argc, char *argv []){

	char* conf = strdup(argv[1]);
	wav_info* w1 = creat_winfo(conf);
	wav_info* w2 = creat_winfo(conf);

	char* f1 = strdup(argv[2]);
	char* f2 = strdup(argv[3]);
	set_reader(w1,f1);
	set_writer(w2,f2);

	short data[w1->size];
	int nread = read_short_wav(w1,data,w1->size);
	int t = write_cdata(w2,data,w1->size);
	close_fd(w1);
	close_fd(w2);
	return 0 ;
} /* main */
