/*RenDev#2616 File Selector Tool
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

Contact me at vgngamingnetwork@gmail.com if you need to contact me*/

#include <sys/stat.h>
#define FU_LIST_END NULL

#ifndef __cplusplus
  #include <stdbool.h>
#endif

typedef struct {
  unsigned long long size;
  union {
    char** text;
    char* bin;
  };
}fu_List;

typedef fu_List fu_TextFile;
typedef fu_List fu_BinFile;



typedef struct{
  struct{
    unsigned char hour   : 5;
    unsigned char minute : 7;
    unsigned char second : 7;
    unsigned char day    : 6;
    unsigned char month  : 5;
    unsigned short year;
  };

}Timestamp;


typedef struct {
  Timestamp modified;
  Timestamp created;
  char * file_name;

  unsigned long bytes;
}FileStats;



fu_List fu_get_dir_contents(char * folder, bool show_hidden);
fu_List fu_get_dir_files(char * folder, bool show_hidden);
fu_List fu_get_dir_folders(char * folder, bool show_hidden);
void fu_free_list(fu_List list);
void fu_free_text_file(fu_TextFile txt_file);
void fu_free_bin_file(fu_BinFile bin_file);

FileStats fu_get_file_info(const char * file_path);
Timestamp fu_get_file_created_time(const char * file_path);
Timestamp fu_get_file_modified_time(const char * file_path);
unsigned long fu_get_file_size(const char * file_path);

fu_BinFile fu_load_bin_file(const char * file_path);
fu_TextFile fu_load_text_file(const char * file_path);

void fu_save_bin_file(const char * file_path, fu_BinFile bin, unsigned long size);
void fu_save_text_file(const char * file_path, fu_TextFile text);

void fu_touch_file(const char * file_path);
char* fu_get_current_user();




#ifdef FILE_UTIL_IMP
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>

struct stat futil__st;
FileStats futil__last_stat;

#include <stdio.h>

#define MAX_OUTPUT 300

#define _popen popen
#define _pclose _pclose


char* exec(const char * command){
    const char * cmd = command;
    char buffer[MAX_OUTPUT];
    char* result = NULL;
    FILE* pipe = popen(cmd, "r");
    if (!pipe){
      perror("exec failed");
      exit(-1);
    }

    long long int exec_size = 0;
    while (fgets(buffer, sizeof buffer, pipe) != NULL) {
        exec_size += strlen(buffer);
        if(result == NULL){
          result = (char*)malloc(MAX_OUTPUT + 1);
          memcpy(result, buffer, MAX_OUTPUT);
          result[MAX_OUTPUT] = '\0';
        }else{
          realloc(result, exec_size);
          strcat(result, buffer);
        }
    }
    pclose(pipe);
    return result;
}

void fu_helper__init_timestamp(Timestamp * __restrict__ ts){
  ts->day = 0;
  ts->month = 0;
  ts->year = 0;
  ts->hour = 0;
  ts->minute = 0;
  ts->second = 0;
}

void fu_helper__init_fstat(FileStats * __restrict__ fstat){
  fstat->bytes = 0;
  fu_helper__init_timestamp(&fstat->created);
  fu_helper__init_timestamp(&fstat->modified);
  fstat->file_name = (char*)malloc(1);
  fstat->file_name[0] = '\0';
}

void fu_helper__set_timestamp(Timestamp * ts, int sec, int min, int h){
  ts->second = sec;
  ts->minute = min;
  ts->hour = h;
}

char fu_helper__isDir(struct dirent* dir){
  //printf("TYPE = %i, TEST = %i, NAME = \"%s\", BOOL = %i\n", dir->d_type, DT_DIR, dir->d_name, dir->d_type == DT_DIR);
  return dir->d_type == DT_DIR || dir->d_type == DT_LNK;
}

char fu_helper__isFile(struct dirent* dir){
  //printf("TYPE = %i, TEST = %i, NAME = \"%s\", BOOL = %i\n", dir->d_type, DT_REG, dir->d_name, dir->d_type == DT_REG);
  return dir->d_type == DT_REG;
}



