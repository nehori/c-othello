/**
 * Copyright (c) 2015 Kazutaka Yasuda
 * Released under the MIT license
 * http://opensource.org/licenses/mit-license.php
 */
#include <stdio.h>
#include <limits.h>

#include "movement.h"
#include "eval.h"

/********** �΂̎���̋󔒐������߂�֐� ************/

int Board_Empty(int row, int col, int r_othello[Board_Size * Board_Size])
{
    int num, xx = 0, yy = 0;
    int Point = 0;

    for (num = 0; num < Board_Turn; ++num) {
        Board_Directons(num, &xx, &yy);

        if (0 <= row + xx && row + xx <= Board_Size - 1 &&
                0 <= col + yy && col + yy <= Board_Size - 1)  {

            /*�@�΂̎���̋󔒂̐������� */
            if (r_othello[(row + xx)*Board_Size + col + yy] == 0) {
                ++Point;
            }
        }
    }

    return (Point);
}

/*********** �R���s���[�^�̎v�l�֐� *****************/
/*                                                  */
/*           �萔�i�����j�Ɋ�Â��v�l             */
/*                                                  */
/****************************************************/

int AI_Tyakusyu(int r_othello[Board_Size * Board_Size])
{
    int col, row;
    int eval = 2;    /* �����̕]���l */
    int S_Point = 0, G_Point = 0;

    for (row = 0; row < Board_Size; ++row) {
        for (col = 0; col < Board_Size; ++col) {
            if ((row == 1 && col == 1) || (row == 1 && col == 6) ||
                    (row == 6 && col == 1) || (row == 6 && col == 6)) {
                /* ����ƌ����Ă�X�ł��̌��͔r�� */
            } else {
                if (board_movable(r_othello, Board_Sente, row, col)) {
                    ++S_Point;
                }

                if (board_movable(r_othello, Board_Gote, row, col)) {
                    ++G_Point;
                }
            }
        }
    }

    return ((S_Point - G_Point) * eval);
}

int AI_Tyakusyu2(int r_othello[Board_Size * Board_Size], int turn)
{
    int col, row;
    int Point = 0;

    for (row = 0; row < Board_Size; ++row) {
        for (col = 0; col < Board_Size; ++col) {
            if (board_movable(r_othello, turn, row, col)) {
                ++Point;
            }
        }
    }

    return Point;
}

/*********** �R���s���[�^�̎v�l�֐� *****************/
/*                                                  */
/*        ���W�I�J���x���_�Ɋ�Â��v�l              */
/*                                                  */
/****************************************************/

int AI_Kaihoudo(int r_othello[Board_Size * Board_Size],
                int rr_othello[Board_Size * Board_Size], int row, int col)
{
    int Point = 0;
    int rrr_othello[Board_Size * Board_Size];

    first(rr_othello, rrr_othello);    /* ������ */

    /* �ł����Ƃ����߂� */
    rrr_othello[row * Board_Size + col] = 0;

    for (row = 0; row < Board_Size; ++row) {
        for (col = 0; col < Board_Size; ++col) {
            if (rrr_othello[row * Board_Size + col] != r_othello[row * Board_Size + col]) {
                Point += Board_Empty(row, col, rrr_othello);
            }
        }
    }

    return (Point);
}

/********** �ӁA�m��΂̔��������֐� *************/
/*                                                 */
/*                ��2��    �c2��                   */
/*                                                 */
/***************************************************/

#define ro(i,j) r_othello[ (i)*dx+(j)*dy ]
int AI_Hen(int r_othello[Board_Size * Board_Size], int dx, int dy, int row, int r1)
{
    int i, Point = 0, order = 0;
    int eval = 5; /* �m��΂̕]���l     */
    int yama = 2; /* �R�̕]���l         */
    int wing = -2; /* ���̕]���l         */
    int Cuti = -3; /* �P��C�ł��̕]���l  */
    int Hen;

    /* ����ŋ������邩���� */
    if (ro(row, 0) != 0 || ro(row, 7) != 0) {
        if (ro(row, 0) != 0) {
            if (ro(row, 7) != 0) {
                int k = 0;

                /* �������ɐΗL�� */
                for (i = 0; i <= 7; ++i) {
                    if (ro(row, i) == 0) {
                        k = 0;
                        break;
                    } else {
                        k += ro(row, i) * eval;
                    }
                }

                Point += k;
            } else {
                /* ����̂ݐΗL�� */
                for (i = 0; i <= 7; ++i) {
                    if (ro(row, i) != ro(row, 0)) {
                        break;
                    }

                    Point += ro(row, i) * eval;
                }
            }
        } else {
            /* ��������̂ݐΗL�� */
            for (i = 7; i >= 0; --i) {
                if (ro(row, i) != ro(row, 7)) {
                    break;
                }

                Point += ro(row, i) * eval;
            }
        }
    } else {
        Hen = 0; /* ������ */

        if (ro(row, 1) != 0 || ro(row, 6) != 0) {

            /* �R�A������A�P��C�ł����� */

            if (ro(row, 1) != 0 &&        ro(row, 2) == 0 &&   ro(row, 3) == 0) {
                Point += ro(row, 1) * Cuti;
            }

            if (ro(row, 6) != 0 &&   ro(row, 5) == 0 &&  ro(row, 4) == 0) {
                Point += ro(row, 6) * Cuti;
            }  else {

                for (i = 0; i <= 7; ++i) {
                    Hen += ro(row, i);
                }

                if (Hen == 6) {
                    Point += yama;
                } else if (Hen == -6) {
                    Point -= yama;
                } else if (Hen == 5 || Hen == -5) {

                    if (Hen == 5) {
                        order = Board_Sente;
                    } else {
                        order = Board_Gote;
                    }

                    if ( ((ro(row, 1) == order &&
                            ro(r1 , 5) == order) ||
                            (ro(row, 6) == order &&
                             ro(r1 , 2) == order))) {
                        Point += wing * order;
                    }

                }
            }
        }
    }

    return (Point);
}

