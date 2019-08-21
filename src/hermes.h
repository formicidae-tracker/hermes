#pragma once

#include <cstdint>
#include <cstddef>
#include <sys/time.h>

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus
	typedef enum fh_readout_error {
		NO_ERROR = 0,
		PROCESS_OVERFLOW = 1,
		PROCESS_TIMEOUT = 2,
		ILLUMINATION_ERROR = 3,
	} fh_readout_error_e;


	typedef enum fh_error_code {
	    FH_NO_ERROR = 0,
	    FH_STREAM_NO_HEADER = 10001,
	    FH_STREAM_WRONG_VERSION = 10002,
	    FH_END_OF_STREAM = 10003,
	    FH_MESSAGE_DECODE_ERROR = 10004,
	} fh_error_code_e;

	typedef struct fh_error {
		char * what;
		fh_error_code_e code;
	} fh_error_t;

	fh_error_t * fh_error_create();
	void fh_error_destroy(fh_error_t * err);

	typedef void fh_frame_readout_t;
	typedef void fh_ant_t;


	fh_frame_readout_t * fh_frame_readout_create();
	uint64_t fh_frame_readout_timestamp(fh_frame_readout_t * re);
	uint64_t fh_frame_readout_frame_id(fh_frame_readout_t * re);
	size_t fh_frame_readout_ant_size(fh_frame_readout_t * re);
	fh_ant_t * fh_frame_readout_ant(fh_frame_readout_t * re, size_t i);
	void fh_frame_readout_time(fh_frame_readout_t * re,struct timeval * res);
	fh_readout_error_e fh_frame_readout_error(fh_frame_readout_t * re);

	void fh_frame_readout_destroy(fh_frame_readout_t * re);


	uint32_t fh_ant_id(fh_ant_t * a);
	double fh_ant_x(fh_ant_t * a);
	double fh_ant_y(fh_ant_t * a);
	double fh_ant_theta(fh_ant_t * a);

	typedef void fh_context_t;

	fh_context_t * fh_open_file(const char * filename, fh_error_t * err);

	fh_context_t * fh_connect(const char * address, fh_error_t * err);

	void fh_context_destroy(fh_context_t * ctx);

	bool fh_context_read(fh_context_t * ctx,fh_frame_readout_t * ro, fh_error_t * err);

	bool fh_context_poll(fh_context_t * ctx,fh_frame_readout_t * ro,fh_error_t * err);


#ifdef __cplusplus
}  // extern "C"
#endif //__cplusplus
