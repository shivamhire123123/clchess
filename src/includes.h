#ifndef INCLUDES_H
#define INCLUDES_H

#include <stdbool.h>
#include <stdint.h>
#include "debug.h"
#include "piece/list/g_slnl.h"
#define ABS(x) ((x) > 0 ? (x) : (-(x)))
#define MAX(x, y) ((x) > (y) ? (x) : (y))
#define MIN(x, y) ((x) < (y) ? (x) : (y))
//stores pos of board
//x = cols
//y = ranks
typedef struct pos {
	unsigned x;
	unsigned y;
}pos_t;
typedef list_t piece_list_t;
//this struct will be use as node in list which is in struct of each piece_type
//eg: protected_by, protecting, attack_by, attacking
//and also in board[]
typedef struct piece {
	char piece;
	pos_t pos;
	uint64_t bitpiece;
	piece_list_t protected_by, protecting, attack_by, attacking;
}piece_t;

typedef piece_t *** board_t;

#endif