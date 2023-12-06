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

#include "StreamServerUTest.hpp"

#include <fort/hermes/Header.pb.h>
#include <google/protobuf/util/delimited_message_util.h>

#include <asio/write.hpp>

namespace fort {
namespace hermes {

StreamServer::StreamServer(const Options &opts)
    : d_options(opts)
    , d_context()
    , d_acceptor(
          d_context, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), opts.Port)
      ) {
	ListenOnce();
	d_thread = std::thread([this]() { d_context.run(); });
}

StreamServer::~StreamServer() {
	d_context.stop();
	d_thread.join();
}

void StreamServer::ListenOnce() {
	d_acceptor.async_accept(
	    [this](asio::error_code ec, asio::ip::tcp::socket socket) {
		    if (!ec) {
			    Write(std::move(socket));
		    }
	    }
	);
}

void WriteMessage(
    asio::ip::tcp::socket &socket, const google::protobuf::MessageLite &m
) {
	std::ostringstream out;
	google::protobuf::util::SerializeDelimitedToOstream(m, &out);
	asio::async_write(
	    socket,
	    asio::buffer(out.str()),
	    [](asio::error_code ec, size_t length) {
		    if (ec) {
			    std::cerr << "could not write message: " << ec << " after "
			              << length << "bytes" << std::endl;
		    }
	    }
	);
}

void StreamServer::Write(asio::ip::tcp::socket socket) {
	if (d_options.WriteHeader == true) {
		Header h;
		h.mutable_version()->set_vmajor(0);
		h.mutable_version()->set_vminor(1);
		h.set_type(Header_Type_Network);
		WriteMessage(socket, h);
	}
	for (auto line = d_options.Begin; line != d_options.End; ++line) {
		WriteMessage(socket, *line);
	}
}

} // namespace hermes
} // namespace fort
