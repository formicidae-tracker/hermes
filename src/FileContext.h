#pragma once

#include "Context.h"

namespace fort {

namespace hermes {

class FileContext : public Context {
public:
	FileContext(const std::string & filename);
	virtual ~FileContext();

	void Read(fort::hermes::FrameReadout * ro);
	void Poll(fort::hermes::FrameReadout * ro);

};

} // namespace hermes

} // namespace fort
