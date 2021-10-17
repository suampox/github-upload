#ifndef ERRORES_H
#define ERRORES_H

#define CADENA_NO_RECONOCIDA 1
#define CARACTER_INESPERADO 2
#define APERTURA_FICHERO 3
#define RESERVA_MEMORIA 4


/*Imprime un mensaje de error que varía dependiendo del código de error pasado
 como argumento. Se muestra el número de línea en el que se produjo el error para 
 aquellos errores de análisis léxico.*/
void imprimir_error(int codigo_error, int linea);

#endif /* ERRORES_H */

