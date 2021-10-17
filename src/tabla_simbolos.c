#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tabla_simbolos.h"

abb tabla;

/*Busca en la tabla de símbolos un elemento con un lexema igual al valor del string
 pasado como argumento y devuelve su id como salida. En caso de que no se encuentre en
 la tabla, devuelve un número negativo.*/
int buscar_tabla(char *lexema) {
    tipoelem nodo;

    nodo.codigo = -1000;
    buscar_nodo(tabla, lexema, &nodo);

    return nodo.codigo;
}

/*Inserta en la tabla un componente léxico utilizando como clave el string pasado
 como primer argumento y utilizando como valor el entero pasado como segundo argumento.*/
void insertar_tabla(char *lexema, int identificador) {
    tipoelem e;

    e.lexema = lexema;
    e.codigo = identificador;
    insertar(&tabla, e);
}

/*Inicializa la tabla de símbolos, creando el árbol binario de búsqueda e insertando
 todas las palabras reservadas. Reserva memoria para cada palabra reservada
 antes de insertarla en la tabla. La memoria se libera con la función _destruir_elem
 de abb.c al eliminar la tabla, de forma que para cada nodo del árbol de búsqueda
 binario se aplicará free() sobre su lexema.*/
void inicializar_tabla() {
    char* palabras[8] = {"package\0", "import", "func", "chan", "for", "range", "go", "var"};

    crear_arbol(&tabla);
    char* palabra = NULL;
    
        for (int i = 0; i < 8; i++) {
            palabra = (char*) malloc((strlen(palabras[i]) + 1) * sizeof (char));
            strncpy(palabra, palabras[i], strlen(palabras[i]) + 1);
            insertar_tabla(palabra, 300 + i + 1);
            palabra = NULL;
        }

}

/*Destruye el árbol binario de búsqueda y libera la memoria*/
void destruir_tabla() {
    destruir_arbol(&tabla);
}

/*Muestra por pantalla los componentes de la tabla de símbolos. Realiza
 un recorrido inorden de la tabla de símbolos*/
void imprimir_tabla() {
    imprimir(tabla);
}