int AI_Hen1(int r_othello[Board_Size * Board_Size])
{
    return AI_Hen(r_othello, Board_Size, 1, 7, 6) + AI_Hen(r_othello, Board_Size, 1, 0, 1);
}

int AI_Hen2(int r_othello[Board_Size * Board_Size])
{
    return AI_Hen(r_othello, 1, Board_Size, 7, 6) + AI_Hen(r_othello, 1, Board_Size, 0, 1);
}

/************** �΍��𔻒肷��֐� *****************/

int eval_storn(int r_othello[Board_Size * Board_Size])
{
    int i, Point = 0;

    for (i = 0; i < Board_Size * Board_Size; ++i) {
        Point += r_othello[i];
    }

    if (Point > 0) {
        return (Board_Sente);
    } else if (Point < 0) {
        return (Board_Gote);
    } else {
        return (0);
    }
}

/***** �]���l��Ԃ��֐��i�v�l�����j*****************/

int eval_node(int r_othello[Board_Size * Board_Size], int rr_othello[Board_Size * Board_Size], int row,
              int col, int turn, int counter)
{
    int X_uti1 = -100, X_uti2 = -5, X_uti3 = -2;
    /* 1�`25���X�ł���100%���s���� */

    int Point = 0;

    if ((row == 1 && col == 1) || (row == 1 && col == 6) ||
            (row == 6 && col == 1) || (row == 6 && col == 6)) {
        if (get_count() <= 25) {
            Point += rr_othello[row * Board_Size + col] * X_uti1;
        } else if (get_count() <= 35) {
            Point += rr_othello[row * Board_Size + col] * X_uti2;
        } else {
            Point += rr_othello[row * Board_Size + col] * X_uti3;
        }
    }

    /* �J���x���_�Ɋ�Â��v�l�֐��ցi�d�l�Q�j */
    Point += turn * AI_Kaihoudo(r_othello, rr_othello, row, col);

    return (Point);
}

/***************** �]���֐��̓��e **********************

 ��΂���ǂݏo���A��΂��Ȃ��Ȃ������_�Œ��Ղɓ��� 9/ 6

 �J���x�ɂ��A��ʐ���܂ł̐΂𔻒�B�ŗǗD��T���ōs��
 �i����ɂ��A�����肪�r�����ꂽ�`�ɂȂ茋�ʋ����Ȃ�j9/17

 * ����肪�����葽���ꍇ�͔��W�I�J���x�ŕ]���l�𓾂�        *
 *�i�[���������ɂȂ�悤�ɁB����̔Ԃ̌v�Z�Ő��x���オ��j9/16 *

 �m��΂������Ȃ�悤�ɑł��A�Ō�͊��S�ǂ݂�����s�� 9/12

 �P��C�ł��͕]���l�������A�R�́{����Ƃ��Ă��� 9/12

 �w�ł��͕]���l�������AWing�́|����Ƃ��Ă��� 10/ 2

 * 30��ڒ��x����J���x���őP��Ŗ����Ȃ�̂ŁA�����œǂ� 9/16  *
 * 30��ڒ��x�܂ł͐΂̐����z���ɓ���ĕ]���l���Z�o���� 10/ 6     *

 �m��΁�����聄�J���x���ӂ̏d�݁@�̏��ɕύX�A�ΐ��r��  11/ 4

 35��ڈȍ~�͓ǂ݂������̂�1��[���ǂ� 9/16

 * 46��ڈȍ~�̊��S�ǂ݂���͑�������ׁA�ʂ̊֐���p�ӂ��� 8/22 *

 44��ȍ~�K���ǂ݂���i�ʊ֐������j 2002/11/12

 45��ȍ~���S�ǂ݂���i�ʊ֐������j 2002/11/12

*******************************************************/
