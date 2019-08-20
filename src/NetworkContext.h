#pragma once

#include "Context.h"

namespace fort {

namespace hermes {

class NetworkContext : public Context {
public:
	NetworkContext(const std::string & address);
	virtual ~NetworkContext();

	void Read(fort::hermes::FrameReadout * ro);
	void Poll(fort::hermes::FrameReadout * ro);

};


} // namespace hermes

} // namespace fort
