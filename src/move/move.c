#include "move.h"
#include "../piece/piece.h"
#include "../error/err.h"
#include "../chess/chess.h"
#include "../ui/ui.h"
bool (*move_mat[15][15])(piece_t *piece, int x, int y) = {
	{ swdiag, invalid, invalid, invalid, invalid, invalid, invalid, nvevertical, invalid, invalid, invalid, invalid, invalid, invalid, sediag },
	{ invalid, swdiag, invalid, invalid, invalid, invalid, invalid, nvevertical, invalid, invalid, invalid, invalid, invalid, sediag, invalid },
	{ invalid, invalid, swdiag, invalid, invalid, invalid, invalid, nvevertical, invalid, invalid, invalid, invalid, sediag, invalid, invalid },
	{ invalid, invalid, invalid, swdiag, invalid, invalid, invalid, nvevertical, invalid, invalid, invalid, sediag, invalid, invalid, invalid },
	{ invalid, invalid, invalid, invalid, swdiag, invalid, invalid, nvevertical, invalid, invalid, sediag, invalid, invalid, invalid, invalid },
	{ invalid, invalid, invalid, invalid, invalid, swdiag, nightswmove, nvevertical, nightsemove, sediag, invalid, invalid, invalid, invalid, invalid },
	{ invalid, invalid, invalid, invalid, invalid, nightwsmove, swdiag, nvevertical, sediag, nightesmove, invalid, invalid, invalid, invalid, invalid },
	{ nvehorizontal, nvehorizontal, nvehorizontal, nvehorizontal, nvehorizontal, nvehorizontal, nvehorizontal, invalid, pvehorizontal, pvehorizontal, pvehorizontal, pvehorizontal, pvehorizontal, pvehorizontal, pvehorizontal},
	{ invalid, invalid, invalid, invalid, invalid, nightwnmove, nwdiag, pvevertical, nediag, nightenmove, invalid, invalid, invalid, invalid, invalid },
	{ invalid, invalid, invalid, invalid, invalid, nwdiag, nightnwmove, pvevertical, nightnemove, nediag, invalid, invalid, invalid, invalid, invalid },
	{ invalid, invalid, invalid, invalid, nwdiag, invalid, invalid, pvevertical, invalid, invalid, nediag, invalid, invalid, invalid, invalid },
	{ invalid, invalid, invalid, nwdiag, invalid, invalid, invalid, pvevertical, invalid, invalid, invalid, nediag, invalid, invalid, invalid },
	{ invalid, invalid, nwdiag, invalid, invalid, invalid, invalid, pvevertical, invalid, invalid, invalid, invalid, nediag, invalid, invalid },
	{ invalid, nwdiag, invalid, invalid, invalid, invalid, invalid, pvevertical, invalid, invalid, invalid, invalid, invalid, nediag, invalid },
	{ nwdiag, invalid, invalid, invalid, invalid, invalid, invalid, pvevertical, invalid, invalid, invalid, invalid, invalid, invalid, nediag },
};
bool (**move_mat_mid)(piece_t*, int, int) = &(move_mat[7][7]);
/*
 * convert to structred move
 * only accept moves in following format
 * 	e 2	 e 4 (without spaces)
 * 	starting ending
 * 	address	 address
 * no caps allowed
 * return structure_move
 * and set str_move.x1 to -1 if invalid move(out of board bound move)
 */
move_t conv_str_move(char *move) {

	move_t str_move;
	if(move[0] >= 'a' && move[0] <= 'h' &&
	   move[1] >= '1' && move[1] <= '8' &&
	   move[2] >= 'a' && move[2] <= 'h' &&
	   move[3] >= '1' && move[3] <= '8') {
		str_move.x1 = move[0] - 'a';
		str_move.y1 = (move[1] - '0' - 1);// 7- since board in array
						    // is opposite of what gets printed
		str_move.x2 = move[2] - 'a';
		str_move.y2 = (move[3] - '0' - 1);
	}
	else {
		str_move.x1 = -1;
	}
	vprint1("src (%c, %d) dest (%c, %d)", str_move.x1 + 'a', str_move.y1,
		       				str_move.x2 + 'a', str_move.y2);
	return str_move;
}

