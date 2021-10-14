/*RenDev#2616 File Selector Tool
    Copyright (C) 2021 RenDev

    Permission to use, copy, modify, and/or distribute this software for any purpose with or without fee is hereby granted.
    Permission is also granted to not credit the author in any way as long as you do not take credit
    for this piece of software.

    Anyone is free to copy, modify, publish, use, compile, sell, or
    distribute this software, either in source code form or as a compiled
    binary, for any purpose, commercial or non-commercial, and by any
    means.

    THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS.
    IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, 
    OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, 
    DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS 
    ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

Contact me at vgngamingnetwork@gmail.com if you need to contact me about this licence*/
#include "FileSelect.h"
#include <string>
#include <iostream>

const char* NAME_OF_CREATOR = "Software made by RenDev#2616, vgngamingnetwork@gmail.com";
const char* DATE_OF_BUILD = "BUILD DATE: "DATE;
const char* VERSION_NUMBER = "VERSION NUMBER: "VER;
const char* PROGRAM_NAME = PROG_NAME;


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
