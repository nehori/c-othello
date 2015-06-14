/**
 * Copyright (c) 2015 Kazutaka Yasuda
 * Released under the MIT license
 * http://opensource.org/licenses/mit-license.php
 */
#ifndef MOVEMENT_H_
#define MOVEMENT_H_

#include "config.h"

void Board_Directons(int num, int *row, int *col);

void first(int r_othello[Board_Size*Board_Size], int rr_othello[Board_Size*Board_Size]);

int check_board(int row, int col);

void board_move(int turn, int row, int col, int r_othello[Board_Size*Board_Size]);

int board_movable(int r_othello[Board_Size*Board_Size],
		  int turn, int row, int col);

void game_result(int r_othello[Board_Size*Board_Size], int *Sente_Point, int *Gote_Point);

#endif /*MOVEMENT_H_*/
