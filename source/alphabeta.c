/**
 * Copyright (c) 2015 Kazutaka Yasuda
 * Released under the MIT license
 * http://opensource.org/licenses/mit-license.php
 */
#include <stdio.h>
#include <limits.h>

#include "movement.h"
#include "eval.h"
#include "alphabeta.h"

/******** �ŏI�ǂ݂��肪�r���ŋN�����ꍇ ************/

int Finish_Board(int r_othello[Board_Size * Board_Size])
{
    /* ���̊֐��Ŏ��̎����ʂɑł��A���肷�� */
    int Sente_Point = 0 , Gote_Point = 0;

    game_result(r_othello, &Sente_Point, &Gote_Point);

    /* ���_�̏ꍇ�i�S�������̎��͂��ꂪ�ǂ��l�Ɂj */
    /* ���̏��� && ���̏����i100�|����͈̂Ӗ������j */
    return ( Sente_Point - Gote_Point) * 100;
}

/**************�@�p�X�𔻒肷��֐��@******************/

int board_movable_any(int r_othello[Board_Size * Board_Size], int turn)
{
    int col, row;
    int r_turn;

    for (row = 0; row < Board_Size; ++row) {
        for (col = 0; col < Board_Size; ++col) {
            if (r_othello[row * Board_Size + col] == 0) {
                /*�@�ǂ�����΂�u���Ă��Ȃ��ꍇ�@*/
                if (board_movable(r_othello, turn, row, col)) {
                    /* �����͒u�����Ƃ��ł���ꍇ */
                    return (TRUE);
                }
            }
        }
    }

    /* �����̓p�X�̏ꍇ */

    for (row = 0; row < Board_Size; ++row) {
        for (col = 0; col < Board_Size; ++col) {
            if (r_othello[row * Board_Size + col] == 0) {

                /*�@�ǂ�����΂�u���Ă��Ȃ��ꍇ�@*/
                r_turn = turn * (-1);

                if (board_movable(r_othello, r_turn, row, col)) {
                    /* ����͒u�����Ƃ��ł���ꍇ */
                    return (PASS);
                }
            }
        }
    }

    /* �ǂ�����łĂȂ� */
    return (END);
}

void sort(int child_count, int Point[30], int ind[30])
{
    int i, j, dummy;

    for (i = 0; i < child_count; ++i) {
        ind[i] = i;
    }

    for (i = 0; i < child_count - 1; ++i) {
        for (j = i + 1; j < child_count; ++j) {
            if (Point[ind[j]] > Point[ind[i]]) {
                dummy = ind[j];
                ind[j] = ind[i];
                ind[i] = dummy;
            }
        }
    }
}

/*** �q�m�[�h��S�ċ��߂�֐��i�����[���@�AMPC�j ***/

int expand_node2(int r_othello[Board_Size * Board_Size],
                 int turn, int array_row[30], int array_col[30],
                 int rrr_othello[][Board_Size * Board_Size], int ind[30])
{
    int inc_row, inc_col, i;
    int Point[30];
    int val = -6; /* �J�b�g����]������ */
    int child_count = 0;

    for (inc_row = 0; inc_row <= Board_Size; ++inc_row) {
        for (inc_col = 0; inc_col <= Board_Size; ++inc_col) {
            if (board_movable(r_othello, turn, inc_row, inc_col)) {
                int *rr_othello = rrr_othello[child_count];

                first(r_othello, rr_othello);    /* ������ */
                board_move(turn, inc_row, inc_col, rr_othello);

                /* �����Ƃ����̐������l */
                if ((inc_row == 0 && inc_col == 0) || (inc_row == 0 && inc_col == 7) ||
                        (inc_row == 7 && inc_col == 0) || (inc_row == 7 && inc_col == 7)) {
                    array_row[child_count] = inc_row;
                    array_col[child_count] = inc_col;
                    Point[child_count] = 1;
                    ++(child_count);
                } else if (inc_row == 0 || inc_col == 0 || inc_row == 7 || inc_col == 7) {
                    /* �Ō�ɕt������ */
                    array_row[child_count] = inc_row;
                    array_col[child_count] = inc_col;
                    Point[child_count] = val + 1;
                    ++child_count;
                } else {
                    array_row[child_count] = inc_row;
                    array_col[child_count] = inc_col;
                    Point[child_count] = (-1) * AI_Kaihoudo(r_othello, rr_othello, inc_row, inc_col);
                    ++(child_count);
                }
            }
        }
    }

    sort(child_count, Point, ind);

    /* ���O��̎�͎n�߂��珜�O�i���キ�Ȃ邩���j */
    if (child_count >= 10) {
        for (i = child_count - 1; i >= 0; --i) {
            if (Point[ind[i]] > val) {
                child_count = i + 1;
                break;
            }
        }
    }

    return child_count;
}

