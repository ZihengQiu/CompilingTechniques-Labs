all: lrparser.tab.c lex.yy.c main.c ast.h ast.c
	gcc lex.yy.c lrparser.tab.c ast.c main.c -o test.exe
	#gcc lex.yy.c lrparser.tab.c ast.c main.c -g 
lex.yy.c: lrlex.l lrparser.y 
	flex lrlex.l
lrparser.tab.c: lrparser.y ast.h ast.c
	bison -d lrparser.y


.PHONY : clean
clean:
	$(RM) *.o *.exe *.out *.tab.c *.tab.h lex.yy.c
