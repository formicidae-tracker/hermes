#pragma once

#include <fort/hermes/FrameReadout.pb.h>

namespace fort {
namespace hermes {

/**
 * Base class for reading FrameReadout from either file sequences or
 * network stream
 */
class Context {
public:
	virtual ~Context(){};

	/**
	 * Reads a FrameReadout from the Context
	 *
	 * @param ro the FrameReadout to parse to
	 *
	 * @throws EndOfFile if the Context does not contains any other
	 *         data
	 * @throws UnexpectedEndOfFileSequence if a sequence of files does
 	 *         not end up gracefully
	 * @throws WouldBlock if a non-blocking connection has no messages
  	 *         yet ready.
	 * @throws InternalError if any other error occured
	 */
	virtual void Read(fort::hermes::FrameReadout * ro) = 0;
};

} // namespace hermes
} //namespace fort
