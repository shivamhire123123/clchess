DEBUG_FLAG = 0

project: src/main.c board.o move.o piece.o slnl.o ui.o chess.o
	gcc -DDEBUG=$(DEBUG_FLAG) src/main.c build/board.o build/move.o build/piece.o build/slnl.o build/ui.o build/chess.o -o project
board.o: src/board/board.h src/board/board.c
	mkdir -p build
	gcc -DDEBUG=$(DEBUG_FLAG) -c src/board/board.c -o build/board.o
move.o: src/move/move.h src/move/move.c
	gcc -DDEBUG=$(DEBUG_FLAG) -c src/move/move.c -o build/move.o
piece.o: src/piece/piece.h src/piece/piece.c
	gcc -DDEBUG=$(DEBUG_FLAG) -c src/piece/piece.c  -o build/piece.o
slnl.o: src/piece/list/slnl.c src/piece/list/slnl.h
	gcc -DDEBUG=$(DEBUG_FLAG) -c src/piece//list/slnl.c -o build/slnl.o
ui.o: src/ui/ui.h src/ui/ui.c
	gcc -DDEBUG=$(DEBUG_FLAG) -c src/ui/ui.c -o build/ui.o
chess.o: src/includes.h src/move/move.h src/move/move.c
	gcc -DDEBUG=$(DEBUG_FLAG) -c src/chess/chess.c -o build/chess.o
