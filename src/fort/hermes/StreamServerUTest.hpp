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

#pragma once

#include <asio/io_context.hpp>
#include <asio/ip/tcp.hpp>
#include <fort/hermes/FrameReadout.pb.h>
#include <thread>

namespace fort {
namespace hermes {

class StreamServer {
public:
	struct Options {
		int                                       Port;
		std::vector<FrameReadout>::const_iterator Begin, End;
		bool                                      WriteHeader;
	};

	StreamServer(const Options &opts);
	~StreamServer();

private:
	void ListenOnce();
	void Write(asio::ip::tcp::socket socket);

	Options                 d_options;
	asio::io_context        d_context;
	asio::ip::tcp::acceptor d_acceptor;
	std::thread             d_thread;
};

} // namespace hermes
} // namespace fort
