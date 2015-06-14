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

/******** 最終読みきりが途中で起きた場合 ************/

int Finish_Board(int r_othello[Board_Size * Board_Size])
{
    /* この関数で次の手を場面に打ち、判定する */
    int Sente_Point = 0 , Gote_Point = 0;

    game_result(r_othello, &Sente_Point, &Gote_Point);

    /* 同点の場合（全部負けの時はこれが良い様に） */
    /* 先手の勝ち && 後手の勝ち（100掛けるのは意味無し） */
    return ( Sente_Point - Gote_Point) * 100;
}

/**************　パスを判定する関数　******************/

int board_movable_any(int r_othello[Board_Size * Board_Size], int turn)
{
    int col, row;
    int r_turn;

    for (row = 0; row < Board_Size; ++row) {
        for (col = 0; col < Board_Size; ++col) {
            if (r_othello[row * Board_Size + col] == 0) {
                /*　どちらも石を置いていない場合　*/
                if (board_movable(r_othello, turn, row, col)) {
                    /* 自分は置くことができる場合 */
                    return (TRUE);
                }
            }
        }
    }

    /* 自分はパスの場合 */

    for (row = 0; row < Board_Size; ++row) {
        for (col = 0; col < Board_Size; ++col) {
            if (r_othello[row * Board_Size + col] == 0) {

                /*　どちらも石を置いていない場合　*/
                r_turn = turn * (-1);

                if (board_movable(r_othello, r_turn, row, col)) {
                    /* 相手は置くことができる場合 */
                    return (PASS);
                }
            }
        }
    }

    /* どちらも打てない */
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

/*** 子ノードを全て求める関数（反復深化法、MPC） ***/

int expand_node2(int r_othello[Board_Size * Board_Size],
                 int turn, int array_row[30], int array_col[30],
                 int rrr_othello[][Board_Size * Board_Size], int ind[30])
{
    int inc_row, inc_col, i;
    int Point[30];
    int val = -6; /* カットする評価数字 */
    int child_count = 0;

    for (inc_row = 0; inc_row <= Board_Size; ++inc_row) {
        for (inc_col = 0; inc_col <= Board_Size; ++inc_col) {
            if (board_movable(r_othello, turn, inc_row, inc_col)) {
                int *rr_othello = rrr_othello[child_count];

                first(r_othello, rr_othello);    /* 初期化 */
                board_move(turn, inc_row, inc_col, rr_othello);

                /* 両方とも正の数が高値 */
                if ((inc_row == 0 && inc_col == 0) || (inc_row == 0 && inc_col == 7) ||
                        (inc_row == 7 && inc_col == 0) || (inc_row == 7 && inc_col == 7)) {
                    array_row[child_count] = inc_row;
                    array_col[child_count] = inc_col;
                    Point[child_count] = 1;
                    ++(child_count);
                } else if (inc_row == 0 || inc_col == 0 || inc_row == 7 || inc_col == 7) {
                    /* 最後に付加する */
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

    /* 桁外れの手は始めから除外（やや弱くなるかも） */
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

/*** 子ノードを全て求める関数（反復深化法） ***/

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

                first(r_othello, rr_othello);    /* 初期化 */
                board_move(turn, inc_row, inc_col, rr_othello);
                /* 打ったところを戻す */

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

/*** 子ノードを全て求める関数（反復深化法） ***/

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

                first(r_othello, rr_othello);    /* 初期化 */
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

/******** αβ法による評価値の検出関数 *************/

int alphabeta(int r_othello[Board_Size * Board_Size], int *row, int *col,
              int turn, int depth, int alpha, int beta, int val, int counter)
{
    int select;
    int array_row[60], array_col[60], ind[60], child_count; /* ノードの数 */
    int _i;
    int rrr_othello[60][Board_Size * Board_Size];

    if (depth <= 0) {/* 最化ノード時処理 */
        /* 発展的開放度＋着手手＋辺の重み１＋辺の重み２ */
        return val + AI_Tyakusyu(r_othello)
               + AI_Hen1(r_othello) + AI_Hen2(r_othello);
    }

    /* 現在の局面から1手進めた状態をa1,a2,a3とする */
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

    select = 0;   /* 一応初期設定を行っておく */

    /* alpha < beta && i < child_countの時は繰り返す */
    int b = beta;

    for (_i = 0; (alpha < beta) && (_i < child_count); ++_i) {
        int i = ind[_i], *rr_othello = rrr_othello[i];
        int a, inc_row, inc_col, end_val;  /* 最終評価値 */

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
            alpha = end_val;  /* αカット */
            select = i;
        }

        b = alpha + 1;
    }

    *row = array_row[select];
    *col = array_col[select];

    return (turn * alpha);
}

/******** αβ法による完全読みの検出関数 *************/

int alphabeta2(int r_othello[Board_Size * Board_Size], int *row, int *col,
               int turn, int depth, int alpha, int beta, int counter)
{
    int select;
    int array_row[30], array_col[30], ind[30], child_count; /* ノードの数 */
    int rrr_othello[30][Board_Size * Board_Size];
    int _i;

    /* 現在の局面から1手進めた状態をa1,a2,a3とする */
    for (_i = 0; !(child_count =
                       expand_node3(r_othello, turn, array_row, array_col, rrr_othello, ind)); _i++)
        if (_i) {
            return Finish_Board(r_othello);
        } else {
            turn = -turn, _i = -alpha, alpha = -beta, beta = _i;
        }

    /* 最初は行列の値が入ってないのでこの処理はいらない */

    select = 0;   /* 一応初期設定を行っておく */

    /* alpha < beta && i < child_countの時は繰り返す */
    int b = beta;

    for (_i = 0; (alpha < beta) && (_i < child_count); ++_i) {
        int i = ind[_i], *rr_othello = rrr_othello[i];
        int inc_row, inc_col, end_val;  /* 最終評価値 */

        end_val = turn * alphabeta2(rr_othello, &inc_row, &inc_col, -turn,
                                    depth - 1, -b, -alpha, counter + 1);

        if (alpha < end_val && end_val <= b && _i) {
            end_val = turn * alphabeta2(rr_othello, &inc_row, &inc_col, -turn,
                                        depth - 1, -beta, -end_val, counter + 1);
        }

        if (end_val > alpha) {
            alpha = end_val;  /* αカット */
            select = i;
        }

        b = alpha + 1;
    }

    *row = array_row[select];
    *col = array_col[select];

    return (turn * alpha);
}

/***************** 更新履歴 ****************************

 2002/11/12  NegaMax法をNegaScout法に変更、読み切り高速化

 2002/11/11  αβ関数をNegaMax法に変更、プログラム最適化

 2002/11/08  バグ修正、αβ法の必勝読み切り、確定石評価

 2002/11/05  定石ファイル3千を棋譜ファイル3万に変更

 2001/11/16  オセロ大会本選優勝。オープンソースに変更（初段）

 2001/11/ 4  思い切って、評価関数排除。辺処理以外最善手。

 2001/11/ 2  オセロ大会予選突破、未だNAOYAに勝てず。

 2001/10/13  NAOYAに負ける。理由分からず。

 2001/10/10  必勝読みきり追加予定。内部ファイル化計画中。

 2001/10/ 6  石の数の差を追加。レベルはほぼ互角にもどる（1級）

 2001/10/ 2  確定石、辺の評価値を下げ、翼の判定変更

 2001/ 9/25  NAOYAに負けて、アルゴリズム一掃。レベル下がる

 2001/ 9/17  反復深化法を全てに適用。速さが各段に上がる

 2001/ 9/17  Thellを破る。1次配列に変更、早さ修正ならず

 2001/ 9/16  手数で場合分けαβ追加、Thellとほぼ互角。（2級）

 2001/ 9/12  手数、辺処理を追加、修正。先手でThellを破る

 2001/ 9/10  αβ法のPASS判定修正、完全読みきり直る（3級）

 2001/ 9/ 6  YonYonさんに修正箇所の指摘をもらう、定石追加

 2001/ 8/29  完全読みきりバグ発生、理由今だ知れず…

 2001/ 8/25  αβ法を修正、PASS判定バグ発見より

 2001/ 8/23  プログラム中の必要無い関数等の排除を行う

 2001/ 8/22  αβ法を修正、αβ法によるオセロが可能になる（4級）

 2001/ 8/20  RedHat Linuxを導入し、C言語開発環境整える

 2001/ 8/16  YonYonさんにαβ法を修正してもらう

 2001/ 8/10  αβ法によるアルゴリズムの追加を試みる

 2001/ 7/ 8  C言語でコンピュータ対戦オセロゲーム作る（初心者）

*******************************************************/
