#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>
#include	<inttypes.h>
#include	<ctype.h>

#include	"core/compare.h"
#include	"core/tools.h"
#include	"sndfile/sndfile.h"
#include	"process.h"

static void usage_exit (const char *progname){
	printf ("Usage :\n  %s <bfile> <ffile>...\n", progname) ;
	exit (1) ;
} /* usage_exit */


int main (int argc, char *argv []){
	if (argc != 3 || strcmp (argv [1], "--help") == 0 || strcmp (argv [1], "-h") == 0)
		usage_exit (argv [0]) ;

	char* conf = strdup(argv[1]);
	wav_info* w1 = creat_winfo(conf);
	wav_info* w2 = creat_winfo(conf);
	char* f1 = strdup(argv[2]);
//	char* f2 = strdup(argv[3]);
	set_reader(w1,f1);
	set_reader(w2,f1);

	double data[w1->size];
	int nread = read_double_wav(w1,data,w1->size);
	close_fd(w1);
	printf("read double size:%d\n",nread);

	short sdata[w2->size];
	nread = read_short_wav(w2,sdata,w2->size);
	close_fd(w2);
	printf("read short data size:%d\n",nread);

	double s2d[w2->size];
	les2d_array(sdata,w2->size,s2d);
	int i = 0;

	for(i = 0;i < w2->size;i++){
//		printf("%d\n",sdata[i]);
		printf("%f----%f\n",data[i],s2d[i]);
	}

	return 0 ;
} /* main */
