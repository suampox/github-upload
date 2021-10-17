#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "analizador_lexico.h"
#include "tabla_simbolos.h"
#include "sistema_entrada.h"
#include "definiciones.h"
#include "errores.h"
#include <ctype.h>


int devolver_flag = 0; /*Toma el valor 1 en caso de que un lexema sea detectado
                        en el carácter siguiente al último carácter de dicho lexema.
                        En este caso, el puntero delantero estará una posición
                        adelantado*/
int numero_lineas = 1; //Contador del números de líneas del fichero de código fuente
int nueva_linea_flag = 0; //Toma el valor 1 si el último carácter leído es \n

/*Autómata que detecta tanto comentarios de una sola línea como de varias líneas.
 Este autómata se invoca cuando se lee el carácter '/'. Si el siguiente carácter
 es también '/' se intentará reconocer un comentario simple, mientras que si
 el carácter que viene después del primer '/' es el carácter '*', se intentará
 reconocer un comentario de varias líneas.
 Los estados 0 y 1 permiten reconocer comentarios simples, mientras que los
 estados 2 y 3 permiten reconocer comentarios de varias líneas.*/
int es_comentario() {
    int respuesta = NO_LEXEMA;
    int estado = 0;
    int c;

    while (estado != 4) {
        c = siguiente_caracter();

        switch (estado) {
            case 0:
                if (c == '/') {
                    estado = 1;
                } else if (c == '*') {
                    estado = 2;
                } else {
                    estado = 4;
                    respuesta = '/';
                    devolver_flag = 1; //Caso en el que no es un comentario. Se devuelve el código ASCII de '/'
                }
                break;
            case 1:
                if (c == 10) { //Igual a carácter de fin de línea
                    estado = 4;
                    devolver_flag = 1;
                    nueva_linea_flag = 1;
                    /*Se pone a 1 la bandera de nueva línea para evitar que se contabilice
                     como componente léxico FIN_INSTRUCCION el salto de línea.*/
                    respuesta = COMENTARIO;
                }
                break;
            case 2:
                if (c == '*') {//Permanecerá en el estado 2 hasta que llegue un asterisco
                    estado = 3;
                } else if (c == 10) {//Carácter de fin de línea
                    numero_lineas++;
                }
                break;
            case 3:
                if (c == '/') {
                    estado = 4;
                    respuesta = COMENTARIO;
                    nueva_linea_flag = 1;
                    /*Se pone a 1 la bandera de nueva línea para evitar que se contabilice
                     como componente léxico FIN_INSTRUCCION el salto de línea.*/
                } else {
                    estado = 2;
                    if (c == 10) {//Salto de línea. Se incrementa el contador de líneas.
                        numero_lineas++;
                    }
                }
                break;
        }
    }
    return respuesta;
}

/*Autómata que detecta strings. Se invoca cuando se lee el carácter '"'
 Finaliza al encontrar un carácter '"' que no se encuentre escapado*/
int es_string() {
    int respuesta = NO_LEXEMA;
    int estado = 0;
    int c;
    int caracter_escapado_flag = 0;

    while (estado != 1) {
        c = siguiente_caracter();

        if (c == 92) { //Carácter de escape
            caracter_escapado_flag = 1;
        } else if (c == '"' && caracter_escapado_flag != 1) {
            estado = 1;
            respuesta = STRING;
        } else {
            caracter_escapado_flag = 0;
        }
    }
    return respuesta;
}

/*Se decidió utilizar una función diferente para cada autómata de detección
 de operadores con dos caracteres, en lugar de utilizar una única función para todos, porque
 en este último caso se incrementaría considerablemente el número de condiciones a comprobar
 cada vez que llegase el carácter inicial de cualquier operador de dos caracteres.*/

/*Autómata que detecta al componente léxico :=*/
int es_asignacion() {
    int codigo = NO_LEXEMA;
    int c;
    c = siguiente_caracter();

    if (c == '=') {
        codigo = ASIGNACION;
    } else {
        /* En caso de que se lea ':' pero a continuación no venga el carácter '=', el código
         devuelto será el de ':' y posteriormente se invocará la función devolver
         para obtener de nuevo el carácter que venga después de ':'*/
        devolver_flag = 1;
        codigo = ':';
    }
    return codigo;
}