FileStats * fu_helper__update(const char * file_path){
  lstat(file_path, &futil__st);

  free(futil__last_stat.file_name);
  futil__last_stat.file_name = (char*)malloc(strlen(file_path));
  futil__last_stat.file_name = strcpy(futil__last_stat.file_name, file_path);
  futil__last_stat.bytes = futil__st.st_size;

  struct tm *conv_tm = (struct tm*)localtime(&futil__st.st_mtim.tv_sec);

  futil__last_stat.modified.second = conv_tm->tm_sec;
  futil__last_stat.modified.minute = conv_tm->tm_min;
  futil__last_stat.modified.hour = conv_tm->tm_hour;

  futil__last_stat.modified.day = conv_tm->tm_mday;
  futil__last_stat.modified.month = conv_tm->tm_mon;
  futil__last_stat.modified.year = conv_tm->tm_year + 1900;

  return &futil__last_stat;
}




fu_List fu_helper__get_dir_contents(char * folder, char folders, char files, bool show_hidden){
  char ** ret = NULL;
  DIR * d = opendir(folder);
  struct dirent* dir;
  unsigned long long element_count = 0;

  char* p;

  while((dir = readdir(d)) != NULL){
    //Get rid of "." and ".." directory entries
    if(dir->d_name[0] == '.'){
      if(!show_hidden)
        continue;
      if(strlen(dir->d_name) == 1)
        continue;
      if(dir->d_name[1] == '.'){
        if(strlen(dir->d_name) == 2)
          continue;
      }
    }
    if(fu_helper__isDir(dir) & !folders){
      continue;
    }
    if(fu_helper__isFile(dir) & !files){
      continue;
    }

    if(ret == NULL){
      ret = (char**)malloc(sizeof(char*));
    }else{
      ret = (char**)realloc(ret, sizeof(char*) * (element_count + 1));
    }
    p = ret[0];
    //printf("ELEMENT %i\n", element_count);
    ret[element_count] = (char*)malloc(256);
    strcpy(ret[element_count], dir->d_name);
    element_count++;
  }

  closedir(d);
  return {(unsigned long long)element_count, ret};
}

fu_List fu_get_dir_contents(char * folder, bool show_hidden){
  return fu_helper__get_dir_contents(folder, 1, 1, show_hidden);
}

fu_List fu_get_dir_files(char * folder, bool show_hidden){
  return fu_helper__get_dir_contents(folder, 0, 1, show_hidden);
}

fu_List fu_get_dir_folders(char * folder, bool show_hidden){
  return fu_helper__get_dir_contents(folder, 1, 0, show_hidden);
}

void fu_free_list(fu_List list){
  for(unsigned long long i = 0; i < list.size; i++){
    free(list.text[i]);
  }
}

void fu_free_text_file(fu_TextFile txt_file){
  fu_free_list(txt_file);
}

void fu_free_bin_file(fu_BinFile bin_file){
  free(bin_file.bin);
}


FileStats fu_get_file_info(const char * file_path){
  return *fu_helper__update(file_path);
}

Timestamp fu_get_file_created_time(const char * file_path){
  return fu_helper__update(file_path)->created;
}

Timestamp fu_get_file_modified_time(const char * file_path){
  return fu_helper__update(file_path)->modified;
}

unsigned long fu_get_file_size(const char * file_path){
  return fu_helper__update(file_path)->bytes;
}


