/**
 * Copyright (c) 2015 Kazutaka Yasuda
 * Released under the MIT license
 * http://opensource.org/licenses/mit-license.php
 */
#ifndef CONFIG_H_
#define CONFIG_H_

#define bCopy(a,b,c,d)	memmove(b,a,sizeof(c)*(d))
#define bZero(a,c,d)	memset(a,0,sizeof(c)*(d))
#define Board_Turn   8  /* ひっくり返す数、8方向 */
#define Board_Size   8  /* 盤の大きさ */
#define Board_Sente  1  /* 先手 */
#define Board_Gote  -1  /* 後手 */
#define   TRUE       1  /* 打てる場合 */
#define   PASS      -1  /* パス */
#define   END        0  /* 終局 */
#define   FALSE      0

#define BLACK  "\033[30;42m●\033[0m"
#define WHITE  "\033[37;42m●\033[0m"
#define BLANK  "\033[30;42m  \033[0m"

extern int get_count();
extern void increase_count();

/* output */
#define printflush(s, ...) do { printf(s, ##__VA_ARGS__); fflush(stdout); } while (0)

#endif /*CONFIG_H_*/
