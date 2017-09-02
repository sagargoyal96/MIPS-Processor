#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <pthread.h>
#include "RegData.h"
#include "Colors.h"
#include "Process_Instr.h"
#include "cache.h"
#include "Processor.h"

const int MEM_LIMIT = 16777216 ;
int instr_lim ;

int instr[8192] ;
int breakpt[8192] ;
int mem[16777216] ;
int reg[32] ;
int Hi ;
int Lo ;
int PC ;

int cycles,I_cache,D_cache ;
int instr_impl ;

int stall ;
int fwdA,fwdB,fwdC ;
int pgrm_fns ;

double freq ;
int latency ;
int perfect ;

FILE *cssFile ;
FILE *jsfile ;
FILE *ofil ;

DataIFID regIFID_buff ;
DataIFID regIFID_act ;

DataIDEX regIDEX_buff ;
DataIDEX regIDEX_act ;

DataEXMEM regEXMEM_buff ;
DataEXMEM regEXMEM_act ;

DataMEMWB regMEMWB_buff ;
DataMEMWB regMEMWB_act ;

int checkFinishCondition(){
	int res = 1 ;
	res=(res && regIFID_act.ict.isNOP) ;
	res=(res && regIDEX_act.ict.isNOP) ;
	res=(res && regEXMEM_act.ict.isNOP) ;
	res=(res && regMEMWB_act.ict.isNOP) ;
	res=(res && (PC>=instr_lim)) ;
	return res ;
}
void initReg(){
	initDataIFID(&regIFID_act) ;
	initDataIDEX(&regIDEX_act) ;
	initDataEXMEM(&regEXMEM_act) ;
	initDataMEMWB(&regMEMWB_act) ;
}