/*
 * check if there is pieces in between the two tiles between which movement of
 * some other piece is going to take place
 * retval
 * 	1 - yes there is piece in btw
 * 	0 - no
 * 	-1- invalid move
 */
int inbtw(board_t board, move_t move) {
	int i, j;
	int min_rank = MIN(move.y1, move.y2),
	    max_rank = MAX(move.y1, move.y2),
	    min_col = MIN(move.x1, move.x2),
	    max_col = MAX(move.x1, move.x2);
	int x = move.x2 - move.x1;
	int y = move.y2 - move.y1;
	//no movement
	if(min_col == max_col && min_rank == max_rank) {
		return -1;
	}
	//horizontal movement
	else if(min_rank == max_rank) {
		for(i = min_col + 1; i < max_col; i++) {
			if(board[min_rank][i] != NULL) {
				return 1;
			}
		}
	}
	//vertical movement
	else if(min_col == max_col) {
		for(i = min_rank + 1; i < max_rank; i++) {
			if(board[i][min_col] != NULL) {
				return 1;
			}
		}
	}
	//diagonal movement
	else if(max_col - min_col == max_rank - min_rank) {
		for(i = min_rank + 1, j = min_col + 1; i < max_rank &&
				j < max_col; i++, j++) {
			if(board[i][j] != NULL) {
				return 1;
			}
		}
	}
	//knight movement
	else if((max_rank - min_rank == 2 &&
		max_col - min_col == 1) ||
		(max_col - min_col == 2 &&
		 max_rank - min_rank == 1)) {
		return 0;
	}
	//anything else
	else {
		return -1;
	}
	return 0;
}
//TODO for any move after checking if that move is doable create a second chess
//instance do that move in that chess instance and see if king is under check
//TODO handle castling
int islegal(chess_t *chess, int tocheckmate) {
	board_t board = chess->board;
	move_t move = chess->move;
	piece_t *src_piece = board[move.y1][move.x1];
	piece_t *dest_piece = board[move.y2][move.x2];
	int x = move.x2 - move.x1;
	int y = move.y2 - move.y1;
	int ret;
	bool (*ptr)(piece_t*, int, int) = move_mat[7 + y][7 + x];
	if(!src_piece) {
		print3("No piece at starting tile");
		return ENOPIECE;
	}
	else if(COLOR(src_piece->bitpiece) ^ chess->player) {
		print3("Play with your own piece");
		return EWCOL;
	}
	//check if capture of same side
	if(dest_piece && (!(COLOR(src_piece->bitpiece) ^ COLOR(dest_piece->bitpiece)))) {
		print3("You cant capture your own piece");
		return EOWNCAP;
	}
	if(GetPiece(src_piece->bitpiece) == King && (x == 2 || x == -2) && (y == 0 || y == 7)) {
		if(GetCastling(src_piece->bitpiece) == 0) {
			print3("Castling not possible");
			return ECAST;
		}
		else {
			chess_t chesscopy;
			creatchesscopy(&chesscopy, chess);
			if((ret = castlingpossible(&chesscopy))) {
				destroy_chess(&chesscopy);
				return ret;
			}
			if(tocheckmate && (ret = ischeckmate(&chesscopy))) {
				destroy_chess(&chesscopy);
				return ret;
			}
			destroy_chess(&chesscopy);
		}
	}
	else {
		ret =  ptr(src_piece, x, y);
		if(ret) {
			return ret;
		}
		else {
			chess_t chesscopy;
			creatchesscopy(&chesscopy, chess);
			update_chess(&chesscopy);
			if((ret = checkforcheck(&chesscopy))) {
				destroy_chess(&chesscopy);
				return ret;
			}
			if(tocheckmate && (ret = ischeckmate(&chesscopy))) {
				destroy_chess(&chesscopy);
				return ret;
			}
			destroy_chess(&chesscopy);
		}
	}
	return 0;
}
void generaterandommove(chess_t *chess, int player) {

	print1();
	int x = 0, y = 0, num, n, dirx, diry, randdir;
	piece_list_t *l;
	piece_t *piece;
	if(player == WHITE) {
		print1("white player selected");
		l = &chess->white;
	}
	else {
		print1("black player selected");
		l = &chess->black;
	}
	while(1) {
		while(1) {
			vprint1("player change to %d", player);
			chess->player = player;
			num = getnumofelem(l);
			n = rand() % num;
			if(n == 0) {
				n = 1;
			}
			piece = (piece_t *)see(l, n);
			randdir = rand() % 8;
			dirx = MoveAlongX[randdir];
			diry = MoveAlongY[randdir];
			if(dirx == 0) {
				if(GetV(piece->bitpiece)) {
					if(diry > 0) {
						if((y = GetVPveField(piece->bitpiece))) {
							break;
						}
					}
					else {
						if((y = -GetVNveField(piece->bitpiece))) {
							break;
						}
					}
				}
			}
			else if(diry == 0) {
				if(GetH(piece->bitpiece)) {
					if(dirx > 0) {
						if((x = GetHPveField(piece->bitpiece))) {
							break;
						}
					}
					else {
						 if((x = -GetHNveField(piece->bitpiece))) {
							break;
						}
					}
				}
			}
			else if(ABS(diry) == ABS(dirx)) {
				if(GetD(piece->bitpiece)) {
					if(dirx == 1 && diry == 1) {
						if(GetDNEField(piece->bitpiece)) {
							y = 1;
							x = -1;
							break;
						}
					}
					else if(dirx == -1 && diry == 1) {
						if(GetDNWField(piece->bitpiece)) {
							y = 1;
							x = 1;
							break;
						}
					}
					else if(dirx == -1 && diry == -1) {
						if(GetDSWField(piece->bitpiece)) {
							y = -1;
							x = 1;
							break;
						}
					}
					else if(dirx == 1 && diry == -1) {
						if(GetDSEField(piece->bitpiece)) {
							y = -1;
							x = -1;
							break;
						}
					}
				}
			}
			dirx = NightMoveAlongX[randdir];
			diry = NightMoveAlongY[randdir];
			if(GetPiece(piece->bitpiece) == Night) {
				if(dirx == 1 && diry == -2) {
					if(GetNightSE(piece->bitpiece)) {
						x = 1;
						y = -2;
						break;
					}
				}
				if(dirx == -1 && diry == -2) {
					if(GetNightSW(piece->bitpiece)) {
						x = -1;
						y = -2;
						break;
					}
				}
				if(dirx == -2 && diry == -1) {
					if(GetNightWS(piece->bitpiece)) {
						x = -2;
						y = -1;
						break;
					}
				}
				if(dirx == 2 && diry == -1) {
					if(GetNightES(piece->bitpiece)) {
						x = 2;
						y = -1;
						break;
					}
				}
				if(dirx == -1 && diry == 2) {
					if(GetNightNW(piece->bitpiece)) {
						x = -1;
						y = 2;
						break;
					}
				}
				if(dirx == 1 && diry == 2) {
					if(GetNightNE(piece->bitpiece)) {
						x = 1;
						y = 2;
						break;
					}
				}
				if(dirx == -2 && diry == 1) {
					if(GetNightWN(piece->bitpiece)) {
						x = -2;
						y = 1;
						break;
					}
				}
				if(dirx == 2 && diry == 1) {
					if(GetNightEN(piece->bitpiece)) {
						x = 2;
						y = 1;
						break;
					}
				}
			}
		}
		chess->move.x1 = piece->pos.x;
		chess->move.x2 = piece->pos.x + x;
		chess->move.y1 = piece->pos.y;
		chess->move.y2 = piece->pos.y + y;
		if(!(chess->move.x2 >= 0 && chess->move.x2 < 8 && chess->move.y2 >= 0 && chess->move.y2 < 8)) {
			continue;
		}
		vprint1("%c at (%c, %d) can illegaly move to (%c, %d)", piece->piece, chess->move.x1 + 'a', chess->move.y1, chess->move.x2 + 'a', chess->move.y2);
		if(!islegal(chess, 0)) {
			break;
		}
	}
	update_chess(chess);
	chess->player = player ^ COLOR_Msk;
	vprint1("%c at (%c, %d) can move to (%c, %d)", piece->piece, chess->move.x1 + 'a', chess->move.y1, chess->move.x2 + 'a', chess->move.y2);
	print_board(chess->board);
}

