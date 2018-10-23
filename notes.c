---global_var---
player - stores which player have turn
move - stores move in PGN form
board - stores all pointer to all pieces or null if not present
char_board - stores all initial position of pieces
---global_var end---
---files---
board_pieces - store variable and function related to board and pieces
debug - for printf style debugging
includes - general include file
list - list adt for storing pieces
main - guides main logic flow
piece_list - funct related to piece list
ui - user interface related
moves - legal move checking
---files end---
---data_types---
move_t - have src and dest -> rank and col
pos_t - can store any position on board x->rank, y->cols
pieces _t -
	rook - pin
	night - pin
	bishop - pin
	queen - pin
	king - castle, last_man_standing for last 50 moves, check - if check
	pawn - enpassant, twomoves, pin
piece_t - piece char storing piece, pos of pos_t type, ptr to above piece types
	attacking, attack_by, protecting, protect_by of piece_list_t
piece_list_t - stores pieces in list form
---data_types end---
---func---
	---boar_pieces---
		init_board
			initialize board[][] to store struct points of different pieces
			according to start position
			also initialize variables in piece_t and to NULL if piece not present
			initialize all list of piece_t
		init_piece
			allocate memory for piece _t and call corresponding init piece _t
		update_board
			update board according to move_t
		conv_str_move
			convert moves writen in string to type move_t
		inbtw
			tells if there is a piece in between two tiles
		init pieces _t
			initialize all var related to pieces
	---board_pieces end---
	---debug---
		vprintn for variable args
		print for normal messages only
	---debug end---
	---piece_list---
		init_piece_list
			initialize piece_list_t
	---piece_list end---
	---ui---
		print_board
			print board on terminal
	---ui end---
---func end---
main -
	main menu
		two player
		one player
		load saved game
	print_board
good move
	if protecting piece
		if protected by other
		else if require to protect
	if attack by opp on new tile
		will protected
			good exchange
	if board cover is degraded
		how important is move
			move for
				checking king - do it
				kill - do it
				get max scores - do it
logic for one player
	four sectors
	0)opening lib
	1)checkmate opp king
		if king can be check
			possible moves of opp_king
			if possibe moves can be cover before checking
				if good move
				stores expected moves in list
	2)kill opp pieces
	3)get max scores on the basis of various pieces position
