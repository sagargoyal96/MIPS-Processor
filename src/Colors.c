#include "Colors.h"
#include "Process_Instr.h"
#include "RegData.h"
#include <stdio.h>
#include <stdlib.h>

void SetColorFill(int isNOP,char *str,char *clr){
	if(!isNOP)
		fprintf(cssFile,"#%s{\n\tfill:%s !important\n}\n",str,clr) ;	
	else
		fprintf(cssFile,"#%s{\n\tfill:grey !important\n}\n",str) ;
}
void SetColorStroke(int isNOP,char *str,char *clr){
	if(!isNOP)
		fprintf(cssFile,"#%s{\n\tstroke:%s !important\n}\n",str,clr) ;	
	else
		fprintf(cssFile,"#%s{\n\tstroke:grey !important\n}\n",str) ;
}

void SetColor_IFID(int isNOP){
	SetColorFill(isNOP,"InstructionMemory","blue") ;
	SetColorFill(isNOP,"PCPlus4Adder","blue") ;
	SetColorFill(stall,"PC","blue") ;
	SetColorStroke(isNOP,"InstrMemOut","orange") ;
	SetColorStroke(isNOP,"PC_in","orange") ;
	SetColorStroke(isNOP,"PCOut","orange") ;
	SetColorStroke(isNOP,"PC_val","orange") ;
	SetColorStroke(isNOP,"path7147","orange") ;
}

void SetColors_IDEX(int isNOP,int op_code){
	SetColorFill(isNOP,"ControlUnit","blue") ;
	SetColorFill(isNOP,"RegisterFile","blue") ;
	SetColorFill(isNOP,"SignExtend","blue") ;
	SetColorStroke(isNOP,"path7155","orange") ;
	SetColorStroke(isNOP,"path7157","orange") ;
	SetColorStroke(isNOP,"path7159","orange") ;
	SetColorStroke(isNOP,"path7167","orange") ;
	SetColorStroke(isNOP,"path7169","orange") ;
	SetColorStroke(isNOP,"path7171","orange") ;
	SetColorStroke(isNOP,"path7173","orange") ;
	SetColorStroke(isNOP,"path7776","orange") ;
	SetColorStroke(isNOP,"path7778","orange") ;
	SetColorStroke(isNOP,"path7780","orange") ;
	SetColorStroke(isNOP,"path7165","orange") ;
	SetColorStroke(isNOP,"path7852","orange") ;
	SetColorStroke(isNOP,"path7854","orange") ;
	SetColorStroke(isNOP,"PCPlus4M","orange") ;	
	SetColorStroke(isNOP,"RegWriteD",(RegWrite(op_code) ? "yellow" : "grey")) ;
	SetColorStroke(isNOP,"MemtoRegD",(MemtoReg(op_code) ? "yellow" : "grey")) ;
	SetColorStroke(isNOP,"MemWriteD",(MemWrite(op_code) ? "yellow" : "grey")) ;
	SetColorStroke(isNOP,"ALUControlD","yellow") ;
	SetColorStroke(isNOP,"AluSrcD",(AluSrc(op_code) ? "yellow" : "grey")) ;
	SetColorStroke(isNOP,"RegDstD",(RegDst(op_code) ? "yellow" : "grey")) ;
	SetColorStroke(isNOP,"BranchD",(Branch(op_code) ? "yellow" : "grey")) ;
	SetColorStroke(isNOP,"RD1out","orange") ;
	SetColorStroke(isNOP,"RD2out",(need_reg2(op_code) ? "orange" : "grey")) ;
}

