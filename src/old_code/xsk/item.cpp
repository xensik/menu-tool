// Copyright 2022 xensik. All rights reserved.
//
// Use of this source code is governed by a GNU GPLv3 license
// that can be found in the LICENSE file.

#include "stdafx.hpp"
#include "iw5.hpp"
#include "memory.hpp"
#include "reader.hpp"
#include "window.hpp"
#include "item.hpp"

namespace xsk::menu::iw5
{

ParseContext item::parseContext;

void item::validate_matrect(rectDef_s* rect, ParseContext* cxt, Material* mat)
{

}

void item::set_screen_coords(itemDef_t* item, float x, float y, int horzAlign, int vertAlign)
{
	if (item)
	{
		if (item->window.border)
		{
			x = item->window.borderSize + x;
			y = item->window.borderSize + y;
		}

		item->window.rect.x = item->window.rectClient.x;
		item->window.rect.y = item->window.rectClient.y;
		item->window.rect.w = item->window.rectClient.w;
		item->window.rect.h = item->window.rectClient.h;
		item->window.rect.horzAlign = item->window.rectClient.horzAlign;
		item->window.rect.vertAlign = item->window.rectClient.vertAlign;
		item->window.rect.x = item->window.rect.x + x;
		item->window.rect.y = item->window.rect.y + y;

		if (!item->window.rect.horzAlign && !item->window.rect.vertAlign)
		{
			item->window.rect.horzAlign = horzAlign;
			item->window.rect.vertAlign = vertAlign;
		}

		//textRect = item->textRect;
		//item->textRect[0].x = textRect->x;
		//item->textRect[0].y = textRect->y;
		item->textRect[0].w = 0.0;
		item->textRect[0].h = 0.0;
		//item->textRect[0].horzAlign = textRect->horzAlign;
		//item->textRect[0].vertAlign = textRect->vertAlign;
	}
}

void item::init(itemDef_t* item)
{
	std::memset(item, 0, sizeof(itemDef_t));
	item->textscale = 0.55;
	//item->floatExpressions = alloc
	window::init(&item->window);
}

void item::end(itemDef_t* item)
{

}

void item::free(itemDef_t* item)
{
    // TODO
    memory::free(item);
}

auto item::parse(itemDef_t* item) -> bool
{
	reader::token token;

	if (!reader::read_token(&token))
		return false;

	if (token.string[0] != '{')
		return false;
	
	parseContext.needsImplicitSize = 0;
	parseContext.impicitSizeRez = 2;
	parseContext.horzRefPoint = 0;
	parseContext.vertRefPoint = 0;
	parseContext.valid = 1;

	while (true)
	{
		if (!reader::read_token(&token))
		{
			reader::source_error("end of file inside menu item\n");
			parseContext.valid = 0;
			return false;
		}

		if (*token.string == '}')
		{
			validate_matrect(&item->window.rectClient, &parseContext, item->window.background);
			return true;
		}	

		if (token.string[0] == ';')
      		continue;

		strlwr(token.string);
		auto func = item::func(token.string);

		if (func == nullptr)
		{
			reader::source_error("unknown itemdef keyword %s", token.string);
			parseContext.valid = 0;
			return false;
		}

		if (!func(item))
		{
			reader::source_error("error parsing the itemdef keyword %s's parameters", token.string);
			parseContext.valid = 0;
			return false;
		}
	}

	return false;
}

auto item::parse_name(itemDef_t* item) -> bool
{
	return reader::read_string(&item->window.name);
}

auto item::parse_rect480(itemDef_t* item) -> bool
{
	if (!reader::read_float(&item->window.rectClient.x)
    	|| !reader::read_float(&item->window.rectClient.y)
    	|| !reader::read_float(&item->window.rectClient.w)
    	|| !reader::read_float(&item->window.rectClient.h))
	{
		return false;
	}

	item->window.rectClient.horzAlign = 0;
	item->window.rectClient.vertAlign = 0;

	if (!reader::read_rect_optionalbyteline((unsigned char *)&item->window.rectClient.horzAlign)
		&& !reader::read_rect_optionalbyteline((unsigned char *)&item->window.rectClient.vertAlign)
		&& !reader::read_rect_optionalbyteline((unsigned char *)&parseContext.horzRefPoint) )
	{
		reader::read_rect_optionalbyteline((unsigned char *)&parseContext.vertRefPoint);
	}

	return true;
}

auto item::parse_rect720(itemDef_t* item) -> bool
{
	return reader::read_rect(&item->window.rectClient, 0, 1, &parseContext);
}

auto item::parse_pos480(itemDef_t* item) -> bool
{
	if (!reader::read_float(&item->window.rectClient.x) || !reader::read_float(&item->window.rectClient.y))
		return false;

	item->window.rectClient.horzAlign = 0;
	item->window.rectClient.vertAlign = 0;

	if (!reader::read_rect_optionalbyteline((unsigned char *)&item->window.rectClient.horzAlign)
		&& !reader::read_rect_optionalbyteline((unsigned char *)&item->window.rectClient.vertAlign)
		&& !reader::read_rect_optionalbyteline((unsigned char *)&parseContext.horzRefPoint))
	{
		reader::read_rect_optionalbyteline((unsigned char *)&parseContext.vertRefPoint);
	}

	parseContext.needsImplicitSize = 1;
	parseContext.impicitSizeRez = 0;
	return true;
}

auto item::parse_pos720(itemDef_t* item) -> bool
{
	if (!reader::read_float(&item->window.rectClient.x) || !reader::read_float(&item->window.rectClient.y))
		return false;

	item->window.rectClient.horzAlign = 0;
	item->window.rectClient.vertAlign = 0;

	item->window.rectClient.x = item->window.rectClient.x * 0.6666666865348816;
	item->window.rectClient.y = item->window.rectClient.y * 0.6666666865348816;

	if (!reader::read_rect_optionalbyteline((unsigned char *)&item->window.rectClient.horzAlign)
		&& !reader::read_rect_optionalbyteline((unsigned char *)&item->window.rectClient.vertAlign)
		&& !reader::read_rect_optionalbyteline((unsigned char *)&parseContext.horzRefPoint))
	{
		reader::read_rect_optionalbyteline((unsigned char *)&parseContext.vertRefPoint);
	}

	parseContext.needsImplicitSize = 1;
	parseContext.impicitSizeRez = 1;
	return true;
}

auto item::parse_origin(itemDef_t* item) -> bool
{
	int v3;
	int v4;

	if ( !reader::read_int(&v3) || !reader::read_int(&v4) )
		return false;

	item->window.rectClient.x = (double)v3 + item->window.rectClient.x;
	item->window.rectClient.y = (double)v4 + item->window.rectClient.y;
	return true;
}

bool item::parse_group(itemDef_t* item)
{
	return reader::read_string(&item->window.group);
}

bool item::parse_style(itemDef_t* item)
{
	char c;

	if(!reader::read_byte(&c))
		return false;
	
	item->window.style = c;
	return true;
}

bool item::parse_border(itemDef_t* item)
{
	char c;

	if(!reader::read_byte(&c))
		return false;
	
	item->window.border = c;
	return true;
}

bool item::parse_ownerdraw(itemDef_t* item)
{
	return reader::read_int(&item->window.ownerDraw);
}

bool item::parse_ownerdrawflags(itemDef_t* item)
{
	return reader::read_int(&item->window.ownerDrawFlags);
}

bool item::parse_bordersize(itemDef_t* item)
{
	return reader::read_float(&item->window.borderSize);
}

bool item::parse_decoration(itemDef_t* item)
{
	item->window.staticFlags |= WINDOWSTATIC_DECORATION;
	return true;
}

bool item::parse_screenspace(itemDef_t* item)
{
	item->window.staticFlags |= WINDOWSTATIC_SCREENSPACE;
	return true;
}

bool item::parse_autowrapped(itemDef_t* item)
{
	item->window.staticFlags |= WINDOWSTATIC_AUTOWRAPPED;
	return true;
}

bool item::parse_popup(itemDef_t* item)
{
	item->window.staticFlags |= WINDOWSTATIC_POPUP;
	return true;
}

bool item::parse_outofbounds(itemDef_t* item)
{
	item->window.staticFlags |= WINDOWSTATIC_OUTOFBOUNDSCLICK;
	return true;
}

bool item::parse_legacysplitscreenscale(itemDef_t* item)
{
	item->window.staticFlags |= WINDOWSTATIC_LEGACYSPLITSCREENSCALE;
	return true;
}

bool item::parse_hiddenduringflashbang(itemDef_t* item)
{
	item->window.staticFlags |= WINDOWSTATIC_HIDDENDURINGFLASH;
	return true;
}

bool item::parse_hiddenduringscope(itemDef_t* item)
{
	item->window.staticFlags |= WINDOWSTATIC_HIDDENDURINGSCOPE;
	return true;
}

bool item::parse_hiddenduringui(itemDef_t* item)
{
	item->window.staticFlags |= WINDOWSTATIC_HIDDENDURINGUI;
	return true;
}

bool item::parse_textonlyfocus(itemDef_t* item)
{
	item->window.staticFlags |= WINDOWSTATIC_TEXTONLYFOCUS;
	return true;
}

bool item::parse_forecolor(itemDef_t* item)
{
	if (!reader::read_color(&item->window.foreColor))
		return false;

	item->window.dynamicFlags[0] |= WINDOWDYNAMIC_FORECOLOR;
	return true;
}

bool item::parse_backcolor(itemDef_t* item)
{
	return reader::read_color(&item->window.backColor);
}

bool item::parse_bordercolor(itemDef_t* item)
{
	return reader::read_color(&item->window.borderColor);
}

bool item::parse_outlinecolor(itemDef_t* item)
{
	return reader::read_color(&item->window.outlineColor);
}

bool item::parse_disablecolor(itemDef_t* item)
{
	return reader::read_color(&item->window.disableColor);
}

bool item::parse_background(itemDef_t* item)
{
	const char* name;

	if (!reader::read_string(&name))
		return false;

	// Replace with material handle
	item->window.background = memory::alloc<Material>(); // MEM LEAK CHECK !!
	item->window.background->name = name;
	strlwr((char*)item->window.background->name);
	return true;
}

bool item::parse_type(itemDef_t *item)
{
  if ( !reader::read_int(&item->type) )
    return false;

  item::validate_type_data(item);
  return true;
}

bool item::parse_align(itemDef_t* item)
{
	return reader::read_int(&item->alignment);
}

bool item::parse_textfont(itemDef_t* item)
{
	return reader::read_int(&item->fontEnum);
}

bool item::parse_textalign(itemDef_t* item)
{
	if (!reader::read_int(&item->textAlignMode))
		return false;

	if (item::is_valid_text_alignment(item->textAlignMode))
		return true;

	reader::source_error("expected ITEM_ALIGN_* value");
	return false;
}

bool item::parse_textalignx(itemDef_t* item)
{
	return reader::read_float(&item->textalignx);
}

bool item::parse_textaligny(itemDef_t* item)
{
	return reader::read_float(&item->textaligny);
}

bool item::parse_textscale(itemDef_t* item)
{
	return reader::read_float(&item->textscale);
}

bool item::parse_textstyle(itemDef_t* item)
{
	return reader::read_int(&item->textStyle);
}

bool item::parse_gamemsgwindowindex(itemDef_t* item)
{
	return reader::read_int(&item->gameMsgWindowIndex);
}

bool item::parse_gamemsgwindowmode(itemDef_t* item)
{
	return reader::read_int(&item->gameMsgWindowMode);
}

bool item::parse_text(itemDef_t* item)
{
	return reader::read_string(&item->text);
}

auto item::parse_textfile(itemDef_t* item) -> bool
{
	// TODO:
	reader::source_error("'textfile' not implemented!");
	return false;
}

auto item::parse_textsavegame(itemDef_t* item) -> bool
{
	item->itemFlags |= 1u;
	item->text = "savegameinfo";
  	return true;
}

auto item::parse_textcinematicsubtitle(itemDef_t* item) -> bool
{
	item->itemFlags |= 2u;
	item->text = "cinematicsubtitle";
  	return true;
}

bool item::parse_mouseentertext(itemDef_t* item)
{
	return reader::read_event(&item->mouseEnterText);
}

bool item::parse_mouseexittext(itemDef_t* item)
{
	return reader::read_event(&item->mouseExitText);
}

bool item::parse_mouseenter(itemDef_t* item)
{
	return reader::read_event(&item->mouseEnter);
}

bool item::parse_mouseexit(itemDef_t* item)
{
	return reader::read_event(&item->mouseExit);
}

bool item::parse_action(itemDef_t* item)
{
	return reader::read_event(&item->action);
}

bool item::parse_accept(itemDef_t* item)
{
	return reader::read_event(&item->accept);
}

bool item::parse_onfocus(itemDef_t* item)
{
	return reader::read_event(&item->onFocus);
}

bool item::parse_hasfocus(itemDef_t* item)
{
	return reader::read_event(&item->hasFocus);
}

bool item::parse_leavefocus(itemDef_t* item)
{
	return reader::read_event(&item->leaveFocus);
}

bool item::parse_dvar(itemDef_t* item)
{
	editFieldDef_s* editPtr;

	item::validate_type_data(item);

	if (!reader::read_string(&item->dvar))
		return false;

	if (item->typeData.editField && item::is_editFieldDef(item))
	{
		editPtr = item::get_editFieldDef(item);

		editPtr->minVal = -1.0f;
		editPtr->maxVal = -1.0f;
		editPtr->stepVal = -1.0f;
	}

	return true;
}

bool item::parse_dvarfloat(itemDef_t *item)
{
	editFieldDef_s* editPtr;

	item::validate_type_data(item);
	if (!item->typeData.editField)
		return false;

	editPtr = item::get_editFieldDef(item);
	if (!editPtr)
		return false;

	return reader::read_string(&item->dvar)
		&& reader::read_float(&editPtr->stepVal)
		&& reader::read_float(&editPtr->minVal)
		&& reader::read_float(&editPtr->maxVal);
}

bool item::parse_dvartest(itemDef_t* item)
{
	return reader::read_string(&item->dvarTest);
}

bool item::parse_execkey(itemDef_t* item)
{
	char key;
	MenuEventHandlerSet* action;
	ItemKeyHandler* handler;
	
	if (!reader::read_char(&key))
		return false;

	if (!reader::read_event(&action))
		return false;

	handler = memory::alloc<ItemKeyHandler>(); // CHECK MEM LEAK!
	handler->key = key;
	handler->next = item->onKey;
	handler->action = action;
	item->onKey = handler;
	return true;
}

bool item::parse_execkeyint(itemDef_t* item)
{
	int key;
	MenuEventHandlerSet* action;
	ItemKeyHandler* handler;

	if (!reader::read_int(&key))
		return false;

	if (!reader::read_event(&action))
		return false;

	handler = memory::alloc<ItemKeyHandler>();
	handler->key = key;
	handler->next = item->onKey;
	handler->action = action;
	item->onKey = handler;
	return true;
}

bool item::parse_enabledvar(itemDef_t* item)
{
	if (!reader::read_script(&item->enableDvar))
		return false;

	item->dvarFlags |= 1u;
	return true;
}

bool item::parse_disabledvar(itemDef_t* item)
{
	if (!reader::read_script(&item->enableDvar))
		return false;

	item->dvarFlags |= 2u;
	return true;
}

bool item::parse_showdvar(itemDef_t* item)
{
	if (!reader::read_script(&item->enableDvar))
		return false;

	item->dvarFlags |= 4u;
	return true;
}

bool item::parse_hidedvar(itemDef_t* item)
{
	if (!reader::read_script(&item->enableDvar))
		return false;

	item->dvarFlags |= 8u;
	return true;
}

bool item::parse_focusdvar(itemDef_t* item)
{
	if (!reader::read_script(&item->enableDvar))
		return false;

	item->dvarFlags |= 0x10u;
	return true;
}

bool item::parse_localvar(itemDef_t* item)
{
	editFieldDef_s* editPtr;

	item::validate_type_data(item);

	if (!reader::read_string(&item->localVar))
		return false;

	if (item->typeData.editField && item::is_editFieldDef(item))
	{
		editPtr = item::get_editFieldDef(item);

		editPtr->minVal = -1.0f;
		editPtr->maxVal = -1.0f;
		editPtr->stepVal = -1.0f;
	}

	return true;
}

bool item::parse_focussound(itemDef_t* item)
{
	const char* name;

	if (!reader::read_string(&name))
		return false;

	item->focusSound = memory::alloc<snd_alias_list_t>(); //CHECK MEM LEAK
	item->focusSound->aliasName = name;
	return true;
}

bool item::parse_special(itemDef_t* item)
{
	return reader::read_float(&item->special);
}

bool item::parse_elementwidth(itemDef_t* item)
{
	listBoxDef_s* listPtr;

	item::validate_type_data(item);
	listPtr = item::get_listBoxDef(item);
	if (listPtr)
		return reader::read_float(&listPtr->elementWidth);

	return false;
}

bool item::parse_elementheight(itemDef_t* item)
{
	listBoxDef_s* listPtr;

	item::validate_type_data(item);
	listPtr = item::get_listBoxDef(item);
	if (listPtr)
		return reader::read_float(&listPtr->elementHeight);

	return false;
}

bool item::parse_elementtype(itemDef_t* item)
{
	int result;
	listBoxDef_s* listPtr;

	item::validate_type_data(item);
	if (item->typeData.data && (listPtr = item::get_listBoxDef(item)) != 0)
		return reader::read_int(&listPtr->elementStyle);
	
	return false;
}

bool item::parse_columns(itemDef_t* item)
{
	int pos;
	int width;
	int align;
	int maxChars;
	listBoxDef_s* listPtr;
	int num;
	int i;

	item::validate_type_data(item);
	if (!item->typeData.listBox)
		return 0;
	listPtr = item::get_listBoxDef(item);
	if (!listPtr)
		return 0;
	if (!reader::read_int(&num))
		return 0;
	if (num > 16)
		num = 16;
	listPtr->numColumns = num;
	for (i = 0; i < num; ++i)
	{
		if (!reader::read_int(&pos) || !reader::read_int(&width) || !reader::read_int(&maxChars))
			return 0;
		listPtr->columnInfo[i].xpos = pos;
		listPtr->columnInfo[i].width = width;
		listPtr->columnInfo[i].height = listPtr->elementHeight;
		listPtr->columnInfo[i].maxChars = maxChars;
		if (reader::read_int_line(&align))
			listPtr->columnInfo[i].alignment = align;
		else
			listPtr->columnInfo[i].alignment = 0;
	}
	return 1;
}

bool item::parse_doubleclick(itemDef_t* item)
{
	listBoxDef_s* listPtr;

	item::validate_type_data(item);
	if (!item->typeData.listBox)
		return 0;
	listPtr = item::get_listBoxDef(item);
	if (!listPtr)
		return 0;
	return reader::read_event(&listPtr->onDoubleClick);
}

bool item::parse_notselectable(itemDef_t* item)
{
	listBoxDef_s* listPtr;

	item::validate_type_data(item);
	listPtr = item::get_listBoxDef(item);
	if (!listPtr)
		return 0;
	if (item->type == 6)
		listPtr->notselectable = 1;
	return 1;
}

bool item::parse_noscrollbars(itemDef_t* item)
{
	listBoxDef_s* listPtr;

	item::validate_type_data(item);
	listPtr = item::get_listBoxDef(item);
	if (!listPtr)
		return 0;
	if (item->type == 6)
		listPtr->noScrollBars = 1;
	return 1;
}

bool item::parse_usepaging(itemDef_t* item)
{
	listBoxDef_s* listPtr;

	item::validate_type_data(item);
	listPtr = item::get_listBoxDef(item);
	if (!listPtr)
		return 0;
	if (item->type == 6)
		listPtr->usePaging = 1;
	return 1;
}

bool item::parse_selectborder(itemDef_t* item)
{
	listBoxDef_s* listPtr;

	item::validate_type_data(item);
	listPtr = item::get_listBoxDef(item);
	if (listPtr)
		return reader::read_color(&listPtr->selectBorder);
	return 0;
}

bool item::parse_selecticon(itemDef_t* item)
{
	listBoxDef_s* listPtr;
	const char* name;

	item::validate_type_data(item);
	listPtr = item::get_listBoxDef(item);
	if (!listPtr)
		return 0;
	if (!reader::read_string(&name))
		return 0;

	listPtr->selectIcon = memory::alloc<Material>(); // CHECK MEM LEAK
	listPtr->selectIcon->name = name;
	strlwr((char*)listPtr->selectIcon->name);
	return 1;
}

bool item::parse_dvarstrlist(itemDef_t* item)
{
	int pass;
	multiDef_s* multiPtr;
	reader::token token;

	item::validate_type_data(item);
	if (!item->typeData.multi)
		return false;
	if (item->type != 12)
		return false;

	multiPtr = item::get_multiDef(item);
	multiPtr->count = 0;
	multiPtr->strDef = 1;

	if (!reader::read_token(&token))
		return false;

	if (token.string[0] != '{')
		return false;
	pass = 0;
	do
	{
		while (1)
		{
			do
			{
				if (!reader::read_token(&token))
				{
					reader::source_error("end of file inside menu item");
					return false;
				}
				if (token.string[0] == '}')
					return true;
			} while (token.string[0] == ',' || token.string[0] == ';');
			if (pass)
				break;
			multiPtr->dvarList[multiPtr->count] = strdup(token.string); // CHECK MEM LEAK
			pass = 1;
		}
		multiPtr->dvarStr[multiPtr->count] = strdup(token.string); // CHECK MEM LEAK
		pass = 0;
		++multiPtr->count;
	} while (multiPtr->count < 32);

	return false;
}

bool item::parse_dvarfloatlist(itemDef_t* item)
{
	multiDef_s* multiPtr;
	reader::token token;

	item::validate_type_data(item);
	if (!item->typeData.multi)
		return 0;
	if (item->type != 12)
		return 0;
	multiPtr = item::get_multiDef(item);
	multiPtr->count = 0;
	multiPtr->strDef = 0;
	if (!reader::read_token(&token))
		return 0;
	if (token.string[0] != '{')
		return 0;
	do
	{
		do
		{
			if (!reader::read_token(&token))
			{
				reader::source_error("end of file inside menu item");
				return 0;
			}
			if (token.string[0] == '}')
				return 1;
		} while (token.string[0] == ',' || token.string[0] == ';');
		multiPtr->dvarList[multiPtr->count] = strdup(token.string);
		if (!reader::read_float(&multiPtr->dvarValue[multiPtr->count]))
			return 0;
		++multiPtr->count;
	} while (multiPtr->count < 32);
	return 0;
}

bool item::parse_dvarenumlist(itemDef_t* item)
{
	const char** enumDvarPtr;

	item::validate_type_data(item);
	enumDvarPtr = item::get_enumDvarName(item);

	if (item->type != 13)
		return false;
	
	if (!enumDvarPtr)
		return reader::read_string(enumDvarPtr);
	
	reader::source_error("enumDvarList already given");
	return false;
}

bool item::parse_maxchars(itemDef_t* item)
{
	editFieldDef_s* editPtr;
	int maxChars;

	item::validate_type_data(item);

	if (!item->typeData.editField)
		return false;

	if (!reader::read_int(&maxChars))
		return false;

	editPtr = item::get_editFieldDef(item);
	if (!editPtr)
		return false;

	editPtr->maxChars = maxChars;
	return true;
}

bool item::parse_maxcharsgotonext(itemDef_t* item)
{
	editFieldDef_s* editPtr;

	item::validate_type_data(item);
	if (!item->typeData.editField)
		return false;

	editPtr = item::get_editFieldDef(item);
	if (!editPtr)
		return false;

	editPtr->maxCharsGotoNext = 1;
	return true;
}

bool item::parse_maxpaintchars(itemDef_t* item)
{
	editFieldDef_s* editPtr;
	int maxChars;

	item::validate_type_data(item);
	if (!item->typeData.editField)
		return false;

	if (!reader::read_int(&maxChars))
		return false;

	editPtr = item::get_editFieldDef(item);
	if (!editPtr)
		return false;

	editPtr->maxPaintChars = maxChars;
	return true;
}

bool item::parse_newsfeed(itemDef_t* item)
{
	newsTickerDef_s* tickerPtr;
	int feedId;

	item::validate_type_data(item);
	tickerPtr = item::get_newsTickerDef(item);

	if (!tickerPtr)
		return false;

	if (!reader::read_int(&feedId))
		return false;

	tickerPtr->feedId = feedId;
	return true;
}

bool item::parse_speed(itemDef_t* item)
{
	newsTickerDef_s* tickerPtr;
	int speed;

	item::validate_type_data(item);
	tickerPtr = item::get_newsTickerDef(item);

	if (!tickerPtr)
		return false;

	if (!reader::read_int(&speed) || speed < 0)
		return false;

	tickerPtr->speed = speed;
	return true;
}

bool item::parse_spacing(itemDef_t* item)
{
	newsTickerDef_s* tickerPtr;
	int spacing;

	item::validate_type_data(item);
	tickerPtr = item::get_newsTickerDef(item);

	if (!tickerPtr)
		return false;
	
	if (!reader::read_int(&spacing) || spacing < 0)
		return false;

	tickerPtr->spacing = spacing;
	return true;
}

bool item::parse_visible(itemDef_t* item)
{
	reader::token token;

	if (!reader::read_token(&token))
		return false;

	if (!stricmp(token.string, "when") || !stricmp(token.string, "if"))
	{
		item->window.dynamicFlags[0] |= WINDOWDYNAMIC_VISIBLE;
		return reader::read_expression(&item->visibleExp);
	}
	if (atoi(token.string))
	{
		item->window.dynamicFlags[0] |= WINDOWDYNAMIC_VISIBLE;
	}
	return true;
}

bool item::parse_disabled(itemDef_t* item)
{
	reader::token token;

	if (!reader::read_token(&token))
		return true;

	if (!stricmp(token.string, "when") || !stricmp(token.string, "if"))
	{
		if (reader::read_expression(&item->disabledExp))
			return true;
	}

	return true;
}

bool item::parse_execexp(itemDef_t* item)
{
	// TODO
	return true;
}

bool item::parse_glowcolor(itemDef_t* item)
{
	item::validate_type_data(item);

	return reader::read_color(&item->glowColor);
}

bool item::parse_decodeeffect(itemDef_t* item)
{
	item::validate_type_data(item);

	if (!reader::read_int(&item->fxLetterTime)
		|| !reader::read_int(&item->fxDecayStartTime)
		|| !reader::read_int(&item->fxDecayDuration))
	{
		return false;
	}

	item->decayActive = 1;
	return true;
}

void item::validate_type_data(itemDef_t* item)
{

}

auto item::is_valid_text_alignment(int align) -> bool
{

}

auto item::is_newsTickerDef(itemDef_t* item) -> bool
{

}

auto item::is_editFieldDef(itemDef_t* item) -> bool
{

}

auto item::get_newsTickerDef(itemDef_t* item) -> newsTickerDef_s*
{

}

auto item::get_editFieldDef(itemDef_t* item) -> editFieldDef_s*
{

}

auto item::get_listBoxDef(itemDef_t* item) -> listBoxDef_s*
{

}

auto item::get_multiDef(itemDef_t* item) -> multiDef_s*
{

}

auto item::get_enumDvarName(itemDef_t* item) -> const char**
{

}

auto item::func(const char* keyword) -> func_type
{
	const auto& itr = funcs_.find(std::string_view(keyword));

	if (itr != funcs_.end())
	{
		return itr->second;
	}

	return nullptr;
}

std::unordered_map<std::string_view, item::func_type> item::funcs_
{
	{ "name"sv, parse_name },
	{ "rect"sv, parse_rect480 },
	{ "rect480"sv, parse_rect480 },
	{ "rect720"sv, parse_rect720 },
	{ "pos480"sv, parse_pos480 },
	{ "pos720"sv, parse_pos720 },
	{ "origin"sv, parse_origin },
	{ "group"sv, parse_group },
	{ "style"sv, parse_style },
	{ "border"sv, parse_border },
	{ "ownerdraw"sv, parse_ownerdraw },
	{ "ownerdrawflags"sv, parse_ownerdrawflags },
	{ "bordersize"sv, parse_bordersize },
	{ "decoration"sv, parse_decoration },
	{ "screenspace"sv, parse_screenspace },
	{ "autowrapped"sv, parse_autowrapped },
	{ "popup"sv, parse_popup },
	{ "outofboundsclick"sv, parse_outofbounds },
	{ "legacysplitscreenscale"sv, parse_legacysplitscreenscale },
	{ "hiddenduringflashbang"sv, parse_hiddenduringflashbang },
	{ "hiddenduringscope"sv, parse_hiddenduringscope },
	{ "hiddenduringui"sv, parse_hiddenduringui },
	{ "textonlyfocus"sv, parse_textonlyfocus },
	{ "forecolor"sv, parse_forecolor },
	{ "backcolor"sv, parse_backcolor },
	{ "bordercolor"sv, parse_bordercolor },
	{ "outlinecolor"sv, parse_outlinecolor },
	{ "disablecolor"sv, parse_disablecolor },
	{ "background"sv, parse_background },
	{ "type"sv, parse_type },
	{ "align"sv, parse_align },
	{ "textfont"sv, parse_textfont },
	{ "textalign"sv, parse_textalign },
	{ "textalignx"sv, parse_textalignx },
	{ "textaligny"sv, parse_textaligny },
	{ "textscale"sv, parse_textscale },
	{ "gamemsgwindowindex"sv, parse_gamemsgwindowindex },
	{ "gamemsgwindowmode"sv, parse_gamemsgwindowmode },
	{ "text"sv, parse_text },
	{ "mouseentertext"sv, parse_mouseentertext },
	{ "mouseexittext"sv, parse_mouseexittext },
	{ "mouseenter"sv, parse_mouseenter },
	{ "mouseexit"sv, parse_mouseexit },
	{ "action"sv, parse_action },
	{ "accept"sv, parse_accept },
	{ "onfocus"sv, parse_onfocus },
	{ "hasfocus"sv, parse_hasfocus },
	{ "leavefocus"sv, parse_leavefocus },
	{ "dvar"sv, parse_dvar },
	{ "dvarfloat"sv, parse_dvarfloat },
	{ "dvartest"sv, parse_dvartest },
	{ "execkey"sv, parse_execkey },
	{ "execkeyint"sv, parse_execkeyint },
	{ "enabledvar"sv, parse_enabledvar },
	{ "disabledvar"sv, parse_disabledvar },
	{ "showdvar"sv, parse_showdvar },
	{ "hidedvar"sv, parse_hidedvar },
	{ "focusdvar"sv, parse_focusdvar },
	{ "localvar"sv, parse_localvar },
	{ "focussound"sv, parse_focussound },
	{ "special"sv, parse_special },
	{ "feeder"sv, parse_special },
	{ "elementwidth"sv, parse_elementwidth },
	{ "elementheight"sv, parse_elementheight },
	{ "elementtype"sv, parse_elementtype },
	{ "columns"sv, parse_columns },
	{ "doubleclick"sv, parse_doubleclick },
	{ "notselectable"sv, parse_notselectable },
	{ "noscrollbars"sv, parse_noscrollbars },
	{ "usepaging"sv, parse_usepaging },
	{ "selectBorder"sv, parse_selectborder },
	{ "selectIcon"sv, parse_selecticon },
    { "dvarStrList"sv, parse_dvarstrlist },
	{ "dvarFloatList"sv, parse_dvarfloatlist },
	{ "dvarenumlist"sv, parse_dvarenumlist },
	{ "maxchars"sv, parse_maxchars },
	{ "maxcharsgotonext"sv, parse_maxcharsgotonext },
	{ "maxpaintchars"sv, parse_maxpaintchars },
	{ "newsfeed"sv, parse_newsfeed },
	{ "speed"sv, parse_speed },
	{ "spacing"sv, parse_spacing },
	{ "visible"sv, parse_visible },
	{ "disabled"sv, parse_disabled },
	{ "exp"sv, parse_execexp },
	{ "glowcolor"sv, parse_glowcolor },
	{ "decodeeffect"sv, parse_decodeeffect },
};

} // namespace xsk::menu::iw5
