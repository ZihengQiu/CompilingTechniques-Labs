objects = rd_parser.c

edit : $(objects) lex.yy.c
	gcc -o test $(objects) lex.yy.c -lfl
	gcc -g $(objects) lex.yy.c -lfl

lex.yy.c: sysy_lex.l 
	flex sysy_lex.l

.PHONY : clean
clean :
	-rm test *.o lex.yy.c
