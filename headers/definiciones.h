#ifndef DEFINICIONES_H
#define DEFINICIONES_H

#define PACKAGE 301
#define IMPORT 302
#define FUNC 303
#define CHAN 304
#define FOR 305
#define RANGE 306
#define GO 307
#define VAR 308
#define ID 309
#define MAS_IGUAL 310 //+=
#define ASIGNACION 311 //:=
#define CANAL 312 // <-
#define STRING 313
#define ENTERO 314 //Números enteros
#define FLOTANTE 315 //Números en punto flotante
#define IMAGINARIO 316 //Números hexadecimales
#define FIN_INSTRUCCION 317 //Salto de línea

/*En Go, el salto de línea es un componente léxico cuando indica el final de una 
 instrucción. Documentación de Go: "newlines (U+000A), is ignored except as it 
 separates tokens that would otherwise combine into a single token."
 En este compilador, cuando se muestre el lexema para el componente léxico FIN_INSTRUCCIÓN,
 se imprimirá la cadena "\n".*/

#endif 
/* DEFINICIONES_H */

