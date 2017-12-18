#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>
#include	<inttypes.h>
#include	<ctype.h>

#include	"core/compare.h"
#include	"sndfile/sndfile.h"
#include	"process.h"

static wav_info* read_data(char* filename){

	SF_INFO sfinfo;
	memset (&sfinfo, 0, sizeof (SF_INFO));
	SNDFILE* sf = sf_open (filename, SFM_READ, &sfinfo);
	if(sf == NULL){
		printf ("Error opening %s.\n", filename) ;
		return NULL;
	}
	sf_count_t samples = sfinfo.frames / sfinfo.channels ;
	double* data = (double*)calloc(sizeof(double),samples);
	sf_count_t nread = sf_readf_double (sf, data, samples);
	sf_close(sf);

	if(nread != samples){
		printf("read data len error\n");
		return NULL;
	}

	wav_info* winfo = init_winfo(samples,sfinfo.samplerate,256,80);

	return winfo;
}

static short* read_short_data(char* filename){

	SF_INFO sfinfo;
	memset (&sfinfo, 0, sizeof (SF_INFO));
	SNDFILE* sf = sf_open (filename, SFM_READ, &sfinfo);
	if(sf == NULL){
		printf ("Error opening %s.\n", filename) ;
		return NULL;
	}
	sf_count_t samples = sfinfo.frames / sfinfo.channels ;
	short* data = (short*)calloc(sizeof(short),samples + 1);
	sf_count_t nread = sf_readf_short (sf, data + 1, samples);
	sf_close(sf);

	if(nread != samples){
		printf("read data len error\n");
		return NULL;
	}
	data[0] = samples;

	return data;
}

static void usage_exit (const char *progname){
	printf ("Usage :\n  %s <bfile> <ffile>...\n", progname) ;
	exit (1) ;
} /* usage_exit */


int main (int argc, char *argv []){
	if (argc != 3 || strcmp (argv [1], "--help") == 0 || strcmp (argv [1], "-h") == 0)
		usage_exit (argv [0]) ;

	char* f1 = strdup(argv[1]);
	char* f2 = strdup(argv[2]);

	short* data = read_short_data(f1);
	printf("read size:%d\n",data[0]);

	wav_info* winfo = creat_wchar_writer(f2,8000,16,1);
	int ret = write_cdata(winfo,data + 1,data[0]);

	close_fd(winfo);
	printf("write status:%d\n",ret);

	return 0 ;
} /* main */
