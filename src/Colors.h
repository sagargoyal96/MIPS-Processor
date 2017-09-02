#ifndef COLORS_H_
#define COLORS_H_
#include <stdlib.h>
#include <stdio.h>

extern FILE *cssFile ;
extern FILE *jsfile ;

void SetColorFill(int isNOP,char *str,char *clr) ;
void SetColorStroke(int isNOP,char *str,char *clr) ;
void SetColor_IFID(int isNOP) ;
void SetColors_IDEX(int isNOP,int op_code) ;
void SetColors_EXMEM(int isNOP,int op_code) ;
void SetColors_MEMWB(int isNOP,int op_code) ;
void SetColors_WB(int isNOP,int op_code) ;
void SetInstr(int isNOP,int ins,char *instr) ;
void setInvisible(char *str,int isNOP) ;

#endif