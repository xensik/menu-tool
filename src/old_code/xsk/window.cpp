// Copyright 2022 xensik. All rights reserved.
//
// Use of this source code is governed by a GNU GPLv3 license
// that can be found in the LICENSE file.

#include "stdafx.hpp"
#include "iw5.hpp"
#include "memory.hpp"
#include "reader.hpp"
#include "window.hpp"

namespace xsk::menu::iw5
{

void window::init(windowDef_t* wnd)
{
	std::memset(wnd, 0, sizeof(windowDef_t));
	wnd->borderSize = 0.0;
	wnd->foreColor[3] = 1.0;
	wnd->foreColor[2] = 1.0;
	wnd->foreColor[1] = 1.0;
	wnd->foreColor[0] = 1.0;
}

void window::free(windowDef_t* wnd)
{
	if(wnd->name)
		memory::free((void*)wnd->name);

	if(wnd->group)
		memory::free((void*)wnd->group);

	// change this to use material handles!!!
	if(wnd->background)
	{
		if(wnd->background->name)
			memory::free((void*)wnd->background->name);

		memory::free((void*)wnd->background);
	}
}

} // namespace xsk::menu::iw5