/*Autómata que detecta al componente léxico >=*/
int es_mas_igual() {
    int codigo = NO_LEXEMA;
    int c;
    c = siguiente_caracter();

    if (c == '=') {
        codigo = MAS_IGUAL;
    } else {
        devolver_flag = 1;
        codigo = '+'; //Se devuelve el código ASCII del propio carácter
    }
    return codigo;
}

/*Autómata que detecta al componente léxico <-*/
int es_canal() {
    int codigo = NO_LEXEMA;
    int c;
    c = siguiente_caracter();

    if (c == '-') {
        codigo = MAS_IGUAL;
    } else {
        devolver_flag = 1;
        codigo = '<'; //Se devuelve el código ASCII del propio carácter
    }
    return codigo;
}

/*Autómata que detecta palabras reservadas. Obtiene el lexema actual del
 sistema de entrada mediante devuelve_lexema() y comprueba si se corresponde
 con alguna de las palabras reservadas.*/
int es_palabra_reservada() {
    int codigo = NO_LEXEMA;
    char *lexema;

    lexema = devuelve_lexema(devolver_flag);
    if (strcmp(lexema, "package") == 0) {
        codigo = PACKAGE;
    } else if (strcmp(lexema, "import") == 0) {
        codigo = IMPORT;
    } else if (strcmp(lexema, "func") == 0) {
        codigo = FUNC;
    } else if (strcmp(lexema, "chan") == 0) {
        codigo = CHAN;
    } else if (strcmp(lexema, "for") == 0) {
        codigo = FOR;
    } else if (strcmp(lexema, "range") == 0) {
        codigo = RANGE;
    } else if (strcmp(lexema, "go") == 0) {
        codigo = GO;
    }

    free(lexema);

    return codigo;
}

/*Autómata que detecta signos de puntuación y operadores compuestos por un único
 carácter. En caso de que el carácter pasado como argumento no se corresponda
 con ninguno de los esperados, devuelve ERROR_DETECTADO.
 Para los caracteres '{', '}' y ';' se activa la bandera nueva_linea_flag, para
 que si después de ellos aparece un salto de línea, NO se contabilice como
 componente léxico FIN_INSTRUCCION.*/
int es_signo_puntuacion_operador(int caracter) {
    int codigo = ERROR_DETECTADO;

    if (caracter == '(' || caracter == ')' || caracter == '[' || caracter == ']'
            || caracter == '+' || caracter == '-'
            || caracter == '*' || caracter == '/' || caracter == '='
            || caracter == ',') {
        codigo = caracter;
    } else if (caracter == '{' || caracter == '}' || caracter == ';') {
        nueva_linea_flag = 1;
        codigo = caracter;
    }

    return codigo;
}

/*Autómata que reconoce variables definidas por el usuario y palabras reservadas.
 En caso de llegar al estado 1, si el siguiente carácter no es un carácter alfanumérico
 ni  es '_', se habrá alcanzado el final del lexema. En ese punto, se comprueba
 si es una palabra reservada.*/
int es_identificador_reservada(int caracter) {
    int respuesta = NO_LEXEMA;
    int estado = 0;
    int c = caracter;

    while (estado != 3 && estado != 4) {
        switch (estado) {
            case 0:
                if (isalpha(c)) {//Comprueba si es una letra mayúscula o minúscula
                    estado = 1;
                    c = siguiente_caracter();
                } else if (c == '_') {
                    estado = 2;
                    c = siguiente_caracter();
                }
                break;
            case 1:
                if (!isalnum(c) && c != '_') {//Comprueba si es un carácter alfanumérico o '_'
                    estado = 3;
                    respuesta = es_palabra_reservada(); //Comprobación de si el lexema es una palabra reservada
                    if (respuesta == NO_LEXEMA && c != '_') {
                        respuesta = ID;
                    }
                    devolver_flag = 1;
                } else {
                    c = siguiente_caracter();
                }
                break;
            case 2:
                if (isalnum(c) || c == '_') {
                    estado = 1;
                    c = siguiente_caracter();
                } else {//Cadena no reconocida. Se devuelve NO_LEXEMA
                    estado = 4;
                    devolver_flag = 1;
                }
                break;
        }
    }
    return respuesta;
}

