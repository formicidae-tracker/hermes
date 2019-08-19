#pragma once

#include <cstdint>
#include <cstddef>
#include <sys/time.h>

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus
	typedef enum fort_hermes_error {
		NO_ERROR = 0,
		PROCESS_OVERFLOW = 1,
		PROCESS_TIMEOUT = 2,
		ILLUMINATION_ERROR = 3,
	} fort_hermes_error_e;


	typedef void fort_hermes_frame_readout_t;
	typedef void fort_hermes_ant_t;

	uint64_t fort_hermes_frame_readout_timestamp(fort_hermes_frame_readout_t * re);
	uint64_t fort_hermes_frame_readout_frame_id(fort_hermes_frame_readout_t * re);
	size_t fort_hermes_frame_readout_ant_size(fort_hermes_frame_readout_t * re);
	fort_hermes_ant_t * fort_hermes_frame_readout_ant(fort_hermes_frame_readout_t * re, size_t i);
	void fort_hermes_frame_readout_time(fort_hermes_frame_readout_t * re,struct timeval * res);
	fort_hermes_error_e fort_hermes_frame_readout_error(fort_hermes_frame_readout_t * re);

	void fort_hermes_frame_readout_destroy(fort_hermes_frame_readout_t * re);


	uint32_t fort_hermes_ant_id(fort_hermes_ant_t * a);
	double fort_hermes_ant_x(fort_hermes_ant_t * a);
	double fort_hermes_ant_y(fort_hermes_ant_t * a);
	double fort_hermes_ant_theta(fort_hermes_ant_t * a);

	typedef void * fort_hermes_context_t;

	fort_hermes_context_t * fort_hermes_open_file(const char * filename);

	fort_hermes_context_t * fort_hermes_connect(const char * address);

	void fort_hermes_context_destroy(fort_hermes_context_t * ctx);

	fort_hermes_frame_readout_t * fort_hermes_context_read(fort_hermes_context_t * ctx);

	fort_hermes_frame_readout_t * fort_hermes_context_poll(fort_hermes_context_t * ctx);


#ifdef __cplusplus
}  // extern "C"
#endif //__cplusplus
