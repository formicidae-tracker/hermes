#pragma once

#include <fort/hermes/Header.pb.h>

namespace fort {
namespace hermes {

void CheckFileHeader(const Header & header);
void CheckNetworkHeader(const Header & header);


} // namespace hermes
} // namespace fort