int addWithOverflow(int op1,int op2){
	int sum = op1+op2 ; 
	if((sum<0 && op1>0 && op2>0) || (sum>0 && op1<0 && op2<0)){
		printf("Overflow Occured in addition\n") ;
		exit(0) ;
	}else
		return sum ;
}
long long int aLU(int op1,int op2,int ins){
	int op_code = extract_OpCode(ins) ;
	int operation = extract_Operation(ins) ;
	if(op_code==0){
		switch(operation){
			case 0  : { return  (op2<<extract_Shamt(ins)) ;}
			case 4  : { return  (op2<<op1) ;}
			case 8  : { return op1 ;}
			case 9  : { return op1 ;}
			case 18 : { return op1 ;}
			case 24 : { return  (((long long int)op1)*op2)  ;}
			case 32 : { return addWithOverflow(op1,op2) ;}
			case 33 : { return op1+op2 ; }
			case 34 : { return addWithOverflow(op1,-op2) ;}
			case 36 : { return  (op1&op2)  ;}
			case 37 : { return  (op1|op2)  ;}
			case 39 : { return ~(op1|op2)  ;}			
			case 43 : { return (((unsigned int)op1)<((unsigned int)op2) ? 1 : 0) ; }
			default :   return 0 ;
		}
	}	
	if(op_code==1 || op_code==7 || op_code==6) return (op1) ;
	if(op_code==2 || op_code==3) return op1 ;
	if(op_code==4) return (op1-op2) ;
	if(op_code==8)  return addWithOverflow(op1,op2) ;
	if(op_code==10) return (((unsigned int)op1)<((unsigned int)op2) ? 1 : 0) ;
	if(op_code==13) return (op1|op2) ;
	if(op_code==15) return (op2<<16) ;
	if(op_code==28) return (((long long int) op1)*op2) ;
	if(op_code==32 || op_code==35 || op_code==40 || op_code==43) return (op1+op2) ;
	return 0 ;
}
void *thread_IFID_fun(void *arg){
	if(!stall){
		regIFID_buff.PC_val = PC+1 ;
		if(PC>=8192){
			printf("The PC is out of bounds\n") ;
			exit(-1) ;
		}
		else{	
			regIFID_buff.ict.val = instr[PC] ;
		}
		regIFID_buff.ict.isNOP = (PC>=instr_lim ? 1 : 0) ;
		if(PC<instr_lim){ 
			I_cache++ ;
			perform_access((unsigned) (PC*4+0x00400000),2) ;
			//printf("%d %08x\n",2,(unsigned) (PC*4+0x00400000)) ;			
		}
		SetInstr(regIFID_buff.ict.isNOP,regIFID_buff.ict.val,"Instruction1") ; 
	}
	//SetColor_IFID((stall||PC>=instr_lim)) ;
	return NULL ;
}
void *thread_IDEX_fun(void *arg){
	int ins = regIFID_act.ict.val ;
	int op_code = extract_OpCode(ins) ;	
	int operation = extract_Operation(ins) ;	
	if(!stall){
		regIDEX_buff.PC_val = regIFID_act.PC_val ;
		regIDEX_buff.ict = regIFID_act.ict ;	
		regIDEX_buff.reg_adr1 = extract_Reg1(ins) ;
		regIDEX_buff.reg_adr2 = extract_Reg2(ins) ;
		regIDEX_buff.Imm = sign_extend(ins,16) ;
		SetInstr(regIFID_act.ict.isNOP,regIFID_act.ict.val,"Instruction2") ;
	}
	//SetColors_IDEX((regIFID_act.ict.isNOP||stall),op_code) ;
	return NULL ;
}
void *thread_EXMEM_fun(void *arg){
	int ins = (regIDEX_act.ict.val) ;
	int op_code = extract_OpCode(ins) ;
	int operation = extract_Operation(ins) ;
	//SetColors_EXMEM(regIDEX_act.ict.isNOP,op_code) ;
	SetInstr(regIDEX_act.ict.isNOP,regIDEX_act.ict.val,"Instruction3") ;
	regEXMEM_buff.ict = regIDEX_act.ict ;
	if(stall) regEXMEM_buff.ict.isNOP=1 ;
	int op_code_MEM = extract_OpCode(regMEMWB_act.ict.val) ; 
	int val1=0,val2=0 ;	
	switch(fwdA){
		case 0 : {val1 = regIDEX_act.opr1 ;}
		break ;
		case 1 : {val1 = (MemtoReg(op_code_MEM) ? regMEMWB_act.ldr_res : (int) regMEMWB_act.alu_res) ;}
		break ;
		case 2 : {val1 = ((int) regEXMEM_act.alu_res) ;}
		break ;
	}
	switch(fwdB){
		case 0 : {val2 = regIDEX_act.opr2 ;}
		break ;
		case 1 : {val2 = (MemtoReg(op_code_MEM) ? regMEMWB_act.ldr_res : (int) regMEMWB_act.alu_res) ;}
		break ;
		case 2 : {val2 = ((int) regEXMEM_act.alu_res) ;}
		break ;
	}
	if(op_code==2 || op_code==3){
		regEXMEM_buff.PC_val = (((((regIDEX_act.PC_val*4+0x00400000)-4)>>2)&(0x3C000000))|(ins&0x03FFFFFF))-(0x00400000/4) ;
	}else if(op_code==0 && (operation==8 || operation==9)){
		regEXMEM_buff.PC_val = val1 ;
	}
	else{
		regEXMEM_buff.PC_val = (regIDEX_act.PC_val+regIDEX_act.Imm) ; 
	}
	
	int op1 = val1 ;
	if(op_code==2 || op_code==3 || (op_code==0 && (operation==9 || operation==8)))
		op1 = (regIDEX_act.PC_val) ;
	int op2 = (AluSrc(op_code) ? regIDEX_act.Imm : val2) ;

	regEXMEM_buff.alu_res = aLU(op1,op2,ins) ;
	regEXMEM_buff.str_res = val2 ;
	return NULL ;
}
void *thread_MEMWB_fun(void *arg){
	int ins = regEXMEM_act.ict.val ;
	int op_code = extract_OpCode(ins) ;
	//SetColors_MEMWB(regEXMEM_act.ict.isNOP,op_code) ;
	SetInstr(regEXMEM_act.ict.isNOP,regEXMEM_act.ict.val,"Instruction4") ;
	regMEMWB_buff.ict = regEXMEM_act.ict ;
	regMEMWB_buff.alu_res = regEXMEM_act.alu_res ;
	if(!regEXMEM_act.ict.isNOP){
		int op_code_MEM = extract_OpCode(regMEMWB_act.ict.val) ; 
		int val=0 ;
		if(fwdC)
			val = (MemtoReg(op_code_MEM) ? regMEMWB_act.ldr_res : (int) regMEMWB_act.alu_res) ;
		else
			val = regEXMEM_act.str_res ;

		int addr = (int) (regEXMEM_act.alu_res/4)-67125248 ;
		int pos = (3-(((int) regEXMEM_act.alu_res)%4))*8 ;
		if(MemWrite(op_code) || MemtoReg(op_code)){
			D_cache++ ;
			if(MemWrite(op_code)){
				perform_access(((unsigned) regEXMEM_act.alu_res),1) ;
				//printf("%d %08x\n",1,((unsigned) regEXMEM_act.alu_res)) ;
			}
			else{
				perform_access(((unsigned) regEXMEM_act.alu_res),0) ;
				//printf("%d %08x\n",1,((unsigned) regEXMEM_act.alu_res)) ;
			}

			if(addr<0 || addr>=MEM_LIMIT){
				printf("The adress is out of bounds :0x%08x\n",(int) regEXMEM_act.alu_res) ;
				exit(-1) ;
			}
		}
		if(op_code==43){
			if(pos!=24){printf("The adress is not a multiple of 4 for store instruction :0x%08x\n",(int) regEXMEM_act.alu_res) ; exit(-1) ;}			
			mem[addr]=val ;
		}else if(op_code==40){
			mem[addr]&=(~(((1<<8)-1)<<(pos))) ;
			val&=((1<<8)-1) ;
			val = val<<(pos) ;
			mem[addr]|=val ;
		}else if(op_code==35){
			if(pos!=24){printf("The adress is not a multiple of 4 for load instruction :0x%08x\n",(int) regEXMEM_act.alu_res) ; exit(-1) ;}
			regMEMWB_buff.ldr_res=mem[addr] ;
		}
		else if(op_code==32)
			regMEMWB_buff.ldr_res=((mem[addr]<<(24-pos))>>24) ;		
	}else
		regMEMWB_buff.ldr_res=0 ;
	return NULL ;	
}

