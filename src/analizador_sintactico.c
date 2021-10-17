#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "analizador_sintactico.h"
#include "analizador_lexico.h"
#include "definiciones.h"

/*Itera indefinidamente, invocando en cada iteración a la función
 sig_componente_lexico() del analizador léxico. La iteración finaliza cuando el componente
 léxico recibido tiene como identificador un número negativo. Este suceso indicará
 que se ha alcanzado el final del fichero.
 Después de imprimir la información sobre un componente léxico, se libera la memoria
 reservada para el lexema, únicamente para los componentes léxicos que no sean identificadores
 que se insertan por primera vez en la tabla de símbolos, puesto que estos deben permanecer 
 en la tabla hasta el final de la ejecución. Su memoria se liberará con la función _destruir_elem 
 de abb.c, que se invoca al destruir la tabla de símbolos al final de la ejecución. Si el componente léxico 
 se trata de un identificador que ya se había insertado previamente en la tabla (flag_insertado == 1), entonces sí que
 se libera la memoria de su puntero char* lexema.*/
void iterar(FILE *fp) {
    componente_lexico c = sig_componente_lexico(fp);
    printf("\n----------------------------------------------\n");
    printf("|Línea|:   |Lexema| ----> |Componente léxico|\n");
    printf("----------------------------------------------\n");
    while (c.codigo >= 0) { //c.identificador >= -1 &&
        if (c.codigo != NO_LEXEMA) {
            printf("\n L%d:  %s ----> %d\n", c.linea, c.lexema, c.codigo);
        }
        if (c.codigo != ID || (c.codigo == ID && c.flag_insertado == 1)) {
            free(c.lexema);
        }
        c = sig_componente_lexico(fp);
    }
    free(c.lexema); /*Necesario para liberar el componente léxico que se corresponde
                     con fin de fichero.*/
}