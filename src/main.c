#include <stdio.h>
#include <stdlib.h>
#include "tabla_simbolos.h"
#include "sistema_entrada.h"
#include "analizador_sintactico.h"
#include "errores.h"


void inicializacion(FILE *fp);
void finalizacion(FILE *fp);

int main(int argc, char** argv) {
    FILE *fp = NULL;

    fp = fopen("concurrentSum.go", "r");

    if (fp == NULL) {
        imprimir_error(APERTURA_FICHERO, 0);
        exit(0);
    }

    inicializacion(fp);
    iterar(fp);
    finalizacion(fp);

    return (EXIT_SUCCESS);
}

/*Inicializa el sistema de entrada, crea e inicializa la tabla de símbolos 
 con las palabras reservadas e imprime el contenido de la tabla*/
void inicializacion(FILE *fp) {
    inicializa_sistema_entrada(fp);
    inicializar_tabla();
    printf("\nTabla de símbolos al inicio:\n");
    printf("\n----------------------------------\n");
    printf("|Lexema| ----> |Componente léxico|\n");
    printf("----------------------------------\n");
    imprimir_tabla();
}

/*Imprime el contenido de la tabla, la destruye (liberando memoria) y cierra
 el fichero de lectura del código fuente.*/
void finalizacion(FILE *fp) {
    printf("\nTabla de símbolos al final:\n");
    printf("\n----------------------------------\n");
    printf("|Lexema| ----> |Componente léxico|\n");
    printf("----------------------------------\n");
    imprimir_tabla();
    destruir_tabla();
    fclose(fp);
}