void *thread_WB_fun(void *arg){
	int ins = regMEMWB_act.ict.val ;
	int op_code = extract_OpCode(ins) ;
	//SetColors_WB(regMEMWB_act.ict.isNOP,op_code) ;
	SetInstr(regMEMWB_act.ict.isNOP,regMEMWB_act.ict.val,"Instruction5") ;
	if(!regMEMWB_act.ict.isNOP){
		instr_impl++ ;
		int dest = extract_Reg_Dest(ins) ;
		if(op_code==0 || op_code==3 || op_code==8 || op_code==13 || op_code==15 || op_code==10){
			int operation = extract_Operation(ins) ;
			if(operation==24){
				Hi = (int) (regMEMWB_act.alu_res>>32) ;
				Lo = (int) (regMEMWB_act.alu_res) ;					
			}
			else if(op_code!=0 || operation!=8)
				reg[dest]=(int) regMEMWB_act.alu_res ;
		}
		else if(op_code==32 || op_code==35)
			reg[dest]=regMEMWB_act.ldr_res ;
		else if(op_code==28){
			Hi += (int) (regMEMWB_act.alu_res>>32) ;
			Lo += (int) (regMEMWB_act.alu_res) ;
		}
	}
	return NULL ;
}
void Set_Controls(){		
	int insIDEX  = regIDEX_act.ict.val ;
	int insEXMEM = regEXMEM_act.ict.val ;
	int insMEMWB = regMEMWB_act.ict.val ;	
	int op_code_IDEX = extract_OpCode(insIDEX) ;	
	int op_code_EXMEM = extract_OpCode(insEXMEM) ;
	int op_code_MEMWB = extract_OpCode(insMEMWB) ;	
	int reg_write_EXMEM = (RegWrite(op_code_EXMEM) && (!regEXMEM_act.ict.isNOP)) ;
	int reg_write_MEMWB = (RegWrite(op_code_MEMWB) && (!regMEMWB_act.ict.isNOP)) ;
	int mem_read_EXMEM =  ((op_code_EXMEM==32 || op_code_EXMEM==35) && (!regIDEX_act.ict.isNOP)) ;
	fwdA=0 ;
	fwdB=0 ;
	fwdC=0 ;
	stall=0 ;
	if(reg_write_EXMEM && extract_Reg_Dest(insEXMEM)==extract_Reg1(insIDEX))
		fwdA=2 ;
	else if(reg_write_MEMWB && extract_Reg_Dest(insMEMWB)==extract_Reg1(insIDEX)){
		if(extract_Reg_Dest(insEXMEM)!=extract_Reg1(insIDEX))
			fwdA=1 ;
	}
	if(reg_write_EXMEM && extract_Reg_Dest(insEXMEM)==extract_Reg2(insIDEX))
		fwdB=2 ;
	else if(reg_write_MEMWB && extract_Reg_Dest(insMEMWB)==extract_Reg2(insIDEX)){
		if(extract_Reg_Dest(insEXMEM)!=extract_Reg2(insIDEX))
			fwdB=1 ;
	}
	if(reg_write_MEMWB && extract_Reg_Dest(insMEMWB)==extract_Reg_Dest(insEXMEM))
		fwdC=1 ;
	if(mem_read_EXMEM){
		if(extract_Reg_Dest(insEXMEM)==extract_Reg1(insIDEX))
			stall=1 ;
		else if(extract_Reg_Dest(insEXMEM)==extract_Reg2(insIDEX)){ 
			if(need_reg2(op_code_IDEX))
				stall=1 ;
		}
	}
}

