// Copyright 2023 xensik. All rights reserved.
//
// Use of this source code is governed by a GNU GPLv3 license
// that can be found in the LICENSE file.

#include "xsk/stdinc.hpp"
#include "xsk/menu/common/location.hpp"
#include "xsk/menu/common/space.hpp"
#include "xsk/menu/common/token.hpp"

namespace xsk::menu
{

auto token::to_string() -> std::string
{
    switch (type)
    {
        case token::PLUS: return "+";
        case token::MINUS: return "-";
        case token::STAR: return "*";
        case token::DIV: return "/";
        case token::MOD: return "%";
        case token::BITOR: return "|";
        case token::BITAND: return "&";
        case token::BITEXOR: return "^";
        case token::SHL: return "<<";
        case token::SHR: return ">>";
        case token::ASSIGN: return "=";
        case token::PLUSEQ: return "+=";
        case token::MINUSEQ: return "-=";
        case token::STAREQ: return "*=";
        case token::DIVEQ: return "/=";
        case token::MODEQ: return "%=";
        case token::BITOREQ: return "|=";
        case token::BITANDEQ: return "&=";
        case token::BITEXOREQ: return "^=";
        case token::SHLEQ: return "<<=";
        case token::SHREQ: return ">>=";
        case token::INC: return "++";
        case token::DEC: return "--";
        case token::GT: return ">";
        case token::LT: return "<";
        case token::GE: return ">=";
        case token::LE: return "<=";
        case token::NE: return "!=";
        case token::EQ: return "==";
        case token::OR: return "||";
        case token::AND: return "&&";
        case token::TILDE: return "~";
        case token::BANG: return "!";
        case token::QMARK: return "?";
        case token::COLON: return ":";
        case token::SHARP: return "#";
        case token::COMMA: return ",";
        case token::DOT: return ".";
        case token::DOUBLEDOT: return "..";
        case token::ELLIPSIS: return "...";
        case token::SEMICOLON: return ";";
        case token::DOUBLECOLON: return "::";
        case token::LBRACKET: return "{";
        case token::RBRACKET: return "}";
        case token::LBRACE: return "[";
        case token::RBRACE: return "]";
        case token::LPAREN: return "(";
        case token::RPAREN: return ")";
        case token::NAME: return data;
        case token::STRING: return data;
        case token::INT: return data;
        case token::FLT: return data;
        default: return "*INTERNAL*";
    }
}

} // namespace xsk::menu
