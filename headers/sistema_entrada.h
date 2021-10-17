#ifndef SISTEMA_ENTRADA_H
#define SISTEMA_ENTRADA_H
#include <stdio.h>
#include <stdlib.h>
#define N 200 //Tamaño de cada uno de los dos bloques del sistema de entrada (sin incluir los EOF)

/*Devuelve el valor al que apunta el puntero delantero (en caso de que sea necesario,
 * carga un nuevo bloque) y actualiza el valor del puntero delantero.*/
int siguiente_caracter();

/*Carga el primero de los dos bloques y asigna el valor EOF en la última
 posición de cada uno de los dos bloques.*/
void inicializa_sistema_entrada(FILE *fp);

/*Reserva memoria para el lexema detectado y copia el lexema en dicha memoria
 reservada. Devuelve un puntero al lexema. El lexema detectado está formado por los
 caracteres situados entre el puntero inicio y el puntero delantero.
 El entero pasado como primer argumento indica que para el último lexema reconocido 
 se invocó a la función devolver(), de manera que el puntero delantero se encuentra
 una posición adelantado.*/
char* devuelve_lexema(int devolver_activado);


/*Apunta con el puntero inicio a la posición del delantero. En caso de que se haya
 invocado a la función devolver previamente, apunta a una posición anterior a la posición
 en la que se encuentra el puntero delantero.*/
void actualizar_inicio(int devolver_activado);

/*Devuelve el carácter que se encuentra en la posición anterior a la posición
 en la que se encuentra el puntero delantero (puesto que esta función se invoca
 cuando ya se ha incrementado el valor de delantero).*/
int devolver();


#endif