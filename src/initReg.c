#include "RegData.h"

void initDataIFID(DataIFID *obj){
	obj->ict.val=0 ;
	obj->ict.isNOP=1 ;
	obj->PC_val=0 ; 
} 

void initDataIDEX(DataIDEX *obj){
	obj->ict.val=0 ;
	obj->ict.isNOP=1 ;
	obj->opr1=0 ;
	obj->opr2=0 ;
	obj->reg_adr1=0 ;
	obj->reg_adr2=0 ;
	obj->Imm=0 ;
	obj->PC_val=0 ;
} 

void initDataEXMEM(DataEXMEM *obj){
	obj->ict.val=0 ;
	obj->ict.isNOP=1 ;
	obj->alu_res=0 ;
	obj->str_res=0 ;
	obj->PC_val=0 ;
} 

void initDataMEMWB(DataMEMWB *obj){
	obj->ict.val=0 ;
	obj->ict.isNOP=1 ;
	obj->ldr_res=0 ;
	obj->alu_res=0 ;
} 
