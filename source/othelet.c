/**
 * Copyright (c) 2015 Kazutaka Yasuda
 * Released under the MIT license
 * http://opensource.org/licenses/mit-license.php
 */
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>

#include "config.h"
#include "alphabeta.h"
#include "movement.h"
#include "jyouseki.h"

int othello[] = {
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, -1, 1, 0, 0, 0,
    0, 0, 0, 1, -1, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0
};

static int count = 1;	/* �@�萔��\���@ */

int get_count()
{
    return count;
}

void increase_count()
{
    count ++;
}

/********** �R�����g������֐��i���V�сj ***********/

const char *comment(char *tmp, int val, int turn, int flag)
{
    if (flag == 0) {

        if (val > 0)      {
            sprintf(tmp, "���� %d �Ώ���������", val / 100);
            return tmp;
        } else if (val < 0) {
            sprintf(tmp, "���� %d �Ε���������", -val / 100);
            return tmp;
        } else {
            return "���������ɂȂ錩����";
        }

    } else if (flag == 2) {

        return (val > 0) ? "���̏���������" :
               (val < 0) ? "���̕���������" :
               "���������ɂȂ錩����";

    } else {
        return (val > 50) ? "�u�����������܂����B�v" :
               (val > 25) ? "�u������c�c�肪������!!�v" :
               (val > 10) ? "�u���낻��{�C�����Ă���������`�v" :
               (val >  5) ? "�u�L���Ȃ񂾂ȁA���ꂪ�B�v" :
               (val >  2) ? "�u�I�ՂŔ҉�o����Ȃ�čl���ĂȂ��H�v" :
               (val < -50) ? "�u�s��̂悤�ł��B�v" :
               (val < -25) ? "�u�����A�_���Ȃ̂��c�H�v" :
               (val < -10) ? "�u�ނ��A���������ł��B�v" :
               (val < -5) ? "�u�s���ł��c�A�����I�v" :
               (val < -2) ? "�u�Ȃ��Ȃ��ǂ����ł��܂��˂��B�v" :
               "�u���̂Ƃ���͌݊p�ł��ˁB�v";
    }
}

void
print_board(int r_othello[Board_Size * Board_Size])
{
    int		row = 0,	col = 0;
    int		color;

    if (get_count() % 2 == 1) {
        color = 41;
    } else {
        color = 44;
    }

    printflush("\n");
    printflush("     \033[30;43m 0 \033[0m");
    printflush(" \033[30;43m 1 \033[0m");
    printflush(" \033[30;43m 2 \033[0m");
    printflush(" \033[30;43m 3 \033[0m");
    printflush(" \033[30;43m 4 \033[0m");
    printflush(" \033[30;43m 5 \033[0m");
    printflush(" \033[30;43m 6 \033[0m");
    printflush(" \033[30;43m 7 \033[0m\n");
    printflush("   \033[30;%dm����������������������������������\033[0m  \n ", color);

    for (row = 0; row < Board_Size; ++row) {
        printflush("\033[30;43m%d \033[0m", row);
        printflush("\033[30;%dm��\033[0m", color);

        for (col = 0; col < Board_Size; ++col) {
            if (r_othello[row * Board_Size + col] == Board_Gote) {
                printflush(WHITE);

                if (col != 7) {
                    printflush("\033[30;42m��\033[0m");
                } else {
                    printflush("\033[30;%dm��\033[0m", color);
                }
            } else if (r_othello[row * Board_Size + col] == Board_Sente) {
                printflush(BLACK);

                if (col != 7) {
                    printflush("\033[30;42m��\033[0m");
                } else {
                    printflush("\033[30;%dm��\033[0m", color);
                }
            } else {
                if (col != 7) {
                    printflush("\033[30;42m  ��\033[0m");
                } else {
                    printflush(BLANK);

                    if (col != 7) {
                        printflush("\033[30;42m��\033[0m");
                    } else {
                        printflush("\033[30;%dm��\033[0m", color);
                    }
                }
            }
        }

        printflush(" \n ");

        if (row != 7) {
            printflush("  \033[30;%dm��\033[0m", color);
            printflush("\033[30;42m������������������������������\033[0m");
            printflush("\033[30;%dm��\033[0m\n ", color);
        } else {
            printflush("  ");
            printflush("\033[30;%dm����������������������������������\033[0m", color);
            printflush("\n ");
        }
    }

    printflush("\n");
}

/****************** ���C���֐� ********************/
int main(void)
{
    int row, col, turn, myturn = 0, end_val, comturn;
    int depth = 6, alpha = -99999, beta = 99999, val = 0;
    int flag = 0; /* �ǉ� */

    // X�\���̏����E����
//  Othelet w("Othelet ver1.3", 13*40, 10*40+15);

    do {
        printflush("���^��� ? �i1/-1�j");
        scanf("%d", &myturn);
    } while (myturn == 0);

    comturn = 1;

    /* �v�l���[�`�� */
    for (turn = 1;; turn *= -1, comturn *= -1) {
        int state = board_movable_any(othello, turn);

        if (state == END) {
            break;
        } else if (state != TRUE) {
            printflush("(%2d) �p�X�ł�", get_count());
        } else {

            printflush(turn == Board_Sente ? "����" : "����");

            if (turn == myturn) {
                print_board(othello);

                do {
                    scanf("%d %d", &row, &col);
                } while (!board_movable(othello, turn, row, col));
            } else {
                char tmp[256] = {0};
                const char *co = "";

                if (get_count() >= 45) { /* ���S�ǂ݂��� */
                    end_val = alphabeta2(othello, &row, &col,
                                         turn, 20, alpha, beta, get_count());
                    co = comment(tmp, comturn * end_val, turn, 0);

                } else if (get_count() >= 44) { /* �K���ǂ݂��� beta<INT_MAX�� */
                    end_val = alphabeta2(othello, &row, &col,
                                         turn, 20, alpha, 1, get_count());
                    co = comment(tmp, comturn * end_val, turn, 2);

                } else if (flag != -1 && returnSeach(&row, &col, &flag, turn)) {

                } else if (get_count() >= 35) { /* 35��ȏ� */
                    end_val = alphabeta(othello, &row, &col,
                                        turn, depth + 1, alpha, beta, val, get_count());
                    co = comment(tmp, comturn * end_val, turn, 1);

                } else { /* ����ȊO */
                    end_val = alphabeta(othello, &row, &col,
                                        turn, depth, alpha, beta, val, get_count());
                    co = comment(tmp, comturn * end_val, turn, 1);
                }

                printflush("%s\n", co);
            }

            increase_count(); /*�@�肪�łĂ�΃J�E���g�𑝂₷�@*/
            board_move(turn, row, col, othello);
            printflush("(%2d) %d %d\n", get_count() - 1, col, row); // �����̑ł����ӏ�
        }
    }

    /************ �Q�[�����ʂ��v�Z���� ****************/
    {
        int sente, gote;
        char *c;
        game_result(othello, &sente, &gote);

        if ((sente - gote)*turn > 0) {
            c = "���Ȃ��̏����ł�!!";
        } else {
            c = "���Ȃ��̕����ł�..";
        }

        printflush("%s �Q�[�����ʂ� %d vs %d ", c, sente, gote);
    }
    return 0;
}
