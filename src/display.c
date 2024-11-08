#include <ncurses.h>
#include <stdlib.h>
#include <wchar.h>

//#include "config.h"
#include "display.h"
//#include "file_manager.h"
//#include "input_handler.h"

void render_screen(char *file_data[], const int current_max_lines);
// ファイルの中身をポインタ配列で渡すとそれを画面に表示する関数
int get_digits(int number);
// 十進数の桁数を求める関数
void input_handler(const int indent_offset, char *file_data[], const int current_max_lines);
// 別の関数を参照すると手間がかかるため、一時的にショートカットとして使用する関数
void move_mouse(int *cursor_pos_x, int *cursor_pos_y, const int indent_offset, const int line_len, const unsigned short left_arrow_flag, char *file_data[]);
// 仮想的なマウスの位置を実際の位置に移動させる関数
int strlen_utf8(const char *str);
// マルチバイト文字を含めた文字列の長さを返す関数
int get_char_size(char *str, int length);
// 表示上の文字列の増加量を取得する関数

void render_screen(char *file_data[], const int current_max_lines)
{
    // ファイルの中身をポインタ配列で渡すとそれを画面に表示する関数
    // 関数の中身を分割して別々に呼んでもらう形に変更する

    int number;
    int indent_space;

    initscr();
    noecho();
    keypad(stdscr, TRUE);
    // ncursesの初期設定

    number = 0;
    indent_space = get_digits(current_max_lines);

    while (file_data[number] != NULL)
    {
        mvprintw(number, 0, "%*d %s", indent_space, number + 1, file_data[number]);

        number++;
    }

    input_handler(indent_space + 1, file_data, current_max_lines);

    endwin();

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

void input_handler(const int indent_offset, char *file_data[], const int current_max_lines)
{
    // 別の関数を参照すると手間がかかるため、一時的にショートカットとして使用する関数
    // 引数として行数インデントの数を持つ

    int cursor_pos_x;
    int cursor_pos_y;

    cursor_pos_x = 0;
    cursor_pos_y = indent_offset;

    move(0, indent_offset);

    while (1)
    {
        switch (getch())
        {
            case KEY_UP:
                if (cursor_pos_x > 0)
                {
                    cursor_pos_x--;
                    move_mouse(&cursor_pos_x, &cursor_pos_y, indent_offset, strlen_utf8(file_data[cursor_pos_x]) + indent_offset, 0, file_data);
                }
                break;
            case KEY_DOWN:
                if (cursor_pos_x < current_max_lines - 1)
                {
                    cursor_pos_x++;
                    move_mouse(&cursor_pos_x, &cursor_pos_y, indent_offset, strlen_utf8(file_data[cursor_pos_x]) + indent_offset, 0, file_data);
                }
                break;
            case KEY_LEFT:
                if (cursor_pos_y > indent_offset)
                {
                    cursor_pos_y--;
                    move_mouse(&cursor_pos_x, &cursor_pos_y, indent_offset, strlen_utf8(file_data[cursor_pos_x]) + indent_offset, 1, file_data);
                }
                break;
            case KEY_RIGHT:
                if (cursor_pos_y < strlen_utf8(file_data[cursor_pos_x]) + indent_offset - 2)
                {
                    cursor_pos_y++;
                    move_mouse(&cursor_pos_x, &cursor_pos_y, indent_offset, strlen_utf8(file_data[cursor_pos_x]) + indent_offset, 0, file_data);
                }
                break;
            case 'q':
                return;
                break;
        }
    }

    return;
}

void move_mouse(int *cursor_pos_x, int *cursor_pos_y, const int indent_offset, const int line_len, const unsigned short left_arrow_flag, char *file_data[])
{
    // 仮想的なマウスの位置を実際の位置に移動させる関数
    // これ他にも引数必要では?
    // 今は一マスの移動のみに対応している
    // 大きく移動させたいときは移動したい方向を引数で渡すことにする

    if (*cursor_pos_y > line_len - 2)
    {
        if (left_arrow_flag == 1)
        {
            if (line_len - 3 > indent_offset)
            {
                *cursor_pos_y = line_len - 3;
            }

            else
            {
                *cursor_pos_y = indent_offset;
            }
        }

        else
        {
            move(*cursor_pos_x, line_len - 2);

            return;
        }
    }

    move(*cursor_pos_x, *cursor_pos_y + get_char_size(file_data[*cursor_pos_x], *cursor_pos_y - indent_offset));
    // オフセットが定義されていない
    // こいつをなんとかする(y)
    // *cursor_pos_yはワイド文字を含めない今の行
    // これにマルチバイトの増加量を加算すれば勝ちでは?
    // ただ、上下に移動した時、位置が合わない問題が発生するので、これに対処する必要がある。

    return;
}

int strlen_utf8(const char *str)
{
    // マルチバイト文字を含めた文字列の長さを返す関数

    int str_index;
    int number;

    str_index = 0;
    number = 0;

    while (str[str_index])
    {
        if ((str[str_index] & 0xc0) != 0x80)
        {
            number++;
        }

        str_index++;
    }

    return number + 1;
}

int get_char_size(char *str, int length)
{
    // 表示上の文字列の増加量を取得する関数
    //
    int width_sum;
    wchar_t wc;
    int bytes_read;
    int chars_processed;
    char *current_ptr;
    int width;

    width_sum = 0;
    bytes_read = 0;
    chars_processed = 0;
    current_ptr = str;

    // 指定された文字数(length)まで処理する
    while (chars_processed < length && *current_ptr != '\0')
    {
        bytes_read = mbtowc(&wc, current_ptr, MB_CUR_MAX);

        if (bytes_read <= 0)
        {
            puts("マルチバイト文字の変換に失敗しました。");

            return -1;
        }

        width = wcwidth(wc);

        if (width >= 0)
        {
            width_sum += width;
        }

        current_ptr += bytes_read;

        chars_processed++;
    }

    // 半角との差分を返すために width_sum - chars_processed を返す
    return width_sum - chars_processed;
}
