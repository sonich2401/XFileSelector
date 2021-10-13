/*
RenDev#2616 File Selector Tool
    Copyright (C) 2021 RenDev

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

Contact me at vgngamingnetwork@gmail.com if you need to contact me
*/
#include "FileSelect.h"
#include <string>
#include <iostream>

const char* NAME_OF_CREATOR = "Software made by RenDev#2616, vgngamingnetwork@gmail.com";

int main(int argc, char* argv[]){
    FileSelect * win;

    v2d<unsigned int> size;
    switch(argc){
        case 1:
            win = new FileSelect("Select a file", {SCREEN_W, SCREEN_H}, true);
        break;
        case 2:
            win = new FileSelect(std::string(argv[1]), {SCREEN_W, SCREEN_H}, true);
        break;
        case 4:
            size = {std::stoi(argv[2]), std::stoi(argv[3])};
            if(size.y < 400)
                size.y = 400;
            if(size.x < 400)
                size.x = 400;
            win = new FileSelect(std::string(argv[1]), size, true);
        break;
        default:
            std::cerr << "ERR: Not enough arguments";
            exit(-1);
        break;
    };

    std::string path_chosen;
    win->Start(&path_chosen);

    puts(path_chosen.c_str());

    return 0;
}
