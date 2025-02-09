// Copyright 2023 xensik. All rights reserved.
//
// Use of this source code is governed by a GNU GPLv3 license
// that can be found in the LICENSE file.

#pragma once

#include "xsk/menu/iw5/structs.hpp"
#include "xsk/menu/common/preprocessor.hpp"

namespace xsk::menu::iw5
{

class parser
{
    std::unique_ptr<preprocessor> ppr_;
    std::queue<token> toks_;

public:
    auto parse(std::string const& name, std::vector<u8> const& data) -> void;
    auto parse_asset() -> void;
    auto parse_menu() -> void;
    auto parse_item() -> void;

    auto parse_char(char* out) -> bool;
    auto parse_byte(char* out) -> bool;
    auto parse_optflag(int* out) -> bool;
    auto parse_int(int* out) -> bool;
    auto parse_int_exp(int* out) -> bool;
    auto parse_flt(float* out) -> bool;
    auto parse_flt_exp(float* out) -> bool;
    auto parse_string(const char** out) -> bool;
    auto parse_color(vec4_t* out) -> bool;
    auto parse_rect(rectDef_s* out) -> bool;
    auto parse_script(const char** out) -> bool;
    auto parse_event(MenuEventHandlerSet** out) -> bool;
    auto parse_expression(Statement_s** out) -> bool;

    auto next() -> token;
    auto unread(token tok) -> void;
};

} // namespace xsk::menu::iw5
