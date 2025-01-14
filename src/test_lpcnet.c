/* Copyright (c) 2018 Mozilla */
/*
   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:

   - Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.

   - Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
   ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
   A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR
   CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
   EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
   PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
   PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
   LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
   NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <getopt.h>
#include <math.h>
#include <stdio.h>
#include "arch.h"
#include "lpcnet.h"
#include "freq.h"
#include "nnet_rw.h"
#include "nnet_data.h"
#include "from_codec2/defines.h"

int main(int argc, char **argv) {
    FILE *fin, *fout;
    LPCNetState *net;
    int mag = 0;
    int frame_size = FRAME_SIZE;
    
    net = lpcnet_create();
    
    int o = 0;
    int opt_idx = 0;
    while( o != -1 ) {
        static struct option long_opts[] = {
            {"frame_size", required_argument, 0, 'f'},
            {"logstates", required_argument, 0, 'l'},
            {"mag", required_argument, 0, 'i'},
            {"nnet", required_argument, 0, 'n'},
            {"no_pitch_embedding", no_argument, 0, 'e'},
            {"pre", required_argument, 0, 'p'},
           {0, 0, 0, 0}
        };
        
	o = getopt_long(argc,argv,"ihn:l:",long_opts,&opt_idx);
        
	switch(o){
	case 'e':
	    lpcnet_set_pitch_embedding(net, 0);
	    break;
	case 'f':
	    frame_size = atoi(optarg);
	    fprintf(stderr, "frame_size: %d\n", frame_size);
	    break;
	case 'i':
	    mag = atoi(optarg);
	    fprintf(stderr, "mag: %d\n", mag);
	    break;
	case 'l':
	    fprintf(stderr, "logstates file: %s\n", optarg);
	    lpcnet_open_test_file(net, optarg);
	    break;
	case 'n':
	    fprintf(stderr, "loading nnet: %s\n", optarg);
	    nnet_read(optarg);
	    break;
	case 'p':
	    if (atoi(optarg) == 0)
		lpcnet_set_preemph(net, 0.0);
	    break;
	case '?':
	    goto helpmsg;
	    break;
	}
    }
    int dx = optind;

    if ((argc - dx) < 2) {
    helpmsg:
        fprintf(stderr, "usage: test_lpcnet [--mag 1|2] [--logstates statesfile] [--nnet lpcnet_xxx.f32]"
		" [--framesize samples] [--pre 0|1] <features.f32> <output.s16>\n");
	fprintf(stderr, "--mag -i 0-cepstrals, 1-logmag, 2-disable LPC (WaveRNN)\n");
        return 0;
    }

    if (strcmp(argv[dx], "-") == 0) fin = stdin;
    else {
        fin = fopen(argv[dx], "rb");
        if (fin == NULL) {
            fprintf(stderr, "Can't open %s\n", argv[dx]);
            exit(1);
        }
    }
    
    if (strcmp(argv[dx+1], "-") == 0) fout = stdout;
    else {
        fout = fopen(argv[dx+1], "wb");
        if (fout == NULL) {
            fprintf(stderr, "Can't open %s\n", argv[dx+1]);
            exit(1);
        }
    }

    VLA_CALLOC(short, pcm, frame_size);
    while (1) {
        float in_features[NB_TOTAL_FEATURES];
        float features[NB_FEATURES];
        int nread = fread(in_features, sizeof(features[0]), NB_TOTAL_FEATURES, fin);
        if (nread != NB_TOTAL_FEATURES) break;
        RNN_COPY(features, in_features, NB_FEATURES);
        lpcnet_synthesize(net, pcm, features, frame_size, mag);
        fwrite(pcm, sizeof(pcm[0]), frame_size, fout);
        if (fout == stdout) fflush(stdout);
    }
    VLA_FREE(pcm);
    fclose(fin);
    fclose(fout);
    lpcnet_destroy(net);
    return 0;
}
