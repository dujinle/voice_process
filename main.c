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
	winfo->wdata = data;

	return winfo;
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

	int j,i = 0;

	wav_info* winfo = read_data(f1);
	wav_info* winfo2 = read_data(f2);

	process_feat(winfo,winfo->wdata);
	process_feat(winfo2,winfo2->wdata);

	printf("%s frames num:%d\n",f1,winfo->frame_num);
	printf("%s frames num:%d\n",f2,winfo2->frame_num);

	/*
	for(i = 0;i < winfo2->frame_num;i++){
		for(j = 0;j < winfo2->mfcc_size;j++){
			printf("%4.10f\t",winfo2->mfccs[i][j]);
		}
		printf("\n");
	}
	*/

	if(winfo2->frame_num >= winfo->frame_num){
		for(i = 0;i < winfo2->frame_num - winfo->frame_num;i++){
			double mfcc_disp = mfcc_comp(winfo->frame_num,winfo->mfcc_size,winfo->mfccs,winfo2->mfccs);
			double massc_disp = massc_comp(winfo->frame_num,winfo->mass,winfo2->mass);
			double rms_disp = massc_comp(winfo->frame_num,winfo->rms,winfo2->rms);
			winfo2->mass++;
			winfo2->rms++;
			winfo2->mfccs++;
			printf("%d mfcc:%4.10f mass:%4.10f rms:%4.10f\n",i,mfcc_disp,massc_disp,rms_disp);
			//break;
		}
	}

	return 0 ;
} /* main */