/*** �q�m�[�h��S�ċ��߂�֐��i�����[���@�j ***/

int expand_node(int r_othello[Board_Size * Board_Size],
                int turn, int array_row[30], int array_col[30],
                int rrr_othello[][Board_Size * Board_Size], int ind[30])
{
    int inc_row, inc_col;
    int Point[30];
    int child_count = 0;

    for (inc_row = 0; inc_row <= Board_Size; ++inc_row) {
        for (inc_col = 0; inc_col <= Board_Size; ++inc_col) {
            if (board_movable(r_othello, turn, inc_row, inc_col)) {
                int *rr_othello = rrr_othello[child_count];

                first(r_othello, rr_othello);    /* ������ */
                board_move(turn, inc_row, inc_col, rr_othello);
                /* �ł����Ƃ����߂� */

                array_row[child_count] = inc_row;
                array_col[child_count] = inc_col;
                Point[child_count] = (-1) * AI_Kaihoudo(r_othello, rr_othello, inc_row, inc_col);
                ++(child_count);
            }
        }
    }

    sort(child_count, Point, ind);
    return child_count;
}

/*** �q�m�[�h��S�ċ��߂�֐��i�����[���@�j ***/

int expand_node3(int r_othello[Board_Size * Board_Size],
                 int turn, int array_row[30], int array_col[30],
                 int rrr_othello[][Board_Size * Board_Size], int ind[30])
{
    int inc_row, inc_col;
    int Point[30];
    int child_count = 0;

    for (inc_row = 0; inc_row <= Board_Size; ++inc_row) {
        for (inc_col = 0; inc_col <= Board_Size; ++inc_col) {
            if (board_movable(r_othello, turn, inc_row, inc_col)) {
                int *rr_othello = rrr_othello[child_count];

                first(r_othello, rr_othello);    /* ������ */
                board_move(turn, inc_row, inc_col, rr_othello);
                array_row[child_count] = inc_row;
                array_col[child_count] = inc_col;
                Point[child_count] = (-1) * AI_Tyakusyu2(rr_othello, -turn);
                ++(child_count);
            }
        }
    }

    sort(child_count, Point, ind);
    return child_count;
}

/******** �����@�ɂ��]���l�̌��o�֐� *************/

int alphabeta(int r_othello[Board_Size * Board_Size], int *row, int *col,
              int turn, int depth, int alpha, int beta, int val, int counter)
{
    int select;
    int array_row[60], array_col[60], ind[60], child_count; /* �m�[�h�̐� */
    int _i;
    int rrr_othello[60][Board_Size * Board_Size];

    if (depth <= 0) {/* �ŉ��m�[�h������ */
        /* ���W�I�J���x�{�����{�ӂ̏d�݂P�{�ӂ̏d�݂Q */
        return val + AI_Tyakusyu(r_othello)
               + AI_Hen1(r_othello) + AI_Hen2(r_othello);
    }

    /* ���݂̋ǖʂ���1��i�߂���Ԃ�a1,a2,a3�Ƃ��� */
    for (_i = 0;
            (child_count = (get_count() > 20)
                           ? expand_node (r_othello, turn, array_row, array_col, rrr_othello, ind)
                           : expand_node2(r_othello, turn, array_row, array_col, rrr_othello, ind)) == 0;
            _i++)  {
        if (_i) {
            return Finish_Board(r_othello);
        } else {
            turn = -turn, _i = -alpha, alpha = -beta, beta = _i;
        }
    }

    select = 0;   /* �ꉞ�����ݒ���s���Ă��� */

    /* alpha < beta && i < child_count�̎��͌J��Ԃ� */
    int b = beta;

    for (_i = 0; (alpha < beta) && (_i < child_count); ++_i) {
        int i = ind[_i], *rr_othello = rrr_othello[i];
        int a, inc_row, inc_col, end_val;  /* �ŏI�]���l */

        inc_row = array_row[i];
        inc_col = array_col[i];

        a = eval_node(r_othello, rr_othello, inc_row, inc_col, turn * (-1), counter);

        end_val = turn * alphabeta(rr_othello, &inc_row, &inc_col, -turn,
                                   depth - 1, -b, -alpha, val + a, counter + 1);

        if (alpha < end_val && end_val <= b && _i) {
            end_val = turn * alphabeta(rr_othello, &inc_row, &inc_col, -turn,
                                       depth - 1, -beta, -end_val, val + a, counter + 1);
        }

        if (end_val > alpha) {
            alpha = end_val;  /* ���J�b�g */
            select = i;
        }

        b = alpha + 1;
    }

    *row = array_row[select];
    *col = array_col[select];

    return (turn * alpha);
}

