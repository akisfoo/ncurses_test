fsm:
	cc fsm.c -o fsm -lncurses -lmenu

.PHONY: clean
clean: 
	-rm -f fsm fsm.o