void SetColors_EXMEM(int isNOP,int op_code){
	SetColorStroke(isNOP,"fwdA",(fwdA==2 ? "blue" : (fwdA==1 ? "yellow" : "grey"))) ;
	SetColorStroke(isNOP,"fwdB",(fwdB==2 ? "blue" : (fwdB==1 ? "yellow" : "grey"))) ;
	SetColorStroke(isNOP,"fwdA_0",(fwdA==0 ? "orange" : "grey")) ;
	SetColorStroke(isNOP,"fwdA_1",(fwdA==1 ? "orange" : "grey")) ;
	SetColorStroke(isNOP,"fwdA_2",(fwdA==2 ? "orange" : "grey")) ;
	SetColorStroke(isNOP,"fwdB_0",(fwdB==0 ? "orange" : "grey")) ;
	SetColorStroke(isNOP,"fwdB_1",(fwdB==1 ? "orange" : "grey")) ;
	SetColorStroke(isNOP,"fwdB_2",(fwdB==2 ? "orange" : "grey")) ;
	SetColorStroke(isNOP,"ALUSrc_0",((AluSrc(op_code)==0) ? "orange" : "grey")) ;
	SetColorStroke(isNOP,"ALUSrc_1",((AluSrc(op_code)==1) ? "orange" : "grey")) ;
	SetColorStroke(isNOP,"RsE","orange") ;
	SetColorStroke(isNOP,"RtE",((RegDst(op_code)==1) ? "orange" : "grey")) ;
	SetColorStroke(isNOP,"RdE",((RegDst(op_code)==0) ? "orange" : "grey")) ;
	SetColorStroke(isNOP,"RegWriteE",(RegWrite(op_code) ? "yellow" : "grey")) ;
	SetColorStroke(isNOP,"MemtoRegE",(MemtoReg(op_code) ? "yellow" : "grey")) ;
	SetColorStroke(isNOP,"MemWriteE",(MemWrite(op_code) ? "yellow" : "grey")) ;
	SetColorStroke(isNOP,"AluControlE","yellow") ;
	SetColorStroke(isNOP,"AluSrcE",AluSrc(op_code) ? "yellow" : "grey") ;
	SetColorStroke(isNOP,"RegDstE",(RegDst(op_code) ? "yellow" : "grey")) ;
	SetColorStroke(isNOP,"BranchE",Branch(op_code) ? "yellow" : "grey") ;
	SetColorStroke(isNOP,"opr1","orange") ;
	SetColorStroke(isNOP,"opr2","orange") ;
	SetColorStroke(isNOP,"ALUOutE","orange") ;
	SetColorStroke(isNOP,"WriteDataE","orange") ;
	SetColorStroke(isNOP,"WriteRegE","orange") ;
	SetColorStroke(isNOP,"BranchAddr","orange") ;
	SetColorStroke(isNOP,"PCPlus4E","orange") ;
	SetColorStroke(isNOP,"SignImmE","orange") ;
	SetColorStroke(isNOP,"path7365","orange") ;
	SetColorStroke(isNOP,"path7798","orange") ;
	SetColorStroke(isNOP,"path7796","orange") ;
	SetColorFill(isNOP,"fwdAMux","blue") ;
	SetColorFill(isNOP,"fwdBMux","blue") ;
	SetColorFill(isNOP,"RegDstMux","blue") ;
	SetColorFill(isNOP,"AluSrcMux","blue") ;
	SetColorFill(isNOP,"ALU","blue") ;
	SetColorFill(isNOP,"Shifter","blue") ;
	SetColorFill(isNOP,"BranchAdder","blue") ;
}

void SetColors_MEMWB(int isNOP,int op_code){
	SetColorFill(isNOP,"DataMemory",((MemtoReg(op_code) || MemWrite(op_code)) ? "blue" : "grey")) ;
	SetColorFill(isNOP,"BranchCheck","blue") ;
	SetColorFill(isNOP,"fwdCMux","blue") ;
	SetColorStroke(isNOP,"DataMemAddr","orange") ;
	SetColorStroke(isNOP,"fwdC_0",(fwdC==0 ? "orange" : "grey")) ;
	SetColorStroke(isNOP,"fwdC_1",(fwdC==1 ? "orange" : "grey")) ;
	SetColorStroke(isNOP,"fwdC",(fwdC==1 ? "yellow" : "grey")) ;
	SetColorStroke(isNOP,"ALUOutM","orange") ;
	SetColorStroke(isNOP,"DataMemIn","orange") ;
	SetColorStroke(isNOP,"DataMemOut",((op_code==32 || op_code==35) ? "orange" : "grey")) ;
	SetColorStroke(isNOP,"WriteRegM","orange") ;
	SetColorStroke(isNOP,"ALUOutfwd","orange") ;
	SetColorStroke(isNOP,"path4677","orange") ;
	SetColorStroke(isNOP,"path7870","orange") ;
	SetColorStroke(isNOP,"RegWriteM",RegWrite(op_code) ? "yellow" : "grey") ;
	SetColorStroke(isNOP,"MemtoRegM",MemtoReg(op_code) ? "yellow" : "grey") ;
	SetColorStroke(isNOP,"MemWriteM",MemWrite(op_code) ? "yellow" : "grey") ;
	SetColorStroke(isNOP,"BranchM",Branch(op_code) ? "yellow" : "grey") ;
}

