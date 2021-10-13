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
#include "Window.h"
#include <X11/Xatom.h>

Color aqua;
MainWin::MainWin(std::string title, v2d<unsigned int> dimentions){
    /* get the colors black and white (see section for details) */        
    unsigned long black,white, grey;

    //Init Display
    dis=XOpenDisplay((char *)0);
    screen=DefaultScreen(dis);
    scroll = 0;
        
    //Init colors
    black=BlackPixel(dis,screen),
    white=WhitePixel(dis, screen);
    grey = Color(57, 57, 58, 1).bytes;

    //Create Window
    size = dimentions;
    win=XCreateSimpleWindow(dis,DefaultRootWindow(dis),0,0,	size.x, size.y, 5,white, grey);
    XSetStandardProperties(dis,win, title.c_str(),"Hi",None,NULL,0,NULL);
    
    XSelectInput(dis, win, ExposureMask|ButtonPressMask|KeyPressMask|StructureNotifyMask|DestroyNotify);
    gc=XCreateGC(dis, win, 0,0);
        
    //Set Drawing Colors
    XSetBackground(dis,gc,white);
    XSetForeground(dis,gc,white);

    //Draw Window
    XClearWindow(dis, win);
    XMapRaised(dis, win);

    aqua = Color("Aqua", dis, win);
}

void MainWin::Redraw(){
    Clear();
}


void MainWin::Event(XEvent * event){
    const float SCROLL_SPEED = 2.1f;
            
    if (event->type == ConfigureNotify)
        if(event->xconfigure.width != size.x || event->xconfigure.height != size.y)
            XResizeWindow(dis, win,size.x,size.y);

    if(event->type == ButtonPress){
        mouse.x = event->xbutton.x;
        mouse.y = event->xbutton.y;
    }
}

void MainWin::Start(void * ret_data){
    XEvent event;
    while(1) {
        XNextEvent(dis, &event);

        Event(&event);
        Redraw();
    }
}

void MainWin::DrawLine(unsigned int x, unsigned int y, unsigned int x1, unsigned int y1){
    XDrawLine(dis,win,gc, x,y, x1, y1);
}

void MainWin::DrawRect(unsigned int x, unsigned int y, unsigned int w, unsigned int h, Color c){
    XSetForeground(dis, gc, c.bytes);
    DrawLine(x,y, x, y + h);
    DrawLine(x,y, x + w, y);
    DrawLine(x,y + h, x + w, y + h);
    DrawLine(x + w,y, x + w, y + h);
    //XSetForeground(dis, gc, WhitePixel(dis, screen));
}

void MainWin::FillRect(unsigned int x, unsigned int y, unsigned int w, unsigned int h, Color c){
    XSetForeground(dis, gc, c.bytes);
    XFillRectangle(dis, win, gc, x, y, w, h);
    XSetForeground(dis, gc, WhitePixel(dis, screen));
}

void MainWin::DrawString(long int x, long int y, char * text, Color c){
    XSetForeground(dis, gc, c.bytes);
    char * str_cpy = (char*)malloc(strlen(text) + 1);
    strcpy(str_cpy, text);
    XDrawString(dis, win, gc, x, y, str_cpy, strlen(str_cpy));
    free(str_cpy);
    XSetForeground(dis, gc, WhitePixel(dis, screen));
}

void MainWin::DrawButton(Button& bt){
    DrawButton(bt, 0, 0);
}


void MainWin::DrawButton(Button& bt, int offx, int offy){
    int x, y;
    x = (bt.width - bt.text.size() * 5)/2;
    y = (bt.height - 10)/2;

    Color c; 
    if(bt.type == 1)
        //c = Color(0, 255, 0, 255); //Green
        c = Color(255,255,255,255); //White
    else
    if(bt.type == 2)
        //c = Color( 255, 255,  255);
        //c = Color(90 + 20, 85 + 20, 68 + 20, 255); //Light Blue  PS: Do not ask me why for some reason its bbbggrrraa
        c = aqua;
    else
        c = Color(255,255,255,255); //White

    DrawRect(bt.x + offx, bt.y + offy, bt.width, bt.height, c);
    DrawString(bt.x + x + offx, bt.y + y + 10 + offy, (char*)bt.text.c_str(), c);
}

void MainWin::AddButton(v2d<int> pos, v2d<unsigned int> wh, std::string text, void (*func)(void*)){
    Button newButton(text, pos.x, pos.y, wh.x, wh.y, func);
    
        
    buttons.push_back(newButton);
}

void MainWin::AddButton(v2d<int> pos, v2d<unsigned int> wh, std::string text, int type,  void (*func)(void*)){
    Button newButton(text, type, pos.x, pos.y, wh.x, wh.y, func);
    
        
    buttons.push_back(newButton);
}

void MainWin::Clear(){
    XClearWindow(dis, win);
}

MainWin::~MainWin(){
    XFreeGC(dis, gc);
    XDestroyWindow(dis,win);
    XCloseDisplay(dis);	
    //exit(1);	
}
