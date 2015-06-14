/**
 * Copyright (c) 2015 Kazutaka Yasuda
 * Released under the MIT license
 * http://opensource.org/licenses/mit-license.php
 */
#ifndef ALPHABETA_H_
#define ALPHABETA_H_

#include "config.h"

int Finish_Board(int r_othello[Board_Size*Board_Size]);

int board_movable_any(int r_othello[Board_Size*Board_Size], int turn);

void sort(int child_count, int Point[30], int ind[30]);

int expand_node2(int r_othello[Board_Size*Board_Size],
                 int turn, int array_row[30], int array_col[30],
		 int rrr_othello[][Board_Size*Board_Size], int ind[30]);

int expand_node(int r_othello[Board_Size*Board_Size],
                 int turn, int array_row[30], int array_col[30],
		 int rrr_othello[][Board_Size*Board_Size], int ind[30]);

int expand_node3(int r_othello[Board_Size*Board_Size],
                 int turn, int array_row[30], int array_col[30],
		 int rrr_othello[][Board_Size*Board_Size], int ind[30]);

int alphabeta(int r_othello[Board_Size*Board_Size], int *row, int *col,
              int turn, int depth, int alpha, int beta, int val, int counter);

int alphabeta2(int r_othello[Board_Size*Board_Size], int *row, int *col,
              int turn, int depth, int alpha, int beta, int counter);

#endif /*ALPHABETA_H_*/
