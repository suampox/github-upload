#ifndef __ABB_LIB
#define __ABB_LIB

typedef char * tipoclave;
typedef struct{
    	char *lexema; //Actúa como clave
	int codigo; //Código del componente léxico

} tipoelem;

typedef struct celda *abb;

/* Crea el arbol vacío. A es un puntero al árbol. Debe estar inicializado. */
void crear_arbol(abb *A);

/*Destruye el arbol recursivamente. A: árbol que se quiere destruir*/
void destruir_arbol(abb *A);

/*Comprueba si el árbol esta vacío.*/
unsigned es_vacio(abb A);

/*Devuelve el subárbol izquierdo de A.*/
abb izq(abb A);

/*Devuelve el subárbol derecho de A.*/
abb der(abb A);

/* Inserta un nuevo nodo en el árbol para el elemento E
 * del que toma su clave. Esta clave no debe existir en
 * el árbol.*/
void insertar(abb *A, tipoelem E);

/*Suprime el nodo del árbol <A> */
void suprimir(abb *A, tipoelem E);

/*Comprueba si el elemento <E> existe en el árbol <A>*/
unsigned es_miembro(abb A, tipoelem E);

/*Comprueba si el elemento con clave <cl> existe en el árbol <A>*/
unsigned es_miembro_clave(abb A, tipoclave cl);

/**
 * Busca un nodo con clave <cl> en el árbol y, si existe, 
 * modifica el puntero <nodo> para que lo apunte.
 * A: árbol binario en el que se busca el nodo.
 * cl: clave que se buscara.
 * nodo: puntero a un tipoelem.
 */
void buscar_nodo(abb A, tipoclave cl, tipoelem *nodo);

/*Busca el nodo con clave la del nodo y lo modifica. De esta forma nos aseguramos
 *de que no puede modificarse la clave.*/
void modificar(abb *A, tipoelem nodo);

/*Recupera la información de la celda de la raíz del árbol.*/
void info(abb A, tipoelem *E);

/*Imprime el contenido del árbol, realizando un recorrido inorden.*/
void imprimir(abb A);

#endif
