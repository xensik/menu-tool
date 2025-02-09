// Copyright 2022 xensik. All rights reserved.
//
// Use of this source code is governed by a GNU GPLv3 license
// that can be found in the LICENSE file.

#pragma once

namespace xsk::menu::iw5
{

class memory
{
public:
template<typename T>
static T* alloc()
{
	return std::malloc(sizeof(T));
}

template<typename T>
static T* alloc(size_t size)
{
	return std::malloc(sizeof(T) * size);
}

static void free(void* ptr)
{
	return std::free(ptr);
}
};

} // namespace xsk::menu::iw5
