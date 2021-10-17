#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "abb.h"

struct celda {
    tipoelem info;
    struct celda *izq, *der;
};


/*Extrae la clave de una celda*/
tipoclave _clave_elem(tipoelem * E) {
    return E->lexema; //Equivalente a (*E).ApellidosNombre
}

/* Esta función puente permite modificar el tipo de
 * de datos del TAD sin tener que cambiar todas las 
 * comparaciones del resto de la biblioteca y en su lugar
 * cambiando solo ésta. 
 */
int _comparar_claves(tipoclave cl1, tipoclave cl2) {
    if (strcmp(cl1, cl2) == 0) {
        return 0;
    } else if (strcmp(cl1, cl2) > 0) {
        return 1;
    } else {
        return -1;
    }
}

/*Libera la memoria reservada para el lexema del componente
 léxico. Esta función se llama en destruir_arbol() para cada uno de los nodos
 del árbol.*/
void _destruir_elem(tipoelem *E) {
    free(E->lexema);
}


int _comparar_clave_elem(tipoclave cl, tipoelem E) {
    return _comparar_claves(cl, _clave_elem(&E));
}

/**Crea el arbol vacío. A es un puntero al árbol. Debe estar inicializado.*/
void crear_arbol(abb *A) {
    *A = NULL;
}

/*Comprueba si el árbol esta vacío.*/
unsigned es_vacio(abb A) {
    return A == NULL;
}

/*Devuelve el subárbol izquierdo de A.*/
abb izq(abb A) {
    return A->izq;
}

/*Devuelve el subárbol derecho de A*/
abb der(abb A) {
    return A->der;
}

/*Destruye el arbol recursivamente. A: árbol que se quiere destruir.*/
void destruir_arbol(abb *A) {
    if (!es_vacio(*A)) {
        destruir_arbol(&((*A)->izq));
        destruir_arbol(&((*A)->der));
        _destruir_elem(&((*A)->info));
        free(*A);
        *A = NULL;
    }
}

/* Función privada para pasar la clave y no tener que extraerla del nodo en las 
 * llamadas recursivas. */
void _modificar(abb *A, tipoclave cl, tipoelem nodo) {
    if (es_vacio(*A)) {
        return;
    }
    int comp = _comparar_clave_elem(cl, (*A)->info);
    if (comp == 0) {
        (*A)->info = nodo;
    } else if (comp < 0) {
        _modificar(&(*A)->izq, cl, nodo);
    } else {
        _modificar(&(*A)->der, cl, nodo);
    }
}

/* Permite modificar el nodo extrayendo del mismo la clave */
void modificar(abb *A, tipoelem nodo) {
    tipoclave cl = _clave_elem(&nodo);
    _modificar(A, cl, nodo);
}

/* Función recursiva para insertar un nuevo nodo en el árbol. Se presupone que no existe un nodo
   con la misma clave en el árbol. */
void insertar(abb *A, tipoelem E) {
    if (es_vacio(*A)) {
        *A = (abb) malloc(sizeof (struct celda));
        (*A)->info = E;
        (*A)->izq = NULL;
        (*A)->der = NULL;
        return;
    }
    tipoclave cl = _clave_elem(&E);
    int comp = _comparar_clave_elem(cl, (*A)->info);
    if (comp > 0) {
        insertar(&(*A)->der, E);
    } else {
        insertar(&(*A)->izq, E);
    }
}

tipoelem _suprimir_min(abb *A) {
    abb aux;
    tipoelem ele;
    if (es_vacio((*A)->izq)) {
        ele = (*A)->info;
        aux = *A;
        *A = (*A)->der;
        _destruir_elem(&aux->info);
        free(aux);
        return ele;
    } else {
        return _suprimir_min(&(*A)->izq);
    }
}

/* Función que permite eliminar un nodo del árbol. */
void suprimir(abb *A, tipoelem E) {
    abb aux;
    if (es_vacio(*A)) {
        return;
    }

    tipoclave cl = _clave_elem(&E);
    int comp = _comparar_clave_elem(cl, (*A)->info);
    if (comp < 0) { //if (E < (*A)->info) {
        suprimir(&(*A)->izq, E);
    } else if (comp > 0) { //(E > (*A)->info) {
        suprimir(&(*A)->der, E);
    } else if (es_vacio((*A)->izq) && es_vacio((*A)->der)) {
        _destruir_elem(&((*A)->info));
        free(*A);
        *A = NULL;
    } else if (es_vacio((*A)->izq)) { // pero no es vacio derecha
        aux = *A;
        *A = (*A)->der;
        _destruir_elem(&aux->info);
        free(aux);
    } else if (es_vacio((*A)->der)) { //pero no es vacio izquierda
        aux = *A;
        *A = (*A)->izq;
        _destruir_elem(&aux->info);
        free(aux);
    } else { //ni derecha ni izquierda esta vacio
        (*A)->info = _suprimir_min(&(*A)->der);
    }
}

unsigned es_miembro(abb A, tipoelem E) {
    return es_miembro_clave(A, _clave_elem(&E));
}

/** Comprueba si el elemento con clave <cl> existe en el árbol <A> */
unsigned es_miembro_clave(abb A, tipoclave cl) {
    if (es_vacio(A)) {
        return 0;
    }
    int comp = _comparar_clave_elem(cl, A->info);

    if (comp == 0) {
        return 1;
    }
    if (comp > 0) {
        return es_miembro_clave(A->der, cl);
    }
    return es_miembro_clave(A->izq, cl);
}

void buscar_nodo(abb A, tipoclave cl, tipoelem *nodo) {
    if (es_vacio(A)) {
        return;
    }
    int comp = _comparar_clave_elem(cl, A->info);

    if (comp == 0) {
        *nodo = A->info;
    } else if (comp < 0) {
        buscar_nodo(A->izq, cl, nodo);
    } else {
        buscar_nodo(A->der, cl, nodo);
    }
}

/*Recupera la información de la celda de la raíz del árbol.*/
void info(abb A, tipoelem *E) {
    *E = A->info;
}

/*Imprime el contenido del árbol, realizando un recorrido inorden*/
void imprimir(abb A){
    	tipoelem E;
	if(!es_vacio(A)){
	  imprimir(izq(A));
	  info(A,&E); printf("%s ----> %d\n",E.lexema, E.codigo);	
	  imprimir(der(A));
	}
}



