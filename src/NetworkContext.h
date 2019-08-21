#pragma once

#include "Context.h"

#include <asio.hpp>



namespace fort {

namespace hermes {

class NetworkContext : public Context {
public:
	NetworkContext(const std::string & address);
	virtual ~NetworkContext();

	void Read(fort::hermes::FrameReadout * ro);
	void Poll(fort::hermes::FrameReadout * ro);

private:

	void ReadMessageUnsafe(google::protobuf::MessageLite & m);

	size_t ReadSome(uint8_t * data, size_t size);

	asio::io_service      d_service;
	asio::ip::tcp::socket d_socket;
	std::vector<uint8_t>  d_buffer;
	bool                  d_sizeReceived;
	size_t                d_readSize,d_readMessage;
	uint32_t              d_messageSize;
};


} // namespace hermes

} // namespace fort
