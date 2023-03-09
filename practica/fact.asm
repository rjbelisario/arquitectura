	li $s0, 4 #limite
	li $s1, 1 #indice
	li $s2, 1
Main:
	bge  $s1,$s0, EXIT
	add $s1, $s1, 1 
	mul $s2,$s2,$s1
	j Main
	
EXIT:
	li $v0, 1
	move $a0, $s2
	syscall