char* fu_helper__resolve_tabs(char* str){
  const int TAB_SIZE = 4;
  char* og_str = str;

  unsigned long long int tab_count = 0;
  for(unsigned long long int i = 0; i < strlen(str); i++){
    tab_count += str[i] == '\t';
  }

  unsigned long long int new_size = strlen(str) - tab_count + (tab_count * TAB_SIZE) + 1;
  char* new_buff = (char*)malloc(new_size);
  char* new_buff_og = new_buff;
  for(unsigned long long int i = 0; i <= new_size; i++){
    if(str > og_str + strlen(og_str)){
      break;
    }
    if(*str == '\t'){
      for(unsigned char i1 = 0; i1 < TAB_SIZE; i1++){
        *new_buff = ' ';
        new_buff++;
      }
      str++;
    }else{
      *new_buff = *str;
      new_buff++;
      str++;
    }
  }
  new_buff_og[new_size - 1] = '\0';

  free(og_str);
  return new_buff_og;
}

fu_BinFile fu_load_bin_file(const char * file_path){
  FILE *f = fopen(file_path, "r");
  if(f == NULL){
    printf("ERR: fu_load_bin_file(): Couldn't load file \"%s\"\n", file_path);
    exit(-1);
  }
  fseek(f, 0, SEEK_END);
  int fsize = ftell(f);
  fseek(f, 0, SEEK_SET);

  if(fsize <= 0){
    printf("ERR: fu_load_bin_file(): File size = 0!\n");
    fclose(f);
    exit(-1);
  }

  #if 0
    char buffer[1000];
    char* ret_buffer = NULL;
    long long int f1size = 0;
    while (fgets(buffer, sizeof buffer, f) != NULL) {
      f1size += strlen(buffer);
      if(ret_buffer == NULL){
        ret_buffer = (char*)malloc(f1size);
        strcpy(ret_buffer, buffer);
      }else{
        realloc(ret_buffer, f1size);
        strcat(ret_buffer, buffer);
      }
    }
    
    //fread(buffer, fsize, fsize, f);
  #else
    char* ret_buffer = (char*)malloc(fsize);
    fread(ret_buffer, fsize, fsize, f);
  #endif

  fclose(f);

  fu_BinFile ret;
  ret.bin = ret_buffer;
  ret.size = fsize;
  return ret;
}

fu_TextFile fu_load_text_file(const char * file_path){
  fu_BinFile binfile = fu_load_bin_file(file_path);
  char* buffer = binfile.bin;
  char* og_buffer_ptr = buffer;

  //Get total amount of lines
  unsigned long long nline_count = 1;
  unsigned long long nline_last = -1;
  for(unsigned long long i = 0; i < strlen(buffer); i++){
    if(buffer[i] == '\n'){
      nline_count++;
      nline_last = i;
    }
  }
  //was there text at the end that we missed?
  if(buffer[strlen(buffer) - 1] != '\n')
    nline_count++;
  

  //create string array
  char** ret_buff = (char**)malloc(sizeof(char*) * nline_count);
  unsigned long long int chars_processed = 0;
  for(unsigned long long i = 0; i + 1 < nline_count; i++){
    unsigned long long nline_strlen = 0;
    unsigned long long i1 = 0;
    while(1){
      if(chars_processed + 1 < binfile.size){
        if(buffer[i1] == '\n')
          break;
        nline_strlen++;
        i1++;
        chars_processed++;
        continue;
      }
      break;
    }

    ret_buff[i] = (char*)malloc(nline_strlen + 1);
    memcpy(ret_buff[i], buffer, nline_strlen);
    ret_buff[i][nline_strlen] = '\0';
    ret_buff[i] = fu_helper__resolve_tabs(ret_buff[i]);

    buffer += (nline_strlen + 1);
  }

  fu_free_bin_file(binfile);

  fu_TextFile ret;
  ret.text = ret_buff;
  ret.size = nline_count - 1;
  return ret;
}

void fu_save_bin_file(const char * file_path, char * bin, unsigned long size);
void fu_save_text_file(const char * file_path, char ** text);

void fu_touch_file(const char * file_path){
  #define CMD_STR "touch "
  char * cmd = (char*)malloc(strlen(CMD_STR) + strlen(file_path));
  strcat(cmd, file_path);
  system(cmd);
  free(cmd);
}

char* fu_get_current_user(){
  return getenv("USER");
}


#endif