void Finish_Clock_Cycle(){
	int ins = regEXMEM_act.ict.val ;
	int op_code = extract_OpCode(ins) ;
	int operation = extract_Operation(ins) ;
	int branch=0 ;
	int res = regEXMEM_act.alu_res ;
	int bit = extract_Reg2(ins) ;
	if(op_code==4 && res==0) branch=1 ;
	else if(op_code==1 && res>=0 && bit==1) branch=1 ;
	else if(op_code==7 && res>0) branch=1 ;
	else if(op_code==6 && res<=0) branch=1 ;
	else if(op_code==1 && res<0 && bit==0) branch=1 ;
	else if(op_code==2 || op_code==3) branch=1 ;
	else if(op_code==0 && (operation==8 || operation==9)) branch=1 ;
	if(!stall){
		if(branch && !regEXMEM_act.ict.isNOP){
			PC = regEXMEM_act.PC_val ; 
			regIFID_buff.ict.isNOP=1 ;
	 		regIDEX_buff.ict.isNOP=1 ;
	 		regEXMEM_buff.ict.isNOP=1 ;
		}
		else
			PC=PC+1 ;
	}

	//SetColorStroke(regEXMEM_act.ict.isNOP,"PCSrcM",(branch ? "yellow" : "grey")) ;
	//SetColorStroke(regEXMEM_act.ict.isNOP,"PC_Src_1",(branch ? "orange" : "grey")) ;
	//SetColorStroke(0,"PC_Src_0",(!branch ? "orange" : "grey")) ;
	//SetColorFill(stall,"PcSrcMux","blue") ;
	regIFID_act=regIFID_buff ;
	if(regIDEX_buff.reg_adr1==33)
		regIDEX_buff.opr1 = Lo ;
	else	
		regIDEX_buff.opr1 = reg[regIDEX_buff.reg_adr1] ;
	regIDEX_buff.opr2 = reg[regIDEX_buff.reg_adr2] ;
	regIDEX_act=regIDEX_buff ;
	regEXMEM_act=regEXMEM_buff ;
	regMEMWB_act=regMEMWB_buff ;
	pgrm_fns = checkFinishCondition() ;
} 
void Simulate_Clock(){
	Set_Controls() ;
	cssFile = fopen("MySVG.css","w") ;
	jsfile = fopen("inst.js","w") ;
	fprintf(jsfile,"function add_text(){\n") ;
	void *exit_status[5] ;
	pthread_t thread_IFID ;
	pthread_t thread_IDEX ;
	pthread_t thread_EXMEM ;
	pthread_t thread_MEMWB ;
	pthread_t thread_WB ;
	pthread_create(&thread_IFID,NULL,thread_IFID_fun,NULL) ;
	pthread_create(&thread_IDEX,NULL,thread_IDEX_fun,NULL) ;
	pthread_create(&thread_EXMEM,NULL,thread_EXMEM_fun,NULL) ;
	pthread_create(&thread_MEMWB,NULL,thread_MEMWB_fun,NULL) ;
	pthread_create(&thread_WB,NULL,thread_WB_fun,NULL) ;
	pthread_join(thread_IFID,&exit_status[0]) ;
	pthread_join(thread_IDEX,&exit_status[1]) ;
	pthread_join(thread_EXMEM,&exit_status[2]) ;
	pthread_join(thread_MEMWB,&exit_status[3]) ;
	pthread_join(thread_WB,&exit_status[4]) ;
	setInvisible("Fetch",stall) ;
	setInvisible("Decode",regIFID_act.ict.isNOP || stall) ;
	setInvisible("Execute",regIDEX_act.ict.isNOP) ;
	setInvisible("Memory",regEXMEM_act.ict.isNOP) ;
	setInvisible("Write-Back",regMEMWB_act.ict.isNOP) ;
	Finish_Clock_Cycle() ;
	fprintf(jsfile,"}\n") ;
	fclose(cssFile) ;
	fclose(jsfile) ;
}
void printReg(){
	printf("\n") ;
	for(int i=0 ; i<32 ; i++)
		printf("$%02d: 0x%08X\n",i,reg[i]) ;
	printf("hi: 0x%08X\n",Hi) ;
	printf("lo: 0x%08X\n",Lo) ;
	printf("pc: 0x%08X\n",(4*PC+0x0040000)) ;
	printf("\n") ;

}
void printMEM(int start_addr,int addr_ct){
	printf("\n") ;
	for(int i=0 ; i<addr_ct ; i++){
		int addr = ((start_addr+i)/4)-67125248 ;
		int pos = (3-((start_addr+i)%4))*8 ;
		if(addr<0 || addr>=MEM_LIMIT)
			printf("The addr is out of bounds:0x%08x\n",start_addr+i) ;
		else{
			int val = ((mem[addr]<<(24-pos))>>24)&((1<<8)-1) ;
			printf("0x%08x: 0x%02x\n",(start_addr+i),val) ;
		}
	}
	printf("\n") ;
}
void printStatus(){
	printf("\nPC:%d\n\n",PC) ;

	printf("DataIFID-\n") ;
	printf("Instruct:%d\n",regIFID_act.ict.val) ;
	printf("isNOP:%d\n",regIFID_act.ict.isNOP);
	printf("op_code:%d\n",extract_OpCode(regIFID_act.ict.val)) ;
	printf("PC_val:%d\n\n",regIFID_act.PC_val) ;

	printf("DataIDEX-\n") ;
	printf("Instruct:%d\n",regIDEX_act.ict.val) ;
	printf("isNOP:%d\n",regIDEX_act.ict.isNOP);
	printf("op_code:%d\n",extract_OpCode(regIDEX_act.ict.val)) ;
	printf("operation:%d\n",extract_Operation(regIDEX_act.ict.val)) ;
	printf("PC_val:%d\n",regIDEX_act.PC_val) ;
	printf("reg_adr1:%d\n",regIDEX_act.reg_adr1) ;
	printf("reg_adr2:%d\n",regIDEX_act.reg_adr2) ;
	printf("opr1:%d\n",regIDEX_act.opr1) ;
	printf("opr2:%d\n",regIDEX_act.opr2) ;
	printf("Imm:%d\n\n",regIDEX_act.Imm) ;

	printf("DataEXMEM-\n") ;	
	printf("Instruct:%d\n",regEXMEM_act.ict.val) ;
	printf("isNOP:%d\n",regEXMEM_act.ict.isNOP);
	printf("op_code:%d\n",extract_OpCode(regEXMEM_act.ict.val)) ;
	printf("operation:%d\n",extract_Operation(regEXMEM_act.ict.val)) ;	
	printf("PC_val:%d\n",regEXMEM_act.PC_val) ;
	printf("alu_res:%d\n",(int) regEXMEM_act.alu_res) ;
	printf("str_res:%d\n\n",regEXMEM_act.str_res) ;

	printf("DataMEMWB-\n") ;
	printf("Instruct:%d\n",regMEMWB_act.ict.val) ;
	printf("isNOP:%d\n",regMEMWB_act.ict.isNOP);
	printf("op_code:%d\n",extract_OpCode(regMEMWB_act.ict.val)) ;
	printf("ldr_res:%d\n",regMEMWB_act.ldr_res) ;
	printf("alu_res:%d\n\n",(int) regMEMWB_act.alu_res) ;
}
void printResults(){	
	Pcache_stat stat_inst = get_Inst_stat() ;
	Pcache_stat stat_data = get_Data_stat() ;
	fprintf(ofil,"Instructions,%d\n",instr_impl) ;
	fprintf(ofil,"Cycles,%d\n",cycles) ;
	fprintf(ofil,"IPC,%.4f\n",(instr_impl+0.0)/(cycles)) ;
	fprintf(ofil,"Time (ns),%.4f\n",cycles/freq) ;
	fprintf(ofil,"Idle time (ns),%.4f\n",(cycles-instr_impl)/freq) ;
	fprintf(ofil,"Idle time (%%),%.4f%%\n",((cycles-instr_impl)*100)/(cycles+0.0)) ;
	fprintf(ofil,"Cache Summary\n") ;

	fprintf(ofil,"Cache L1-I\n") ;
	if(!perfect) I_cache = (stat_inst->accesses) ;
	int misses_inst = (perfect ? 0 : (stat_inst->misses)) ;
	fprintf(ofil,"num cache accesses,%d\n",I_cache) ;
	fprintf(ofil,"num cache misses,%d\n",misses_inst) ;
	fprintf(ofil,"miss rate,%.4f%%\n",(misses_inst*100)/(I_cache+0.0)) ;

	fprintf(ofil,"Cache L1-D\n") ;
	if(!perfect) D_cache = (stat_data->accesses) ;
	int misses_data = (perfect ? 0 : (stat_data->misses)) ;
	fprintf(ofil,"num cache accesses,%d\n",D_cache) ;
	fprintf(ofil,"num cache misses,%d\n",misses_data) ;
	fprintf(ofil,"miss rate,%.4f%%\n",(misses_data*100)/(D_cache+0.0)) ;

	fprintf(ofil,"DRAM summary\n") ;
	int dram_acc_read = (perfect ? 0 : ((stat_data->demand_fetches)+(stat_inst->demand_fetches))) ;
	int dram_acc_write = (perfect ? 0 : ((stat_data->copies_back)+(stat_inst->copies_back))) ; 
	fprintf(ofil,"num dram accesses,%d\n",dram_acc_read+dram_acc_write) ;
	double miss_rate = (misses_data+misses_inst+0.0)/(I_cache+D_cache) ;
	double avglaten = (miss_rate*latency)+(1-miss_rate) ;
	fprintf(ofil,"average dram access latency (ns),%.4f\n",avglaten) ;
	fclose(ofil) ;
}
int HextoInt(char c[8]){
	int res=0 ;
	int val=0 ;
	for(int i=0 ; i<8 ; i++){		
		if(c[i]>='0' && c[i]<='9')
			val=(c[i]-'0') ;
		else if(c[i]>='a' && c[i]<='f')
			val=((c[i]-'a')+10) ;
		else{
			printf("The given hex code is Invalid : %s\n",c) ;
			exit(-1) ;
		}
		res|=(val<<((7-i)*4)) ;		
	}
	if(!parseIns(res)){
		printf("The given instruction is not supported : %s\n",c) ;
		exit(-1) ;
	}
	return res ;
}
void SetImagetoBase(){
	FILE* base = fopen("Base.css","r") ;	
	cssFile = fopen("MySVG.css","w") ;
	jsfile = fopen("inst.js","w") ;
	char a ;
	do {
      a = fgetc(base) ;
      fputc(a,cssFile) ; 
    }while(a != EOF) ;
    fprintf(jsfile,"function add_text(){\n}\n") ;
    fclose(base) ;
    fclose(cssFile) ;
    fclose(jsfile) ;
}

