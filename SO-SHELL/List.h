#ifndef HISTORY_LIST_H
#define HISTORY_LIST_H

#define LNULL NULL
#define MAX_LENGTH 256

#include "stdlib.h"
#include "stdbool.h"
#include "string.h"

/////////////DEFINICIÓN DE LOS TIPOS DE DATOS A USAR/////////////

typedef int PositionNumber;

typedef struct {
    PositionNumber posNum;
    char data[MAX_LENGTH][MAX_LENGTH];
} Item;

typedef struct Node* Position;
struct Node{
    Item item;
    Position next;
};

typedef Position List;

/////////////DECLARACIÓN DE FUNCIONES BÁSICAS PARA OPERAR CON EL TAD/////////////

//Función cuyo parámetro de entrada es el nombre de una lista para almacenar el historial de comandos y devuelve una lista vacía.
void createEmptyList(List *list);

//Función que elimina un historial introducido.
void deleteList(List* list);
void deleteFileList(List *list);
void deleteMemList(List *list);
void deleteAtPosition(Position Pos,List *list);
void deleteListVars(List *list);

//Función que verifica si un historial está vacío.
bool isEmptyList(List list);

//Función que devuelve la posición del primer comando del historial.
Position first(List list);

//Función que devuelve la posición del último comando del historial.
Position last(List list);

//Función que inserta un comando al historial en una posición deseada.
bool insertToList(Item Cmd, Position Pos, List* list);

//Función que devuelve un elemento de tipo Command de un historial en función de una posición indicada.
Item getItem(Position Pos);

//Función que devuelve la posición de un comando del historial en función de su posición.
Position findItem(PositionNumber Item, List list);
Position findItemStr(char *data_str, int num, int index, List list, bool useNum);
/////////////DECLARACIÓN DE FUNCIONES AUXILIARES QUE USAN FUNCIONES BÁSICAS/////////////

//Función que imprime un historial hasta una posición indicada.
void printItem(Item item);
void printList(Position Pos, List list, int code);
void printListWithFilter(List list,char* filter);
void initializeFileList(List *list);
#endif