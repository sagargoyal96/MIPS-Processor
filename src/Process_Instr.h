#ifndef PROCESS_INSTR_H_
#define PROCESS_INSTR_H_

int RegWrite(int op_code);
int MemtoReg(int op_code);
int MemWrite(int op_code);
int AluSrc(int op_code);
int RegDst(int op_code);
int Branch(int op_code);
int extract_OpCode(int ins);
int extract_Operation(int ins);
int extract_Reg1(int ins);
int extract_Reg2(int ins);
int extract_Shamt(int ins);
int extract_Reg_Dest(int ins);
int sign_extend(int i,int cap);
int need_reg2(int op_code);
int parseIns(int ins);
char* type_of_instr(int op_code,int operation,int bit);

#endif 