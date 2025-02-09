// Copyright 2023 xensik. All rights reserved.
//
// Use of this source code is governed by a GNU GPLv3 license
// that can be found in the LICENSE file.

#include "xsk/stdinc.hpp"
#include "xsk/menu/iw5/parser.hpp"

namespace xsk::menu::iw5
{

auto parser::parse(std::string const& name, std::vector<u8> const& data) -> void
{
    ppr_ = std::make_unique<preprocessor>(name, reinterpret_cast<char const*>(data.data()), data.size());
    
    parse_asset();
}

auto parser::parse_asset() -> void
{

}

auto parser::parse_menu() -> void
{

}

auto parser::parse_item() -> void
{

}

auto parser::parse_optflag(int* out) -> bool
{
    auto tok = next();

    if (tok.type == token::EOS)
        return false;
    
    if (tok.type == token::INT)
    {
        *out = std::stoi(tok.data);
        return true;
    }

    unread(tok);
    return false;
}

auto parser::parse_char(char* out) -> bool
{
    return false;
}

auto parser::parse_byte(char* out) -> bool
{
    return false;
}

auto parser::parse_int(int* out) -> bool
{
    return false;
}

auto parser::parse_int_exp(int* out) -> bool
{
    return false;
}

auto parser::parse_flt(float* out) -> bool
{
    return false;
}

auto parser::parse_flt_exp(float* out) -> bool
{
    return false;
}

auto parser::parse_string(const char** out) -> bool
{
    return false;
}

auto parser::parse_color(vec4_t* out) -> bool
{
    return false;
}

auto parser::parse_rect(rectDef_s* out) -> bool
{
    return false;
}

auto parser::parse_script(const char** out) -> bool
{
    return false;
}

auto parser::parse_event(MenuEventHandlerSet** out) -> bool
{
    return false;
}

auto parser::parse_expression(Statement_s** out) -> bool
{
    return false;
}


auto parser::next() -> token
{
    if (toks_.size())
    {
        auto tok = toks_.front();
        toks_.pop();
        return tok;
    }

    auto ntok = ppr_->process();
    // tok->type = ntok.type;
    // tok->space = ntok.space;
    // tok->pos = ntok.pos;
    // tok->data = std::move(ntok.data);
    return ntok;
}

auto parser::unread(token tok) -> void
{
    toks_.push(tok);
}

} // namespace xsk::menu::iw5
