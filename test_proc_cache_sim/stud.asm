.text
addi $21, $21, 0
addi $4, $4, 1000
Lab1:
	addi $7, $21, 1
	sw $7, 0($3)
	addi $2, $21, 0
	addi $6, $21, 1
	addi $5, $21, 1
	addi $10, $21, 15
	addi $9, $21, 1
	addi $12, $21, 0
Look:
	addi $9, $9, 1
  Say:
  	lw $7, 0($3)
  	addi $2, $2, 1
  	addi $6, $6, 1
  	add $0, $21, $6
  	bge $5, hi
  finish:
  	sw $2, 0($4)
  	addi $4, $4, 4
  	addi $12, $12, 1
  	sw $7, 0($4)
  	addi $4, $4, 4
  	addi $12, $12, 1
  	addi $2, $21, 0
  	add $0, $21, $6
  	bge $5, Say
  	