/*Función auxiliar. Devuelve 1 si el carácter c pasado como argumento es un carácter hexadecimal (un 
 * dígito o una letra mayúscula/minúscula de la 'a' a la 'f')*/
int es_caracter_hexadecimal(int c) {
    if (isdigit(c) || c == 'a' || c == 'b' || c == 'c' || c == 'd' || c == 'e' || c == 'f'
            || c == 'A' || c == 'B' || c == 'C' || c == 'D' || c == 'E' || c == 'F') {
        return 1;
    } else {
        return 0;
    }
}

/*Función auxiliar. Devuelve 1 si el carácter c pasado como argumento es un dígito del 0 al 7.*/
int es_caracter_octal(int c) {
    if (isdigit(c) && c != '8' && c != '9') {
        return 1;
    } else {
        return 0;
    }
}

/*Autómata que reconoce números hexadecimales en punto flotante. Se llama desde
 dos funciones:
 1) Desde automata_numeros() cuando se encuentra en un estado de números
 decimales y recibe un '.' como entrada.
 2) Desde ejecuta_automatas() cuando llega el carácter '.', por lo que en este
 caso el valor del argumento flag_punto_inicial es 1, permitiendo así diferenciar
 los dos casos.
 En caso de que llegue el carácter 'i' desde un estado válido, se reconocerá un
 número imaginario.*/
int es_flotante_decimal(int flag_punto_inicial) {
    int respuesta = NO_LEXEMA;
    int estado = 0;
    int c;

    while (estado != 7) {
        c = siguiente_caracter();

        switch (estado) {
            case 0:
                if (isdigit(c)) {
                    estado = 1;
                } else if (c == 'e' || c == 'E') {
                    estado = 3;
                } else if (c == 'i') {
                    estado = 7;
                    respuesta = IMAGINARIO;
                } else {
                    estado = 7;
                    devolver_flag = 1;
                    if (flag_punto_inicial == 0) {
                        respuesta = FLOTANTE;
                    } else {//Caso en el que el lexema es únicamente el carácter '.'
                        respuesta = '.'; //Se devuelve el código ASCII de '.'
                    }
                }
                break;
            case 1:
                if (c == 'e' || c == 'E') {
                    estado = 3;
                } else if (c == 'i') {
                    estado = 7;
                    respuesta = IMAGINARIO;
                } else if (!isdigit(c)) {
                    estado = 7;
                    respuesta = FLOTANTE;
                    devolver_flag = 1;
                }
                break;
            case 2:
                if (isdigit(c)) {
                    estado = 1;
                } else {//Cadena no reconocida. Se devuelve NO_LEXEMA
                    estado = 7;
                    devolver_flag = 1;
                }
                break;
            case 3:
                if (c == '+' || c == '-') {
                    estado = 4;
                } else if (isdigit(c)) {
                    estado = 5;
                } else {//Cadena no reconocida. Se devuelve NO_LEXEMA
                    estado = 7;
                    devolver_flag = 1;
                }
                break;
            case 4:
                if (isdigit(c)) {
                    estado = 5;
                } else {//Cadena no reconocida. Se devuelve NO_LEXEMA
                    estado = 7;
                    devolver_flag = 1;
                }
                break;
            case 5:
                if (c == '_') {
                    estado = 6;
                } else if (c == 'i') {
                    estado = 7;
                    respuesta = IMAGINARIO;
                } else if (!isdigit(c)) {
                    estado = 7;
                    respuesta = FLOTANTE;
                    devolver_flag = 1;
                }
                break;
            case 6:
                if (isdigit(c)) {
                    estado = 5;
                } else {//Cadena no reconocida. Se devuelve NO_LEXEMA
                    estado = 7;
                    devolver_flag = 1;
                }
                break;
        }
    }

    return respuesta;
}

/*Autómata que reconoce números hexadecimales en punto flotante. Se llama desde
 el autómata automata_numeros cuando se encuentra en un estado de números
 hexadecimales y recibe un '.' como entrada. Si flag_p es 1, significa
 que se llamó a esta función desde automata_numeros() tras recibir una 'p'/'P'
 como carácter. En caso contrario, a esta función se la llamó tras recibir un '.'
 como carácter. Flag_mantisa se utiliza para saber si se leyó algún dígito antes
 de recibir un '.' (sólo se utiliza cuando se invoca esta función al leer el carácter '.').
 Su valor es irrelevante cuando flag_p == 1.
 Mediante este flag, se evita que se acepten números del tipo 0x.p3, puesto que no tienen mantisa.
 En caso de que llegue el carácter 'i' desde un estado válido, se reconocerá un
 número imaginario.*/
