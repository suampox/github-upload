#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sistema_entrada.h"
#include "errores.h"

char bloque[2 * N + 2];
//Punteros para señalizar el inicio y el fin del lexema actual
int inicio = 0;
int delantero = 0;
FILE *fp; /*Se guarda en el sistema de entrada un puntero al FILE abierto, para evitar
          tener que pasar esta variable como argumento de las funciones del analizador
          léxico que invoquen a siguiente_caracter().*/

/*Devuelve el valor al que apunta el puntero delantero (en caso de que sea necesario,
 * carga un nuevo bloque) y actualiza el valor del puntero delantero. Si el puntero
 * delantero está en la última del bloque B, se cargan caracteres en el bloque A.Si 
 * el puntero delantero está en la última posición del bloque A, se carga el bloque B.
 En caso de que el número de bytes devueltos por fread (n) sea menor que N - 1 (n < N-1)
 significa que llegó al final de fichero. Escribo un -1 en la posición correspondiente
 */
int siguiente_caracter() {
    char c;
    int n = 0; //Número de caracteres leídos por fread

    if (bloque[delantero] < 0) {
        if (delantero == 2 * N + 1) {//Bloque B terminado
            n = fread(bloque, 1, N, fp); //Leo N caracteres y los almacenao en bloque.

            if (n < N) {
                bloque[n] = -1; //EOF del fichero
            }
            delantero = 0; //Coloco el puntero delantero en la primera posición de A
        } else if (delantero == N) {//Bloque A terminado
            n = fread(bloque + N+1, 1, N, fp); //Cargo N elementos en el bloque B
            if (n < N) {
                bloque[N+1 + n] = -1; //EOF del fichero
            }
            delantero = N+1; //Coloco el puntero delantero en la primera posición de B
        } else {
            return -1; //Final del fichero alcanzado
        }
    }
    c = bloque[delantero];
    delantero++;
    return c;
}

/*Carga el primero de los dos bloques y asigna el valor EOF en la última
 posición de cada uno de los dos bloques.*/
void inicializa_sistema_entrada(FILE *archivo) {
    int n;
    fp = archivo; //Apunto al archivo abierto en el main.
    n = fread(bloque, 1, N, fp);
    if (n < N) { //Caso en el que se leen menos de N-1 caracteres (fin de fichero)
        bloque[n] = -1; //EOF del fichero
    }
    bloque[N] = -1; //EOF del primer bloque
    bloque[2 * N + 1] = -1; //EOF del segundo  bloque
}

/*Devuelve el número de caracteres de los que consta el lexema actual, formado
 por los caracteres situados entre el puntero inicio y el puntero delantero.
 
 Se contemplan tres casos: 
 1)El puntero inicio está en una posición anterior a la del delantero. Dentro de este caso
 se contemplan dos posibilidades:
    1.1) El puntero inicio y el puntero delantero están en bloques diferentes.
    1.2) El puntero inicio y el puntero delantero están en el mismo bloque.
 2)Ambos punteros están en la misma posición
 3)El puntero inicio está en una posición posterior a la del puntero delantero.
 */
int calcula_tamano_lexema(int devolver_activado) {
    int tamano;

    if (inicio < delantero) {
        if (delantero > N && inicio <= N) {//Puede ocurrir que inicio sea momentáneamente N-1 hasta que no se carga un nuevo bloque.
            tamano = delantero - devolver_activado - inicio - 1; //Se resta una unidad adicional correspondiente a la posición de EOF del primer bloque.
        } else {
            tamano = delantero - devolver_activado - inicio;
        }
    } else if (inicio == delantero) {
        tamano = 1;
    } else {
        tamano = (2 * N + 1) - inicio + delantero;
    }
    return tamano;
}

/*Reserva memoria para el lexema detectado y copia el lexema en dicha memoria
 reservada. Devuelve un puntero al lexema. El lexema detectado está formado por los
 caracteres situados entre el puntero inicio y el puntero delantero.
 El entero pasado como primer argumento indica que para el último lexema reconocido 
 se invocó a la función devolver(), de manera que el puntero delantero se encuentra
 una posición adelantado.

 Se contemplan tres casos: 
 1)El puntero inicio está en una posición anterior a la del delantero. Dentro de este caso
 se contemplan dos posibilidades:
    1.1) El puntero inicio y el puntero delantero están en bloques diferentes.
         En este caso, primero se copian los caracteres que se encuentran entre
         el puntero inicio y el final del primer bloque y después se copian los
         caracteres que se encuentran entre el inicio del segundo bloque y el puntero delantero.
    1.2) El puntero inicio y el puntero delantero están en el mismo bloque.
 2)Ambos punteros están en la misma posición
 3)El puntero inicio está en una posición posterior a la del puntero delantero.
   Este caso sólo se puede producir cuando el puntero inicio está en el segundo bloque
   y el puntero delantero está en el primer bloque (jamás se da la situación en la
   que inicio > delantero y ambos se encuentran en el mismo bloque.
   En este caso, primero se copian los caracteres que están entre el puntero inicio y
   el final del segundo bloque y después se copian los caracteres que están entre el inicio
   del primer bloque y el puntero delantero.
 
 En todos los casos se tiene en cuenta si el puntero delantero se encuentra una posición
 adelantada debido a la invocación de la función devolver().*/
char* devuelve_lexema(int devolver_activado) {
    char *lex = NULL;
    int tamano_lexema = calcula_tamano_lexema(devolver_activado);
    /*Reserva de memoria para el lexema: se reserva una unidad adicional de memoria
    para el carácter '\0'*/
    lex = (char*) malloc((tamano_lexema + 1) * sizeof (char));

    if (lex == NULL) {
        imprimir_error(RESERVA_MEMORIA, 0);
        exit(0);
    }

    if (inicio < delantero) {
        if (delantero > N && inicio <= N) {//Puntero inicio y delantero están en bloques diferentes
            int n1 = N - inicio;
            memcpy(lex, bloque + inicio, n1);
            memcpy(lex + n1, bloque + N + 1, delantero - N - devolver_activado);
        } else {//Puntero inicio delantero están en el mismo bloque
            memcpy(lex, bloque + inicio, tamano_lexema);
        }
    } else if (inicio == delantero) {
        lex[0] = bloque[inicio];
    } else {
        int n2 = (2 * N + 1) - inicio;
        memcpy(lex, bloque + inicio, n2);
        memcpy(lex + n2, bloque, delantero - devolver_activado);
    }
    lex[tamano_lexema] = '\0';
    return lex;
}

/*Apunta con el puntero inicio a la posición del delantero. En caso de que se haya
 invocado a la función devolver previamente, apunta a una posición anterior a la posición
 en la que se encuentra el puntero delantero.*/
void actualizar_inicio(int devolver_activado) {
    inicio = delantero - devolver_activado;
}

/*Devuelve el carácter que se encuentra en la posición anterior a la posición
 en la que está situado el puntero delantero (puesto que esta función se invoca
 cuando ya se ha incrementado el valor de delantero).
 En caso de que esa posición contenga el valor EOF, se devolverá el valor de la posición
 anterior a esa. En caso de que delantero se encuentre en la primera posición
 del primer bloque, se devolverá el carácter que se encontraba en la última posición
 del segundo bloque, antes del EOF.*/
int devolver() {
    int caracter;

    if (delantero == N + 1) { //Primera posición del segundo bloque.
        caracter = bloque[delantero - 2];
    } else if (delantero == 0) {
        caracter = bloque[2 * N]; //Última posición del segundo bloque antes del EOF
    } else {
        caracter = bloque[delantero - 1];
    }

    return caracter;
}