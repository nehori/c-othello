/**
 * Copyright (c) 2015 Kazutaka Yasuda
 * Released under the MIT license
 * http://opensource.org/licenses/mit-license.php
 */
#ifndef CONFIG_H_
#define CONFIG_H_

#define bCopy(a,b,c,d)	memmove(b,a,sizeof(c)*(d))
#define bZero(a,c,d)	memset(a,0,sizeof(c)*(d))
#define Board_Turn   8  /* �Ђ�����Ԃ����A8���� */
#define Board_Size   8  /* �Ղ̑傫�� */
#define Board_Sente  1  /* ��� */
#define Board_Gote  -1  /* ��� */
#define   TRUE       1  /* �łĂ�ꍇ */
#define   PASS      -1  /* �p�X */
#define   END        0  /* �I�� */
#define   FALSE      0

#define BLACK  "\033[30;42m��\033[0m"
#define WHITE  "\033[37;42m��\033[0m"
#define BLANK  "\033[30;42m  \033[0m"

extern int get_count();
extern void increase_count();

/* output */
#define printflush(s, ...) do { printf(s, ##__VA_ARGS__); fflush(stdout); } while (0)

#endif /*CONFIG_H_*/
