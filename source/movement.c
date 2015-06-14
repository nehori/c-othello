/**
 * Copyright (c) 2015 Kazutaka Yasuda
 * Released under the MIT license
 * http://opensource.org/licenses/mit-license.php
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "movement.h"

/************ 8������\���萔 **********************/

void Board_Directons(int num, int *row, int *col)
{
    const static signed char d[] = {1, 0, -1, -1, -1, 0, 1, 1, 1, 0};
    *row = d[num  ];
    *col = d[num + 2];
}

/***************** �Ղ̏������֐� *******************/

void first(int src[], int dst[])
{
    bCopy(src, dst, int, Board_Size * Board_Size);
}

/************ �Ղ͈̔͂���������֐� ******************/

int check_board(int row, int col)
{
    return ~(row | col) & 8;
}

/************ �΂��Ђ�����Ԃ��֐� *******************/

void board_move(int turn, int row, int col,
                int r_othello[Board_Size * Board_Size])
{
    int dr, dc, base = row * Board_Size + col;
    int num = 0, xx, yy, t;

    r_othello[base] = turn;

    for (num = 0; num < Board_Turn; ++num) {
        Board_Directons(num, &xx, &yy);

        for (dr = xx, dc = yy;
                check_board(row + dr, col + dc);
                dr += xx, dc += yy) {
            t = r_othello[base + dr * Board_Size + dc];

            if (t == 0) {
                break;
            }

            if (t == turn) {
                while ( (dr -= xx) | (dc -= yy) ) {
                    r_othello[base + dr * Board_Size + dc] = turn;
                }

                break;
            }
        }
    }
}

/**** �΂�u���邩���ׂ�֐� �u�����T,�u���Ȃ����F *******/

int board_movable(int r_othello[Board_Size * Board_Size],
                  int turn, int row, int col)
{
    int num = 0;
    int row_inc, col_inc;
    int xx, yy; /* 8���������Ɋm�F���邽�߂̕ϐ� */

    if (!check_board(row, col)) {
        return FALSE;    /* �͈͊O */
    } else if (r_othello[row * Board_Size + col] != 0) {
        return FALSE;
    }

    /* �����ɐ΂͖��� */
    for (num = 0; num < Board_Turn; ++num) {
        Board_Directons(num, &xx, &yy);
        row_inc = xx;
        col_inc = yy;

        /*�@�ׂɑ���̐΂��u����Ă���ꍇ */
        if (check_board(row + row_inc, col + col_inc)) {
            if (r_othello[(row + row_inc)*Board_Size + col + col_inc] == -turn) {
                for (row_inc += xx, col_inc += yy;
                        check_board(row + row_inc, col + col_inc);
                        row_inc += xx, col_inc += yy) {

                    if (r_othello[(row + row_inc)*Board_Size + col + col_inc] == -turn) {
                        /*�@�܂��A�ׂ�����̐΂̏ꍇ */
                    } else if (r_othello[(row + row_inc)*Board_Size + col + col_inc] == turn) {
                        /*�@�����̐΂ő���̐΂�����ł���ꍇ�@*/
                        return TRUE;
                    } else {
                        break;
                    }
                }
            }
        }
    }

    return FALSE;
}

/************ �Q�[�����ʂ��v�Z����֐� ****************/

void game_result(int r_othello[Board_Size * Board_Size],
                 int *Sente_Point, int *Gote_Point)
{
    int col, row;
    *Sente_Point = 0;
    *Gote_Point = 0;

    for (row = 0; row < Board_Size; ++row) {
        for (col = 0; col < Board_Size; ++col) {
            if (r_othello[row * Board_Size + col] == Board_Sente) {
                ++*Sente_Point;  /*�@���̐΁@*/
            } else if (r_othello[row * Board_Size + col] == Board_Gote) {
                ++*Gote_Point;   /*�@���̐΁@*/
            }
        }
    }
}
