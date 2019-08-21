#include "NetworkContext.h"

#include <stdexcept>

#include "Error.h"
#include "Header.pb.h"

using namespace fort::hermes;


NetworkContext::NetworkContext(const std::string & address)
	: d_socket(d_service)
	, d_sizeReceived(false)
	, d_readSize(0)
	, d_readMessage(0) {

	asio::ip::tcp::resolver resolver(d_service);
	asio::ip::tcp::resolver::query query(address,"tcp");
	try {
		asio::connect(d_socket,resolver.resolve(query));
	} catch ( const asio::system_error & e ) {
		throw InternalError(e.what(),FH_COULD_NOT_CONNECT);
	}

	fort::hermes::Header h;
	d_socket.non_blocking(true);
	for (;;) {
		try {
			ReadMessageUnsafe(h);
			break;
		} catch (const WouldBlock & e ) {
			asio::error_code ec;
			d_socket.read_some(asio::null_buffers(),ec);
			if (ec) {
				throw InternalError(ec.message(),FH_SOCKET_ERROR);
			}
		} catch (const InternalError & e ) {
			throw InternalError(e.what(),FH_STREAM_NO_HEADER);
		}
	}

	//TODO check version
	if (false) {
		throw InternalError("Wrong version",FH_STREAM_WRONG_VERSION);
	}

}
NetworkContext::~NetworkContext() {
}

void NetworkContext::Read(fort::hermes::FrameReadout * ro) {
	for (;;) {
		try {
			ReadMessageUnsafe(*ro);
			break;
		} catch (const WouldBlock & e ) {
			asio::error_code ec;
			d_socket.read_some(asio::null_buffers(),ec);
			if (ec) {
				throw InternalError(ec.message(),FH_SOCKET_ERROR);
			}
		}
	}
}

void NetworkContext::Poll(fort::hermes::FrameReadout * ro) {
	ReadMessageUnsafe(*ro);
}


size_t NetworkContext::ReadSome(uint8_t * data, size_t size) {
	asio::error_code ec;
	size_t read = d_socket.read_some(asio::mutable_buffers_1(data,size),ec);
	if ( ec == asio::error::interrupted ) {
		return ReadSome(data,size);

	}

	if ( ec == asio::error::would_block || ec == asio::error::try_again ) {
		throw WouldBlock();
	}

	if ( ec ) {
		throw InternalError(ec.message(),FH_SOCKET_ERROR);
	}

	return read;
}

void NetworkContext::ReadMessageUnsafe(google::protobuf::MessageLite & m) {
	for (;;) {
		if ( d_sizeReceived == false ) {
			if ( d_readSize == 0 ) {
				d_buffer.resize(10);
			}
			try {
				d_readSize += ReadSome(&(d_buffer[d_readSize]),1);
			} catch ( const InternalError & e ) {
				d_sizeReceived = false;
				d_readSize = 0;
				throw;
			}

			if (d_buffer[d_readSize-1] & 0x80 != 0 ) {
				continue;
			}

			google::protobuf::io::CodedInputStream cs(&(d_buffer[0]),d_readSize);

			if ( cs.ReadVarint32(&d_messageSize) == false ) {
				d_sizeReceived = false;
				d_readSize = 0;
				throw InternalError("Could not read varint32",FH_MESSAGE_DECODE_ERROR);
			}

			d_sizeReceived = true;
			d_readMessage = 0;
			d_buffer.clear();
			d_buffer.resize(d_messageSize);
		} else {
			try {
				d_readMessage += ReadSome(&(d_buffer[d_readMessage]),d_messageSize-d_readMessage);
			} catch ( const InternalError & e ) {
				d_sizeReceived = false;
				d_readSize = 0;
				throw;
			}
			if ( d_readMessage < d_messageSize ) {
				continue;
			}
			d_sizeReceived = false;
			d_readSize = 0;
			if (m.ParseFromArray(&(d_buffer[0]),d_messageSize) == false ) {
				throw InternalError("Could not parse message",FH_MESSAGE_DECODE_ERROR);
			}
			return;
		}
	}
}
