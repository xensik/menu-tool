// Copyright 2022 xensik. All rights reserved.
//
// Use of this source code is governed by a GNU GPLv3 license
// that can be found in the LICENSE file.

#pragma once

namespace xsk::menu::iw5
{

class window
{
public:
    static void init(windowDef_t* wnd);
    static void free(windowDef_t* wnd);
};

} // namespace xsk::menu::iw5
