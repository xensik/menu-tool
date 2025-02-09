// Copyright 2022 xensik. All rights reserved.
//
// Use of this source code is governed by a GNU GPLv3 license
// that can be found in the LICENSE file.

#pragma once

namespace xsk::menu::iw5
{

class item
{
public:
    static void validate_matrect(rectDef_s* rect, ParseContext* cxt, Material* mat);
    static void set_screen_coords(itemDef_t* item, float x, float y, int horzAlign, int vertAlign);
    static void init(itemDef_t* item);
    static void end(itemDef_t* item);
    static void free(itemDef_t* item);
    static auto parse(itemDef_t* item) -> bool;

private:
    using func_type = std::function<bool(itemDef_t*)>;
    static std::unordered_map<std::string_view, func_type> funcs_;
    static ParseContext parseContext;
    static auto parse_name(itemDef_t* item) -> bool;
    static auto parse_rect(itemDef_t* item) -> bool;
    static auto parse_rect480(itemDef_t* item) -> bool;
    static auto parse_rect720(itemDef_t* item) -> bool;
    static auto parse_pos480(itemDef_t* item) -> bool;
    static auto parse_pos720(itemDef_t* item) -> bool;
    static auto parse_origin(itemDef_t* item) -> bool;
    static auto parse_group(itemDef_t* item) -> bool;
    static auto parse_style(itemDef_t* item) -> bool;
    static auto parse_border(itemDef_t* item) -> bool;
    static auto parse_ownerdraw(itemDef_t* item) -> bool;
    static auto parse_ownerdrawflags(itemDef_t* item) -> bool;
    static auto parse_bordersize(itemDef_t* item) -> bool;
    static auto parse_decoration(itemDef_t* item) -> bool;
    static auto parse_screenspace(itemDef_t* item) -> bool;
    static auto parse_autowrapped(itemDef_t* item) -> bool;
    static auto parse_popup(itemDef_t* item) -> bool;
    static auto parse_outofbounds(itemDef_t* item) -> bool;
    static auto parse_legacysplitscreenscale(itemDef_t* item) -> bool;
    static auto parse_hiddenduringflashbang(itemDef_t* item) -> bool;
    static auto parse_hiddenduringscope(itemDef_t* item) -> bool;
    static auto parse_hiddenduringui(itemDef_t* item) -> bool;
    static auto parse_textonlyfocus(itemDef_t* item) -> bool;
    static auto parse_forecolor(itemDef_t* item) -> bool;
    static auto parse_backcolor(itemDef_t* item) -> bool;
    static auto parse_bordercolor(itemDef_t* item) -> bool;
    static auto parse_outlinecolor(itemDef_t* item) -> bool;
    static auto parse_disablecolor(itemDef_t* item) -> bool;
    static auto parse_background(itemDef_t* item) -> bool;
    static auto parse_type(itemDef_t* item) -> bool;
    static auto parse_align(itemDef_t* item) -> bool;
    static auto parse_textfont(itemDef_t* item) -> bool;
    static auto parse_textalign(itemDef_t* item) -> bool;
    static auto parse_textalignx(itemDef_t* item) -> bool;
    static auto parse_textaligny(itemDef_t* item) -> bool;
    static auto parse_textscale(itemDef_t* item) -> bool;
    static auto parse_textstyle(itemDef_t* item) -> bool;
    static auto parse_gamemsgwindowindex(itemDef_t* item) -> bool;
    static auto parse_gamemsgwindowmode(itemDef_t* item) -> bool;
    static auto parse_text(itemDef_t* item) -> bool;
    static auto parse_textfile(itemDef_t* item) -> bool;
    static auto parse_textsavegame(itemDef_t* item) -> bool;
    static auto parse_textcinematicsubtitle(itemDef_t* item) -> bool;
    static auto parse_mouseentertext(itemDef_t* item) -> bool;
    static auto parse_mouseexittext(itemDef_t* item) -> bool;
    static auto parse_mouseenter(itemDef_t* item) -> bool;
    static auto parse_mouseexit(itemDef_t* item) -> bool;
    static auto parse_action(itemDef_t* item) -> bool;
    static auto parse_accept(itemDef_t* item) -> bool;
    static auto parse_onfocus(itemDef_t* item) -> bool;
    static auto parse_hasfocus(itemDef_t* item) -> bool;
    static auto parse_leavefocus(itemDef_t* item) -> bool;
    static auto parse_dvar(itemDef_t* item) -> bool;
    static auto parse_dvarfloat(itemDef_t* item) -> bool;
    static auto parse_dvartest(itemDef_t* item) -> bool;
    static auto parse_execkey(itemDef_t* item) -> bool;
    static auto parse_execkeyint(itemDef_t* item) -> bool;
    static auto parse_enabledvar(itemDef_t* item) -> bool;
    static auto parse_disabledvar(itemDef_t* item) -> bool;
    static auto parse_showdvar(itemDef_t* item) -> bool;
    static auto parse_hidedvar(itemDef_t* item) -> bool;
    static auto parse_focusdvar(itemDef_t* item) -> bool;
    static auto parse_localvar(itemDef_t* item) -> bool;
    static auto parse_focussound(itemDef_t* item) -> bool;
    static auto parse_special(itemDef_t* item) -> bool;
    static auto parse_elementwidth(itemDef_t* item) -> bool;
    static auto parse_elementheight(itemDef_t* item) -> bool;
    static auto parse_elementtype(itemDef_t* item) -> bool;
    static auto parse_columns(itemDef_t* item) -> bool;
    static auto parse_doubleclick(itemDef_t* item) -> bool;
    static auto parse_notselectable(itemDef_t* item) -> bool;
    static auto parse_noscrollbars(itemDef_t* item) -> bool;
    static auto parse_usepaging(itemDef_t* item) -> bool;
    static auto parse_selectborder(itemDef_t* item) -> bool;
    static auto parse_selecticon(itemDef_t* item) -> bool;
    static auto parse_dvarstrlist(itemDef_t* item) -> bool;
    static auto parse_dvarfloatlist(itemDef_t* item) -> bool;
    static auto parse_dvarenumlist(itemDef_t* item) -> bool;
    static auto parse_maxchars(itemDef_t* item) -> bool;
    static auto parse_maxcharsgotonext(itemDef_t* item) -> bool;
    static auto parse_maxpaintchars(itemDef_t* item) -> bool;
    static auto parse_newsfeed(itemDef_t* item) -> bool;
    static auto parse_speed(itemDef_t* item) -> bool;
    static auto parse_spacing(itemDef_t* item) -> bool;
    static auto parse_visible(itemDef_t* item) -> bool;
    static auto parse_disabled(itemDef_t* item) -> bool;
    static auto parse_execexp(itemDef_t* item) -> bool;
    static auto parse_glowcolor(itemDef_t* item) -> bool;
    static auto parse_decodeeffect(itemDef_t* item) -> bool;
    static void validate_type_data(itemDef_t* item);
    static auto is_valid_text_alignment(int align) -> bool;
    static auto is_newsTickerDef(itemDef_t* item) -> bool;
    static auto is_editFieldDef(itemDef_t* item) -> bool;
    static auto get_newsTickerDef(itemDef_t* item) -> newsTickerDef_s*;
    static auto get_editFieldDef(itemDef_t* item) -> editFieldDef_s*;
    static auto get_listBoxDef(itemDef_t* item) -> listBoxDef_s*;
    static auto get_multiDef(itemDef_t* item) -> multiDef_s*;
    static auto get_enumDvarName(itemDef_t* item) -> const char**;
    static auto func(const char* keyword) -> func_type;
};

} // namespace xsk::menu::iw5
