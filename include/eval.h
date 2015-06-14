/**
 * Copyright (c) 2015 Kazutaka Yasuda
 * Released under the MIT license
 * http://opensource.org/licenses/mit-license.php
 */
#ifndef EVAL_H_
#define EVAL_H_

#include "config.h"

int Board_Empty(int row, int col, int r_othello[Board_Size*Board_Size]);

int AI_Tyakusyu(int r_othello[Board_Size*Board_Size]);

int AI_Tyakusyu2(int r_othello[Board_Size*Board_Size],int turn);

int AI_Kaihoudo(int r_othello[Board_Size*Board_Size],
                int rr_othello[Board_Size*Board_Size], int row,int col);

int AI_Hen(int r_othello[Board_Size*Board_Size], int dx, int dy, int row, int r1);

int AI_Hen1(int r_othello[Board_Size*Board_Size]);

int AI_Hen2(int r_othello[Board_Size*Board_Size]);

int eval_storn(int r_othello[Board_Size*Board_Size]);

int eval_node(int r_othello[Board_Size*Board_Size], int rr_othello[Board_Size*Board_Size], int row,
              int col, int turn, int counter);

#endif /*EVAL_H_*/
