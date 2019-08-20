#include "hermes.h"

#include "FrameReadout.pb.h"
#include <google/protobuf/util/time_util.h>

#include "Context.h"


#ifdef __cplusplus
extern "C" {
#endif //__cplusplus


	uint32_t fort_hermes_ant_id(fort_hermes_ant_t * a) {
		return reinterpret_cast<fort::hermes::Ant*>(a)->id();
	}

	double fort_hermes_ant_x(fort_hermes_ant_t * a) {
		return reinterpret_cast<fort::hermes::Ant*>(a)->x();
	}

	double fort_hermes_ant_y(fort_hermes_ant_t * a) {
		return reinterpret_cast<fort::hermes::Ant*>(a)->y();
	}

	double fort_hermes_ant_theta(fort_hermes_ant_t * a) {
		return reinterpret_cast<fort::hermes::Ant*>(a)->theta();
	}

	uint64_t fort_hermes_frame_readout_timestamp(fort_hermes_frame_readout_t * re) {
		return reinterpret_cast<fort::hermes::FrameReadout*>(re)->timestamp();
	}
	uint64_t fort_hermes_frame_readout_frame_id(fort_hermes_frame_readout_t * re) {
		return reinterpret_cast<fort::hermes::FrameReadout*>(re)->frameid();
	}
	size_t fort_hermes_frame_readout_ant_size(fort_hermes_frame_readout_t * re) {
		return reinterpret_cast<fort::hermes::FrameReadout*>(re)->ants_size();
	}
	fort_hermes_ant_t * fort_hermes_frame_readout_ant(fort_hermes_frame_readout_t * re, size_t i) {
		fort::hermes::FrameReadout * re_ = reinterpret_cast<fort::hermes::FrameReadout*>(re);
		if ( i >= re_->ants_size() ) {
			return NULL;
		}
		return reinterpret_cast<void*>(re_->mutable_ants(i));
	}

	void fort_hermes_frame_readout_time(fort_hermes_frame_readout_t * re,struct timeval * res) {
		*res = google::protobuf::util::TimeUtil::TimestampToTimeval(reinterpret_cast<fort::hermes::FrameReadout*>(re)->time());
	}

	fort_hermes_error_e fort_hermes_frame_readout_error(fort_hermes_frame_readout_t * re) {
		return (fort_hermes_error_e)reinterpret_cast<fort::hermes::FrameReadout*>(re)->error();
	}

	void fort_hermes_frame_readout_destroy(fort_hermes_frame_readout_t * re) {
		delete reinterpret_cast<fort::hermes::FrameReadout*>(re);
	}

	fort_hermes_context_t * fort_hermes_open_file(const char * filename,fort_hermes_error_e * err) {
		if ( err != NULL ) {
			*err = UNHANDLED_INTERNAL_ERROR;
		}
		return NULL;
	}

	fort_hermes_context_t * fort_hermes_connect(const char * address,fort_hermes_error_e * err) {
		if ( err != NULL ) {
			*err = UNHANDLED_INTERNAL_ERROR;
		}
		return NULL;
	}

	void fort_hermes_context_destroy(fort_hermes_context_t * ctx) {
		delete reinterpret_cast<fort::hermes::Context*>(ctx);
	}

	fort_hermes_frame_readout_t * fort_hermes_context_read(fort_hermes_context_t * ctx,fort_hermes_error_e * err) {
		fort::hermes::FrameReadout * ro = new fort::hermes::FrameReadout();
		try {
			reinterpret_cast<fort::hermes::Context*>(ctx)->Read(ro);
			return reinterpret_cast<void*>(ro);
		} catch ( const std::exception & e ) {
			if ( err != NULL ) {
				*err = UNHANDLED_INTERNAL_ERROR;
			}
			return NULL;
		}
	}

	fort_hermes_frame_readout_t * fort_hermes_context_poll(fort_hermes_context_t * ctx,fort_hermes_error_e * err) {
		fort::hermes::FrameReadout * ro = new fort::hermes::FrameReadout();
		try {
			reinterpret_cast<fort::hermes::Context*>(ctx)->Poll(ro);
			return reinterpret_cast<void*>(ro);
		} catch ( const std::exception & e) {
			if ( err != NULL ) {
				*err = UNHANDLED_INTERNAL_ERROR;
			}
			return NULL;
		}
	}



#ifdef __cplusplus
}  // extern "C"
#endif //__cplusplus
