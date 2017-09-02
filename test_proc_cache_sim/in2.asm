main:
# initialize values to 3 registers
addi $a0,$zero,100    
jal sum  # call method

# Print out the summation upto 10 
#li $v0,1        # print integer

#add $a0,$v0,$zero   #load return value into argument 
#li $v0,1        # print integer
#syscall

#li $v0,10  # Exit
#syscall

sum:    

addi    $sp,$sp,-8      #   allocate space  on  stack   
sw  $ra,0($sp)      #   store   the return  address 
sw  $a0,4($sp)      #   store   the argument    

slti    $t0,$a0,1       #   check   if  n   >   0   
beq $t0,$0,recurse      #   n   >   0   case    
add $v0,$0,$0       #   start   return  value   to  0   
addi    $sp,$sp,8       #   pop 2   items   off stack   
jr  $ra         #   return  to  caller  

recurse:    
addi    $a0,$a0,-1      #   calculate   n-1 
jal sum         #   recursively call    sum(n-1)    

lw  $ra,0($sp)      #   restore saved   return  address 
lw  $a0,4($sp)      #   restore saved   argument    
addi    $sp,$sp,8       #   pop 2   items   off stack   

add $v0,$a0,$v0     #   calculate   n   +   sum(n-1)    
jr  $ra         #   return  to  caller 