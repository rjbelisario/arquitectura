main:
    # parametros iniciales y primera llamada
    li      $a0, 10 #veces a sumar
    li	    $a1, 15 #numero a sumar( puede venir de una operacion pero para este caso tomaremos el valor final)
    jal      SUM       # Salta a la funcion
    move      $t0, $v0        # Hacemos un backup del resultado obtenido
    
    # imprime el resultado
    li        $v0, 1        # preparamos para imprimir numero entero
    move      $a0, $t0        # movemos el numero a imprimir
    syscall                # indicacion al sistema
    # Finaliza el programa
    li        $v0, 10        # preparamos el sistema para finalizar 
    syscall #indicacion al sistema

#ALGORITMO PRINCIPAL
SUM:
    #Estructuramos los datos en una pila
    addi    $sp, $sp, -8
    sw      $s0, 4($sp)
    sw      $ra, 0($sp)
    bne     $a0, 0, llamada_recursiva
    j retorno

llamada_recursiva:
    addi    $a0, $a0, -1 # el indice decrece  ($a0)
    jal     SUM
    # se guarda el resultado en $v0
    addu $v0, $a1, $v0
retorno: #se actualiza lo datos de la pila y se retorna los nuevos valores en el registro de memoria
    lw      $s0, 4($sp)
    lw      $ra, 0($sp)
    addi    $sp, $sp, 8
    jr      $ra
