#pragma once

#include "Context.hpp"

#include <asio.hpp>



namespace fort {
namespace hermes {

/**
 * A Context that reads FrameReadout from a live leto instance.
 */
class NetworkContext : public Context {
public:
	/**
	 * Opens a leto broadcast
	 *
	 * @param host the host to connect to
	 * @param port the port to connect to, usually 4002
	 * @param nonblocking if true the connection will be non-blocking,
	 *        meaning that WouldBlock can be thrown if no message are
	 *        ready when Reading
	 *
	 * @throws asio::system_error if no connection could be made
	 * @throws InternalError if no valid header is found in the stream
	 */
	NetworkContext(const std::string & host,int port = 4002, bool nonblocking = false);
	virtual ~NetworkContext();

	void Read(fort::hermes::FrameReadout * ro);

private:
	void Reset();
	bool ReadMessageUnsafe(google::protobuf::MessageLite & m);

	void ReadSome(size_t size);

	asio::io_service      d_service;
	asio::ip::tcp::socket d_socket;
	std::vector<uint8_t>  d_buffer;
	bool                  d_sizeReceived;
	size_t                d_bufferReceived,d_sizeByteLength;
	uint32_t              d_messageSize;
};


} // namespace hermes
} // namespace fort
