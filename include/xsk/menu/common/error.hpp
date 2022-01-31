// Copyright 2023 xensik. All rights reserved.
//
// Use of this source code is governed by a GNU GPLv3 license
// that can be found in the LICENSE file.

#pragma once

namespace xsk::menu
{

class error : public std::runtime_error
{
public:
    error(std::string const& what);
};

class parse_error : public std::runtime_error
{
public:
    parse_error(location const& loc, std::string const& what);
};

} // namespace xsk::menu
