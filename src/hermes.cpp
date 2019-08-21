#include "hermes.h"

#include "FrameReadout.pb.h"
#include <google/protobuf/util/time_util.h>

#include "NetworkContext.h"
#include "FileContext.h"
#include "Error.h"

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus


	uint32_t fh_ant_id(fh_ant_t * a) {
		return reinterpret_cast<fort::hermes::Ant*>(a)->id();
	}

	double fh_ant_x(fh_ant_t * a) {
		return reinterpret_cast<fort::hermes::Ant*>(a)->x();
	}

	double fh_ant_y(fh_ant_t * a) {
		return reinterpret_cast<fort::hermes::Ant*>(a)->y();
	}

	double fh_ant_theta(fh_ant_t * a) {
		return reinterpret_cast<fort::hermes::Ant*>(a)->theta();
	}

	uint64_t fh_frame_readout_timestamp(fh_frame_readout_t * re) {
		return reinterpret_cast<fort::hermes::FrameReadout*>(re)->timestamp();
	}
	uint64_t fh_frame_readout_frame_id(fh_frame_readout_t * re) {
		return reinterpret_cast<fort::hermes::FrameReadout*>(re)->frameid();
	}
	size_t fh_frame_readout_ant_size(fh_frame_readout_t * re) {
		return reinterpret_cast<fort::hermes::FrameReadout*>(re)->ants_size();
	}
	fh_ant_t * fh_frame_readout_ant(fh_frame_readout_t * re, size_t i) {
		fort::hermes::FrameReadout * re_ = reinterpret_cast<fort::hermes::FrameReadout*>(re);
		if ( i >= re_->ants_size() ) {
			return NULL;
		}
		return reinterpret_cast<void*>(re_->mutable_ants(i));
	}

	void fh_frame_readout_time(fh_frame_readout_t * re,struct timeval * res) {
		*res = google::protobuf::util::TimeUtil::TimestampToTimeval(reinterpret_cast<fort::hermes::FrameReadout*>(re)->time());
	}

	fh_readout_error_e fh_frame_readout_error(fh_frame_readout_t * re) {
		return (fh_readout_error_e)reinterpret_cast<fort::hermes::FrameReadout*>(re)->error();
	}

	void fh_frame_readout_destroy(fh_frame_readout_t * re) {
		if ( re == NULL ) {
			return;
		}
		delete reinterpret_cast<fort::hermes::FrameReadout*>(re);
	}

	fh_context_t * fh_open_file(const char * filename,fh_error_t * err) {
		try {
			return reinterpret_cast<void*>(new fort::hermes::FileContext(filename));
		} catch (const std::exception & e) {
			if ( err != NULL ) {
				err->what = strdup(e.what());
			}
		}
		return NULL;
	}

	fh_context_t * fh_connect(const char * address,fh_error_t * err) {
		try {
			return reinterpret_cast<void*>(new fort::hermes::NetworkContext(address));
		} catch (const std::exception & e) {
			if ( err != NULL ) {
				err->what = strdup(e.what());
			}
		}
		return NULL;
	}

	void fh_context_destroy(fh_context_t * ctx) {
		if ( ctx != NULL ) {
			delete reinterpret_cast<fort::hermes::Context*>(ctx);
		}
	}

	bool fh_context_read(fh_context_t * ctx,fh_frame_readout_t * ro,fh_error_t * err) {
		try {
			reinterpret_cast<fort::hermes::Context*>(ctx)->Read(reinterpret_cast<fort::hermes::FrameReadout*>(ro));
		} catch ( const fort::hermes::EndOfFile & e) {
			if ( err != NULL ) {
				err->what = strdup("End Of File");
				err->code = FH_END_OF_STREAM;
			}
			return false;
		} catch ( const fort::hermes::InternalError & e ) {
			if ( err != NULL ) {
				err->what = strdup(e.what());
				err->code = e.Code();
			}
			return false;
		}

		return true;
	}

	bool fh_context_poll(fh_context_t * ctx,fh_frame_readout_t * ro,fh_error_t * err) {
		try {
			reinterpret_cast<fort::hermes::Context*>(ctx)->Poll(reinterpret_cast<fort::hermes::FrameReadout*>(ro));
		} catch ( const fort::hermes::EndOfFile & e) {
			err->code = FH_END_OF_STREAM;
			err->what = strdup("End Of Stream");
			return false;
		} catch ( const fort::hermes::WouldBlock & e ) {
			return false;
		} catch ( const fort::hermes::InternalError & e ) {
			err->what = strdup(e.what());
			err->code = e.Code();
			return false;
		}
		return true;
	}

	fh_error_t * fh_error_create() {
		auto res = new fh_error_t();
		res->what = NULL;
		return res;
	}

	void fh_error_destroy(fh_error_t * err) {
		if ( err == NULL ) {
			return;
		}
		if ( err->what != NULL ) {
			free(err->what);
		}
		delete err;
	}

	fh_frame_readout_t * fh_frame_readout_create() {
		return reinterpret_cast<void*>(new fort::hermes::FrameReadout());
	}

#ifdef __cplusplus
}  // extern "C"
#endif //__cplusplus