int es_flotante_hexadecimal(int flag_p, int flag_mantisa) {
    int respuesta = NO_LEXEMA;
    int estado = 0;
    int c;

    if (flag_p == 1) {//Caso en el que se llamó a esta función tras leer el carácter 'p'/'P' en automata_numeros()
        estado = 3;
    }
    while (estado != 6) {
        c = siguiente_caracter();

        switch (estado) {
            case 0:
                if (es_caracter_hexadecimal(c)) {
                    estado = 1;
                } else if (c == 'p' || c == 'P') {
                    if (flag_mantisa == 1) {
                        estado = 3;
                    } else {//Cadena no reconocida. Se devuelve NO_LEXEMA
                        estado = 6;
                        devolver_flag = 1;
                    }
                } else {
                    estado = 6;
                    if (flag_mantisa == 1) {
                        respuesta = FLOTANTE;
                    }//En caso contrario, la cadena no es reconocida
                    devolver_flag = 1;
                }
                break;
            case 1:
                if (c == 'p' || c == 'P') {
                    estado = 3;
                } else if (c == 'i') {
                    estado = 6;
                    respuesta = IMAGINARIO;
                } else if (!es_caracter_hexadecimal(c)) {
                    estado = 6;
                    respuesta = FLOTANTE;
                    devolver_flag = 1;
                }
                break;
            case 2:
                if (es_caracter_hexadecimal(c)) {
                    estado = 1;
                } else {//Cadena no reconocida. Se devuelve NO_LEXEMA
                    estado = 6;
                    devolver_flag = 1;
                }
                break;
            case 3:
                if (c == '+' || c == '-') {
                    estado = 4;
                } else if (es_caracter_hexadecimal(c)) {
                    estado = 5;
                } else {//Cadena no reconocida. Se devuelve NO_LEXEMA
                    estado = 6;
                    devolver_flag = 1;
                }
                break;
            case 4:
                if (es_caracter_hexadecimal(c)) {
                    estado = 5;
                } else {//Cadena no reconocida. Se devuelve NO_LEXEMA
                    estado = 6;
                    devolver_flag = 1;
                }
                break;
            case 5:
                if (c == '_') {//Cadena no reconocida. Se devuelve NO_LEXEMA
                    estado = 6;
                } else if (c == 'i') {
                    estado = 6;
                    respuesta = IMAGINARIO;
                } else if (!es_caracter_hexadecimal(c)) {
                    estado = 6;
                    respuesta = FLOTANTE;
                    devolver_flag = 1;
                }
                break;
        }
    }
    return respuesta;
}

/*Autómata que permite reconocer números en punto flotante decimales que no contienen el
 carácter '.'
 Este autómata se invoca cuando en automata_numeros() se encuentra en un estado
 de números decimales y llega el carácter 'e' o el carácter 'E'.
 En caso de que llegue el carácter 'i' desde un estado válido, se reconocerá un
 número imaginario.*/
int es_flotante_decimal_sin_punto() {
    int respuesta = NO_LEXEMA;
    int estado = 0;
    int c;

    while (estado != 4) {
        c = siguiente_caracter();
        switch (estado) {
            case 0:
                if (c == '+' || c == '-') {
                    estado = 1;
                } else if (isdigit(c)) {
                    estado = 2;
                } else {//Cadena no reconocida. Se devuelve NO_LEXEMA.
                    estado = 4;
                    devolver_flag = 1;
                }
                break;
            case 1:
                if (isdigit(c)) {
                    estado = 2;
                } else {
                    estado = 4;
                    devolver_flag = 1;
                }
                break;
            case 2:
                if (c == '_') {
                    estado = 3;
                } else if (c == 'i') {
                    estado = 4;
                    respuesta = IMAGINARIO;
                } else if (!isdigit(c)) {
                    estado = 4;
                    devolver_flag = 1;
                    respuesta = FLOTANTE;
                }
                break;
            case 3:
                if (isdigit(c)) {
                    estado = 2;
                } else {//Cadena no reconocida.
                    estado = 4;
                    devolver_flag = 1;
                }
                break;
        }
    }
    return respuesta;
}

