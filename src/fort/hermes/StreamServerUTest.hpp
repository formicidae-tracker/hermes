#pragma once


namespace fort {
namespace hermes {

class StreamServer {
public:
	typedef std::unique_ptr<StreamServer> Ptr;

	static Ptr Listen(int port);


private :
	StreamServer(int port);
};


} // namespace hermes
} // namespace fort
