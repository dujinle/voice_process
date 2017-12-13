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

static void usage_exit (const char *progname){
	printf ("Usage :\n  %s <bfile> <ffile>...\n", progname) ;
	exit (1) ;
} /* usage_exit */


int main (int argc, char *argv []){
	if (argc != 3 || strcmp (argv [1], "--help") == 0 || strcmp (argv [1], "-h") == 0)
		usage_exit (argv [0]) ;

	char* f1 = strdup(argv[1]);
	char* f2 = strdup(argv[2]);

	int j,flg,i = 0;

	wav_info* w1 = init_winfo(3000,8000,256,80);
	wav_info* w2 = init_winfo(3000,8000,256,80);

	double* data = calloc(sizeof(double),3000);
	for(i = 0;i < 3000;i++){
		if(i%2 == 0){
			flg = 1;
		}else{
			flg = -1;
		}
		data[i] = i * flg ;
	}

	process_feat(w1,data);
	process_feat(w2,data);

	printf("%s frames num:%d\n",f1,w1->frame_num);
	printf("%s frames num:%d\n",f2,w2->frame_num);


	for(i = 0;i < w2->frame_num;i++){
		for(j = 0;j < w2->mfcc_size;j++){
			printf("%4.10f\t",w2->mfccs[i][j]);
		}
		printf("\n");
	}


	if(w2->frame_num >= w1->frame_num){
		for(i = 0;i < w2->frame_num - w1->frame_num;i++){
			double mfcc_disp = mfcc_comp(w1->frame_num,w1->mfcc_size,w1->mfccs,w2->mfccs);
			double massc_disp = massc_comp(w1->frame_num,w1->mass,w2->mass);
			double rms_disp = massc_comp(w1->frame_num,w1->rms,w2->rms);
			w2->mass++;
			w2->rms++;
			w2->mfccs++;
			printf("%d mfcc:%4.10f mass:%4.10f rms:%4.10f\n",i,mfcc_disp,massc_disp,rms_disp);
			//break;
		}
	}

	return 0 ;
} /* main */
