#include "CheckHeader.hpp"

#include "Error.hpp"

namespace fort {
namespace hermes {

void CheckVersion(const Version & version) {
	std::set<std::pair<int,int>> supportedVersions = {
	                                                  {0,1},
	                                                  {0,2},
	                                                  {0,3},
	                                                  {0,4},
	                                                  {0,5},
	};

	if ( supportedVersions.count(std::make_pair(version.vmajor(),
	                                            version.vminor())) == 0 ) {
		throw InternalError("unsupported version "
		                    + std::to_string(version.vmajor())
		                    + "."
		                    + std::to_string(version.vminor()),
		                    FH_STREAM_WRONG_VERSION);
	}
}



void CheckFileHeader(const Header & header) {
	CheckVersion(header.version());
	if ( header.type() != Header_Type_File ) {
		throw InternalError("wrong header type",FH_STREAM_NO_HEADER);
	}
	if ( header.width() == 0 ) {
		throw InternalError("missing width",FH_STREAM_NO_HEADER);
	}
	if ( header.height() == 0 ) {
		throw InternalError("missing height",FH_STREAM_NO_HEADER);
	}
}

void CheckNetworkHeader(const Header & header) {
	CheckVersion(header.version());
	if ( header.type() != Header_Type_Network ) {
		throw InternalError("wrong header type",FH_STREAM_NO_HEADER);
	}
}

} // namespace hermes
} // namespace fort
