// libfort-hermes - Tracking File I/O library.
//
// Copyright (C) 2018-2023  Universitée de Lausanne
//
// This file is part of libfort-hermes.
//
// libfort-hermes is free software: you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation, either version 3 of the License, or (at your option) any
// later version.
//
// libfort-hermes is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
// details.
//
// You should have received a copy of the GNU General Public License along with
// libfort-hermes.  If not, see <http://www.gnu.org/licenses/>.

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
	virtual ~Context() = default;

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
	virtual void Read(fort::hermes::FrameReadout *ro) = 0;
};

} // namespace hermes
} // namespace fort
