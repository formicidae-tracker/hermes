#include "FileContext.h"

#include <stdexcept>

using namespace fort::hermes;


FileContext::FileContext(const std::string & filename) {
	throw std::logic_error("Not yet implemented");
}
FileContext::~FileContext() {
}

void FileContext::Read(fort::hermes::FrameReadout * ro) {
	throw std::logic_error("Not yet implemented");
}

void FileContext::Poll(fort::hermes::FrameReadout * ro) {
	throw std::logic_error("Not yet implemented");
}
