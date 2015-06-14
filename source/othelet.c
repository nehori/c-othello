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

static int count = 1;	/* 　手数を表す　 */

int get_count()
{
    return count;
}

void increase_count()
{
    count ++;
}

/********** コメントをつける関数（お遊び） ***********/

const char *comment(char *tmp, int val, int turn, int flag)
{
    if (flag == 0) {

        if (val > 0)      {
            sprintf(tmp, "私の %d 石勝ち見込み", val / 100);
            return tmp;
        } else if (val < 0) {
            sprintf(tmp, "私の %d 石負け見込み", -val / 100);
            return tmp;
        } else {
            return "引き分けになる見こみ";
        }

    } else if (flag == 2) {

        return (val > 0) ? "私の勝ち見込み" :
               (val < 0) ? "私の負け見込み" :
               "引き分けになる見こみ";

    } else {
        return (val > 50) ? "「勝利が見えました。」" :
               (val > 25) ? "「見える……手が見える!!」" :
               (val > 10) ? "「そろそろ本気だしてくださいよ～」" :
               (val >  5) ? "「有利なんだな、これが。」" :
               (val >  2) ? "「終盤で挽回出来るなんて考えてない？」" :
               (val < -50) ? "「敗戦のようです。」" :
               (val < -25) ? "「もう、ダメなのか…？」" :
               (val < -10) ? "「むぅ、負けそうです。」" :
               (val < -5) ? "「不利です…、くっ！」" :
               (val < -2) ? "「なかなか良い手を打ちますねぇ。」" :
               "「今のところは互角ですね。」";
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
    printflush("   \033[30;%dm┌─┬─┬─┬─┬─┬─┬─┬─┐\033[0m  \n ", color);

    for (row = 0; row < Board_Size; ++row) {
        printflush("\033[30;43m%d \033[0m", row);
        printflush("\033[30;%dm│\033[0m", color);

        for (col = 0; col < Board_Size; ++col) {
            if (r_othello[row * Board_Size + col] == Board_Gote) {
                printflush(WHITE);

                if (col != 7) {
                    printflush("\033[30;42m│\033[0m");
                } else {
                    printflush("\033[30;%dm│\033[0m", color);
                }
            } else if (r_othello[row * Board_Size + col] == Board_Sente) {
                printflush(BLACK);

                if (col != 7) {
                    printflush("\033[30;42m│\033[0m");
                } else {
                    printflush("\033[30;%dm│\033[0m", color);
                }
            } else {
                if (col != 7) {
                    printflush("\033[30;42m  │\033[0m");
                } else {
                    printflush(BLANK);

                    if (col != 7) {
                        printflush("\033[30;42m│\033[0m");
                    } else {
                        printflush("\033[30;%dm│\033[0m", color);
                    }
                }
            }
        }

        printflush(" \n ");

        if (row != 7) {
            printflush("  \033[30;%dm├\033[0m", color);
            printflush("\033[30;42m─┼─┼─┼─┼─┼─┼─┼─\033[0m");
            printflush("\033[30;%dm┤\033[0m\n ", color);
        } else {
            printflush("  ");
            printflush("\033[30;%dm└─┴─┴─┴─┴─┴─┴─┴─┘\033[0m", color);
            printflush("\n ");
        }
    }

    printflush("\n");
}

/****************** メイン関数 ********************/
int main(void)
{
    int row, col, turn, myturn = 0, end_val, comturn;
    int depth = 6, alpha = -99999, beta = 99999, val = 0;
    int flag = 0; /* 追加 */

    // X表示の準備・呪文
//  Othelet w("Othelet ver1.3", 13*40, 10*40+15);

    do {
        printflush("先手／後手 ? （1/-1）");
        scanf("%d", &myturn);
    } while (myturn == 0);

    comturn = 1;

    /* 思考ルーチン */
    for (turn = 1;; turn *= -1, comturn *= -1) {
        int state = board_movable_any(othello, turn);

        if (state == END) {
            break;
        } else if (state != TRUE) {
            printflush("(%2d) パスです", get_count());
        } else {

            printflush(turn == Board_Sente ? "黒番" : "白番");

            if (turn == myturn) {
                print_board(othello);

                do {
                    scanf("%d %d", &row, &col);
                } while (!board_movable(othello, turn, row, col));
            } else {
                char tmp[256] = {0};
                const char *co = "";

                if (get_count() >= 45) { /* 完全読みきり */
                    end_val = alphabeta2(othello, &row, &col,
                                         turn, 20, alpha, beta, get_count());
                    co = comment(tmp, comturn * end_val, turn, 0);

                } else if (get_count() >= 44) { /* 必勝読みきり beta<INT_MAXで */
                    end_val = alphabeta2(othello, &row, &col,
                                         turn, 20, alpha, 1, get_count());
                    co = comment(tmp, comturn * end_val, turn, 2);

                } else if (flag != -1 && returnSeach(&row, &col, &flag, turn)) {

                } else if (get_count() >= 35) { /* 35手以上 */
                    end_val = alphabeta(othello, &row, &col,
                                        turn, depth + 1, alpha, beta, val, get_count());
                    co = comment(tmp, comturn * end_val, turn, 1);

                } else { /* それ以外 */
                    end_val = alphabeta(othello, &row, &col,
                                        turn, depth, alpha, beta, val, get_count());
                    co = comment(tmp, comturn * end_val, turn, 1);
                }

                printflush("%s\n", co);
            }

            increase_count(); /*　手が打てればカウントを増やす　*/
            board_move(turn, row, col, othello);
            printflush("(%2d) %d %d\n", get_count() - 1, col, row); // 自分の打った箇所
        }
    }

    /************ ゲーム結果を計算する ****************/
    {
        int sente, gote;
        char *c;
        game_result(othello, &sente, &gote);

        if ((sente - gote)*turn > 0) {
            c = "あなたの勝ちです!!";
        } else {
            c = "あなたの負けです..";
        }

        printflush("%s ゲーム結果は %d vs %d ", c, sente, gote);
    }
    return 0;
}
