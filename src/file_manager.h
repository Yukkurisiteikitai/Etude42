#ifndef FILE_MANAGER_H
#define FILE_MANAGER_H

#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>

//#include "config.h"
#include "display.h"
//#include "input_handler.h"

int read_file(const char *file_path);
// ファイルのパスを引数に指定するとファイルを読み込む関数
FILE *get_file_pointer(const char *file_path);
// ファイルのポインタを返す関数
int save_file(const char *file_path, const char *data);
// ファイルをセーブする関数
DIR *get_directory_pointer(const char *directory_path);
// 指定された一番上のディレクトリのポインタを返す関数

#endif // FILE_MANAGER_H
