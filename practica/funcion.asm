	main:

		li $a1, 100
		li $a2, 500

		jal suma

		li $v0, 1
		addi $a0, $v1, 0
		syscall
	li $v0, 10
	syscall

	suma:
		add $v1, $a1, $a2
		jr $ra
