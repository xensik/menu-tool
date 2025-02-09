// Copyright 2022 xensik. All rights reserved.
//
// Use of this source code is governed by a GNU GPLv3 license
// that can be found in the LICENSE file.

#pragma once

namespace xsk::menu::iw5
{

constexpr std::uint32_t MAX_ITEMDEFS_PER_MENUDEF = 512;
constexpr std::uint32_t MAX_MENUDEFS_PER_MENULIST = 512;
constexpr std::uint32_t MAX_TOKEN = 1024;

struct asset
{
    static float fadeClamp;
    static float fadeAmount;
    static float fadeInAmount;
    static int   fadeCycle;
    static auto load(const char* filename) -> MenuList*;
    static auto parse() -> bool;
};

class menu
{
public:
    static auto create() -> bool;

private:
    using func_type = std::function<bool(menuDef_t*)>;
    static std::unordered_map<std::string_view, func_type> funcs_;
    static ParseContext itemParseContext;
    static void init(menuDef_t* menu);
    static void end(menuDef_t* menu);
    static void free(menuDef_t* menu);
    static void update_position(menuDef_t* menu);
    static auto parse(menuDef_t* menu) -> bool;
    static auto parse_fullscreen(menuDef_t* menu) -> bool;
    static auto parse_fadecycle(menuDef_t* menu) -> bool;
    static auto parse_fadeclamp(menuDef_t* menu) -> bool;
    static auto parse_fadeamount(menuDef_t* menu) -> bool;
    static auto parse_fadeinamount(menuDef_t* menu) -> bool;
    static auto parse_blurworld(menuDef_t* menu) -> bool;
    static auto parse_onopen(menuDef_t* menu) -> bool;
    static auto parse_oncloserequest(menuDef_t* menu) -> bool;
    static auto parse_onclose(menuDef_t* menu) -> bool;
    static auto parse_onesc(menuDef_t* menu) -> bool;
    static auto parse_onfocusduetoclose(menuDef_t* menu) -> bool;
    static auto parse_execkey(menuDef_t* menu) -> bool;
    static auto parse_execkeyint(menuDef_t* menu) -> bool;
    static auto parse_visible(menuDef_t* menu) -> bool;
    static auto parse_allowedbinding(menuDef_t* menu) -> bool;
    static auto parse_soundloop(menuDef_t* menu) -> bool;
    static auto parse_focuscolor(menuDef_t* menu) -> bool;
    static auto parse_execexp(menuDef_t* menu) -> bool;
    static auto parse_priority(menuDef_t* menu) -> bool;
    static auto parse_name(menuDef_t* menu) -> bool;
    static auto parse_rect480(menuDef_t* menu) -> bool;
    static auto parse_rect720(menuDef_t* menu) -> bool;
    static auto parse_pos480(menuDef_t* menu) -> bool;
    static auto parse_pos720(menuDef_t* menu) -> bool;
    static auto parse_style(menuDef_t* menu) -> bool;
    static auto parse_border(menuDef_t* menu) -> bool;
    static auto parse_ownerdraw(menuDef_t* menu) -> bool;
    static auto parse_ownerdrawflags(menuDef_t* menu) -> bool;
    static auto parse_bordersize(menuDef_t* menu) -> bool;
    static auto parse_decoration(menuDef_t* menu) -> bool;
    static auto parse_screenspace(menuDef_t* menu) -> bool;
    static auto parse_popup(menuDef_t* menu) -> bool;
    static auto parse_outofbounds(menuDef_t* menu) -> bool;
    static auto parse_legacysplitscreenscale(menuDef_t* menu) -> bool;
    static auto parse_hiddenduringflashbang(menuDef_t* menu) -> bool;
    static auto parse_hiddenduringscope(menuDef_t* menu) -> bool;
    static auto parse_hiddenduringui(menuDef_t* menu) -> bool;
    static auto parse_textonlyfocus(menuDef_t* menu) -> bool;
    static auto parse_forecolor(menuDef_t* menu) -> bool;
    static auto parse_backcolor(menuDef_t* menu) -> bool;
    static auto parse_bordercolor(menuDef_t* menu) -> bool;
    static auto parse_outlinecolor(menuDef_t* menu) -> bool;
    static auto parse_disablecolor(menuDef_t* menu) -> bool;
    static auto parse_background(menuDef_t* menu) -> bool;
    static auto parse_itemdef(menuDef_t* menu) -> bool;
    static auto func(const char* keyword) -> func_type;
};

} // namespace xsk::menu::iw5
