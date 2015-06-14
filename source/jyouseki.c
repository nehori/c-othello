/**
 * Copyright (c) 2015 Kazutaka Yasuda
 * Released under the MIT license
 * http://opensource.org/licenses/mit-license.php
 */
/************* ��Ό����v���O���� ********************

  (4,5)f5��f5(5,4+1)     flag=0
  (2,3)d3��f5(5,4+1)     flag=3
  (3,2)c4��f5(5,4+1)     flag=2
  (5,4)e6��f5(5,4+1)     flag=1

  4��ڂŎ΂߂�d6(5,3)��f4(5,3+1)�ɕύX����i�����j
                   flag=4

******************************************************/

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

#include "jyouseki.h"

#define JYOUSEKI "book.dat"
#define LOGFILE  "log.dat"

/*************** �����֐� *****************************/

int SearchSt(char *s, char *t)
{
    int n;
    char *p;

    n = strlen(t);

    if ((p = strchr(s, t[0])) != NULL)
        if (strncmp(t, p, n) == 0) {
            return (1);
        }

    return (0);
}

/************** �p���������ϊ������֐� ***************/

void eng_num(int *row, int *col, int *flag)
{
    int i;

    *row = *row - '1'; /* 48(ASCII)�́u0�v������ */

    *col = *col - 1;  /* 1�������� */

    if (*flag & 2) { /* 7������ */
        *row = 7 - *row;
        *col = 7 - *col;
    }

    if ((*flag & 1) == 0) { /* row,col�𔽓] */
        i = *row;
        *row = *col;
        *col = i;
    }
}

/************** �������p���ϊ������֐� ***************/

void num_eng(char *row, char *col, int *flag)
{
    char i;

    if ((*flag & 1) == 0) { /* row,col�𔽓] */
        i = *row;
        *row = *col;
        *col = i;
    }

    if (*flag & 2) { /* 8������ */
        *row = 103 - *row;
        *col = 103 - *col;
    }

    *row += 'a' - '0';
    *col = *col + 1;
}

/************* 3��ڈȍ~ ***********************/

int turn_Change(char *row, char *col, int *flag)
{
    FILE *fp, *lg;
    int i;
    char s[130], van[100];

    int length;

    for (i = 0; i < 100; ++i) {
        s[i]   = '\0'; /* ������ */
        van[i] = '\0'; /* ������ */
    }

    lg = fopen(LOGFILE, "r"); /* �����t�@�C���ǂ݂������� */
    fgets(van , 100, lg);
    fclose(lg);

    lg = fopen(LOGFILE, "a"); /* �ǉ��������ݏ��� */

    length = strlen(van); /* �ł�����̒����擾 */

    /* �����J�n */

    fp = fopen(JYOUSEKI, "r");

    if (fp == NULL) {
        printflush("File Open Error\n");
        *flag = -1; /* ��΂�p�ӂ��Ă��Ȃ����� */
        return (0);
    }

    for (i = 0; fgets(s, 130, fp) != NULL; ++i) {
        if (SearchSt(s, van) != 0 && s[length + 4] >= 'a' && s[length + 4] <= 'h') {

            /* �肪���ł���Β�΂Ƃ��Ă݂Ȃ��Ȃ� */
            if (get_count() % 2 == 1 && s[0] == '-')      ;
            else if (get_count() % 2 == 0 && s[0] == '+') ;
            else {
                /* ���̎�𑗂�ŏ�4�o�C�g�ז� */
                *row = s[length + 4];
                *col = s[length + 1 + 4];
                fprintf(lg , "%c%c", s[length + 4], s[length + 1 + 4]);
                printflush("�����ԍ�:%d �]���l: %c%c%c ", i, s[0], s[1], s[2]);
                /* ���������̂ŏI�� */
                fclose(fp);
                fclose(lg);
                return (1);
            }
        }
    }

    printflush("��΂͏I�����܂��� ..");
    *flag = -1; /* ������΂͑��݂��Ȃ� */
    fclose(fp);
    fclose(lg);
    return (0);

}

