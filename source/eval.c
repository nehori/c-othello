/**
 * Copyright (c) 2015 Kazutaka Yasuda
 * Released under the MIT license
 * http://opensource.org/licenses/mit-license.php
 */
#include <stdio.h>
#include <limits.h>

#include "movement.h"
#include "eval.h"

/********** 石の周りの空白数を求める関数 ************/

int Board_Empty(int row, int col, int r_othello[Board_Size * Board_Size])
{
    int num, xx = 0, yy = 0;
    int Point = 0;

    for (num = 0; num < Board_Turn; ++num) {
        Board_Directons(num, &xx, &yy);

        if (0 <= row + xx && row + xx <= Board_Size - 1 &&
                0 <= col + yy && col + yy <= Board_Size - 1)  {

            /*　石の周りの空白の数を検査 */
            if (r_othello[(row + xx)*Board_Size + col + yy] == 0) {
                ++Point;
            }
        }
    }

    return (Point);
}

/*********** コンピュータの思考関数 *****************/
/*                                                  */
/*           手数（着手手）に基づく思考             */
/*                                                  */
/****************************************************/

int AI_Tyakusyu(int r_othello[Board_Size * Board_Size])
{
    int col, row;
    int eval = 2;    /* 着手手の評価値 */
    int S_Point = 0, G_Point = 0;

    for (row = 0; row < Board_Size; ++row) {
        for (col = 0; col < Board_Size; ++col) {
            if ((row == 1 && col == 1) || (row == 1 && col == 6) ||
                    (row == 6 && col == 1) || (row == 6 && col == 6)) {
                /* 着手と言ってもX打ちの個所は排除 */
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

/*********** コンピュータの思考関数 *****************/
/*                                                  */
/*        発展的開放度理論に基づく思考              */
/*                                                  */
/****************************************************/

int AI_Kaihoudo(int r_othello[Board_Size * Board_Size],
                int rr_othello[Board_Size * Board_Size], int row, int col)
{
    int Point = 0;
    int rrr_othello[Board_Size * Board_Size];

    first(rr_othello, rrr_othello);    /* 初期化 */

    /* 打ったところを戻す */
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

/********** 辺、確定石の判定をする関数 *************/
/*                                                 */
/*                横2列    縦2列                   */
/*                                                 */
/***************************************************/

#define ro(i,j) r_othello[ (i)*dx+(j)*dy ]
int AI_Hen(int r_othello[Board_Size * Board_Size], int dx, int dy, int row, int r1)
{
    int i, Point = 0, order = 0;
    int eval = 5; /* 確定石の評価値     */
    int yama = 2; /* 山の評価値         */
    int wing = -2; /* 翼の評価値         */
    int Cuti = -3; /* 単独C打ちの評価値  */
    int Hen;

    /* 次手で隅が取れるか判定 */
    if (ro(row, 0) != 0 || ro(row, 7) != 0) {
        if (ro(row, 0) != 0) {
            if (ro(row, 7) != 0) {
                int k = 0;

                /* 両方向に石有り */
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
                /* 一方のみ石有り */
                for (i = 0; i <= 7; ++i) {
                    if (ro(row, i) != ro(row, 0)) {
                        break;
                    }

                    Point += ro(row, i) * eval;
                }
            }
        } else {
            /* もう一方のみ石有り */
            for (i = 7; i >= 0; --i) {
                if (ro(row, i) != ro(row, 7)) {
                    break;
                }

                Point += ro(row, i) * eval;
            }
        }
    } else {
        Hen = 0; /* 初期化 */

        if (ro(row, 1) != 0 || ro(row, 6) != 0) {

            /* 山、翼判定、単独C打ち判定 */

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

/************** 石差を判定する関数 *****************/

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

/***** 評価値を返す関数（思考部分）*****************/

int eval_node(int r_othello[Board_Size * Board_Size], int rr_othello[Board_Size * Board_Size], int row,
              int col, int turn, int counter)
{
    int X_uti1 = -100, X_uti2 = -5, X_uti3 = -2;
    /* 1～25手のX打ちは100%失敗する */

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

    /* 開放度理論に基づく思考関数へ（仕様２） */
    Point += turn * AI_Kaihoudo(r_othello, rr_othello, row, col);

    return (Point);
}

/***************** 評価関数の内容 **********************

 定石から読み出し、定石がなくなった時点で中盤に入る 9/ 6

 開放度により、上位数手までの石を判定。最良優先探索で行う
 （これにより、悪い手が排除された形になり結果強くなる）9/17

 * 着手手が相手より多い場合は発展的開放度で評価値を得る        *
 *（深さが偶数になるように。相手の番の計算で精度が上がる）9/16 *

 確定石が多くなるように打ち、最後は完全読みきりを行う 9/12

 単独C打ちは評価値を下げ、山は＋判定としている 9/12

 Ｘ打ちは評価値を下げ、Wingは－判定としている 10/ 2

 * 30手目程度から開放度が最善手で無くなるので、着手手で読む 9/16  *
 * 30手目程度までは石の数も配慮に入れて評価値を算出する 10/ 6     *

 確定石＞着手手＞開放度＞辺の重み　の順に変更、石数排除  11/ 4

 35手目以降は読みが早いので1手深く読む 9/16

 * 46手目以降の完全読みきりは早くする為、別の関数を用意する 8/22 *

 44手以降必勝読みきり（別関数消去） 2002/11/12

 45手以降完全読みきり（別関数消去） 2002/11/12

*******************************************************/
