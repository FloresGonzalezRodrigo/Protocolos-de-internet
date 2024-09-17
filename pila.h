//
// Created by Rodrigo on 16/09/2024.
//

#ifndef PILA_H
#define PILA_H

typedef struct Pila {
    struct Pila* psig;
    unsigned short cuenta;
} Pila;

Pila* pila(void* pdato,size_t sdato);

Pila* apilar(Pila* ppila,void* pdato, size_t sdato);

Pila* contar(Pila *ppila, void* pdato, size_t sdato);

void destruir(Pila *ppila);

#endif //PILA_H



