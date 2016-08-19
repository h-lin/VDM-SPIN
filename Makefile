all: vdmj

vdmj: lex.yy.c vdmj.tab.c vdmj.tab.h ast.h ast.c xml.c xml.h pan.c
	gcc -g -O2 -DSAFETY -DBITSTATE -o pan pan.c lex.yy.c vdmj.tab.c ast.c xml.c -L/usr/local/lib -lfl

lex.yy.c: vdmj.l
	flex vdmj.l

vdmj.tab.c vdmj.tab.h: vdmj.y
	bison -v -d vdmj.y

clean:
	-rm -f *.output a.out lex.yy.c *.tab.c *.tab.h pan *~ \#*\#