void SetColors_WB(int isNOP,int op_code){
	SetColorFill(isNOP,"MemtoRegMux","blue") ;
	SetColorStroke(isNOP,"RegWriteData","orange") ;
	SetColorStroke(isNOP,"WriteRegW","orange") ;
	SetColorStroke(isNOP,"path7836","orange") ;
	SetColorStroke(isNOP,"ReadDataW",(MemtoReg(op_code)==1 ? "orange" : "grey")) ;
	SetColorStroke(isNOP,"AluOutW",(MemtoReg(op_code)==0 ? "orange" : "grey")) ;
	SetColorStroke(isNOP,"RegWriteW",(RegWrite(op_code)==1 ? "yellow" : "grey")) ;
	SetColorStroke(isNOP,"MemtoRegW",(MemtoReg(op_code)==1 ? "yellow" : "grey")) ;
}

void SetInstr(int isNOP,int ins,char *instr){
	int op_code = extract_OpCode(ins) ;
	int operation = extract_Operation(ins) ;
	int bit = extract_Reg2(ins) ;
	if(isNOP) 
		fprintf(jsfile,"\tdocument.getElementById('%s').textContent=\"NOP\";\n",instr) ;
	else{
		char *cr ;
		cr = type_of_instr(op_code,operation,bit) ;
		if(MemWrite(op_code) ||MemtoReg(op_code))
			fprintf(jsfile,"\tdocument.getElementById('%s').textContent=\"%s $r%d,0x%08x($r%d)\";\n",instr,cr,extract_Reg2(ins),sign_extend(ins,16),extract_Reg1(ins)) ;
		else if(op_code==1 || op_code==7 || op_code==6) 
			fprintf(jsfile,"\tdocument.getElementById('%s').textContent=\"%s $r%d,0x%08x\";\n",instr,cr,extract_Reg1(ins),sign_extend(ins,16)) ;
		else if(op_code==0){
			if(operation==32 || operation==33 || operation==34 || operation==36 || operation==37 || operation==39 || operation==43)
				fprintf(jsfile,"\tdocument.getElementById('%s').textContent=\"%s $r%d,$r%d,$r%d\";\n",instr,cr,extract_Reg_Dest(ins),extract_Reg1(ins),extract_Reg2(ins)) ;
			else if(operation==24)
				fprintf(jsfile,"\tdocument.getElementById('%s').textContent=\"%s $r%d,$r%d\";\n",instr,cr,extract_Reg1(ins),extract_Reg2(ins)) ;
			else if(operation==0)
				fprintf(jsfile,"\tdocument.getElementById('%s').textContent=\"%s $r%d,$r%d,%d\";\n",instr,cr,extract_Reg_Dest(ins),extract_Reg2(ins),extract_Shamt(ins)) ;
			else if(operation==4)
				fprintf(jsfile,"\tdocument.getElementById('%s').textContent=\"%s $r%d,$r%d,$r%d\";\n",instr,cr,extract_Reg_Dest(ins),extract_Reg2(ins),extract_Reg1(ins)) ;
			else if(operation==8)
				fprintf(jsfile,"\tdocument.getElementById('%s').textContent=\"%s $r%d\";\n",instr,cr,extract_Reg1(ins)) ;
			else if(operation==9)
				fprintf(jsfile,"\tdocument.getElementById('%s').textContent=\"%s $r%d,$r%d\";\n",instr,cr,extract_Reg1(ins),extract_Reg_Dest(ins)) ;
			else if(operation==18)
				fprintf(jsfile,"\tdocument.getElementById('%s').textContent=\"%s $r%d\";\n",instr,cr,extract_Reg_Dest(ins)) ;
		}else if(op_code==28)
			fprintf(jsfile,"\tdocument.getElementById('%s').textContent=\"%s $r%d,$r%d\";\n",instr,cr,extract_Reg1(ins),extract_Reg2(ins)) ;
		else if(op_code==4)
			fprintf(jsfile,"\tdocument.getElementById('%s').textContent=\"%s $r%d,$r%d,0x%08x\";\n",instr,cr,extract_Reg1(ins),extract_Reg2(ins),sign_extend(ins,16)) ;
		else if(op_code==8 || op_code==10 || op_code==13)
			fprintf(jsfile,"\tdocument.getElementById('%s').textContent=\"%s $r%d,$r%d,0x%08x\";\n",instr,cr,extract_Reg2(ins),extract_Reg1(ins),sign_extend(ins,16)) ;
		else if(op_code==15)
			fprintf(jsfile,"\tdocument.getElementById('%s').textContent=\"%s $r%d,0x%08x\";\n",instr,cr,extract_Reg2(ins),sign_extend(ins,16)) ;
		else if(op_code==2 || op_code==3)
			fprintf(jsfile,"\tdocument.getElementById('%s').textContent=\"%s 0x%08x\";\n",instr,cr,(ins&0x3FFFFFF)) ;
	}
}

void setInvisible(char *str,int isNOP){
	if(isNOP) fprintf(cssFile,"#%s{\n\tvisibility: hidden !important \n}\n",str) ;
}
