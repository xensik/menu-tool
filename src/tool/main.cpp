// Copyright 2023 xensik. All rights reserved.
//
// Use of this source code is governed by a GNU GPLv3 license
// that can be found in the LICENSE file.

#include "xsk/stdinc.hpp"
#include "xsk/utils/file.hpp"
#include "xsk/menu/iw5/parser.hpp"

namespace fs = std::filesystem;

namespace xsk::menu
{

} // namespace xsk::menu

int main(int argc, char** argv)
{
    if (argc != 2) return 1;

    auto file = xsk::utils::file::read(fs::path{ argv[1] });

    auto parser = xsk::menu::iw5::parser();

    parser.parse(argv[1], file);

    return 0;
}
