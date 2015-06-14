/**
 * Copyright (c) 2015 Kazutaka Yasuda
 * Released under the MIT license
 * http://opensource.org/licenses/mit-license.php
 */
#ifndef JYOUSEKI_H_
#define JYOUSEKI_H_

#include "config.h"

int SearchSt(char *s, char *t);

void eng_num(int *row, int *col, int *flag);

void num_eng(char *row, char *col, int *flag);

int turn_Change(char *row, char *col, int *flag);

int turn_Sente(int *row, int *col, int *flag);

int turn_Gote(int *row, int *col, int *flag);

int returnSeach(int *row, int *col, int *flag, int turn);

#endif /*JYOUSEKI_H_*/
