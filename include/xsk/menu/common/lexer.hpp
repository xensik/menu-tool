// Copyright 2023 xensik. All rights reserved.
//
// Use of this source code is governed by a GNU GPLv3 license
// that can be found in the LICENSE file.

#pragma once

#include "xsk/menu/common/lookahead.hpp"
#include "xsk/menu/common/location.hpp"
#include "xsk/menu/common/space.hpp"
#include "xsk/menu/common/token.hpp"

namespace xsk::menu
{

class lexer
{
    lookahead reader_;
    location loc_;
    usize buflen_;
    spacing spacing_;
    std::array<char, 0x1000> buffer_;

public:
    lexer(std::string const& name, char const* data, usize size);
    auto lex() -> token;

private:
    auto push(char c) -> void;
    auto advance() -> void;
    auto linewrap() -> void;
};

} // namespace xsk::menu
