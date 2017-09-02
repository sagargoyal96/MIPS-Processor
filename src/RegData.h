#ifndef REG_DATA_H_
#define REG_DATA_H_


typedef struct Instruct{
	int val ;
	int isNOP ;
} Instruct ;

typedef struct DataIFID{
	Instruct ict ;	
	int PC_val ;
} DataIFID ;

typedef struct DataIDEX{
	Instruct ict ;
	int reg_adr1 ;
	int reg_adr2 ;
	int opr1 ;
	int opr2 ;
	int Imm ;
	int PC_val ;
} DataIDEX ;

typedef struct DataEXMEM{
	Instruct ict ;
	long long int alu_res ;
	int str_res ;
	int PC_val ;
} DataEXMEM ;

typedef struct DataMEMWB{
	Instruct ict ;
	int ldr_res ;
	long long int alu_res ;
} DataMEMWB ;

extern DataIFID regIFID_buff ;
extern DataIFID regIFID_act ;

extern DataIDEX regIDEX_buff ;
extern DataIDEX regIDEX_act ;

extern DataEXMEM regEXMEM_buff ;
extern DataEXMEM regEXMEM_act ;

extern DataMEMWB regMEMWB_buff ;
extern DataMEMWB regMEMWB_act ;

extern int stall,fwdA,fwdB,fwdC ;

void initDataIFID(DataIFID *obj) ;
void initDataIDEX(DataIDEX *obj) ;
void initDataEXMEM(DataEXMEM *obj) ;
void initDataMEMWB(DataMEMWB *obj) ;


#endif 
