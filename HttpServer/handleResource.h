#ifndef HANDLERESOURCE_H
#define HANDLERESOURCE_H

#include <stdio.h>
#include <stdlib.h>
#include <io.h>
#include <errno.h>

typedef int bool;
#define true 1
#define false 0

/*  */
int translate_uri_to_winpath(const char* uri, char* winPath);

bool is_existed(const char *winPath);
bool is_dir(const char *winPath);
int get_index(char* winPath);

int get_file_size(const char* winpath);
int get_file_type(const char* fileName, char *fileType);
int get_content_type(const char* fileType, char *contentType);
int get_file_content(const char* winPath, char *content);

/* 根据目录生成html */
int get_html_with_dir(const char *dir, char *html);

#endif