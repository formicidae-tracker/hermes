// libfort-hermes - Tracking File I/O library.
//
// Copyright (C) 2018-2023  Universitée de Lausanne
//
// This file is part of libfort-hermes.
//
// libfort-hermes is free software: you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation, either version 3 of the License, or (at your option) any
// later version.
//
// libfort-hermes is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
// details.
//
// You should have received a copy of the GNU General Public License along with
// libfort-hermes.  If not, see <http://www.gnu.org/licenses/>.

#include "hermes.h"

#include <fort/hermes/FrameReadout.pb.h>
#include <google/protobuf/util/time_util.h>

#include "Error.hpp"
#include "FileContext.hpp"
#include "NetworkContext.hpp"

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

void fh_error_safe_reset(fh_error_t *e) {
	if (e == NULL) {
		return;
	}
	e->code = FH_NO_ERROR;
	if (e->what == NULL) {
		return;
	}
	free(e->what);
	e->what = NULL;
}

uint32_t fh_tag_id(fh_tag_t *a) {
	return reinterpret_cast<fort::hermes::Tag *>(a)->id();
}

double fh_tag_x(fh_tag_t *a) {
	return reinterpret_cast<fort::hermes::Tag *>(a)->x();
}

double fh_tag_y(fh_tag_t *a) {
	return reinterpret_cast<fort::hermes::Tag *>(a)->y();
}

double fh_tag_theta(fh_tag_t *a) {
	return reinterpret_cast<fort::hermes::Tag *>(a)->theta();
}

uint64_t fh_frame_readout_timestamp(fh_frame_readout_t *re) {
	return reinterpret_cast<fort::hermes::FrameReadout *>(re)->timestamp();
}

uint64_t fh_frame_readout_frame_id(fh_frame_readout_t *re) {
	return reinterpret_cast<fort::hermes::FrameReadout *>(re)->frameid();
}

size_t fh_frame_readout_tag_size(fh_frame_readout_t *re) {
	return reinterpret_cast<fort::hermes::FrameReadout *>(re)->tags_size();
}

fh_tag_t *fh_frame_readout_tag(fh_frame_readout_t *re, size_t i) {
	fort::hermes::FrameReadout *re_ =
	    reinterpret_cast<fort::hermes::FrameReadout *>(re);
	if (i >= re_->tags_size()) {
		return NULL;
	}
	return reinterpret_cast<void *>(re_->mutable_tags(i));
}

void fh_frame_readout_time(fh_frame_readout_t *re, struct timeval *res) {
	*res = google::protobuf::util::TimeUtil::TimestampToTimeval(
	    reinterpret_cast<fort::hermes::FrameReadout *>(re)->time()
	);
}

fh_readout_error_e fh_frame_readout_error(fh_frame_readout_t *re) {
	return (fh_readout_error_e
	) reinterpret_cast<fort::hermes::FrameReadout *>(re)
	    ->error();
}

void fh_frame_readout_destroy(fh_frame_readout_t *re) {
	if (re == NULL) {
		return;
	}
	delete reinterpret_cast<fort::hermes::FrameReadout *>(re);
}

fh_context_t *fh_open_file(const char *filename, fh_error_t *err) {
	fh_error_safe_reset(err);
	try {
		return reinterpret_cast<void *>(new fort::hermes::FileContext(filename)
		);
	} catch (const fort::hermes::InternalError &e) {
		if (err != NULL) {
			err->what = strdup(e.what());
			err->code = e.Code();
		}
	}
	return NULL;
}

fh_context_t *
fh_connect(const char *host, int port, bool nonblocking, fh_error_t *err) {
	fh_error_safe_reset(err);
	try {
		return reinterpret_cast<void *>(
		    new fort::hermes::NetworkContext(host, port, nonblocking)
		);
	} catch (const fort::hermes::InternalError &e) {
		if (err != NULL) {
			err->what = strdup(e.what());
			err->code = e.Code();
		}
	}
	return NULL;
}

void fh_context_destroy(fh_context_t *ctx) {
	if (ctx != NULL) {
		delete reinterpret_cast<fort::hermes::Context *>(ctx);
	}
}

bool fh_context_read(
    fh_context_t *ctx, fh_frame_readout_t *ro, fh_error_t *err
) {
	fh_error_safe_reset(err);
	try {
		reinterpret_cast<fort::hermes::Context *>(ctx)->Read(
		    reinterpret_cast<fort::hermes::FrameReadout *>(ro)
		);
	} catch (const fort::hermes::WouldBlock &e) {
		if (err != NULL) {
			err->code = FH_NO_ERROR;
		}
		return false;
	} catch (const fort::hermes::EndOfFile &e) {
		if (err != NULL) {
			err->what = strdup("End Of File");
			err->code = FH_END_OF_STREAM;
		}
		return false;
	} catch (const fort::hermes::UnexpectedEndOfFileSequence &e) {
		if (err != NULL) {
			err->what = strdup(e.what());
			err->code = FH_UNEXPECTED_END_OF_FILE_SEQUENCE;
		}
		return false;
	} catch (const fort::hermes::InternalError &e) {
		if (err != NULL) {
			err->what = strdup(e.what());
			err->code = e.Code();
		}
		return false;
	}

	return true;
}

fh_error_t *fh_error_create() {
	auto res  = new fh_error_t();
	res->what = NULL;
	res->code = FH_NO_ERROR;
	return res;
}

void fh_error_destroy(fh_error_t *err) {
	if (err == NULL) {
		return;
	}
	if (err->what != NULL) {
		free(err->what);
	}
	delete err;
}

fh_frame_readout_t *fh_frame_readout_create() {
	return reinterpret_cast<void *>(new fort::hermes::FrameReadout());
}

int32_t fh_frame_readout_width(fh_frame_readout_t *re) {
	return reinterpret_cast<fort::hermes::FrameReadout *>(re)->width();
}

int32_t fh_frame_readout_height(fh_frame_readout_t *re) {
	return reinterpret_cast<fort::hermes::FrameReadout *>(re)->height();
}

#ifdef __cplusplus
} // extern "C"
#endif //__cplusplus
