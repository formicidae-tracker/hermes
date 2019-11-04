#include <stdlib.h>

#include <fort-hermes/hermes.h>

#include <stdio.h>


int main( int argc, char ** argv) {
	if (argc != 2 ) {
		fprintf(stderr,"I need 1 argument\n");
	}

	fh_error_t * err = fh_error_create();
	fh_context_t * ctx = fh_open_file(argv[1],err);
	if ( ctx == NULL ) {
		fprintf(stderr,"Got error %d: %s\n",err->code,err->what);
		return 1;
	}

	fh_frame_readout_t * ro = fh_frame_readout_create();
	while(true) {
		bool ok = fh_context_read(ctx,ro,err);
		if ( ok == false ) {
			if ( err->code == FH_END_OF_STREAM ) {
				break;
			}
			fprintf(stderr,"Got error %d: %s\n",err->code,err->what);
			return 1;
		}
		printf("Frame: %d nb ants: %d\n",fh_frame_readout_frame_id(ro),fh_frame_readout_tag_size(ro));
	}
	fh_frame_readout_destroy(ro);
	fh_context_destroy(ctx);
	fh_error_destroy(err);
	return 0;
}