/*************** ��肪�s�������֐� ****************/

int turn_Sente(int *row, int *col, int *flag)
{
    FILE *lg;
    char inc_row, inc_col;

    if (get_count() == 1) { /* ��1��ڂ� f5 �ɑł� */

        *row = 4, *col = 5; /* f5 */

        lg = fopen(LOGFILE, "w"); /* �������ݏ���+�����t�@�C���폜 */
        fprintf(lg, "f5");  /* fputs("f5", lg); */
        *flag = 0;
        fclose(lg);
        return (1);

    } else { /* 3��ڈȍ~�̏ꍇ */

        /* �܂��A�����ő����int row,col��ϊ�����K�v���� */
        inc_row = (char)(*row + 48);
        inc_col = (char)(*col + 48);

        if (get_count() == 5 && inc_row == '5' && inc_col == '3') {
            *flag = 4; /* �ǉ������i�����`��ύX�j */
        }

        num_eng(&inc_row, &inc_col, flag);

        /* ����̎���t�@�C���ɕۑ�����K�v���� */

        lg = fopen(LOGFILE, "a");
        fprintf(lg , "%c%c", inc_row, inc_col);
        fclose(lg);

        if (turn_Change(&inc_row, &inc_col, flag)) {

            /* 1�ł����char inc_row,inc_col�̕ϊ����� */
            *row = (int)(inc_row - 48);
            *col = (int)(inc_col - 48);
            eng_num(row, col, flag);
            return (1);
        } else {
            return (0);
        }
    }

}

/************** ��肪�s�������֐� *****************/

int turn_Gote(int *row, int *col, int *flag)
{
    FILE *lg;
    char inc_row, inc_col;

    if (get_count() == 2) { /* ����̑�1��ڂ� f5�ɕϊ� */

        if     (*row == 4 && *col == 5) {
            *flag = 0;
        } else if (*row == 5 && *col == 4) {
            *flag = 1;
        } else if (*row == 3 && *col == 2) {
            *flag = 2;
        } else { /*  (*row=2 && *col == 3) */
            *flag = 3;
        }

        /* �����_���ɏc�A�΂ߎ������߂� */
        srand(time(0));  /* seed��time�ō�� */

        if (rand() % 2 == 0) {
            inc_row = 'f';
            inc_col = '6';
        } else {
            inc_row = 'd';
            inc_col = '6';
        }

        lg = fopen(LOGFILE, "w"); /* �������ݏ���+�����t�@�C���폜 */
        fprintf(lg, "f5%c%c", inc_row, inc_col);
        fclose(lg);

        /* inc_row,inc_col�̕ϊ����� */
        *row = (int)(inc_row - 48);
        *col = (int)(inc_col - 48);
        eng_num(row, col, flag);
        return (1);

    } else {   /* 4��ڈȍ~ */

        /* ��������flag�������K�v */
        /* �܂��A�����ő����int row,col��ϊ�����K�v���� */
        inc_row = (char)(*row + 48);
        inc_col = (char)(*col + 48);

        num_eng(&inc_row, &inc_col, flag);

        /* ����̎���t�@�C���ɕۑ�����K�v���� */
        lg = fopen(LOGFILE, "a");
        fprintf(lg , "%c%c", inc_row, inc_col);
        fclose(lg);

        if (turn_Change(&inc_row, &inc_col, flag)) {

            /* 1�ł���΁Ainc_row,inc_col�̕ϊ����� */
            *row = (int)(inc_row - 48);
            *col = (int)(inc_col - 48);
            eng_num(row, col, flag);
            return (1);
        } else {
            return (0);
        }

    }
}

/********* �萔�⏇�ԓ��̕ύX��S���֐� ************/

int returnSeach(int *row, int *col, int *flag, int turn)
{
    if (turn == 1) {
        return turn_Sente(row, col, flag);    /* ���������̏ꍇ */
    } else {
        return turn_Gote(row, col, flag);    /* ���������̏ꍇ */
    }
}
