all: 
	bison -d readConfig.y
	flex readConfig.l
	gcc -g -o ../processor_cache_simulator readConfig.tab.c cache.c Processor.c Process_Instr.c Colors.c initReg.c lex.yy.c -lpthread -lfl -lm