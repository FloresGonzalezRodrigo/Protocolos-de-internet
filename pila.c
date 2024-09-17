//
// Created by Rodrigo on 16/09/2024.
//

#include <stdlib.h>
#include <string.h>
#include "pila.h"

Pila* pila(void* pdato, size_t sdato) {
    Pila* ppila = malloc(sizeof(Pila) + sdato);
    if (ppila) {
        memcpy(ppila+1, pdato, sdato);
        ppila->psig = NULL;
        ppila->cuenta = 1;  // Inicializa en 1, ya que es el primer nodo
    }
    return ppila;
}

Pila* apilar(Pila* ppila, void* pdato, size_t sdato) {
    Pila* pnuevo = malloc(sizeof(Pila) + sdato);
    if (pnuevo) {
        memcpy(pnuevo+1, pdato, sdato);  // Copia los datos en el nuevo nodo
        pnuevo->psig = ppila;
        pnuevo->cuenta = 1;  // El nuevo nodo tiene una cuenta de 1
    }
    return pnuevo;
}

Pila* contar(Pila* ppila, void* pdato, size_t sdato) {
    for (Pila *paux = ppila; paux; paux = paux->psig) {
        if (!memcmp(paux+1, pdato, sdato)) {
            paux->cuenta++;  // Si los datos ya existen, aumenta la cuenta
            return ppila;     // Devuelve la pila sin apilar un nuevo nodo
        }
    }
    // Si no encuentra el dato, apila un nuevo nodo
    return apilar(ppila, pdato, sdato);
}

void destruir(Pila* ppila) {
    Pila* paux;
    while (ppila) {
        paux = ppila->psig;
        free(ppila);  // Libera el nodo actual
        ppila = paux;
    }
}
