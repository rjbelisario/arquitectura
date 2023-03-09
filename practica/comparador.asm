	li $s0, 4 # a
	li $s1, 2 # b
	li $v0, 1
	jal func
	sge $a0,$v1,$zero
	syscall
	li $v0,10
	syscall
	
	func:
		sub $v1,$s0,$s1
		jr $ra
