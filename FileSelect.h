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
#include "utils/Window.h"
#include <functional>

#define SCREEN_W 500
#define SCREEN_H 500


class FileSelect : public MainWin{
    public:
        FileSelect(std::string title, v2d<unsigned int> dimentions, bool save_mode, bool isPipe = false, bool select_mode = true);

    public:
        void Start(void* ret_string_ptr);

        void Event(XEvent* event);

        void Redraw();

        void ClearItems();

        void AddItems();

        void RefreshItemList();
    public:
        Directory curdir;
        std::string ret;
        unsigned int selected;
        std::string usr;
        

        struct dirnode{std::string name; int type;};
        std::vector<dirnode> dircontents;
        unsigned int static_buttons_count = 0;

    public:
        struct{
            bool end : 1;
            bool select_mode : 1;
            bool show_hidden : 1;
            bool isPipe : 1;
        };
    private:
        

        void init_main_buttons();
};

extern FileSelect* fsel;
