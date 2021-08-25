#pragma once

#include <fort/hermes/FrameReadout.pb.h>

namespace fort {
namespace hermes {

class Context {
public:
	virtual ~Context(){};

	virtual void Read(fort::hermes::FrameReadout * ro) = 0;
};

} // namespace hermes
} //namespace fort