/*Autómata general que reconoce todo tipo de números. Desde este autómata se llama
 a las funciones de autómatas de números específicos.
 Partiendo del estado inicial, si el primer carácter es '0' y el segundo es 'x' o 'X',
 se intentará reconocer un número hexadecimal (estados 4,5,6,7).
 Si el primer carácter que llega es '0' y el segundo es 'o' o es 'O', se intentará
 reconocer un número octal (estados 8,9,10,11).
 Si el primer carácter no es un 0, se intentará reconocer un número decimal (estado 1).
 Cuando el autómata se encuentra en algún estado de reconocimientode números decimales,
 si llega el carácter '.', el carácter 'e' o el carácter 'E', se ejecutará es_flotante_decimal()
 o es flotante_decimal_sin_punto(), para reconocer números decimales en punto flotante.
 En el caso de que el autómata se encuentre en un estado de reconocimiento de números
 hexadecimales y llegue el carácter '.', el carácter 'p' o el carácter 'P', se ejecutará
 es_flotante_hexadecimal(), para reconocer números hexadecimales en punto flotante.
 En caso de que llegue el carácter 'i' desde un estado válido, se reconocerá un
 número imaginario.*/
int automata_numeros(int caracter) {
    int respuesta = NO_LEXEMA;
    int estado = 0;
    int c = caracter;

    while (1) {
        switch (estado) {
            case 0:
                if (isdigit(c) && c != '0') {
                    estado = 1;
                } else if (c == '0') {
                    estado = 2;
                } else {
                    devolver_flag = 1;
                    return respuesta;
                }
                break;
            case 1:
                if (c == '_') {
                    estado = 3;
                } else if (c == '.') {
                    respuesta = es_flotante_decimal(0);
                    return respuesta;
                } else if (c == 'i') {
                    respuesta = IMAGINARIO;
                    return respuesta;
                } else if (c == 'e' || c == 'E') {
                    respuesta = es_flotante_decimal_sin_punto();
                    return respuesta;
                } else if (!isdigit(c)) {
                    respuesta = ENTERO;
                    devolver_flag = 1;
                    return respuesta;
                }
                break;
            case 2:
                if (c == 'x' || c == 'X') {//Posible número hexadecimal
                    estado = 4;
                } else if (c == 'i') {
                    respuesta = IMAGINARIO;
                    return respuesta;
                } else if (c == 'o' || c == 'O') {//Posible número octal
                    estado = 8;
                } else if (c == '_') {
                    estado = 3;
                } else if (isdigit(c)) {
                    estado = 1;
                } else if (c == '.') {//Posible número en punto flotante
                    respuesta = es_flotante_decimal(0);
                    return respuesta;
                } else {
                    respuesta = ENTERO;
                    devolver_flag = 1;
                    return respuesta;
                }
                break;
            case 3:
                if (isdigit(c)) {
                    estado = 1;
                } else {//Cadena no reconocida. Se devuelve NO_LEXEMA.
                    devolver_flag = 1;
                    return respuesta;
                }
                break;
            case 4:
                if (c == '_') {
                    estado = 5;
                } else if (es_caracter_hexadecimal(c)) {
                    estado = 6;
                } else if (c == '.') {//Posible número hexadecimal en punto flotante
                    respuesta = es_flotante_hexadecimal(0, 0);
                    //flag_p = 0 porque no se llama tras leer 'p'/'P'
                    //flag_mantisa = 0 porque la mantisa no tiene ningún dígito
                    return respuesta;
                } else {//Cadena no reconocida.
                    devolver_flag = 1;
                    return respuesta;
                }
                break;
            case 5:
                if (es_caracter_hexadecimal(c)) {
                    estado = 6;
                } else {//Cadena no reconocida.
                    devolver_flag = 1;
                    return respuesta;
                }
                break;
            case 6:
                if (c == '_') {
                    estado = 7;
                } else if (c == '.') {//Posible número hexadecimal en punto flotante
                    respuesta = es_flotante_hexadecimal(0, 1);
                    //flag_p = 0 porque no se llama tras leer 'p'/'P'
                    //flag_mantisa = 1 porque la mantisa tiene al menos un dígito
                    return respuesta;
                } else if (c == 'p' || c == 'P') {//Posible número hexadecimal en punto flotante
                    respuesta = es_flotante_hexadecimal(1, 1);
                    //flag_p = 1 porque se llama tras leer 'p'/'P'
                    //flag_mantisa = 1. Su valor es irrelevante cuando flag_p = 1.
                    return respuesta;
                } else if (c == 'i') {
                    respuesta = IMAGINARIO;
                    return respuesta;
                } else if (!es_caracter_hexadecimal(c)) {
                    respuesta = ENTERO;
                    devolver_flag = 1;
                    return respuesta;
                }
                break;
            case 7:
                if (es_caracter_hexadecimal(c)) {
                    estado = 6;
                } else {//Cadena no reconocida.
                    devolver_flag = 1;
                    return respuesta;
                }
                break;
            case 8:
                if (c == '_') {
                    estado = 9;
                } else if (es_caracter_octal(c)) {
                    estado = 10;
                } else {//Cadena no reconocida.
                    devolver_flag = 1;
                    return respuesta;
                }
                break;
            case 9:
                if (es_caracter_octal(c)) {
                    estado = 10;
                } else {
                    devolver_flag = 1;
                    return respuesta;
                }
                break;
            case 10:
                if (c == '_') {
                    estado = 11;
                } else if (c == 'i') {
                    respuesta = IMAGINARIO;
                    return respuesta;
                } else if (!es_caracter_octal(c)) {
                    respuesta = ENTERO;
                    devolver_flag = 1;
                    return respuesta;
                }
                break;
            case 11:
                if (es_caracter_octal(c)) {
                    estado = 10;
                } else {//Cadena no reconocida.
                    devolver_flag = 1;
                    return respuesta;
                }
                break;
        }
        c = siguiente_caracter();
    }
    return respuesta;
}

