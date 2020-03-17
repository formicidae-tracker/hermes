#include "NetworkContext.h"

#include <stdexcept>

#include "Error.h"
#include <fort/hermes/Header.pb.h>

using namespace fort::hermes;


void NetworkContext::Reset() {
	d_bufferReceived = 0;
	d_sizeReceived = false;
	d_buffer.resize(10);
	d_sizeByteLength = 1;
}

NetworkContext::NetworkContext(const std::string & host, int port, bool nonblocking)
	: d_socket(d_service)
	, d_sizeReceived(false) {
	Reset();
	asio::ip::tcp::resolver resolver(d_service);
	asio::ip::tcp::resolver::query query(host,"4002");
	try {
		asio::connect(d_socket,resolver.resolve(query));
	} catch ( const asio::system_error & e ) {
		throw InternalError(e.what(),FH_COULD_NOT_CONNECT);
	}

	fort::hermes::Header h;
	for (;;) {
		try {
			ReadMessageUnsafe(h);
			break;
		} catch (const InternalError & e ) {
			throw InternalError(e.what(),FH_STREAM_NO_HEADER);
		}
	}


	//TODO check version
	if (false) {
		d_socket.close();
		throw InternalError("Wrong version",FH_STREAM_WRONG_VERSION);
	}

	if (nonblocking == true ) {
		d_socket.non_blocking(true);
	}
}
NetworkContext::~NetworkContext() {
	d_socket.close();
}

void NetworkContext::Read(fort::hermes::FrameReadout * ro) {
	for (;;) {
		if (ReadMessageUnsafe(*ro) == true ) {
			break;
		}
	}
}


void NetworkContext::ReadSome(size_t size) {
	if ( d_bufferReceived >= size ) {
		return;
	}
	asio::error_code ec;
	size_t read = d_socket.read_some(asio::mutable_buffers_1(&(d_buffer[d_bufferReceived]),size-d_bufferReceived),ec);
	d_bufferReceived += read;
	if ( ec == asio::error::interrupted ) {
		return ReadSome(size);
	}

	if ( ec == asio::error::would_block || ec == asio::error::try_again ) {
		throw WouldBlock();
	}

	if ( ec ) {
		throw InternalError(ec.message(),FH_SOCKET_ERROR);
	}
}

bool NetworkContext::ReadMessageUnsafe(google::protobuf::MessageLite & m) {
	if ( d_sizeReceived == false ) {
		try {
			ReadSome(d_sizeByteLength);
		} catch ( const InternalError & e ) {
			Reset();
			throw;
		}

		if ( (d_buffer[d_sizeByteLength-1] & 0x80) != 0 ) {
			++d_sizeByteLength;
			return false;
		}

		google::protobuf::io::CodedInputStream cs(&(d_buffer[0]),d_bufferReceived);

		if ( cs.ReadVarint32(&d_messageSize) == false ) {
			Reset();
			throw InternalError("Could not read varint32",FH_MESSAGE_DECODE_ERROR);
		}

		d_sizeReceived = true;
		d_bufferReceived = 0;
		d_buffer.clear();
		d_buffer.resize(d_messageSize);
		return false;
	} else {
		try {
			ReadSome(d_messageSize);
		} catch ( const InternalError & e ) {
			Reset();
			throw;
		}
		if ( d_bufferReceived < d_messageSize ) {
			return false;
		}
		if (m.ParseFromArray(&(d_buffer[0]),d_messageSize) == false ) {
			Reset();
			throw InternalError("Could not parse message",FH_MESSAGE_DECODE_ERROR);
		}
		Reset();
		return true;
	}

}
