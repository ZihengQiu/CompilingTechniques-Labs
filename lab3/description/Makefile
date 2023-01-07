all: calc2.tab.c lex.yy.c 
	gcc lex.yy.c calc2.tab.c -o calc.exe
lex.yy.c: calc2.l calc2.tab.h
	flex calc2.l
calc2.tab.c: calc2.y
	bison -d calc2.y


.PHONY : clean
clean:
	$(RM) *.o *.exe calc2.tab.c calc2.tab.h lex.yy.c