/*En caso de que devolver_flag sea 0 (no es necesario devolver carácter), ejecuta
 siguiente_caracter() y en caso contrario ejecuta devolver(). En función del carácter
 recibido, llamará a un autómata u otro. Este autómata puede devolver diferentes
 códigos:
 1) Código de un componente léxico identificado.
 2) Código NO_LEXEMA si después de ejecutar los autómatas correspondientes, no se ha reconocido ningún lexema.
 3) Código COMENTARIO en caso de haber detectado un comentario.
 4) Código ERROR_DETECTADO en caso de haber encontrado un error. Esto se producirá únicamente
    para el caso de encontrar un carácter inesperado. El error debido a cadenas non reconocidas
    se gestionará cuando se produce el punto 2).
 5) Código IGNORAR_CARACTER en caso de de que el carácter leído sea un espacio en
    blanco o un salto de línea que no se corresponde con el componente léxico FIN_INSTRUCCION.
 6) Código -1 en caso de haber alcanzado el final del fichero.
 
 Para contabilizar el número de líneas del fichero de código fuente, se incrementa
 el valor de la variable global numero_lineas, cuando se lee el carácter de nueva línea.*/
int ejecuta_automatas() {
    int codigo = NO_LEXEMA;
    int c;
    if (devolver_flag == 0) {
        c = siguiente_caracter();
    } else {
        c = devolver();
        devolver_flag = 0;
    }
    if (c != -1) {
        if (c != ' ' && c != 10 && c != 13) {
            nueva_linea_flag = 0;
            if (c == '"') {
                codigo = es_string();
            } else if (c == ':') {
                codigo = es_asignacion();
            } else if (c == '+') {
                codigo = es_mas_igual();
            } else if (c == '<') {
                codigo = es_canal();
            } else if (c == '/') {
                codigo = es_comentario(); //Los comentarios se detectan y se ignoran. Sólo devuelve un código en el caso de / individual.
            } else if (c == '_') {
                codigo = es_identificador_reservada(c);
                if (codigo == NO_LEXEMA) {
                    codigo = '_'; //En caso de que no sea un identificador o palabra reservada, es un "_" individual
                }
            } else if (isdigit(c)) {
                codigo = automata_numeros(c);
            } else if (c == '.') {
                codigo = es_flotante_decimal(1);
            } else if (isalpha(c) || c == '_') {
                codigo = es_identificador_reservada(c);
            } else {
                codigo = es_signo_puntuacion_operador(c);
                if (codigo == ERROR_DETECTADO) {
                    imprimir_error(CARACTER_INESPERADO, numero_lineas);
                }
            }
        } else {
            codigo = IGNORAR_CARACTER;
            if (c == 10) {//Nueva línea
                if (nueva_linea_flag == 0) {
                    codigo = FIN_INSTRUCCION;
                }
                nueva_linea_flag = 1;//Se activa el flag para no detectar como FIN_INSTRUCCION los siguientes saltos de línea.
                numero_lineas++;
            } else {
                nueva_linea_flag = 0;
            }
        }
    } else {
        codigo = -1;
    }
    return codigo;
}

