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

#include "NetworkContext.hpp"

#include <stdexcept>

#include "CheckHeader.hpp"
#include "Error.hpp"
#include <fort/hermes/Header.pb.h>

namespace fort {
namespace hermes {

void NetworkContext::Reset() {
	d_bufferReceived = 0;
	d_sizeReceived   = false;
	d_buffer.resize(10);
	d_sizeByteLength = 1;
}

NetworkContext::NetworkContext(
    const std::string &host, int port, bool nonblocking
)
    : d_socket(d_service)
    , d_sizeReceived(false) {
	Reset();
	asio::ip::tcp::resolver        resolver(d_service);
	asio::ip::tcp::resolver::query query(host, std::to_string(port));
	try {
		asio::connect(d_socket, resolver.resolve(query));
	} catch (const asio::system_error &e) {
		throw InternalError(e.what(), FH_COULD_NOT_CONNECT);
	}

	fort::hermes::Header h;
	for (;;) {
		try {
			if (ReadMessageUnsafe(h) == true) {
				break;
			}
		} catch (const std::exception &e) {
			throw InternalError(
			    std::string("could not get header: ") + e.what(),
			    FH_STREAM_NO_HEADER
			);
		}
	}

	CheckNetworkHeader(h);

	if (nonblocking == true) {
		d_socket.non_blocking(true);
	}
}

NetworkContext::~NetworkContext() {
	d_socket.close();
}

void NetworkContext::Read(fort::hermes::FrameReadout *ro) {
	for (;;) {
		if (ReadMessageUnsafe(*ro) == true) {
			break;
		}
	}
}

void NetworkContext::ReadSome(size_t size) {
	if (d_bufferReceived >= size) {
		return;
	}
	asio::error_code ec;
	size_t           read = d_socket.read_some(
        asio::mutable_buffers_1(
            &(d_buffer[d_bufferReceived]),
            size - d_bufferReceived
        ),
        ec
    );
	d_bufferReceived += read;
	if (ec == asio::error::interrupted) {
		return ReadSome(size);
	}

	if (ec == asio::error::would_block || ec == asio::error::try_again) {
		throw WouldBlock();
	}
	if (ec == asio::error::eof) {
		throw EndOfFile();
	}

	if (ec) {
		throw InternalError(ec.message(), FH_SOCKET_ERROR);
	}
}

bool NetworkContext::ReadMessageUnsafe(google::protobuf::MessageLite &m) {
	if (d_sizeReceived == false) {
		try {
			ReadSome(d_sizeByteLength);
		} catch (const InternalError &e) {
			Reset();
			throw;
		}

		if ((d_buffer[d_sizeByteLength - 1] & 0x80) != 0) {
			++d_sizeByteLength;
			return false;
		}

		google::protobuf::io::CodedInputStream cs(
		    &(d_buffer[0]),
		    d_bufferReceived
		);

		if (cs.ReadVarint32(&d_messageSize) == false) {
			Reset();
			throw InternalError(
			    "Could not read varint32",
			    FH_MESSAGE_DECODE_ERROR
			);
		}

		d_sizeReceived   = true;
		d_bufferReceived = 0;
		d_buffer.clear();
		d_buffer.resize(d_messageSize);
		return false;
	} else {
		try {
			ReadSome(d_messageSize);
		} catch (const InternalError &e) {
			Reset();
			throw;
		}
		if (d_bufferReceived < d_messageSize) {
			return false;
		}
		if (m.ParseFromArray(&(d_buffer[0]), d_messageSize) == false) {
			Reset();
			throw InternalError(
			    "Could not parse message",
			    FH_MESSAGE_DECODE_ERROR
			);
		}
		Reset();
		return true;
	}
}

} // namespace hermes
} // namespace fort
