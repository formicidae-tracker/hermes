#include "NetworkContext.h"

#include <stdexcept>

using namespace fort::hermes;


NetworkContext::NetworkContext(const std::string & address) {
	throw std::logic_error("Not yet implemented");
}
NetworkContext::~NetworkContext() {
}

void NetworkContext::Read(fort::hermes::FrameReadout * ro) {
	throw std::logic_error("Not yet implemented");
}

void NetworkContext::Poll(fort::hermes::FrameReadout * ro) {
	throw std::logic_error("Not yet implemented");
}
