%{
	 #include "readConfig.tab.h"
	 #include <math.h>
%}

%%
"# Common config file for MIPs core"    { return HEADER ;}
"[perf_model/l1_icache]"				{ return MODICACHE ;}
"[perf_model/l1_dcache]"				{ return MODDCACHE ;}
"[perf_model/core]"						{ return CORE ;}
"[perf_model/dram]"						{ return DRAM ;}
"perfect"    							{ return PERF ;}
"true"    								{ yylval.intv = 1 ; return TF ; }
"false"    								{ yylval.intv = 0 ; return TF ; }
"cache_size"							{ return CSIZE;}
"associativity"							{ return CASSOC;}
"replacement_policy"					{ return RPLC ;}
"lru"									{ return LRU ;}
"writethrough"							{ return WRITETHROUGH;}
"block_size"							{ return BSIZE;}
"frequency"								{ return FREQ;}
"latency"								{ return LATEN;}
([0-9]+)    							{ yylval.intv = atoi(yytext) ; return NUM ; }
([0-9]+)"."([0-9]+)						{ yylval.dblv = atof(yytext) ; return FLT ;}
"="    									{ return EQ ;}
"\n"     								{ return NEWL; }
[ \t]  									{ }
<<EOF>>									{ return EF ;}
.      									{ printf("The given syntax in the config file is wrong. Kindly Correct it: %s\n",yytext) ; exit(-1) ;}

