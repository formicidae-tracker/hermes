#pragma once

#include "Context.h"

#include <asio.hpp>



namespace fort {

namespace hermes {

class NetworkContext : public Context {
public:
	NetworkContext(const std::string & host,int port, bool nonblocking);
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
