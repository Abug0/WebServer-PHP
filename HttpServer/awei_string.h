#ifndef AWEI_STRING_H
#define AWEI_STRING_H

int get_char_index(const char *srcStr, char chr);
int replace_str(char *srcStr, const char *from, const char *to);
int get_sub_str(const char *srcStr, char *destStr, int pos, int count);
int insert_to_str(char *destStr, const char* subStr, int pos);
int del_sub_str(char *srcStr, int pos, int count);

#endif