bool pvevertical(piece_t *piece, int x, int y) {
	vprint1("%lx %ld", GetVPveField(piece->bitpiece), (uint64_t)y);
	return !(GetVPveField(piece->bitpiece) >= (uint64_t)y);
}
bool nvevertical(piece_t *piece, int x, int y) {
	print1();
	vprint1("%lx %ld", GetVNveField(piece->bitpiece), (uint64_t)-y);
	return !(GetVNveField(piece->bitpiece) >= (uint64_t)-y);
}
bool pvehorizontal(piece_t *piece, int x, int y) {
	print1();
	return !(GetHPveField(piece->bitpiece) >= (uint64_t)x);
}
bool nvehorizontal(piece_t *piece, int x, int y) {
	print1();
	return !(GetHNveField(piece->bitpiece) >= (uint64_t)-x);
}
bool nwdiag(piece_t *piece, int x, int y) {
	print1();
	return !(GetDNWField(piece->bitpiece) >= (uint64_t)y);
}
bool nediag(piece_t *piece, int x, int y) {
	print1();
	return !(GetDNEField(piece->bitpiece) >= (uint64_t)y);
}
bool swdiag(piece_t *piece, int x, int y) {
	print1();
	return !(GetDSWField(piece->bitpiece) >= (uint64_t)-y);
}
bool sediag(piece_t *piece, int x, int y) {
	print1();
	return !(GetDSEField(piece->bitpiece) >= (uint64_t)x);
}
bool nightnwmove(piece_t *piece, int x, int y) {
	return GetNightNW(piece->bitpiece) ^ (NightNWMsk >> NightNWPos);
}
bool nightnemove(piece_t *piece, int x, int y) {
	print1();
	return GetNightNE(piece->bitpiece) ^ (NightNEMsk >> NightNEPos);
}
bool nightswmove(piece_t *piece, int x, int y) {
	print1();
	return GetNightSW(piece->bitpiece) ^ (NightSWMsk >> NightSWPos);
}
bool nightsemove(piece_t *piece, int x, int y) {
	print1();
	return GetNightSE(piece->bitpiece) ^ (NightSEMsk >> NightSEPos);
}
bool nightwnmove(piece_t *piece, int x, int y) {
	print1();
	return GetNightWN(piece->bitpiece) ^ (NightWNMsk >> NightWNPos);
}
bool nightenmove(piece_t *piece, int x, int y) {
	print1();
	return GetNightEN(piece->bitpiece) ^ (NightENMsk >> NightENPos);
}
bool nightwsmove(piece_t *piece, int x, int y) {
	print1();
	return GetNightWS(piece->bitpiece) ^ (NightWSMsk >> NightWSPos);
}
bool nightesmove(piece_t *piece, int x, int y) {
	print1();
	return GetNightES(piece->bitpiece) ^ (NightESMsk >> NightESPos);
}

bool invalid(piece_t *piece, int x, int y) {
	print1("in invalid");
	return 1;
}
