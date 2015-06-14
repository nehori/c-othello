/**
 * Copyright (c) 2015 Kazutaka Yasuda
 * Released under the MIT license
 * http://opensource.org/licenses/mit-license.php
 */
/************* 定石検索プログラム ********************

  (4,5)f5→f5(5,4+1)     flag=0
  (2,3)d3→f5(5,4+1)     flag=3
  (3,2)c4→f5(5,4+1)     flag=2
  (5,4)e6→f5(5,4+1)     flag=1

  4手目で斜めをd6(5,3)→f4(5,3+1)に変更する（相似）
                   flag=4

******************************************************/

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

#include "jyouseki.h"

#define JYOUSEKI "book.dat"
#define LOGFILE  "log.dat"

/*************** 検索関数 *****************************/

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

/************** 英字→数字変換処理関数 ***************/

void eng_num(int *row, int *col, int *flag)
{
    int i;

    *row = *row - '1'; /* 48(ASCII)は「0」を示す */

    *col = *col - 1;  /* 1多いため */

    if (*flag & 2) { /* 7を引く */
        *row = 7 - *row;
        *col = 7 - *col;
    }

    if ((*flag & 1) == 0) { /* row,colを反転 */
        i = *row;
        *row = *col;
        *col = i;
    }
}

/************** 数字→英字変換処理関数 ***************/

void num_eng(char *row, char *col, int *flag)
{
    char i;

    if ((*flag & 1) == 0) { /* row,colを反転 */
        i = *row;
        *row = *col;
        *col = i;
    }

    if (*flag & 2) { /* 8を引く */
        *row = 103 - *row;
        *col = 103 - *col;
    }

    *row += 'a' - '0';
    *col = *col + 1;
}

/************* 3手目以降 ***********************/

int turn_Change(char *row, char *col, int *flag)
{
    FILE *fp, *lg;
    int i;
    char s[130], van[100];

    int length;

    for (i = 0; i < 100; ++i) {
        s[i]   = '\0'; /* 初期化 */
        van[i] = '\0'; /* 初期化 */
    }

    lg = fopen(LOGFILE, "r"); /* 既存ファイル読みだし処理 */
    fgets(van , 100, lg);
    fclose(lg);

    lg = fopen(LOGFILE, "a"); /* 追加書き込み処理 */

    length = strlen(van); /* 打った手の長さ取得 */

    /* 検索開始 */

    fp = fopen(JYOUSEKI, "r");

    if (fp == NULL) {
        printflush("File Open Error\n");
        *flag = -1; /* 定石を用意していなかった */
        return (0);
    }

    for (i = 0; fgets(s, 130, fp) != NULL; ++i) {
        if (SearchSt(s, van) != 0 && s[length + 4] >= 'a' && s[length + 4] <= 'h') {

            /* 手が負であれば定石としてみなさない */
            if (get_count() % 2 == 1 && s[0] == '-')      ;
            else if (get_count() % 2 == 0 && s[0] == '+') ;
            else {
                /* 次の手を送る最初4バイト邪魔 */
                *row = s[length + 4];
                *col = s[length + 1 + 4];
                fprintf(lg , "%c%c", s[length + 4], s[length + 1 + 4]);
                printflush("検索番号:%d 評価値: %c%c%c ", i, s[0], s[1], s[2]);
                /* 見つかったので終了 */
                fclose(fp);
                fclose(lg);
                return (1);
            }
        }
    }

    printflush("定石は終了しました ..");
    *flag = -1; /* もう定石は存在しない */
    fclose(fp);
    fclose(lg);
    return (0);

}

/*************** 先手が行う処理関数 ****************/

int turn_Sente(int *row, int *col, int *flag)
{
    FILE *lg;
    char inc_row, inc_col;

    if (get_count() == 1) { /* 第1手目は f5 に打つ */

        *row = 4, *col = 5; /* f5 */

        lg = fopen(LOGFILE, "w"); /* 書き込み処理+既存ファイル削除 */
        fprintf(lg, "f5");  /* fputs("f5", lg); */
        *flag = 0;
        fclose(lg);
        return (1);

    } else { /* 3手目以降の場合 */

        /* まず、ここで相手のint row,colを変換する必要あり */
        inc_row = (char)(*row + 48);
        inc_col = (char)(*col + 48);

        if (get_count() == 5 && inc_row == '5' && inc_col == '3') {
            *flag = 4; /* 追加処理（相似形を変更） */
        }

        num_eng(&inc_row, &inc_col, flag);

        /* 相手の手をファイルに保存する必要あり */

        lg = fopen(LOGFILE, "a");
        fprintf(lg , "%c%c", inc_row, inc_col);
        fclose(lg);

        if (turn_Change(&inc_row, &inc_col, flag)) {

            /* 1であればchar inc_row,inc_colの変換処理 */
            *row = (int)(inc_row - 48);
            *col = (int)(inc_col - 48);
            eng_num(row, col, flag);
            return (1);
        } else {
            return (0);
        }
    }

}

/************** 後手が行う処理関数 *****************/

int turn_Gote(int *row, int *col, int *flag)
{
    FILE *lg;
    char inc_row, inc_col;

    if (get_count() == 2) { /* 相手の第1手目を f5に変換 */

        if     (*row == 4 && *col == 5) {
            *flag = 0;
        } else if (*row == 5 && *col == 4) {
            *flag = 1;
        } else if (*row == 3 && *col == 2) {
            *flag = 2;
        } else { /*  (*row=2 && *col == 3) */
            *flag = 3;
        }

        /* ランダムに縦、斜め取りを決める */
        srand(time(0));  /* seedをtimeで作る */

        if (rand() % 2 == 0) {
            inc_row = 'f';
            inc_col = '6';
        } else {
            inc_row = 'd';
            inc_col = '6';
        }

        lg = fopen(LOGFILE, "w"); /* 書き込み処理+既存ファイル削除 */
        fprintf(lg, "f5%c%c", inc_row, inc_col);
        fclose(lg);

        /* inc_row,inc_colの変換処理 */
        *row = (int)(inc_row - 48);
        *col = (int)(inc_col - 48);
        eng_num(row, col, flag);
        return (1);

    } else {   /* 4手目以降 */

        /* こっちはflag処理が必要 */
        /* まず、ここで相手のint row,colを変換する必要あり */
        inc_row = (char)(*row + 48);
        inc_col = (char)(*col + 48);

        num_eng(&inc_row, &inc_col, flag);

        /* 相手の手をファイルに保存する必要あり */
        lg = fopen(LOGFILE, "a");
        fprintf(lg , "%c%c", inc_row, inc_col);
        fclose(lg);

        if (turn_Change(&inc_row, &inc_col, flag)) {

            /* 1であれば、inc_row,inc_colの変換処理 */
            *row = (int)(inc_row - 48);
            *col = (int)(inc_col - 48);
            eng_num(row, col, flag);
            return (1);
        } else {
            return (0);
        }

    }
}

/********* 手数や順番等の変更を担う関数 ************/

int returnSeach(int *row, int *col, int *flag, int turn)
{
    if (turn == 1) {
        return turn_Sente(row, col, flag);    /* 自分が先手の場合 */
    } else {
        return turn_Gote(row, col, flag);    /* 自分が後手の場合 */
    }
}
