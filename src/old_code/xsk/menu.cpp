// Copyright 2022 xensik. All rights reserved.
//
// Use of this source code is governed by a GNU GPLv3 license
// that can be found in the LICENSE file.

#include "stdafx.hpp"
#include "iw5.hpp"
#include "memory.hpp"
#include "reader.hpp"
#include "menu.hpp"
#include "item.hpp"
#include "window.hpp"

namespace xsk::menu::iw5
{

itemDef_t* itemDefs[MAX_ITEMDEFS_PER_MENUDEF];
MenuList* menuList;

int   asset::fadeCycle = 0;
float asset::fadeClamp = 0.0;
float asset::fadeAmount = 0.0;
float asset::fadeInAmount = 0.0;

ParseContext menu::itemParseContext;

auto asset::load(const char* filename) -> MenuList*
{
	reader::token token;

	asset::fadeCycle = 0;
	asset::fadeClamp = 0.0;
	asset::fadeAmount = 0.0;
	asset::fadeInAmount = 0.0;

	std::memset(itemDefs, 0, sizeof(itemDefs));
	menuList = memory::alloc<MenuList>();

	printf("\tLoading menu '%s'...\n", filename);

	if (reader::init(filename))
	{
		while (reader::read_token(&token))
		{
			if (stricmp(token.string, "}") && stricmp(token.string, "{"))
			{
				if (stricmp(token.string, "assetglobaldef") )
				{
					if (stricmp(token.string, "menudef"))
					{
						reader::source_error("Unknown token %s in menu file.  Expected \"menudef\" or \"assetglobaldef\".\n", token.string);
					}
					else if (!menu::create())
					{
						break;
					}
				}
				else if (!asset::parse())
				{
					break;
				}
			}
		}
	
		reader::end();
		return menuList;
	}

	printf("Couldn't find menu file '%s'\n");
	return 0;
}

auto asset::parse() -> bool
{
	reader::token token;

	if (!reader::read_token(&token))
		return false;

	if (*token.string != '{')
		return false;

	while (true)
	{
		do
		{
			if (!reader::read_token(&token))
				return false;
		}
		while (!stricmp(token.string, ";"));

		if (*token.string == '}')
			return true;

		if (stricmp(token.string, "fadeClamp"))
		{
			if (!reader::read_float(&fadeClamp))
            	return false;
		}
		else if (stricmp(token.string, "fadeCycle"))
		{
			if (!reader::read_int(&fadeCycle))
            	return false;
		}
		else if (stricmp(token.string, "fadeAmount"))
		{
			if (!reader::read_float(&fadeAmount))
            	return false;
		}
		else if (stricmp(token.string, "fadeInAmount"))
		{
			if (!reader::read_float(&fadeInAmount))
            	return false;
		}
		else
		{
			reader::source_error("Unknown token %s in assetGlobalDef.  Valid commands are 'fadeClamp', 'fadeCycle', 'fadeAmount', and 'fadeInAmount'\n", token.string);
		}
	}

	return false;
}

auto menu::create() -> bool
{
	auto menu = memory::alloc<menuDef_t>();

	menu::init(menu);

	if (!menu::parse(menu))
	{
        menu::free(menu);
		memory::free(menu);
		return false;
	}

	if (!menu->window.name)
	{
		menu::free(menu);
		memory::free(menu);
		reader::source_error("menu has no name");
		return false;
	}

	menu::end(menu);

	if (menuList->menuCount >= MAX_MENUDEFS_PER_MENULIST)
	{
		menu::free(menu);
		memory::free(menu);
		//Com_Error(ERR_DROP, "too many menuDefs in menuList %s, MAX is %i", menuList->name, MAX_MENUDEFS_PER_MENULIST);
		return false;
	}

	menuList->menus[menuList->menuCount++] = menu;
	return true;
}

void menu::init(menuDef_t* menu)
{
    std::memset(menu, 0, sizeof(menuDef_t));
	menu->items = &itemDefs[0];
	menu->data = memory::alloc<menuData_t>();
	std::memset(menu->data, 0, sizeof(menuData_t));
	menu->data->fadeAmount = asset::fadeAmount;
	menu->data->fadeInAmount = asset::fadeInAmount;
	menu->data->fadeClamp = asset::fadeClamp;
	menu->data->fadeCycle = asset::fadeCycle;
	window::init(&menu->window);
}

void menu::end(menuDef_t* menu)
{
    menu->items = memory::alloc<itemDef_t*>(menu->itemCount);
	std::memcpy(menu->items, itemDefs, sizeof(itemDef_t*) * menu->itemCount);

	if (menu->data->fullScreen)
	{
		menu->window.rect.x = 0.0;
		menu->window.rect.y = 0.0;
		menu->window.rect.w = 640.0;
		menu->window.rect.h = 480.0;
	}

	menu::update_position(menu);
}

void menu::free(menuDef_t* menu)
{
    for (auto i = 0; i < menu->itemCount; i++)
    {
        item::free(menu->items[i]);
		memory::free(menu->items[i]);
    }

	// TODO:

	if(menu->data->allowedBinding)
		memory::free((void*)menu->data->allowedBinding);
	
	if(menu->data->soundName)
		memory::free((void*)menu->data->soundName);

	memory::free(menu->data);
	window::free(&menu->window);
    memory::free(menu);
}

void menu::update_position(menuDef_t* menu)
{
	if (menu)
	{
		int x = menu->window.rect.x;
		int y = menu->window.rect.y;
	
		if (menu->window.border)
		{
			x = menu->window.borderSize + x;
			y = menu->window.borderSize + y;
		}

		for (int i = 0; i < menu->itemCount; ++i )
		{
			item::set_screen_coords(menu->items[i], x, y, menu->window.rect.horzAlign, menu->window.rect.vertAlign);
		}
	}
}

auto menu::parse(menuDef_t* menu) -> bool
{
    reader::token token;

	if (!reader::read_token(&token))
		return false;

	if (*token.string != '{')
		return false;

	itemParseContext.needsImplicitSize = 0;
	itemParseContext.impicitSizeRez = 2;
	itemParseContext.horzRefPoint = 0;
	itemParseContext.vertRefPoint = 0;
	itemParseContext.valid = 1;

	while (true)
	{
		if (!reader::read_token(&token))
		{
			reader::source_error("end of file inside menu");
			itemParseContext.valid = 0;
			return false;
		}

		if (*token.string == '}')
		{
			item::validate_matrect(&menu->window.rect, &itemParseContext, menu->window.background);
			return true;
		}

		if (token.string[0] == ';')
      		continue;

		strlwr(token.string);
		auto func = menu::func(token.string);

		if (func == nullptr)
		{
			reader::source_error("unknown menu keyword %s", token.string);
			itemParseContext.valid = 0;
			return false;
		}

		if (!func(menu))
		{
			reader::source_error("couldn't parse menu keyword %s", token.string);
			itemParseContext.valid = 0;
			return false;
		}
	}

	return false;
}

auto menu::parse_fullscreen(menuDef_t* menu) -> bool
{
	return reader::read_int(&menu->data->fullScreen);
}

auto menu::parse_fadecycle(menuDef_t* menu) -> bool
{
	return reader::read_int(&menu->data->fadeCycle);
}

auto menu::parse_fadeclamp(menuDef_t* menu) -> bool
{
	return reader::read_float(&menu->data->fadeClamp);
}

auto menu::parse_fadeamount(menuDef_t* menu) -> bool
{
	return reader::read_float(&menu->data->fadeAmount);
}

auto menu::parse_fadeinamount(menuDef_t* menu) -> bool
{
	return reader::read_float(&menu->data->fadeInAmount);
}

auto menu::parse_blurworld(menuDef_t* menu) -> bool
{
	if (!reader::read_float(&menu->data->blurRadius))
		return false;

	if (menu->data->blurRadius >= 0.0)
		return true;

	reader::source_error("blur must be >= 0; %g is invalid", menu->data->blurRadius);
	return false;
}

auto menu::parse_onopen(menuDef_t* menu) -> bool
{
	return reader::read_event(&menu->data->onOpen);
}

auto menu::parse_oncloserequest(menuDef_t* menu) -> bool
{
	return reader::read_event(&menu->data->onCloseRequest);
}

auto menu::parse_onclose(menuDef_t* menu) -> bool
{
	return reader::read_event(&menu->data->onClose);
}

auto menu::parse_onesc(menuDef_t* menu) -> bool
{
	return reader::read_event(&menu->data->onESC);
}

auto menu::parse_onfocusduetoclose(menuDef_t* menu) -> bool
{
	return reader::read_event(&menu->data->onFocusDueToClose);
}

auto menu::parse_execkey(menuDef_t* menu) -> bool
{
	char key;
	MenuEventHandlerSet* action;
	ItemKeyHandler* handler;

	if (!reader::read_char(&key))
		return false;

	if (!reader::read_event(&action))
		return false;

	handler = memory::alloc<ItemKeyHandler>(); // MEM LEAK CHECK !!
	handler->key = key;
	handler->next = menu->data->onKey;
	handler->action = action;
	menu->data->onKey = handler;
	return true;
}

auto menu::parse_execkeyint(menuDef_t* menu) -> bool
{
	int key;
	MenuEventHandlerSet* action;
	ItemKeyHandler* handler;

	if (!reader::read_int(&key))
		return false;

	if (!reader::read_event(&action))
		return false;

	handler = memory::alloc<ItemKeyHandler>(); // MEM LEAK CHECK !!
	handler->key = key;
	handler->next = menu->data->onKey;
	handler->action = action;
	menu->data->onKey = handler;
	return true;
}

auto menu::parse_visible(menuDef_t* menu) -> bool
{
	reader::token token;

	if (!reader::read_token(&token))
		return false;

	if (!stricmp(token.string, "when") || !stricmp(token.string, "if"))
	{
		menu->window.dynamicFlags[0] |= WINDOWDYNAMIC_VISIBLE;
		return reader::read_expression(&menu->data->visibleExp);
	}

	if (atoi(token.string))
	{
		menu->window.dynamicFlags[0] |= WINDOWDYNAMIC_VISIBLE;
	}

	return true;
}

auto menu::parse_allowedbinding(menuDef_t* menu) -> bool
{
	if (menu->data->allowedBinding)
		reader::source_error("Only one 'allowedBinding' is supported");

	return reader::read_string(&menu->data->allowedBinding);
}

auto menu::parse_soundloop(menuDef_t* menu) -> bool
{
	return reader::read_string(&menu->data->soundName);
}

auto menu::parse_focuscolor(menuDef_t* menu) -> bool
{
	return reader::read_color(&menu->data->focusColor);
}

auto menu::parse_execexp(menuDef_t* menu) -> bool
{
	const char* expressionType;
	reader::token token;

	if (!reader::read_string(&expressionType))
		return false;

	if (!stricmp(expressionType, "visible"))
	{
		if (!reader::read_token(&token))
		{
			reader::source_error("ERROR: line ended early after \"visible\"");
			return false;
		}
		if (stricmp(token.string, "when"))
		{
			reader::source_error("ERROR: Expected 'when' after \"visible\" but found \"%s\"", token.string);
			return false;
		}

		menu->window.dynamicFlags[0] |= WINDOWDYNAMIC_VISIBLE;

		if (reader::read_expression(&menu->data->visibleExp))
			return true;
	}
	else if (!stricmp(expressionType, "rect"))
	{
		if (!reader::read_token(&token))
		{
			reader::source_error("ERROR: line ended early after \"exp rect\"");
			return false;
		}
		if (!stricmp(token.string, "X"))
		{
			return reader::read_expression(&menu->data->rectXExp);
		}
		else if (!stricmp(token.string, "Y"))
		{
			return reader::read_expression(&menu->data->rectYExp);
		}
		else if (!stricmp(token.string, "W"))
		{
			return reader::read_expression(&menu->data->rectWExp);
		}
		else if (!stricmp(token.string, "H"))
		{
			return reader::read_expression(&menu->data->rectHExp);
		}

		reader::source_error("ERROR: Expected 'X', 'Y', 'W' or 'H' after \"exp rect\" but found \"%s\"", token.string);
		return false;
	}
	else if (!stricmp(expressionType, "openSound"))
	{
		if (reader::read_expression(&menu->data->openSoundExp))
			return true;
	}
	else if (!stricmp(expressionType, "closeSound"))
	{
		if (reader::read_expression(&menu->data->closeSoundExp))
			return true;
	}
	else if (!stricmp(expressionType, "soundLoop"))
	{
		if (reader::read_expression(&menu->data->soundLoopExp))
			return true;
	}
	else
	{
		reader::source_error("ERROR: Unknown exp type '%s'", token.string);
	}
	return false;
}

auto menu::parse_priority(menuDef_t *menu) -> bool
{
	int v;

	if ( !reader::read_int(&v) )
		return false;

	menu->data->priority = v;
	return true;
}

auto menu::parse_name(menuDef_t *menu) -> bool
{
	if (!reader::read_string(&menu->window.name))
		return false;

	strlwr((char*)menu->window.name);
	return true;
}

auto menu::parse_rect480(menuDef_t* menu) -> bool
{
	if (!reader::read_float(&menu->window.rect.x)
    	|| !reader::read_float(&menu->window.rect.y)
    	|| !reader::read_float(&menu->window.rect.w)
    	|| !reader::read_float(&menu->window.rect.h))
	{
		return false;
	}

	menu->window.rect.horzAlign = 0;
	menu->window.rect.vertAlign = 0;

	if (!reader::read_rect_optionalbyteline((unsigned char *)&menu->window.rect.horzAlign)
		&& !reader::read_rect_optionalbyteline((unsigned char *)&menu->window.rect.vertAlign)
		&& !reader::read_rect_optionalbyteline((unsigned char *)&itemParseContext.horzRefPoint) )
	{
		reader::read_rect_optionalbyteline((unsigned char *)&itemParseContext.vertRefPoint);
	}

	return true;
}

auto menu::parse_rect720(menuDef_t* menu) -> bool
{
	return reader::read_rect(&menu->window.rect, 0, 1, &itemParseContext);
}

auto menu::parse_pos480(menuDef_t* menu) -> bool
{
	if (!reader::read_float(&menu->window.rect.x) || !reader::read_float(&menu->window.rect.y))
		return false;

	menu->window.rect.horzAlign = 0;
	menu->window.rect.vertAlign = 0;

	if (!reader::read_rect_optionalbyteline((unsigned char *)&menu->window.rect.horzAlign)
		&& !reader::read_rect_optionalbyteline((unsigned char *)&menu->window.rect.vertAlign)
		&& !reader::read_rect_optionalbyteline((unsigned char *)&itemParseContext.horzRefPoint))
	{
		reader::read_rect_optionalbyteline((unsigned char *)&itemParseContext.vertRefPoint);
	}

	itemParseContext.needsImplicitSize = 1;
	itemParseContext.impicitSizeRez = 0;
	return true;
}

auto menu::parse_pos720(menuDef_t* menu) -> bool
{
	if (!reader::read_float(&menu->window.rect.x) || !reader::read_float(&menu->window.rect.y))
		return false;

	menu->window.rect.horzAlign = 0;
	menu->window.rect.vertAlign = 0;

	menu->window.rect.x = menu->window.rect.x * 0.6666666865348816;
	menu->window.rect.y = menu->window.rect.y * 0.6666666865348816;

	if (!reader::read_rect_optionalbyteline((unsigned char *)&menu->window.rect.horzAlign)
		&& !reader::read_rect_optionalbyteline((unsigned char *)&menu->window.rect.vertAlign)
		&& !reader::read_rect_optionalbyteline((unsigned char *)&itemParseContext.horzRefPoint))
	{
		reader::read_rect_optionalbyteline((unsigned char *)&itemParseContext.vertRefPoint);
	}

	itemParseContext.needsImplicitSize = 1;
	itemParseContext.impicitSizeRez = 1;
	return true;
}

auto menu::parse_style(menuDef_t* menu) -> bool
{
	char c;

	if(!reader::read_byte(&c))
		return false;
	
	menu->window.style = c;
	return true;
}

auto menu::parse_border(menuDef_t* menu) -> bool
{
	char c;

	if(!reader::read_byte(&c))
		return false;
	
	menu->window.border = c;
	return true;
}

auto menu::parse_ownerdraw(menuDef_t* menu) -> bool
{
	return reader::read_int(&menu->window.ownerDraw);
}

auto menu::parse_ownerdrawflags(menuDef_t* menu) -> bool
{
	return reader::read_int(&menu->window.ownerDrawFlags);
}

auto menu::parse_bordersize(menuDef_t* menu) -> bool
{
	return reader::read_float(&menu->window.borderSize);
}

auto menu::parse_decoration(menuDef_t* menu) -> bool
{
	menu->window.staticFlags |= WINDOWSTATIC_DECORATION;
	return true;
}

auto menu::parse_screenspace(menuDef_t* menu) -> bool
{
	menu->window.staticFlags |= WINDOWSTATIC_SCREENSPACE;
	return true;
}

auto menu::parse_popup(menuDef_t* menu) -> bool
{
	menu->window.staticFlags |= WINDOWSTATIC_POPUP;
	return true;
}

auto menu::parse_outofbounds(menuDef_t* menu) -> bool
{
	menu->window.staticFlags |= WINDOWSTATIC_OUTOFBOUNDSCLICK;
	return true;
}

auto menu::parse_legacysplitscreenscale(menuDef_t* menu) -> bool
{
	menu->window.staticFlags |= WINDOWSTATIC_LEGACYSPLITSCREENSCALE;
	return true;
}

auto menu::parse_hiddenduringflashbang(menuDef_t* menu) -> bool
{
	menu->window.staticFlags |= WINDOWSTATIC_HIDDENDURINGFLASH;
	return true;
}

auto menu::parse_hiddenduringscope(menuDef_t* menu) -> bool
{
	menu->window.staticFlags |= WINDOWSTATIC_HIDDENDURINGSCOPE;
	return true;
}

auto menu::parse_hiddenduringui(menuDef_t* menu) -> bool
{
	menu->window.staticFlags |= WINDOWSTATIC_HIDDENDURINGUI;
	return true;
}

auto menu::parse_textonlyfocus(menuDef_t* menu) -> bool
{
	menu->window.staticFlags |= WINDOWSTATIC_TEXTONLYFOCUS;
	return true;
}

auto menu::parse_forecolor(menuDef_t* menu) -> bool
{
	if (!reader::read_color(&menu->window.foreColor))
		return false;

	menu->window.dynamicFlags[0] |= WINDOWDYNAMIC_FORECOLOR;
	return true;
}

auto menu::parse_backcolor(menuDef_t* menu) -> bool
{
	return reader::read_color(&menu->window.backColor);
}

auto menu::parse_bordercolor(menuDef_t* menu) -> bool
{
	return reader::read_color(&menu->window.borderColor);
}

auto menu::parse_outlinecolor(menuDef_t* menu) -> bool
{
	return reader::read_color(&menu->window.outlineColor);
}

auto menu::parse_disablecolor(menuDef_t* menu) -> bool
{
	return reader::read_color(&menu->window.disableColor);
}

auto menu::parse_background(menuDef_t* menu) -> bool
{
	const char* name;

	if (!reader::read_string(&name))
		return false;

	// Replace this with material register handle
	menu->window.background = memory::alloc<Material>(); // MEM LEAK CHECK !!
	menu->window.background->name = name;
	strlwr((char*)menu->window.background->name);
	return true;
}

auto menu::parse_itemdef(menuDef_t* menu) -> bool
{
	if (menu->itemCount >= MAX_ITEMDEFS_PER_MENUDEF)
	{
		reader::source_error("too many itemDefs for menu '%s', MAX is %i", menu->window.name, MAX_ITEMDEFS_PER_MENUDEF);
		return false;
	}

	auto item = memory::alloc<itemDef_t>();

	item::init(item);

	if (!item::parse(item))
	{
		item::free(item);
		return false;
	}

	item::end(item);
	item->parent = menu;
	menu->items[menu->itemCount++] = item;
	return true;
}

auto menu::func(const char* keyword) -> func_type
{
	const auto& itr = funcs_.find(std::string_view(keyword));

	if (itr != funcs_.end())
	{
		return itr->second;
	}

	return nullptr;
}

std::unordered_map<std::string_view, menu::func_type> menu::funcs_
{
	{ "fullscreen"sv, parse_fullscreen },
	{ "fadecycle"sv, parse_fadecycle },
	{ "fadeclamp"sv, parse_fadeclamp },
	{ "fadeamount"sv, parse_fadeamount },
	{ "fadeinamount"sv, parse_fadeinamount },
	{ "blurworld"sv, parse_blurworld },
	{ "onopen"sv, parse_onopen },
	{ "onrequestclose"sv, parse_oncloserequest },
	{ "onclose"sv, parse_onclose },
	{ "onesc"sv, parse_onesc },
	{ "onfocusduetoclose"sv, parse_onfocusduetoclose },
	{ "execkey", parse_execkey },
	{ "execkeyint", parse_execkeyint },
	{ "visible"sv, parse_visible },
	{ "allowedbinding"sv, parse_allowedbinding },
	{ "soundloop", parse_soundloop },
	{ "focuscolor"sv, parse_focuscolor },
	{ "exp"sv, parse_execexp },
	{ "priority", parse_priority },
	{ "name"sv, parse_name },
	{ "rect"sv, parse_rect480 },
	{ "rect480"sv, parse_rect480 },
	{ "rect720"sv, parse_rect720 },
	{ "pos480"sv, parse_pos480 },
	{ "pos720"sv, parse_pos720 },
	{ "style"sv, parse_style },
	{ "border"sv, parse_border },
	{ "ownerdraw", parse_ownerdraw },
	{ "ownerdrawflags", parse_ownerdrawflags },
	{ "bordersize", parse_bordersize },
	{ "decoration", parse_decoration },
	{ "screenspace", parse_screenspace },
	{ "popup", parse_popup },
	{ "outofboundsclick", parse_outofbounds },
	{ "legacysplitscreenscale", parse_legacysplitscreenscale },
	{ "hiddenduringflashbang", parse_hiddenduringflashbang },
	{ "hiddenduringscope", parse_hiddenduringscope },
	{ "hiddenduringui", parse_hiddenduringui },
	{ "textonlyfocus", parse_textonlyfocus },
	{ "forecolor", parse_forecolor },
	{ "backcolor", parse_backcolor },
	{ "bordercolor", parse_bordercolor },
	{ "outlinecolor", parse_outlinecolor },
	{ "disablecolor", parse_disablecolor },
	{ "background", parse_background },
	{ "itemdef", parse_itemdef },
};

} // namespace xsk::menu::iw5