/******** �����@�ɂ�銮�S�ǂ݂̌��o�֐� *************/

int alphabeta2(int r_othello[Board_Size * Board_Size], int *row, int *col,
               int turn, int depth, int alpha, int beta, int counter)
{
    int select;
    int array_row[30], array_col[30], ind[30], child_count; /* �m�[�h�̐� */
    int rrr_othello[30][Board_Size * Board_Size];
    int _i;

    /* ���݂̋ǖʂ���1��i�߂���Ԃ�a1,a2,a3�Ƃ��� */
    for (_i = 0; !(child_count =
                       expand_node3(r_othello, turn, array_row, array_col, rrr_othello, ind)); _i++)
        if (_i) {
            return Finish_Board(r_othello);
        } else {
            turn = -turn, _i = -alpha, alpha = -beta, beta = _i;
        }

    /* �ŏ��͍s��̒l�������ĂȂ��̂ł��̏����͂���Ȃ� */

    select = 0;   /* �ꉞ�����ݒ���s���Ă��� */

    /* alpha < beta && i < child_count�̎��͌J��Ԃ� */
    int b = beta;

    for (_i = 0; (alpha < beta) && (_i < child_count); ++_i) {
        int i = ind[_i], *rr_othello = rrr_othello[i];
        int inc_row, inc_col, end_val;  /* �ŏI�]���l */

        end_val = turn * alphabeta2(rr_othello, &inc_row, &inc_col, -turn,
                                    depth - 1, -b, -alpha, counter + 1);

        if (alpha < end_val && end_val <= b && _i) {
            end_val = turn * alphabeta2(rr_othello, &inc_row, &inc_col, -turn,
                                        depth - 1, -beta, -end_val, counter + 1);
        }

        if (end_val > alpha) {
            alpha = end_val;  /* ���J�b�g */
            select = i;
        }

        b = alpha + 1;
    }

    *row = array_row[select];
    *col = array_col[select];

    return (turn * alpha);
}

/***************** �X�V���� ****************************

 2002/11/12  NegaMax�@��NegaScout�@�ɕύX�A�ǂݐ؂荂����

 2002/11/11  �����֐���NegaMax�@�ɕύX�A�v���O�����œK��

 2002/11/08  �o�O�C���A�����@�̕K���ǂݐ؂�A�m��Ε]��

 2002/11/05  ��΃t�@�C��3��������t�@�C��3���ɕύX

 2001/11/16  �I�Z�����{�I�D���B�I�[�v���\�[�X�ɕύX�i���i�j

 2001/11/ 4  �v���؂��āA�]���֐��r���B�ӏ����ȊO�őP��B

 2001/11/ 2  �I�Z�����\�I�˔j�A����NAOYA�ɏ��Ă��B

 2001/10/13  NAOYA�ɕ�����B���R�����炸�B

 2001/10/10  �K���ǂ݂���ǉ��\��B�����t�@�C�����v�撆�B

 2001/10/ 6  �΂̐��̍���ǉ��B���x���͂قڌ݊p�ɂ��ǂ�i1���j

 2001/10/ 2  �m��΁A�ӂ̕]���l�������A���̔���ύX

 2001/ 9/25  NAOYA�ɕ����āA�A���S���Y����|�B���x��������

 2001/ 9/17  �����[���@��S�ĂɓK�p�B�������e�i�ɏオ��

 2001/ 9/17  Thell��j��B1���z��ɕύX�A�����C���Ȃ炸

 2001/ 9/16  �萔�ŏꍇ���������ǉ��AThell�Ƃقڌ݊p�B�i2���j

 2001/ 9/12  �萔�A�ӏ�����ǉ��A�C���B����Thell��j��

 2001/ 9/10  �����@��PASS����C���A���S�ǂ݂��蒼��i3���j

 2001/ 9/ 6  YonYon����ɏC���ӏ��̎w�E�����炤�A��Βǉ�

 2001/ 8/29  ���S�ǂ݂���o�O�����A���R�����m�ꂸ�c

 2001/ 8/25  �����@���C���APASS����o�O�������

 2001/ 8/23  �v���O�������̕K�v�����֐����̔r�����s��

 2001/ 8/22  �����@���C���A�����@�ɂ��I�Z�����\�ɂȂ�i4���j

 2001/ 8/20  RedHat Linux�𓱓����AC����J����������

 2001/ 8/16  YonYon����Ƀ����@���C�����Ă��炤

 2001/ 8/10  �����@�ɂ��A���S���Y���̒ǉ������݂�

 2001/ 7/ 8  C����ŃR���s���[�^�ΐ�I�Z���Q�[�����i���S�ҁj

*******************************************************/
