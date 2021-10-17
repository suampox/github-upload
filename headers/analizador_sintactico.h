#ifndef ANALIZADOR_SINTACTICO_H
#define ANALIZADOR_SINTACTICO_H

/*Itera indefinidamente, invocando en cada iteración a la función
 sig_componente_lexico() del analizador léxico. La iteración finaliza cuando el componente
 léxico recibido tiene como identificador un número negativo. Este suceso indicará
 que se ha alcanzado el final del fichero.*/
void iterar();

#endif /* ANALIZADOR_SINTACTICO_H */