int run_Processor(int args,char **argv){	
	reg[29]=0x14010000 ;
	SetImagetoBase() ;	
	ofil = fopen(argv[4],"w") ;	
	FILE* infil = fopen(argv[1],"r") ;
	if(infil==NULL){
		printf("Sorry the the required file is not available:%s\n",argv[1]) ;
		exit(-1) ;
	}
	FILE* out_svg = fopen(strcat(argv[3],".html"),"w") ;
	fprintf(out_svg,"<!DOCTYPE html>\n<html>\n<head>\n<meta http-equiv=\"refresh\" content=\"1.5\"/>\n</head>\n<body>\n<object id=\"mysvg\" type=\"image/svg+xml\" data=\"drawin.svg\">\n  Your browser does not support SVG\n</object>\n</body>\n</html>") ;
	fclose(out_svg) ;
	initReg() ;
	int ptr=0 ;
	char itr[8] ;
	char buff[255] ;	
	int i ;
	for(i=0 ; i<8 ; i++) itr[i] = '\0' ;
	for(i=0 ; i<255 ; i++) buff[i] = '\0' ;
	while(!feof(infil)){
		fscanf(infil,"%0255s",buff) ;
		int len=0 ;
		while(buff[len]!='\0') len++ ;
		if(len!=8){
			if(len!=0){
				printf("The given hex code is invalid : %s\n",buff) ;
				exit(-1) ;
			}
		}
		else{
			for(i=0 ; i<8 ; i++) itr[i]=buff[i] ;
			instr[ptr++]=HextoInt(itr) ;
			for(i=0 ; i<8 ; i++) itr[i] = '\0' ;
			for(i=0 ; i<255 ; i++) buff[i] = '\0' ;
		}

	}
	fclose(infil) ;
	instr_lim = ptr ;
	char c[20] ;
	while(!pgrm_fns){
		scanf("%s",c) ;
		if(!strcmp(c,"step")){
			Simulate_Clock() ;
			cycles++ ;
		}else if(!strcmp(c,"regdump"))
			printReg() ;
		else if(!strcmp(c,"memdump")){
		 	int start_addr=0 ;
		 	int addr_ct=0 ;
		 	scanf(" 0x%08x %d",&start_addr,&addr_ct) ; 
			printMEM(start_addr,addr_ct) ;
		}
		else if(!strcmp(c,"break")){
			int pt ;
			scanf(" 0x%08x",&pt) ;
			breakpt[(pt-0x00400000)/4]=1 ;
		}else if(!strcmp(c,"delete")){
			int pt ;
			scanf(" 0x%08x",&pt) ;
			if(breakpt[(pt-0x00400000)/4])
				breakpt[(pt-0x00400000)/4]=0 ;
			else
				printf("There was not breakpoint at that location\n") ;
		}else if(!strcmp(c,"continue")){
			do{
				Simulate_Clock() ;
				cycles++ ;
			}while(!breakpt[PC] && !pgrm_fns);
		}else if(!strcmp(c,"run")){
			while(!pgrm_fns){
				Simulate_Clock() ;
				cycles++ ;
			}
		}
		else if(!strcmp(c,"P"))
		 	printStatus() ;
		else if(!strcmp(c,"exit"))
		 	break ;
		else
		 	printf("Invalid Command\n") ;
	}	
	flush() ;
	printResults() ;
	SetImagetoBase() ;
	return 0 ;
}
