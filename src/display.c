#include <ncurses.h>
#include <stdlib.h>
#include <wchar.h>

//#include "config.h"
#include "display.h"
//#include "file_manager.h"
#include "input_handler.h"

void render_screen(char *file_data[], const int current_max_lines);
// ファイルの中身をポインタ配列で渡すとそれを画面に表示する関数
void update_screen(char *file_data[], const int current_max_lines, const int current_scroll, const int window_x, const int window_y, int x_display_offset);
// ファイルの中身をポインタ配列で渡すとそれを画面に表示する関数
int get_digits(int number);
// 十進数の桁数を求める関数
int get_display_width(char *str);
// 画面上の表示幅を取得する関数

void render_screen(char *file_data[], const int current_max_lines)
{
    // ファイルの中身をポインタ配列で渡すとそれを画面に表示する関数
    // 最初に呼び出す関数ともう一度書き直すものに分割する
    // ディスプレイサイズによる描画の範囲（下限）が書いていないため、対応させる必要がある。

    int number;
    int indent_space;
    int x_offset;
    int line_len;
    int window_x;
    int window_y;

    initscr();
    noecho();
    keypad(stdscr, TRUE);
    getmaxyx(stdscr, window_x, window_y);
    // ncursesの初期設定

    number = 0;
    indent_space = get_digits(current_max_lines);
    x_offset = 0;

    while (number < current_max_lines)
    {
        mvprintw(number + x_offset, 0, "%*d %s", indent_space, number + 1, file_data[number]);

        line_len = get_display_width(file_data[number]) + indent_space + 1;

        while (line_len > window_y)
        {
            line_len -= window_y;

            x_offset++;
        }

        number++;
    }

    refresh();

    input_handler(indent_space + 1, file_data, current_max_lines, window_x, window_y);

    endwin();
    // 描画を終了する

    return;
}

void update_screen(char *file_data[], const int current_max_lines, const int current_scroll, const int window_x, const int window_y, int x_display_offset)
{
    // ファイルの中身をポインタ配列で渡すとそれを画面に表示する関数

    int number;
    int indent_space;
    int x_offset;
    int line_len;

    erase();
    // 描画した画面を削除

    number = 0;
    indent_space = get_digits(current_max_lines);
    x_offset = 0;

    while (number + current_scroll < current_max_lines)
    {
        mvprintw(number + x_offset - x_display_offset, 0, "%*d %s", indent_space, number + 1 + current_scroll, file_data[number + current_scroll]);
        // x_display_offsetはまだバグがあるので、ここを修正する。

        line_len = get_display_width(file_data[number + current_scroll]) + indent_space + 1;

        while (line_len > window_y)
        {
            line_len -= window_y;

            x_offset++;
        }

        number++;
    }

    refresh();

    return;
}

int get_digits(int number)
{
    // 十進数の桁数を求める関数

    int answer;

    answer = 1;

    while (number >= 10)
    {
        number /= 10;

        answer++;
    }

    return answer;
}

int get_display_width(char *str)
{
    // 画面上の表示幅を取得する関数

    int len;
    wchar_t *wstr;
    int test;
    int conversion_result;
    int i;
    int char_width;

    len = mbstowcs(NULL, str, 0);

    if (len == -1)
    {
        puts("char*をwchar_t*に変換する段階で問題が発生しました。");

        exit(1);
    }

    wstr = malloc((len + 1) * sizeof(wchar_t));

    if (wstr == NULL)
    {
        puts("mallocに失敗しました。");

        exit(1);
    }

    test = mbstowcs(wstr, str, len + 1);

    if (test == -1)
    {
        puts("char*をwchar_t*に変換する段階で問題が発生しました。");

        free(wstr);

        exit(1);
    }

    i = 0;
    conversion_result = 0;

    while (i < len)
    {
        char_width = wcwidth(wstr[i]);

        if (char_width == -1)
        {
            char_width = 0;
        }

        conversion_result += char_width;

        i++;
    }

    free(wstr);

    return conversion_result;
}