/*Devuelve un componente léxico. Se itera con un bucle do-while, ejecutando en cada iteración
 ejecuta_automatas(), hasta encontrar un componente léxico, de forma que se ignoran los 
 comentarios, ciertos caracteres como espacios o nueva línea (excepto si se trata 
 del componente léxico de fin de instrucción). 
 Cuando se encuentra un componente léxico, se sale del bucle do-while y en caso de
 que el componente léxico NO sea FIN_INSTRUCCION se obtiene el lexema identificado, 
 mediante devuelve_lexema(). En caso de que el componente léxico sea FIN_INSTRUCCION,
 el lexema que se asigna es "\\n", para evitar que el analizador_sintactico.c se imprima
 el salto de línea.
 En caso de que el componente léxico sea un identificador,se comprueba si está insertado
 en la tabla de símbolos y, en caso contrario, se inserta.
 En caso de que el código devuelto por ejecuta_automatas() sea
 NO_LEXEMA, se invoca al sistema de gestión de errores.*/

componente_lexico sig_componente_lexico() {
    componente_lexico componente;
    componente.lexema = NULL;
    /*Itera hasta detectar un componente léxico, descartando espacios en blanco,
     saltos de línea y comentarios.*/
    do {
        componente.codigo = ejecuta_automatas();

        if (componente.codigo == COMENTARIO || componente.codigo == ERROR_DETECTADO || componente.codigo == IGNORAR_CARACTER) {
            actualizar_inicio(devolver_flag); //Se hace que puntero inicio = delantero después de detectar un comentario o un error
        }
    } while (componente.codigo == IGNORAR_CARACTER || componente.codigo == COMENTARIO || componente.codigo == ERROR_DETECTADO);

    //El componente léxico FIN_INSTRUCCIÓN se trata de forma diferenciada.
    if (componente.codigo != FIN_INSTRUCCION) {
        componente.linea = numero_lineas;
        componente.lexema = devuelve_lexema(devolver_flag);
    } else {
        componente.linea = numero_lineas - 1; /*Se resta una unidad porque al detectar el componente léxico
                                                FIN_INSTRUCCION se incrementó el contador numero_lineas.*/
        componente.lexema = (char*) malloc(3 * sizeof (char));
        //Se inserta directamente la cadena "\n", sin necesidad de llamar a devolver_lexema().
        componente.lexema[0] = '\\';
        componente.lexema[1] = 'n';
        componente.lexema[2] = '\0';
    }

    actualizar_inicio(devolver_flag);

    if (componente.codigo != NO_LEXEMA) {
        /*En caso de que el componente léxico sea un identificador, se busca en la
         tabla de śimbolos y, si no se encuentra en ella, se inserta.*/
        if (componente.codigo == ID) {
            int resultado;
            resultado = buscar_tabla(componente.lexema);
            if (resultado < 0) {//El componente léxico no se encontraba en la tabla de símbolos
                insertar_tabla(componente.lexema, componente.codigo);
                componente.flag_insertado = 0;
            }else{
                componente.flag_insertado = 1;//Indica que el componente léxico ya se encontraba en la tabla de símbolos
            }
        }
    } else {//Caso en el que el código devuelto por ejecuta_automatas() es NO_LEXEMA
        imprimir_error(CADENA_NO_RECONOCIDA, numero_lineas);
    }
    return componente;
}

