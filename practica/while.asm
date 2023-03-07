	# ciclo con indice creciente
	li $t1, 1 #i = 1
	li $s0, 0 # variable = 0

	Mientras:
		bgt $t1,100, Salida # Si $t1<100 hacer
		addi $s0, $s0, 2 #sumar 2 a la variable
		addi $t1, $t1, 1 # i++
		j Mientras

	Salida: #finaliza el codigo
	
	# ciclo con indice decreciente
	
	li $t1, 99 # i = 99 mas el cero 100 (n-1 veces)
	li $s0, 0 # variable = 0

	Mientras:
		bgt $zero,$t1, Salida # Si 0<$t1 hacer
		addi $s0, $s0, 2 #sumar 2 a la variable
		addi $t1, $t1, -1 # i++ o i--
		j Mientras

	Salida:
