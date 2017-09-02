%{
	#include <math.h>
	#include <stdlib.h>
	#include <stdio.h>
	#include "cache.h"
	#include "Processor.h"
	int yylex() ;
	int yyerror(char *s);
%}


%union {
	int intv ;
	double dblv ;
}

%token HEADER MODICACHE MODDCACHE CORE DRAM PERF CSIZE CASSOC RPLC LRU WRITETHROUGH BSIZE FREQ LATEN EQ NEWL EF
%token<intv>  NUM TF
%token<dblv> FLT
%%


whole_file:
	|	HEADER NEWL icache_line NEWL NEWL dcache_line NEWL NEWL core_line NEWL NEWL dram_line EF		{return 0 ;}
	|	HEADER NEWL icache_line NEWL NEWL dcache_line NEWL NEWL core_line NEWL NEWL dram_line NEWL EF	{return 0 ;}
;

icache_line: 
	| MODICACHE NEWL PERF EQ TF NEWL CSIZE EQ NUM NEWL CASSOC EQ NUM NEWL RPLC EQ LRU NEWL WRITETHROUGH EQ NUM NEWL BSIZE EQ NUM{
		set_cache_param(CACHE_PARAM_ISIZE,($9*1024)) ;
		set_cache_param(CACHE_PARAM_ASSOC,$13) ;
		if($21)	
			set_cache_param(CACHE_PARAM_WRITETHROUGH,0) ; 
		else 
			set_cache_param(CACHE_PARAM_WRITEBACK,0) ;
		set_cache_param(CACHE_PARAM_BLOCK_SIZE,$25) ;
	}
;

dcache_line: 
	| MODDCACHE NEWL PERF EQ TF NEWL CSIZE EQ NUM NEWL CASSOC EQ NUM NEWL RPLC EQ LRU NEWL WRITETHROUGH EQ NUM NEWL BSIZE EQ NUM{
		set_cache_param(CACHE_PARAM_DSIZE,($9*1024)) ;
		set_cache_param(CACHE_PARAM_ASSOC,$13) ;
		if($21)	
			set_cache_param(CACHE_PARAM_WRITETHROUGH,0) ; 
		else 
			set_cache_param(CACHE_PARAM_WRITEBACK,0) ;
		set_cache_param(CACHE_PARAM_BLOCK_SIZE,$25) ;
		perfect=$5 ;
	}

;

core_line: CORE NEWL FREQ EQ FLT { freq=$5 ;}
;
dram_line: DRAM NEWL LATEN EQ NUM { latency=$5 ;}
;

%%

int main(int argc, char **argv){
	if(argc!=5){
		printf("Correct format is processor_cache_simulator <input_HEX_file> <input_CFG_file> <output_SVG_file> <output_results_file>\n") ;
		exit(-1) ;
	}
	extern FILE* yyin ;
	yyin = fopen(argv[2],"r") ;
 	yyparse() ;
	init_cache() ;
	run_Processor(argc,argv) ;
}

int yyerror(char *s){
  	printf("SynError\n") ;
}