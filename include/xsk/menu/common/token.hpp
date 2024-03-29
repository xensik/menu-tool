// Copyright 2023 xensik. All rights reserved.
//
// Use of this source code is governed by a GNU GPLv3 license
// that can be found in the LICENSE file.

#pragma once

namespace xsk::menu
{

struct token
{
    enum kind : u8
    {
        PLUS, MINUS, STAR, DIV, MOD, BITOR, BITAND, BITEXOR, SHL, SHR,
        ASSIGN, PLUSEQ, MINUSEQ, STAREQ, DIVEQ, MODEQ, BITOREQ, BITANDEQ, BITEXOREQ, SHLEQ, SHREQ,
        INC, DEC, GT, LT, GE, LE, NE, EQ, OR, AND, TILDE, BANG, QMARK, COLON, SHARP, COMMA, DOT,
        DOUBLEDOT, ELLIPSIS, SEMICOLON, DOUBLECOLON, LBRACKET, RBRACKET, LBRACE, RBRACE, LPAREN, RPAREN,

        NAME, STRING, INT, FLT,

        HASH, NEWLINE, EOS, DEFINED, MACROBEGIN, MACROEND, MACROARG, MACROVAOPT, MACROVAARGS, STRINGIZE, PASTE
    };

    kind type;
    spacing space;
    location pos;
    std::string data;

    token(kind type, spacing space, location pos) : type{ type }, space{ space },  pos{ pos }, data{} {}
    token(kind type, spacing space, location pos, std::string data) : type{ type }, space{ space },  pos{ pos }, data{ std::move(data) } {}
    auto to_string() -> std::string;
};

} // namespace xsk::menu
