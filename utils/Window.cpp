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


std::vector<std::string> split_c(std::string str, char term){
    std::vector<std::string> ret;
    std::string cur_line = "";

    for(unsigned int i = 0; i < str.size(); i++){
        if(str[i] == term){
            ret.push_back(cur_line);
            cur_line = "";
            continue;
        }
        cur_line += str[i];
    }

    if(cur_line != "")
        ret.push_back(cur_line);
    return ret;
}

std::vector<std::string> split(std::string str, std::string term){
    std::vector<std::string> ret;
    std::string cur_line = "";

    for(unsigned int i = 0; i < str.size(); i++){
        if(i + term.size() < str.size())
            if(str.substr(i, term.size()) != term)
                cur_line += str[i];
            else{
                i += term.size();
                ret.push_back(cur_line);
                cur_line = "";
            }

    }
    if(cur_line != "")
        ret.push_back(cur_line);
    return ret;
}


std::string exec(std::string command){
    const char * cmd = command.c_str();
    char buffer[MAX_OUTPUT];
    std::string result = "";
    FILE* pipe = popen(cmd, "r");
    if (!pipe) throw std::runtime_error("popen() failed!");
    try {
        while (fgets(buffer, sizeof buffer, pipe) != NULL) {
            result += buffer;
        }
    } catch (...) {
        pclose(pipe);
        throw;
    }
    pclose(pipe);
    return result;
}

std::vector<std::string> getSubDir(std::string rootDir, bool fullPath){
    #ifdef WIN32
        const std::string CMD = "dir \"" + rootDir + "\" /b /AD";
    #else
        const std::string CMD = "find \"" + rootDir + "\" -maxdepth 1 -type d";
    #endif

    std::vector<std::string> ret = split(exec(CMD), "\n");
    
    //Add the full path if required
    //Add the full path if required
    #ifdef WIN32 //Only windows needs this
        if(fullPath)
            for(std::string& str : ret){
                if(rootDir.back() != '/' || rootDir.back() != '\\')
                    rootDir += "/";
                printf("PATH: %s\n", (rootDir + str).c_str());
                str = rootDir + str;
            }
    #else //Only linux needs this
        if(!ret.empty())
            ret.erase(ret.begin() + 0); //On linux, '.' is listed as a directory so we delete it
    #endif

    for(std::string& str : ret){
        str = split_c(str, '/').back();
    }

    return ret;
}

std::vector<std::string> getDirFiles(std::string rootDir, std::string fileType, bool fullPath){
    #ifdef WIN32
        const std::string CMD = "dir \"" + rootDir + "\" /b | find \"" + fileType + "\"";
    #else
        const std::string CMD = "find \"" + rootDir + "\" -maxdepth 1 -name *." + fileType;
    #endif
    std::vector<std::string> ret;
    if(rootDir == "\n") //Some OS will 
        return ret;

    ret = split(exec(CMD), "\n");

    //Add the full path if required
    #ifdef WIN32 //Only windows needs this
        if(fullPath)
            for(std::string& str : ret){
                if(rootDir.back() != '/' || rootDir.back() != '\\')
                    rootDir += "/";
                printf("PATH: %s\n", (rootDir + str).c_str());
                str = rootDir + str;
            }
    #else //Only linux needs this
        if(!ret.empty())
            ret.erase(ret.begin() + 0); //On linux, '.' is listed as a directory so we delete it
    #endif
    return ret;
}


time_t getFileModifiedTime(std::string filePath){
    return getFileModifiedTime(filePath.c_str());
}

time_t getFileModifiedTime(char * filePath){
    return getFileModifiedTime((const char *)(void*)filePath);
}


time_t getFileModifiedTime(const char * filePath){
    struct stat fileInfo;

    assert(!stat(filePath, &fileInfo));

    return fileInfo.st_mtime;
}




Color::Color(){
    this->bytes = 0;
}

Color::Color(unsigned char r, unsigned char g, unsigned char b){
    this->xcmsColor.spec.RGB.red = r;
    this->xcmsColor.spec.RGB.green = g;
    this->xcmsColor.spec.RGB.blue = b;
}

Color::Color(std::string fn, Display * dis, Window win){
    int s = DefaultScreen(dis);
    Colormap colormap = DefaultColormap(dis, s);
    XColor used, unused;
    XAllocNamedColor(dis, colormap, fn.c_str(),  &used, &unused);
    this->bytes = used.pixel;
}

Color::Color(unsigned char r, unsigned char g, unsigned char b, unsigned char a){
    this->rgba.r = r;
    this->rgba.g = g;
    this->rgba.b = b;
    this->rgba.a = a;
    
}



Directory::Directory(){
    
}

void Directory::cd(std::string new_dir){
    if(next != nullptr){ 
        next->cd(new_dir);
        return;
    }

	this->next = new Directory();
	this->next->last = this;
	this->next->dir = new_dir;
    this->next->next = nullptr;
    
}

std::string Directory::getFullDir(){
    std::string ret = "/" + this->dir;
    if(this->dir == "/")
        ret = "";
    if(this->next != nullptr)
        return ret + this->next->getFullDir();
    if(this->last == nullptr)
        return "/";
    return ret;
}

void Directory::upDirectory(){
    if(this->next != nullptr){
        if(this->next->next == nullptr){
            delete this->next;
            this->next = nullptr;
            return;
        }
        this->next->upDirectory();
    }
}

void Directory::setDirectory(std::string dir){
    std::vector<std::string> foldernames = split_c(dir, '/');

    this->dir = foldernames[0];
    if(this->dir == "")
        this->dir = "/";
    foldernames.erase(foldernames.begin() + 0);

    this->clear();

    for(std::string& fname : foldernames){
        this->cd(fname);
    }
}

void Directory::clear(){
    if(this->next != nullptr)
        this->next->clear();
    delete this->next;
    this->next = nullptr;
}

Directory* Directory::copy(Directory* rlast){
    Directory* node_copy = new Directory();
    node_copy->dir = this->dir;
    node_copy->last = rlast;
    node_copy->next = nullptr;
    if(this->next != nullptr){
        node_copy->next = copy(node_copy);
    }
    return node_copy;
}



Button::Button(std::string text, unsigned int x, unsigned int y, int width, int height, void (*_action)(void*)){
    this->text = text;
    this->x = x;
    this->y = y;
    this->width = width;
    this->height = height;
    this->action = _action;
    this->clicked = false;
}

Button::Button(std::string text, int type, unsigned int x, unsigned int y, int width, int height, void (*_action)(void*)){
    this->text = text;
    this->type = type;
    this->x = x;
    this->y = y;
    this->width = width;
    this->height = height;
    this->action = _action;
    this->clicked = false;
}

bool Button::ButtonClick(unsigned int clickx, unsigned int clicky){
    return clickx > this->x && 
           clickx < this->x + this->width &&
           clicky > this->y &&
           clicky < this->y + this->height;
}


