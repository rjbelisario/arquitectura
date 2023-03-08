li $s0, 0 #a
li $s1, 1 #b
li $s2, 10 #N

Main:	
	beq $s2, $zero, Caso_Base
	move $s3, $s0
	add $s0,$s0,$s1
	move $s1,$s3
	addi $s2, $s2, -1
	j Main
Caso_Base:
li $v0, 1
move $a0, $s1
syscall
