
// Copyright 2022 xensik. All rights reserved.
//
// Use of this source code is governed by a GNU GPLv3 license
// that can be found in the LICENSE file.

#pragma once

namespace xsk::menu::iw5
{

class reader
{
public:
    struct token
    {
        int type;
        int subtype;
        int intvalue;
        float floatvalue;
        char string[1024];
    };

    static auto init(const char* filename) -> bool;
    static void end();
    static auto read_token(token* out) -> bool;
    static auto read_flag_optional(int* out) -> bool;
    static auto read_float(float* out) -> bool;
    static auto read_float_expr(float* out) -> bool;
    static auto read_int(int* out) -> bool;
    static auto read_int_expr(int* out) -> bool;
    static auto read_int_line(int* out) -> bool;
    static auto read_char(char* out) -> bool;
    static auto read_byte(char* out) -> bool;
    static auto read_string(const char** out) -> bool;
    static auto read_color(vec4_t* out) -> bool;
    static auto read_rect(rectDef_s* out) -> bool;
    static auto read_rect(rectDef_s* out, int sizeMode, int virtualResolution, ParseContext *parseContext) -> bool;
    static auto read_rect_optionalbyteline(unsigned char* out) -> bool;
    static auto read_rect_checksemicolon() -> bool;
    static auto read_script(const char** out) -> bool;
    static auto read_event(MenuEventHandlerSet** out) -> bool;
    static auto read_expression(Statement_s** out) -> bool;
    static void source_error(const char* fmt, ...);
};

} // namespace xsk::menu::iw5
