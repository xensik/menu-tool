// Copyright 2023 xensik. All rights reserved.
//
// Use of this source code is governed by a GNU GPLv3 license
// that can be found in the LICENSE file.

#include "xsk/stdinc.hpp"
#include "xsk/menu/common/location.hpp"
#include "xsk/menu/common/error.hpp"

namespace xsk::menu
{

error::error(std::string const& what) : std::runtime_error(fmt::format("[ERROR] {}", what))
{
}

parse_error::parse_error(location const& loc, std::string const& what) : std::runtime_error(fmt::format("[ERROR] parse:{}: {}", loc.print(), what))
{
}

} // namespace xsk::menu
