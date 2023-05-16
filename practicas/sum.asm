main:
    # Parametros iniciales y primera llamada
    li      $a0, 10 # Veces a sumar
    li      $a1, 15 # Numero a sumar (puede venir de una operacion pero para este caso tomaremos el valor final)
    jal     SUM     # Salta a la funcion
    move    $t0, $v0        # Hacemos un backup del resultado obtenido
    
    # Imprime el resultado
    li      $v0, 1        # Preparamos para imprimir numero entero
    move    $a0, $t0        # Movemos el numero a imprimir
    syscall                # Indicacion al sistema
    
    # Finaliza el programa
    li      $v0, 10        # Preparamos el sistema para finalizar 
    syscall                 # Indicacion al sistema

# Funcion SUM
SUM:
    # Estructuramos los datos en una pila
    addi    $sp, $sp, -8
    sw      $s0, 4($sp)
    sw      $ra, 0($sp)
    
    # Verificamos si se ha llegado al final de la recursion
    bne     $a0, 0, llamada_recursiva
    j       retorno

llamada_recursiva:
    # Decrementamos el indice de la recursion ($a0)
    addi    $a0, $a0, -1
    
    # Llamamos a la funcion SUM recursivamente
    jal     SUM
    
    # Sumamos el valor constante ($a1) al valor de retorno de la funcion
    addu    $v0, $v0, $a1

retorno:
    # Restauramos los datos de la pila y retornamos los nuevos valores en el registro de memoria
    lw      $s0, 4($sp)
    lw      $ra, 0($sp)
    addi    $sp, $sp, 8
    jr      $ra
