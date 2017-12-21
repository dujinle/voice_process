#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>
#include	<inttypes.h>
#include	<ctype.h>

#include	"core/compare.h"
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

	char* f1 = strdup(argv[2]);
	set_reader(w1,f1);

	int total = 0;
	double data[2048];
	while(1){
		int nread = read_double_wav(w1,data,2048);
		if(nread < 2048){
			break;
		}
		total = total + nread;
	}
	printf("read size:%d",total);
	return 0 ;
} /* main */
