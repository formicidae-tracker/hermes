#pragma once

#include <fort/hermes/FrameReadout.pb.h>
#include <asio/io_context.hpp>
#include <asio/ip/tcp.hpp>
#include <thread>
namespace fort {
namespace hermes {

class StreamServer {
public:
	struct Options {
		int Port;
		std::vector<FrameReadout>::const_iterator Begin,End;
		bool WriteHeader;
	};

	StreamServer(const Options & opts);
	~StreamServer();
private :

	void ListenOnce();
	void Write(asio::ip::tcp::socket socket);

	Options d_options;
	asio::io_context        d_context;
	asio::ip::tcp::acceptor d_acceptor;
	std::thread             d_thread;
};


} // namespace hermes
} // namespace fort
