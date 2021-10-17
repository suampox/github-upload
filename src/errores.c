#include <stdio.h>
#include <stdlib.h>
#include "errores.h"

/*Imprime un mensaje de error que varía dependiendo del código de error pasado
 como argumento. Se muestra el número de línea en el que se produjo el error para 
 aquellos errores que no se corresponden con un intento fallido de apertura de fichero.
 
 
 Respecto a la gestión de errores, ésta NO se realiza de forma específica en los
 autómatas, de forma que cuando un autómata finaliza por la llegada de un carácter
 que no pertenece al lexema, se invoca sistemáticamente a la función devolver().
 De esta forma, se reconoce siempre el lexema más largo posible. Por ejemplo,
 en el caso de la entrada 1.5_e1, se reconocerían dos componentes léxicos: 1.5(FLOTANTE) 
 y _e1 (ID), en lugar de lanzar un error.
 El error se lanzará únicamente en los casos en los que llegue un carácter no esperado
 por ningún autómata, como por ejemplo '¨' o en el caso de que se detecte una cadena no
 reconocida. Por ejemplo, en el caso de 0xwm3, se lanzaría un error CADENA_NO_RECONOCIDA. En
 este caso no se reconocerían dos componentes léxicos, puesto que 0x no es un lexema válido. 
 Sin embargo, el lexema wm3 sí que se detectaría como un ID.*/
void imprimir_error(int codigo_error, int linea) {
    switch (codigo_error) {
        case CADENA_NO_RECONOCIDA:
            printf("\n Línea %d. Error %d: CADENA NO RECONOCIDA.\n", linea, CADENA_NO_RECONOCIDA);
            break;
        case CARACTER_INESPERADO:
            printf("\n Línea %d. Error %d: CARÁCTER INESPERADO.\n", linea, CARACTER_INESPERADO);
            break;
        case APERTURA_FICHERO:
            printf("\n Error %d: ERROR EN LA APERTURA DEL FICHERO DE CÓDIGO FUENTE.\n", APERTURA_FICHERO);
            break;
        case RESERVA_MEMORIA:
            printf("\n Error %d: ERROR EN LA RESERVA DE MEMORIA PARA EL LEXEMA.\n", RESERVA_MEMORIA);
            break;
    }
}
