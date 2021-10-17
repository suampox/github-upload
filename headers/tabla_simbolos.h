#ifndef TABLA_SIMBOLOS_H
#define TABLA_SIMBOLOS_H
#include "abb.h"

typedef abb tabla_simbolos;
/*La tabla de símbolos se ha definido de forma opaca, de manera que la estructura
 de datos utilizada para implementarla puede cambiarse sin que el resto de componentes
 del compilador se vean afectados.*/


/*Busca en la tabla de símbolos un elemento con un lexema igual al valor del string
 pasado como argumento y devuelve su id como salida. En caso de que no se encuentre en
 la tabla, devuelve un número negativo.*/
int buscar_tabla(char *lexema);

/*Inserta en la tabla un componente léxico utilizando como clave el string pasado
 como primer argumento y utilizando como valor el entero pasado como segundo argumento.*/
void insertar_tabla(char *lexema, int identificador);

/*Inicializa la tabla de símbolos, creando el árbol binario de búsqueda e insertando
 todas las palabras reservadas.*/
void inicializar_tabla();

/*Destruye el árbol binario de búsqueda y libera la memoria*/
void destruir_tabla();

/*Muestra por pantalla los componentes de la tabla de símbolos.*/
void imprimir_tabla();

#endif

