#include "includes.h"
#include "board/board.h"
#include "ui/ui.h"
#include "chess/chess.h"
#include "piece/piece.h"
#define MOVE_LEN 8

char move[MOVE_LEN];
void twoplayer(void);
int main() {
	// TODO main menu
	twoplayer();
}
void twoplayer() {

	print4("In twoplayer\n");
	chess_t chess;
	init_chess(&chess);
	print_board(chess.board);
	move_t str_move;
	while(1) {
		// take next move
		if(chess.player == WHITE) {
			printf("%c->", 'w');
		}
		else {
			printf("%c->", 'b');
		}
		if(scanf("%s", move) == -1) {
			printf("\n");
			break;
		}

		//TODO add option of inputing move according to PGN
		// update board according to next move
		str_move = conv_str_move(move);
		if(str_move.x1 == -1) {
			printf("Invalid move\n");
			continue;
		}
		if(inbtw(chess.board, str_move) || islegal(chess.board, str_move)) {
			printf("Invalid move\n");
			continue;
		}
		update_board(chess.board, str_move);
		print_board(chess.board);

		// change player
		chess.player ^= COLOR_Msk;
	}
}