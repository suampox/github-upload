#ifndef ANALIZADOR_LEXICO_H
#define ANALIZADOR_LEXICO_H

#include <stdio.h>
#include <stdlib.h>
#include "tabla_simbolos.h"

/*El analizador léxico utilizará los siguientes códigos para gestionar
 la detección de lexemas:*/
#define NO_LEXEMA 1000 //Código para cuando no se detecta un nuevo lexema
#define COMENTARIO 1001 //Código para cuando se detecta un comentario
#define ERROR_DETECTADO 1002 //Código para cuando se detecta un error (no es lo mismo que el código de un error en particular)
#define IGNORAR_CARACTER 1003 //Código para cuando se detecta un espacio o salto de línea

/*Se utilizará la siguiente estructura para transmitir la informaciónde un componente léxico
 desde el analizador léxico al analizador sintáctico. */
typedef struct{
    	char *lexema; //Lexema identificado. Actúa como clave
	int codigo;//Código del componente léxico.
        int linea; //Número de línea en la que se detectó el lexema
        int flag_insertado;/*Permite saber si un identificador ha sido insertado o no en
                             la tabla de símbolos. Es necesario para saber desde el analizador
                            sintáctico si hay que utilizar free() o no. En caso de que el identificador
                            ya estuviese insertado en la tabla de símbolos, se libera la memoria de la
                            estructura utilizada en esa iteración.*/
} componente_lexico;

/*Devuelve un componente léxico. En caso de que al llamar al método ejecuta_autómatas
 no se haya detectado ningún componente léxico, el código del componente devuelto
 será NO_LEXEMA.*/
componente_lexico sig_componente_lexico();
#endif