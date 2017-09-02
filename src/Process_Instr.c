#include "Process_Instr.h"
#include <stdio.h>

int RegWrite(int op_code){
	return (op_code==0 || op_code==32 || op_code==35 || op_code==8 || op_code==10 || op_code==13 || op_code==15 || op_code==28 || op_code==3) ;
}
int MemtoReg(int op_code){
	return (op_code==32 || op_code==35) ;
}
int MemWrite(int op_code){
	return (op_code==40 || op_code==43) ;
}
int AluSrc(int op_code){
	return (op_code==32 || op_code==35 || op_code==40 || op_code==43 || op_code==8 || op_code==10 || op_code==13 || op_code==15) ; 
}
int RegDst(int op_code){
	return (op_code==32 || op_code==35 || op_code==40 || op_code==43 || op_code==8 || op_code==10 || op_code==13 || op_code==15) ;
}
int Branch(int op_code){
	return (op_code==1 || op_code==4 || op_code==6 || op_code==7) ;
}
int extract_OpCode(int ins){
	return ((ins>>26)&((1<<6)-1)) ; 
}
int extract_Operation(int ins){
	return	ins&((1<<6)-1) ; 
}
int extract_Reg1(int ins){
	int op_code = extract_OpCode(ins) ;
	int operation = extract_Operation(ins) ;
	if(op_code==0 && (operation==18))
		return 33 ;		 
	return ((ins>>21)&((1<<5)-1)) ;
}
int extract_Reg2(int ins){
	return ((ins>>16)&((1<<5)-1)) ;
}
int extract_Shamt(int ins){
	return ((ins>>6)&((1<<5)-1)) ;
}
int extract_Reg_Dest(int ins){
	int op_code = extract_OpCode(ins) ;	
	int operation = extract_Operation(ins) ;
	if(op_code==28 || (op_code==0 && operation==24))
		return 33 ;
	if(RegDst(op_code)) return extract_Reg2(ins) ;
	if(op_code==3) return 31 ;
	return ((ins>>11)&((1<<5)-1)) ;
}
int sign_extend(int i,int cap){
	return (i<<(32-cap))>>(32-cap) ;
}
int need_reg2(int op_code){
	return (op_code==0 || op_code==4 || op_code==28 || op_code==40 || op_code==43) ;
}
int parseIns(int ins){
	int op_code = extract_OpCode(ins) ;
	int operation = extract_Operation(ins) ;
	int bit = extract_Reg2(ins) ;
	if(op_code==0){
		switch(operation){
			case 0  : { return  1  ;}
			case 4  : { return  1  ;}
			case 8  : { return  1  ;}
			case 9  : { return  1  ;}
			case 18 : { return  1  ;}
			case 24 : { return  1  ;}
			case 32 : { return  1  ;}
			case 33 : { return  1  ;}
			case 34 : { return  1  ;}
			case 36 : { return  1  ;}
			case 37 : { return  1  ;}
			case 39 : { return  1  ;}			
			case 43 : { return  1  ;}
			default : { return  0  ;}
		}
	}
	if(op_code==1) return 1 ;
	if(op_code==2) return 1 ;
	if(op_code==3) return 1 ;
	if(op_code==4) return 1 ;
	if((op_code==6 || op_code==7) && bit==0) return 1 ;
	if(op_code==8)  return 1  ;  
	if(op_code==10) return 1  ;
	if(op_code==13) return 1  ;
	if(op_code==15) return 1  ;
	if(op_code==28) return 1  ;
	if(op_code==32) return 1  ;
	if(op_code==35) return 1  ;
	if(op_code==40) return 1  ;
	if(op_code==43) return 1  ;
	printf("%d %d\n",op_code,operation) ;
	return 0 ;

}

char* type_of_instr(int op_code,int operation,int bit){
	if(op_code==0){
		switch(operation){
			case 0  : { return  "sll"  ;}
			case 4  : { return  "sllv" ;}
			case 8  : { return  "jr"   ;}
			case 9  : { return  "jalr" ;}
			case 18 : { return  "mflo" ;}			
			case 24 : { return  "mul"  ;}
			case 32 : { return  "add"  ;}
			case 33 : { return  "move" ;}
			case 34 : { return  "sub"  ;}
			case 36 : { return  "and"  ;}
			case 37 : { return  "or"   ;}
			case 39 : { return  "nor"  ;}			
			case 43 : { return  "slt"  ;}
			default :   return  "Bad" ;
		}
	}	
	if(op_code==1){ 
		if(bit==1) 
			return  "bgez" ; 
		else if(bit==0) 
			return "bltz" ; 
		else 
			return "Bad" ;
	}
	if(op_code==2) return "j" ;
	if(op_code==3) return "jal" ;	
	if(op_code==4)  return "beq"  ;
	if(op_code==6)  return "blez" ;
	if(op_code==7)  return "bgtz" ;	
	if(op_code==8)  return "addi" ;
	if(op_code==10) return "slti" ;
	if(op_code==13) return "ori"  ;
	if(op_code==15) return "lui"  ;
	if(op_code==28) return "madd" ;
	if(op_code==32) return "lb"   ;
	if(op_code==35) return "lw"   ;
	if(op_code==40) return "sb"   ;
	if(op_code==43) return "sw"   ;
	return "Bad" ;
}

