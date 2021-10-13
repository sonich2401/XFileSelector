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
#pragma once
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <X11/Xcms.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <string>
#include <vector>
#include <functional>



std::vector<std::string> split(std::string str, std::string term);


#include <iostream>
#include <stdexcept>
#include <stdio.h>

#define MAX_OUTPUT 300

#ifdef WIN32
	#define popen _popen
	#define pclose _pclose
#endif

std::string exec(std::string command);

#include <sys/types.h>
#include <sys/stat.h>
#include <assert.h>
#ifndef WIN32
#include <unistd.h>
#endif

#ifdef WIN32
#define stat _stat
#endif

std::vector<std::string> getSubDir(std::string rootDir, bool fullPath);

std::vector<std::string> getDirFiles(std::string rootDir, std::string fileType, bool fullPath);


time_t getFileModifiedTime(std::string filePath);

time_t getFileModifiedTime(char * filePath);

time_t getFileModifiedTime(const char * filePath);


union Color{
    Color();
    Color(std::string fn, Display * dis, Window win);
    Color(unsigned char r, unsigned char g, unsigned char b);
    Color(unsigned char r, unsigned char g, unsigned char b, unsigned char a);

    struct{
        //unsigned char a, b, g, r;   //Big endian
        unsigned char r, g, b, a;   //litte endian
    }rgba;

    long bytes;

    XColor xcolor;

    XcmsColor xcmsColor;
};

template<typename T>
struct v2d{
    T x, y;
};


struct Directory;

struct Directory {
    Directory();
    
	Directory * last = nullptr;
	std::string dir;
	Directory * next = nullptr;

	void cd(std::string new_dir);

	std::string getFullDir();

    void upDirectory();

    void setDirectory(std::string dir);

    void clear();

    Directory* copy(Directory*rlast = nullptr);
};


typedef void (*Callback)(void *);

struct Button{
    Button(std::string text, unsigned int x, unsigned int y, int width, int height, void (*action)(void*));
    Button(std::string text, int type, unsigned int x, unsigned int y, int width, int height, void (*action)(void*));
    std::string text;
	unsigned int x,y;
	int width, height;
    int type;
    bool clicked;
	//Callback action;
    std::function<void(void*)> action;

    bool ButtonClick(unsigned int clickx, unsigned int clicky);
};

class MainWin{
    public:
        MainWin(std::string title, v2d<unsigned int> dimentions);

        virtual void Redraw();

        virtual void Event(XEvent * event);

        virtual void Start(void * ret_data = nullptr);

    public:
        Display *dis;
        int screen;
        Window win;
        GC gc;

    public:
        float scroll;
        v2d<unsigned int> mouse;
        v2d<unsigned int> size;
        std::vector<Button> buttons;


    void DrawRect(unsigned int x, unsigned int y, unsigned int w, unsigned int h, Color c = Color(255, 255, 255, 255));

    void FillRect(unsigned int x, unsigned int y, unsigned int w, unsigned int h, Color c = Color(57, 57, 58, 1));

    void DrawString(long int x, long int y, char * text, Color c = Color(255, 255, 255, 255));

    void DrawLine(unsigned int x, unsigned int y, unsigned int x1, unsigned int y1);

    void DrawButton(Button& bt);

    void DrawButton(Button& bt, int offx, int offy);

    void AddButton(v2d<int> pos, v2d<unsigned int> wh, std::string text, void (*func)(void*));

    void AddButton(v2d<int> pos, v2d<unsigned int> wh, std::string text, int type,  void (*func)(void*));
  
    void Clear();

    public:
        ~MainWin();
};
