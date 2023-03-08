	li $s0, 0 # a
	li $s1, 2 # b
	li $v0, 1
	j sub
	
	sub:
		sub $s2,$s0,$s1
		j Main
	Main: 
		bge $s2,$zero, True
		li $a0, 0
		syscall
		j EXIT
	
	True:
		li $a0, 1
		syscall
		j EXIT
	EXIT:
